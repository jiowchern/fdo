#include "stdafx.h"
#include <list>
#include <string.h>
#include "CRoleRenderer.h"
#include"CRoleSkinStruct.h"
#include "CRPCFileStruct.h"
#include "CRATFileStruct.h"
#include "CRATFileFactory.h"
#include "CRPCFileFactory.h"
#include "CRoleSkinFactory.h"
#include "ResourceManager.h"
#include "FDO_RolememoryMgr.h"
#include "debug.h"

#include"DrawPlane.h"
#pragma hdrstop



using std::list;
//////////////////////////////////////////////////////////////////////////
namespace FDO
{
	bool LayerSort(SRoleSkinImageNew* i, SRoleSkinImageNew* j)
	{
		return (i->pRATImg->shLayer < j->pRATImg->shLayer);
	}
//////////////////////////////////////////////////////////////////////////
    struct RoleVertex
    {
        // default constructor
	    RoleVertex() {}

        // constructor
        RoleVertex(const D3DXVECTOR3& pos, D3DCOLOR color, const D3DXVECTOR2& tcoords)
            : Pos(pos), Color(color), Tcoords(tcoords) {}

        // Position
        D3DXVECTOR3 Pos;

        // Color
        D3DCOLOR	Color;

        // Texture coordinates
        D3DXVECTOR2 Tcoords;
		
		// uv�K�Ϧ�m
		//float tu;
		//float tv;

        static const DWORD FVF;

        bool operator==(const RoleVertex& other) const
        {
            return ((Pos == other.Pos) && (Color == other.Color) &&
                (Tcoords == other.Tcoords));
        }

        bool operator!=(const RoleVertex& other) const
        {
            return ((Pos != other.Pos) || (Color != other.Color) ||
                (Tcoords != other.Tcoords));
        }
    };

    const DWORD RoleVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    unsigned int sizeof_RoleVertex = sizeof(RoleVertex);

    void RenderRoleInstance(LPDIRECT3DDEVICE9 pd3dDevice,
        SRoleSkinImageNew* pRoleInstance, const D3DXMATRIX& matInvWorld, const D3DCOLOR Color)
    {
        // �p��x�}
        D3DXMATRIX matWorld;
       // D3DXMatrixMultiply( &matWorld, &pRoleInstance->vMatLocal, &matInvWorld );

        // �� Matrix �אּ�@�ɮɡA�i�����o��
        //D3DXMatrixMultiply( &matWorld, &pRoleInstance->pMatrix->vMatLocal, &matWorld );

        // �]�w�x�}
        pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        // �]�w����
        pd3dDevice->SetTexture( 0, pRoleInstance->pSkinTexture->pTexture );

        RoleVertex vtx[4];

        vtx[0].Pos      = D3DXVECTOR3(-pRoleInstance->pRPCImg->fWidth, 0.0f, 0.0f );
        vtx[0].Color    = pRoleInstance->pRPCImg->dwColor & Color;
        vtx[1].Pos      = D3DXVECTOR3( pRoleInstance->pRPCImg->fWidth, 0.0f, 0.0f );
        vtx[1].Color    = pRoleInstance->pRPCImg->dwColor & Color;
        vtx[2].Pos      = D3DXVECTOR3(-pRoleInstance->pRPCImg->fWidth, pRoleInstance->pRPCImg->fHeight, 0.0f );
        vtx[2].Color    = pRoleInstance->pRPCImg->dwColor & Color;
        vtx[3].Pos      = D3DXVECTOR3( pRoleInstance->pRPCImg->fWidth, pRoleInstance->pRPCImg->fHeight, 0.0f );
        vtx[3].Color    = pRoleInstance->pRPCImg->dwColor & Color;

        if( pRoleInstance->pRATImg->bTexInvert )
        { // �ϦV�K����
            vtx[0].Tcoords.x = pRoleInstance->pRPCImg->fTU1;
            vtx[0].Tcoords.y = pRoleInstance->pRPCImg->fTV2;
            vtx[1].Tcoords.x = pRoleInstance->pRPCImg->fTU2;
            vtx[1].Tcoords.y = pRoleInstance->pRPCImg->fTV2;
            vtx[2].Tcoords.x = pRoleInstance->pRPCImg->fTU1;
            vtx[2].Tcoords.y = pRoleInstance->pRPCImg->fTV1;
            vtx[3].Tcoords.x = pRoleInstance->pRPCImg->fTU2;
            vtx[3].Tcoords.y = pRoleInstance->pRPCImg->fTV1;
        }
        else
        {
            vtx[0].Tcoords.x = pRoleInstance->pRPCImg->fTU2;
            vtx[0].Tcoords.y = pRoleInstance->pRPCImg->fTV2;
            vtx[1].Tcoords.x = pRoleInstance->pRPCImg->fTU1;
            vtx[1].Tcoords.y = pRoleInstance->pRPCImg->fTV2;
            vtx[2].Tcoords.x = pRoleInstance->pRPCImg->fTU2;
            vtx[2].Tcoords.y = pRoleInstance->pRPCImg->fTV1;
            vtx[3].Tcoords.x = pRoleInstance->pRPCImg->fTU1;
            vtx[3].Tcoords.y = pRoleInstance->pRPCImg->fTV1;
        }
        
        pd3dDevice->SetFVF( RoleVertex::FVF );
        
      pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
				    &vtx[0], sizeof_RoleVertex );
    }
//////////////////////////////////////////////////////////////////////////
    CRoleRenderer::CRoleRenderer()
        : m_pCurrRATAct(NULL), m_pRATFactory(NULL), m_pRPCFactory(NULL) , m_RoleSkinIDVec()/*,
        m_pMTXFactory(pMTXFactory), m_pSkinFactory(pSkinFactory)*/
    {
    }

    CRoleRenderer::CRoleRenderer(CRATFactory* pRATFactory, CRPCFactory* pRPCFactory/*,
        CMTXFactory* pMTXFactory, CSkinFactory pSkinFactory*/)
        : m_pCurrRATAct(NULL), m_pRATFactory(pRATFactory), m_pRPCFactory(pRPCFactory) , m_RoleSkinIDVec()/*,
        m_pMTXFactory(pMTXFactory), m_pSkinFactory(pSkinFactory)*/
    {
    }

    CRoleRenderer::~CRoleRenderer()
    {
		
        // maybe must to do this...
        //m_pRATFactory->Release( m_spRATFile );

        while (!m_spRPCFileList.empty())
        {
            // maybe there are something need to do...
            //m_pRPCFactory->Release( m_spRPCFileList.front() );

            m_spRPCFileList.pop_front();
        }

		/*for (size_t i(0); i<m_vRoleInstance.size(); ++i)
         {
             // ��Matrix��share pointer�ɡA���i��ݭn���o���
             //m_pMTXFactory->Release( m_RoleSkinImageNews.at(i).pMatrix );
 
             // ���ϥ�SkinFactory�ɡA�ݭn���o���
             //m_pSknFactory->Release( m_RoleSkinImageNews.at(i) );
 
             delete m_vRoleInstance.at(i);
         }*/
         //m_vRoleInstance.clear();

        //m_pSknFactory = NULL;
        //m_pMTXFactory = NULL;
        m_pRPCFactory = NULL;
        m_pRATFactory = NULL;

        m_pCurrRATAct = NULL;

		//�i�Dthread���n����Role�@Skin Link�ʧ@
		UnRegisterSkin() ; 
		
    }

    
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool CRoleRenderer::LoadRATFile(const char* pFileName)
{
    if ( !ValidString( pFileName ) )
    {
        return false;
    }

    if (m_pRATFactory == NULL)
    {
        return false;
    }

    if (m_spRATFile.get())
    { // �p�G�w�g�����[�ɦs�b�A������
        //m_pRATFactory->Release( m_spRATFile );
    }

    // TODO: �o�̤��ݭn�ǤJ m_vRoleInstance�A�]������Ƴ��S����
    if ( !m_pRATFactory->GetRATFile( pFileName, m_spRATFile ) )
    {
	    return false;
    }
	//�s��rat��
    //CRoleSkinFactory::LinkRoleSkinRATFile( m_pNewRoleSkin, m_spRATFile );
	PerformanceMonitor pm;
	pm.Begin("CRoleSkinFactory::LinkRoleSkinRATFile");
    CRoleSkinFactory::LinkRoleSkinRATFile( m_vRoleInstance, m_spRATFile );
    pm.End();
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool CRoleRenderer::LoadRPCFile(const char* pFileName, int nRoleKind)
{
    if ( !ValidString(pFileName) )
    {
        return false;
    }

    if ( m_pRPCFactory == NULL )
    {
        return false;
    }

    SP_RPCFile spRPCFile;

    if ( !m_pRPCFactory->GetRPCFile( pFileName, spRPCFile, m_spRATFile ) ) 
    {
	    return false;
    }

    //�N����s�� list
    m_spRPCFileList.push_back( spRPCFile );
	
    CRoleSkinFactory::LinkRoleSkinRPCFile( m_vRoleInstance, spRPCFile );

    return true;
}

bool CRoleRenderer::LoadRPCFileThread(const char* pFileName, uint32 nRoleID)
{

	

	if ( !ValidString(pFileName) )
    {
        return false;
    }

    if ( m_pRPCFactory == NULL )
    {
        return false;
    }



	SP_RPCFile spRPCFile ; 

	uint32 skinID = nRoleID ; 

	if(m_pRPCFactory->GetRPCFileThread(pFileName , this ,  spRPCFile , m_spRATFile , skinID))
	{
		stSkinData data ; 

		data.skinID = (DWORD)skinID ; 
		data.rpcName = pFileName ; 

		m_RoleSkinIDVec.push_back(data) ; 

		return true ;
	}

    m_spRPCFileList.push_back( spRPCFile );
	
    CRoleSkinFactory::LinkRoleSkinRPCFile( m_vRoleInstance, spRPCFile );



	return false ; 

}

void CRoleRenderer::SkinLinkRPC(SP_RPCFile pRPCFile)
{
    //�N����s�� list
    m_spRPCFileList.push_back( pRPCFile );
	
    CRoleSkinFactory::LinkRoleSkinRPCFile( m_vRoleInstance, pRPCFile );

}

SRATActFile* CRoleRenderer::Play(const char* szActionName, bool bLoop)
{
    if (!ValidString(szActionName))
    {
        return NULL;
    }
	if(m_spRATFile == 0)
		return 0;
    for(size_t i = 0; i < m_spRATFile->vRATActList.size(); ++i)
    {
	    SRATActFile& rRATAct = m_spRATFile->vRATActList.at(i);	
	    if(strcmp( rRATAct.szName, szActionName ) != 0)
	    {
            continue;
	    }

        //�]�w�ثe�ʧ@
        m_bPlayLoop = bLoop;
	    m_pCurrRATAct = &rRATAct;
		return m_pCurrRATAct;
    }
	return NULL;
}



bool CRoleRenderer::Render(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX& matInvWorld,
                           D3DCOLOR Color, int iShowDir, unsigned short usFrameNum,
                           int nRoleKind , CRenderer *pRender , float slope , bool divide)
{
    if (m_pCurrRATAct == NULL)
    { // �p�G�ثe�ʧ@�S�����V�ū��СA�N��ø��
        return false;
    }

		// �Y�ɺ�x�}
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	//std::vector<SRoleSkinImageNew*> RoleInsanceToRender;

    // �`���Ҧ��n�e���s��
    for (size_t i = 0; i < m_pCurrRATAct->vRATComList.size(); ++i)
    {
        SRATComFile& rSRATComFile = m_pCurrRATAct->vRATComList.at(i);

        SRATDirFile& RATDirFile = rSRATComFile.RATDir[iShowDir];

        // �ΨӬ����ݭnø�ϼv�檺ID
        int ID = -1;
        // record next image for linear.
        int NextID = -1;

        for (size_t j = 0; j < RATDirFile.vRATImageList.size(); ++j)
        { // �� usFrameNum ��|�ݭnø�Ϫ��v��
            if (RATDirFile.vRATImageList[j].usPlayTime <= usFrameNum)
            {
                ID = RATDirFile.vRATImageList[j].nRATImageID;
                if (j + 1 == RATDirFile.vRATImageList.size())
                {
                    if (m_bPlayLoop)
                    {
                        NextID = RATDirFile.vRATImageList[0].nRATImageID;
                    }
                }
                else
                {
                    NextID = RATDirFile.vRATImageList[j + 1].nRATImageID;
                }
            }
            else
            {
                break;
            }
        }

        if (ID != -1)
        {
            SRoleSkinImageNew* pRoleInstance = &m_vRoleInstance.at(ID);

            if (pRoleInstance->pRPCImg == NULL)
            { // ��S���o��RPC�ɡA��ܳo�ӹs��S�����J�A�ҥH���ݭnø�X�A�������L�o�ӹs��                    
                continue;
            }

// <yuming> modify the way to get texture file name.
            if (pRoleInstance->pSkinTexture == NULL)
            { // ø�Ϯɤ~�j�M�����ɡA�n�B�O���ݭn�Ψ쪺����~Ū���A���ݭn���ɭԴN����Ū��
                // get texture file name from texture id.
                const char* pTextrueName = m_pRPCFactory->GetTextureStringAt(pRoleInstance->pRPCImg->nTextureNameID);
                if( nRoleKind == 1 )
                { //�D���O1 
	                //pRoleInstance->pSkinTexture = g_RoleImageManager.GetStaticImageRes( pRoleInstance->pRPCImg->szTexFile );
                    pRoleInstance->pSkinTexture = g_RoleImageManager.GetDynamicImageRes(pTextrueName);
                }
                else
                {
	                //pRoleInstance->pSkinTexture = g_RoleImageManager.GetDynamicImageRes( pRoleInstance->pRPCImg->szTexFile );
                    pRoleInstance->pSkinTexture = g_RoleImageManager.GetDynamicImageRes(pTextrueName);
                }
            }
// </yuming>

            // ��x�}�u�t�����ɥi�H�ϥ�
            /*if (pRoleInstance->pMatrix == NULL)
            { // ��J���Ѽƥi��ݭn���GID...
                pRoleInstance->pMatrix = m_pMTXFactory->Get(...);
            }*/

			//�p�G�x�}�S��L�A�N�i�h�p��
//			if( !pRoleInstance->bMatOver )
//			{
//				CRoleSkinFactory::UpdateLocalMatrix( &pRoleInstance->vMatLocal, pRoleInstance->pRATImg, pRoleInstance->pRPCImg );		
//				pRoleInstance->bMatOver = true;
//			}

			if (pRoleInstance->pSkinTexture && pRoleInstance->pSkinTexture->pTexture != NULL)
            { // ���ӫ��г������ɭԤ~�ݭnø��

                /// <yuming> add linear ability
                RoleSkinImageNew newRSIN;
                newRSIN.pSkinTexture = pRoleInstance->pSkinTexture;
                SRATImageFile newRATImg = *pRoleInstance->pRATImg;
                newRSIN.pRATImg = &newRATImg;
                SRPCImageFile newRPCImg = *pRoleInstance->pRPCImg;
                newRSIN.pRPCImg = &newRPCImg;

                RoleSkinImageNewPtr pNextRSIN = NULL;
                if (NextID != -1)
                {
                    pNextRSIN = &m_vRoleInstance.at(NextID);
                }
                newRSIN.Lerp(slope, pNextRSIN);
                pRoleInstance = &newRSIN;
                /// </yuming>

				//local matrix
				CRoleSkinFactory::UpdateLocalMatrix( &matWorld, pRoleInstance->pRATImg, pRoleInstance->pRPCImg);
				// �p��x�}
				D3DXMatrixMultiply( &matWorld, &matWorld, &matInvWorld );

				pRender->CreateFrameOp(pRoleInstance , matWorld , Color & pRoleInstance->pRPCImg->dwColor , divide ) ; 

             //   RoleInsanceToRender.push_back( pRoleInstance );
            }
            else
            { // �C�L�X���~�T���G�S������ίx�}����
                // CPrinter::Print( "error message" );
            }
        }
            
    }

    // �s��Ƨ�
	//std::sort(RoleInsanceToRender.begin() , RoleInsanceToRender.end() , LayerSort) ; 

    //RoleInsanceToRender.sort( LayerSort );



	//for(unsigned int i = 0 ; i < RoleInsanceToRender.size() ; ++i)
	//{
	//		
	//	SRATImageFile*	pRATImg = RoleInsanceToRender[i]->pRATImg;
	//	SRPCImageFile*	pRPCImg = RoleInsanceToRender[i]->pRPCImg;
	//	
	//	
	//	//local matrix
	//	CRoleSkinFactory::UpdateLocalMatrix( &matWorld, pRATImg, pRPCImg);
	//	// �p��x�}
	//	D3DXMatrixMultiply( &matWorld, &matWorld, &matInvWorld );
	//	// �]�w�x�}
	//	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	//	// �]�w����
	//	pd3dDevice->SetTexture( 0, RoleInsanceToRender[i]->pSkinTexture->pTexture );


	//	DrawRole( RoleInsanceToRender[i] , Color & pRPCImg->dwColor );
	//}

	return true;
}

void CRoleRenderer::UnloadRPCFile(const char* pRPCFileName)
{
	if ( !ValidString(pRPCFileName) )
	{
		return;
	}

	//�i�Dthread���n����Role�@Skin Link�ʧ@
	UnRegisterSkin(pRPCFileName) ; 

	SP_RPC_List::const_iterator end( m_spRPCFileList.end() );
	SP_RPC_List::iterator pos( m_spRPCFileList.begin() );

	for (; pos != end; ++pos)
	{
		if(stricmp((*pos)->strFilename.c_str(), pRPCFileName) != 0)
		{
			continue;
		}

		for(size_t i(0); i < (*pos)->vRPCImgPointList1.size(); ++i)
		{
			int nRPCImgID = (*pos)->vRPCImgPointList1.at(i)->nRPCImgID;
			m_vRoleInstance.at( nRPCImgID ).pRPCImg = NULL;
			m_vRoleInstance.at( nRPCImgID ).pSkinTexture = NULL;

		}


		// ���� RPC
		if (m_pRPCFactory)
		{
			//pRPCFactory->Release( (*pos)->strFilename, (*pos) );
		}

		m_spRPCFileList.erase( pos );
		break;
	}
}

void CRoleRenderer::UnlinkRPC(const char* pComName)
{
    if (!ValidString(pComName))
    {
        return;
    }

	//�i�Dthread���n����Role�@Skin Link�ʧ@
	UnRegisterSkin(pComName) ; 


    for (size_t i(0); i < m_spRATFile->vRATActList.size(); ++i)
    { // �����Ҧ��ʧ@
        SRATActFile* pRATAct = &m_spRATFile->vRATActList[i];

        this->UnlinkRPCAct( pRATAct, pComName );
    }
}

void CRoleRenderer::UnlinkRPCAct(SRATActFile* pRATAct, const char* pComName)
{
    for (size_t i(0); i < pRATAct->vRATComList.size(); ++i)
    { // �j�M�Y�ӹs��
        if (strcmp(pRATAct->vRATComList[i].szComName,pComName) == 0)
        { 
            this->UnlinkRPCCom( &pRATAct->vRATComList[i] );

            break; // �u�ݭn Unlink �@�ӹs��A�ҥH���X�j��
        }
    }
}

void CRoleRenderer::UnlinkRPCCom(SRATComFile* pRATCom)
{
    for (int i=0; i<MAX_DIR; ++i)
    {
        this->UnlinkRPCDir( pRATCom->RATDir[i] );
    }
}

void CRoleRenderer::UnlinkRPCDir(SRATDirFile& RATDir)
{
    for (size_t i(0); i<RATDir.vRATImageList.size(); ++i)
    { // ���éҦ����v��
        int ID = RATDir.vRATImageList[i].nRATImageID;

        UnlinkRPCImg( &m_vRoleInstance.at(ID) );
    }
}

void CRoleRenderer::UnlinkRPCImg(SRoleSkinImageNew* pRoleInstance)
{
    pRoleInstance->pRPCImg = NULL;
	pRoleInstance->pSkinTexture = NULL ; 
}

void CRoleRenderer::UnRegisterSkin(const char *rpcName)
{
	std::list<stSkinData>::iterator iter(m_RoleSkinIDVec.begin())  ; 

	for(; iter != m_RoleSkinIDVec.end() ; ++iter)
	{
		if(strcmp(iter->rpcName.c_str() , rpcName) == 0)
		{
			g_RoleMemoryMgr.UnRegisterLinkRPC(iter->skinID) ;

			m_RoleSkinIDVec.erase(iter) ;

			return ; 
		}

	}

}

void CRoleRenderer::UnRegisterSkin()
{
	while(!m_RoleSkinIDVec.empty())
	{
		DWORD skinID = m_RoleSkinIDVec.front().skinID ; 

		g_RoleMemoryMgr.UnRegisterLinkRPC(skinID) ;

		m_RoleSkinIDVec.pop_front() ; 

	}
}

bool CRoleRenderer::ValidString(const char* pString)
{
    if (pString == NULL)
    {
        return false;
    }

    if (strlen(pString) == 0)
    {
        return false;
    }
	
	if ( strcmp( pString,"Null" ) ==0 )
	{
		return false; // �������]�w�������J����ϧ�   
	}

    return true;
}

void CRoleRenderer::UnloadAllRPCFile()
{
	//�i�Dthread���n����Role�@Skin Link�ʧ@
	UnRegisterSkin() ; 

    SP_RPC_List::const_iterator end( m_spRPCFileList.end() );
    SP_RPC_List::iterator pos( m_spRPCFileList.begin() );

    while (pos != end)
    {
        this->UnlinkRPC( (*pos)->RPCFileHeader.szComponentName1 );
        this->UnlinkRPC( (*pos)->RPCFileHeader.szComponentName2 );

        // ���� RPC
        if (m_pRPCFactory)
        {
            //m_pRPCFactory->Release( (*pos)->strFilename, (*pos) );
        }

        pos = m_spRPCFileList.erase( pos );
    }
}
//////////////////////////////////////////////////////////////////////////
}