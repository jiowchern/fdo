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
		
		// uv貼圖位置
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
        // 計算矩陣
        D3DXMATRIX matWorld;
       // D3DXMatrixMultiply( &matWorld, &pRoleInstance->vMatLocal, &matInvWorld );

        // 當 Matrix 改為共享時，可換成這個
        //D3DXMatrixMultiply( &matWorld, &pRoleInstance->pMatrix->vMatLocal, &matWorld );

        // 設定矩陣
        pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        // 設定材質
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
        { // 反向貼材質
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
             // 當Matrix為share pointer時，有可能需要做這件事
             //m_pMTXFactory->Release( m_RoleSkinImageNews.at(i).pMatrix );
 
             // 當有使用SkinFactory時，需要做這件事
             //m_pSknFactory->Release( m_RoleSkinImageNews.at(i) );
 
             delete m_vRoleInstance.at(i);
         }*/
         //m_vRoleInstance.clear();

        //m_pSknFactory = NULL;
        //m_pMTXFactory = NULL;
        m_pRPCFactory = NULL;
        m_pRATFactory = NULL;

        m_pCurrRATAct = NULL;

		//告訴thread不要給此Role作Skin Link動作
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
    { // 如果已經有骨架檔存在，先移除
        //m_pRATFactory->Release( m_spRATFile );
    }

    // TODO: 這裡不需要傳入 m_vRoleInstance，因為什麼事都沒有做
    if ( !m_pRATFactory->GetRATFile( pFileName, m_spRATFile ) )
    {
	    return false;
    }
	//連結rat檔
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

    //將材質存到 list
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
    //將材質存到 list
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

        //設定目前動作
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
    { // 如果目前動作沒有指向空指標，就不繪圖
        return false;
    }

		// 即時算矩陣
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	//std::vector<SRoleSkinImageNew*> RoleInsanceToRender;

    // 蒐集所有要畫的零件
    for (size_t i = 0; i < m_pCurrRATAct->vRATComList.size(); ++i)
    {
        SRATComFile& rSRATComFile = m_pCurrRATAct->vRATComList.at(i);

        SRATDirFile& RATDirFile = rSRATComFile.RATDir[iShowDir];

        // 用來紀錄需要繪圖影格的ID
        int ID = -1;
        // record next image for linear.
        int NextID = -1;

        for (size_t j = 0; j < RATDirFile.vRATImageList.size(); ++j)
        { // 由 usFrameNum 找會需要繪圖的影格
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
            { // 當沒有這個RPC時，表示這個零件沒有載入，所以不需要繪出，直接跳過這個零件                    
                continue;
            }

// <yuming> modify the way to get texture file name.
            if (pRoleInstance->pSkinTexture == NULL)
            { // 繪圖時才搜尋材質檔，好處是有需要用到的材質才讀取，不需要的時候就不用讀取
                // get texture file name from texture id.
                const char* pTextrueName = m_pRPCFactory->GetTextureStringAt(pRoleInstance->pRPCImg->nTextureNameID);
                if( nRoleKind == 1 )
                { //主角是1 
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

            // 當矩陣工廠完成時可以使用
            /*if (pRoleInstance->pMatrix == NULL)
            { // 輸入的參數可能需要有：ID...
                pRoleInstance->pMatrix = m_pMTXFactory->Get(...);
            }*/

			//如果矩陣沒算過，就進去計算
//			if( !pRoleInstance->bMatOver )
//			{
//				CRoleSkinFactory::UpdateLocalMatrix( &pRoleInstance->vMatLocal, pRoleInstance->pRATImg, pRoleInstance->pRPCImg );		
//				pRoleInstance->bMatOver = true;
//			}

			if (pRoleInstance->pSkinTexture && pRoleInstance->pSkinTexture->pTexture != NULL)
            { // 當兩個指標都有的時候才需要繪圖

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
				// 計算矩陣
				D3DXMatrixMultiply( &matWorld, &matWorld, &matInvWorld );

				pRender->CreateFrameOp(pRoleInstance , matWorld , Color & pRoleInstance->pRPCImg->dwColor , divide ) ; 

             //   RoleInsanceToRender.push_back( pRoleInstance );
            }
            else
            { // 列印出錯誤訊息：沒有材質或矩陣指標
                // CPrinter::Print( "error message" );
            }
        }
            
    }

    // 零件排序
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
	//	// 計算矩陣
	//	D3DXMatrixMultiply( &matWorld, &matWorld, &matInvWorld );
	//	// 設定矩陣
	//	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	//	// 設定材質
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

	//告訴thread不要給此Role作Skin Link動作
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


		// 移除 RPC
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

	//告訴thread不要給此Role作Skin Link動作
	UnRegisterSkin(pComName) ; 


    for (size_t i(0); i < m_spRATFile->vRATActList.size(); ++i)
    { // 取消所有動作
        SRATActFile* pRATAct = &m_spRATFile->vRATActList[i];

        this->UnlinkRPCAct( pRATAct, pComName );
    }
}

void CRoleRenderer::UnlinkRPCAct(SRATActFile* pRATAct, const char* pComName)
{
    for (size_t i(0); i < pRATAct->vRATComList.size(); ++i)
    { // 搜尋某個零件
        if (strcmp(pRATAct->vRATComList[i].szComName,pComName) == 0)
        { 
            this->UnlinkRPCCom( &pRATAct->vRATComList[i] );

            break; // 只需要 Unlink 一個零件，所以跳出迴圈
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
    { // 隱藏所有的影格
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
		return false; // 企劃表格設定為不載入任何圖形   
	}

    return true;
}

void CRoleRenderer::UnloadAllRPCFile()
{
	//告訴thread不要給此Role作Skin Link動作
	UnRegisterSkin() ; 

    SP_RPC_List::const_iterator end( m_spRPCFileList.end() );
    SP_RPC_List::iterator pos( m_spRPCFileList.begin() );

    while (pos != end)
    {
        this->UnlinkRPC( (*pos)->RPCFileHeader.szComponentName1 );
        this->UnlinkRPC( (*pos)->RPCFileHeader.szComponentName2 );

        // 移除 RPC
        if (m_pRPCFactory)
        {
            //m_pRPCFactory->Release( (*pos)->strFilename, (*pos) );
        }

        pos = m_spRPCFileList.erase( pos );
    }
}
//////////////////////////////////////////////////////////////////////////
}