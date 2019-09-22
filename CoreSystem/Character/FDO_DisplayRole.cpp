#include "stdafx.h"
//#pragma hdrstop
//----------------------------------------------------------------------------
#include "PacketReader.h"
#include "ResourceManager.h"
#include "DrawPlane.h"
#include "PhenixD3D9RenderSystem.h"
#include "FDO_RoleBase.h"
//#include "PhenixAssert.h"
#include "JLCamera.h"
#pragma hdrstop
#include "FDO_DisplayRole.h"
#include "FDO_ParticleObject.h"
#include "ParticleSystem.h"
#include "CoreSystem\\Command\\GameCommand.h"

//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("�޲z�Ҧ��Ұ�") 
#else
#pragma message("�@��Ҧ��Ұ�") 
#endif

////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
namespace FDO
{
//----------------------------------------------------------------------------
D3DVIEWPORT9 DisplayRole::s_ViewPort;
D3DXMATRIX DisplayRole::s_matProj;
D3DXMATRIX DisplayRole::s_matView;

D3DVIEWPORT9 CNumber::s_ViewPort;
D3DXMATRIX CNumber::s_matProj;
D3DXMATRIX CNumber::s_matView;

float CNumber::s_fixCount = 1.0f ; //�@��
CNumber::CNumber()
{
	m_pd3dDevice        = g_RenderSystem.GetD3DDevice(); 
	m_bZero = false ;
	m_ViewPort.X = 0 ; 
	m_ViewPort.Y = 0 ; 
	m_ViewPort.Width = 0 ; 
	m_ViewPort.Height = 0 ; 
	m_ViewPort.MinZ = 0.0f ; 
	m_ViewPort.MaxZ = 1.0f; 
	m_disScale = 1.0f ; 
	m_resetValue = 0 ; 
	m_color = D3DCOLOR_ARGB( 255 , 255, 255, 255 ) ; 

    D3DXMatrixLookAtLH( &m_matView,
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), // ��v����m
        &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), // �`���I
        &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ); // up vector

}
void CNumber::RenderPipeline()
{

	BeginRender2D(m_pd3dDevice , s_ViewPort , s_matProj , s_matView ,  m_ViewPort , m_matProj , m_matView) ; 

	////////////////

	D3DXMATRIX mat ; 
	D3DXMatrixIdentity(&mat) ; 

	D3DXMatrixScaling(&mat , m_disScale ,  m_disScale , m_disScale) ; 
	mat._41 = m_ViewPort.Width * 0.5f ; 
	mat._42 = m_ViewPort.Height * 0.5f ; 
	mat._43 = 1.0f ; 
	DrawDamage(0x00 , m_currValue  , m_color , mat) ; 



	///////////////

	EndRender2D(m_pd3dDevice , s_ViewPort , s_matProj , s_matView) ; 

	D3DXMatrixIdentity(&mat) ; 

	m_pd3dDevice->SetTransform(D3DTS_WORLD , &mat) ; 

	m_pd3dDevice->SetTexture(0 , NULL) ; 

}
void CNumber::Update(float fElapsedTime)
{
	static float curCount = 0.0f ; 

	if(m_bZero)
	{
		return ; 
	}

	curCount += fElapsedTime ; 

	if(curCount > s_fixCount)
	{
		m_currValue-- ; 

		if(m_currValue <= 0)
		{
			m_currValue = m_resetValue ;
			m_bZero = true ; 
		}

		curCount = 0.0f ; 
	}
}
void CNumber::SetViewPosition(float left , float top) 
{
	m_ViewPort.X = (DWORD)left ; 
	m_ViewPort.Y = (DWORD)top ; 

}
void CNumber::SetViewSize(unsigned int width , unsigned int height)
{
	m_ViewPort.Width = width ; 
	m_ViewPort.Height = height ; 

	GetProjection2D(m_matProj , width , height) ; 

}

//----------------------------------------------------------------------------
DisplayRole::DisplayRole() : m_bDynamicRender(true) , m_lockPartVec() , m_actionVec() , m_playStack()
{
	GameCommand::Factory& fac = CommandSystem::fnGetFactory();
	m_hRoleResRequester = fac.MakeLight<DisplayRole , void>(this , &DisplayRole::ReleaseRoleRes);	
	RoleMemoryMgr::GetSingleton().ResourceRequesterRegister(m_hRoleResRequester);
    m_pd3dDevice        = g_RenderSystem.GetD3DDevice();
    m_pNowAction        = NULL;
    m_bVisible          = false;
    m_eShowDir          = RD_South;
    m_vPosition         = D3DXVECTOR2( 0.0f, 0.0f );
    m_vScale            = D3DXVECTOR2( 1.0f, 1.0f );
    m_byClassID         = 0;
//     m_iWeaponID         = 0;
//     m_iShieldID         = 0;
//     m_iHelmetID         = 0;
//     m_iAdomID           = 0;
//     m_iCloakID          = 0;

    D3DXMatrixLookAtLH( &m_matView,
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), // ��v����m
        &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), // �`���I
        &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ); // up vector

    D3DXMATRIX matBillboard;
    D3DXMatrixLookAtLH( &matBillboard,
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
        &D3DXVECTOR3( 0.0f, 0.0f, -1.0f ),
        &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    D3DXMatrixInverse(&matBillboard, NULL, &matBillboard); // �f�x�}(�ϯx�})

    // �]�w�����m
    D3DXMATRIX matWorld;
    //D3DXMatrixIdentity( &matWorld );
    D3DXMatrixTranslation( &m_matWorld, 0.0f , 0.0f, 0);//-10.0f );
    D3DXMatrixMultiply( &m_matInvWorld, &m_matWorld, &matBillboard );

    this->SetPosition( m_vPosition );
    m_ViewPort.MinZ = 0.0f;
    m_ViewPort.MaxZ = 1.0f;    

	for( int i = nHelmet; i < EQPART_MAX; ++i )
	{
		PDArray[i].nEQPart = i;
		PDArray[i].nItemID = -1;
	}
	//m_frameCenter		= 0.0f; 
	m_disScale			= 1.0f ; 
	//m_viewOffset		= D3DXVECTOR2(0.0f , 0.0f); 

	m_passTime = 0 ; 
	m_usFrameMax = 0  ; 
	m_usFrameNum = 1  ; 
	m_pParticle = NULL ; 


}
//----------------------------------------------------------------------------
DisplayRole::~DisplayRole()
{
	RoleMemoryMgr::GetSingleton().ResourceRequesterUnregister(m_hRoleResRequester);
	GameCommand::Factory& fac = CommandSystem::fnGetFactory();
	fac.Recycle(m_hRoleResRequester);
	m_hRoleResRequester = TdgHandle();
    RemoveAllAction();

	m_lockPartVec.clear() ; 

	delete m_pParticle ; 
	m_pParticle = NULL ; 

}

//----------------------------------------------------------------------------
void DisplayRole::Reset()
{
    m_bVisible          = false;
    m_eShowDir          = RD_South;
    m_vPosition         = D3DXVECTOR2( 0.0f, 0.0f );
    m_vScale            = D3DXVECTOR2( 1.0f, 1.0f );
    m_byClassID         = 0;
	//m_frameCenter		= 0.0f; 
	m_disScale			= 1.0f ;
	//m_viewOffset		= D3DXVECTOR2(0.0f , 0.0f); 
//     m_iWeaponID         = 0;
//     m_iShieldID         = 0;
//     m_iHelmetID         = 0;
//     m_iAdomID           = 0;
//     m_iCloakID          = 0;

    RemoveAllAction();

	m_lockPartVec.clear() ; 
}
//////////////////////////////////////////////////////////////////////////
void DisplayRole::ReleaseRoleRes()
{
	RemoveAllAction();
}
//----------------------------------------------------------------------------
//�����˳ưʧ@
//----------------------------------------------------------------------------
void DisplayRole::RemoveAllAction()
{
	for(unsigned int i = 0 ; i < m_actionVec.size() ; ++i)
	{
		if(m_actionVec[i] != NULL)
			g_RoleMemoryMgr.FreeAction( m_actionVec[i] );
	}

	m_actionVec.clear() ; 

	m_pNowAction = NULL ; 

    //if( m_pNowAction != NULL )
    //{
    //    g_RoleMemoryMgr.FreeAction( m_pNowAction );
    //    m_pNowAction = NULL;
    //}
}

//----------------------------------------------------------------------------
// ���J��Ө���
//----------------------------------------------------------------------------
bool DisplayRole::LoadRoleAll( const string& strFileName )
{

#ifdef RESMGR

    memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;


	string str;

#else
    // Ū���ʸ���
    memPack* pMem = LoadFromPacket( strFileName );
    if( pMem == NULL ) return false;

    string str;

    // �x�s�ʸ��ɸ��
    BYTE* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );

#endif




    // Ū�����Y��
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->szFileID, "ALL" ) != 0 )
    {
        /* str.assign("ALL");
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//�ɮ׮榡���~
        str.append( ",LoadRoleAll" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//����
        Assert( 0, str.c_str() ); */
        return false;
    }

    pData += sizeof( sRoleHeader );

    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    D3DXVECTOR3 vScale, vPosition;
    char szTexFile[50];

    // ����귽
    RemoveAllAction();

	

    // �إ߫ݾ��ʧ@���
	if(m_pNowAction)
		g_RoleMemoryMgr.FreeAction(m_pNowAction);
    m_pNowAction = g_RoleMemoryMgr.GetAction();

    // ���o���
    strcpy( m_pNowAction->szActionName, (char*)pData );
    pData += 20;
    m_pNowAction->bPlayLoop = *(bool*)pData;
    pData += sizeof( bool );
    m_pNowAction->dwUpdateTime = *(DWORD*)pData;
    pData += sizeof( DWORD );
    m_pNowAction->bySoundKind = *(BYTE*)pData;
    pData += sizeof( BYTE );
    m_pNowAction->usHitTime1 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usHitTime2 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usPlaySoundTime1 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usPlaySoundTime2 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usPlaySoundTime3 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usPlayParticleTime = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    strcpy( m_pNowAction->szSoundFile1, (char*)pData );    //���� ***
    pData += 50;
    strcpy( m_pNowAction->szSoundFile2, (char*)pData );   //���� ***
    pData += 50;
    strcpy( m_pNowAction->szSoundFile3, (char*)pData );   //���� ***
    pData += 50;
    strcpy( m_pNowAction->szParticleFile, (char*)pData );  //���� ***
    pData += 50;
    pData += 20;    // �w�d���

    // �s���`��
    unsigned short usComponentCount = *(unsigned short*)pData;
    pData += sizeof( unsigned short );

    bool bBackAdornment = false;

    // �إ߹s����
    for( unsigned short j=0; j<usComponentCount; ++j )
    {
        pRoleComponent = g_RoleMemoryMgr.GetComponent();
        pRoleComponent->bShow = true;
        m_pNowAction->ComponentList->push_back( pRoleComponent );

        // ���o���
        strcpy( pRoleComponent->szComponentName, (char*)pData );
        pData += 20;

        // �P�_�O�_���I���s��
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 863, CodeInlineText_Text ) );//�I��
        str.assign( "�I��" );
        if( strcmp( pRoleComponent->szComponentName, str.c_str() ) == 0 )
            bBackAdornment = true;
        else
            bBackAdornment = false;

        for( unsigned short k=0; k<8; ++k )
        {
            // ���o���
            pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
            pData += sizeof( unsigned short );

            // �e���`��
            unsigned short usFrameCount = *(unsigned short*)pData;
            pData += sizeof( unsigned short );

            // �إߵe�����
            for( unsigned short l=0; l<usFrameCount; ++l )
            {
                pFrame = g_RoleMemoryMgr.GetFrame();
				sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
				sFrameList& sfl = *scd.FrameList;
				sint nJCDebugCode_addr = (sint)scd.FrameList; 
				sfl.push_back(pFrame);

                // ���o���
                //pFrame->usUnitTime = *(unsigned short*)pData;
                pData += sizeof( unsigned short );
                pFrame->usPlayTime = *(unsigned short*)pData;
                pData += sizeof( unsigned short );
                pFrame->shLayer = *(short*)pData;
                pData += sizeof( short );
                pData += sizeof( float ); // ����

                pFrame->matLocal._11 = *(float*)pData;
                pData += sizeof( float ); // �Y��X
                pFrame->matLocal._12 = *(float*)pData;
                pData += sizeof( float ); // �Y��Y

                pFrame->matLocal._13 = *(float*)pData;
                pData += sizeof( float ); // ��mX
                pFrame->matLocal._14 = *(float*)pData;
                pData += sizeof( float ); // ��mY

                pData += sizeof( float ); // ��mZ
                pFrame->bTexInvert = *(bool*)pData;
                pData += sizeof( bool );

                pFrame->fWidth = *(float*)pData;
                pData += sizeof( float );
                pFrame->fHeight = *(float*)pData;
                pData += sizeof( float );
                pFrame->fTU1 = *(float*)pData;
                pData += sizeof( float );
                pFrame->fTV1 = *(float*)pData;
                pData += sizeof( float );
                pFrame->fTU2 = *(float*)pData;
                pData += sizeof( float );
                pFrame->fTV2 = *(float*)pData;
                pData += sizeof( float );
                strncpy( szTexFile, (char*)pData, 50 );
                pData += 50;
                if( pHeader->usVersion > 1 )
                    pFrame->Color = *(DWORD*)pData;
                else
                    pFrame->Color = 0xFFFFFFFF;
                pData += sizeof_DWORD;

                // Ū�������I��T
                if (pHeader->usVersion > 2) {
                    pFrame->fPercentX = *(float*) pData;
                    pData += sizeof_float;
                    pFrame->fPercentY = *(float*) pData;
                    pData += sizeof_float;
                } else {
                    pFrame->fPercentX = 50.0f;
                    pData += sizeof_float;
                    pFrame->fPercentY = 100.0f;
                    pData += sizeof_float;
                }

                // �u�ʰѼ�
                pFrame->LinearFlag = *(DWORD*) pData;
                pData += sizeof_DWORD;
                // �w�d���
                pData += 4;

                // Ū������
                if( szTexFile[0] == '\0' )
                {
                    pFrame->pImage = NULL;
                }
                else
                {
                    pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
                }

                // �I���s��
                if( bBackAdornment )
                    pFrame->matLocal._21 = 1.0f;
                else
                    pFrame->matLocal._21 = -1.0f;
            }
        }
    }

    return true;
}

bool DisplayRole::LoadMonsterEx(const string& strFileName )
{
  // Ū���ʸ���
    memPack* pMem = LoadFromPacket( strFileName );
	
	if( pMem->Data == NULL )
        return false;

    BYTE* pData = pMem->Data;

    // Ū�����Y��
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->szFileID, "ALL" ) != 0 )
    {
	   Assert( 0, "not ALL fileformat" );
        return false;
    }

    //pRoleRes->fScale = pHeader->fScaleX;
    //pRoleRes->usShadowSize = pHeader->usShadowSize;
    //pRoleRes->usShadowOffset = pHeader->usShadowOffset;
    //pRoleRes->usBoundingBoxSize = pHeader->usBoundingBoxSize;
    //pRoleRes->usDeadBoundBoxSize = pHeader->usDeadBoundBoxSize;

    pData += sizeof( sRoleHeader );


	static sRoleActionList s_tempList;
	static sRoleActionList *tempList = &s_tempList ; 

	tempList->clear() ; 

	//g_RoleMemoryMgr.ClearLoadActions() ; 

	g_RoleMemoryMgr.PushUnLoadingAction(pHeader->usVersion , tempList , pData , pHeader->usActCount) ; 

	return true ; 


	//m_pNowAction = g_RoleMemoryMgr.GetAction();
	if(m_pNowAction)
		g_RoleMemoryMgr.FreeAction(m_pNowAction);
	sRoleAction* pAction = m_pNowAction;
    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    char szTexFile[50];
    D3DXVECTOR3 vScale, vPosition;

    //// �إ߰ʧ@���
    for( unsigned short i=0; i<pHeader->usActCount; ++i )
    {
        pAction = g_RoleMemoryMgr.GetAction();
      //  pRoleRes->mActionList.push_back( pAction );

        // ���o���
        strcpy( pAction->szActionName, (char*)pData );
        pData += 20;
        pAction->bPlayLoop = *(bool*)pData;
        pData += sizeof( bool );
        pAction->dwUpdateTime = *(DWORD*)pData;
        pData += sizeof( DWORD );
        pAction->bySoundKind = *(BYTE*)pData;
        pData += sizeof( BYTE );
        pAction->usHitTime1 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usHitTime2 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usPlaySoundTime1 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usPlaySoundTime2 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usPlaySoundTime3 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usPlayParticleTime = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        strcpy( pAction->szSoundFile1, (char*)pData ); //���� ***
        pData += 50;
        strcpy( pAction->szSoundFile2, (char*)pData );   //���� ***
        pData += 50;
        strcpy( pAction->szSoundFile3, (char*)pData );   //���� ***
        pData += 50;
        strcpy( pAction->szParticleFile, (char*)pData );  //���� ***
        pData += 50;
        pData += 20;    // �w�d���

        // �s���`��
        unsigned short usComponentCount = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // �إ߹s����
        for( unsigned short j=0; j<usComponentCount; ++j )
        {
            pRoleComponent = g_RoleMemoryMgr.GetComponent();
            pAction->ComponentList->push_back( pRoleComponent );

            // ���o���
            strcpy( pRoleComponent->szComponentName, (char*)pData );
            pData += 20;

            for( unsigned short k=0; k<8; ++k )
            {
                // ���o���
                pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
                pData += sizeof( unsigned short );

                // �e���`��
                unsigned short usFrameCount = *(unsigned short*)pData;
                pData += sizeof( unsigned short );

                // �إߵe�����
                for( unsigned short l=0; l<usFrameCount; ++l )
                {
                    pFrame = g_RoleMemoryMgr.GetFrame();                    

					sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
					sFrameList& sfl = *scd.FrameList;
					sint nJCDebugCode_addr = (sint)scd.FrameList; 
					sfl.push_back(pFrame);


                    // ���o���
                    //pFrame->usUnitTime = *(unsigned short*)pData;
                    pData += sizeof( unsigned short );
                    pFrame->usPlayTime = *(unsigned short*)pData;
                    pData += sizeof( unsigned short );
                    pFrame->shLayer = *(short*)pData;
                    pData += sizeof( short );
                    vScale.z = *(float*)pData;
                    pData += sizeof( float );
                    vScale.x = *(float*)pData;
                    pData += sizeof( float );
                    vScale.y = *(float*)pData;
                    pData += sizeof( float );
                    vPosition = *(D3DXVECTOR3*)pData;
                    pData += sizeof( D3DXVECTOR3 );
                    pFrame->bTexInvert = *(bool*)pData;
                    pData += sizeof( bool );
                    pFrame->fWidth = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fHeight = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fTU1 = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fTV1 = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fTU2 = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fTV2 = *(float*)pData;
                    pData += sizeof( float );
                    strncpy( szTexFile, (char*)pData, 50 );
                    pData += 50;
                    if( pHeader->usVersion > 1 )
						pFrame->Color = *(DWORD*)pData;
					else
						pFrame->Color = 0xFFFFFFFF;
                    pData += sizeof( DWORD );

                    // Ū�������I��T
					if( pHeader->usVersion > 2 )
					{
						pFrame->fPercentX = *(float*)pData;
						pData += sizeof_float;
						pFrame->fPercentY = *(float*)pData;
						pData += sizeof_float;
					}
					else
					{
						pFrame->fPercentX = 50.0f;
						pData += sizeof_float;
						pFrame->fPercentY = 100.0f;
						pData += sizeof_float;
					}

                    // �u�ʰѼ�
                    pFrame->LinearFlag = *(DWORD*) pData;
                    pData += sizeof_DWORD;
                    // �w�d���
					pData += 4;

                    // Ū������
                    if( szTexFile[0] == '\0' )
                        pFrame->pImage = NULL;
                    else
                        pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );

                    // ��s�e���x�}
                    pFrame->vPosition = vPosition;
                    pFrame->vScale = vScale;
                    pFrame->RefreshMatrix( vScale, vPosition );
                }
            }
		}
    }

    return true;
}

bool DisplayRole::LoadMonster(const string& strFileName )
{
   // Ū���ʸ���
    memPack* pMem = LoadFromPacket( strFileName );

#ifdef RESMGR
	
	if( pMem->Data == NULL )
        return false;

#else
      if( pMem == NULL )
	  {
		  Assert( 0, "LoadMonster file  is error" );

		 return false;

	  }

#endif


    BYTE* pData = pMem->Data;

    // Ū�����Y��
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->szFileID, "ALL" ) != 0 )
    {
  //      string str;
  //      str.assign( "ALL" );
  //      str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//�ɮ׮榡���~
  //      str.append( ",LoadRoleAll" );
  //      str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//����
		//str.append( "file name : ");
		//str.append( strFileName.c_str() );
        Assert( 0, "not ALL fileformat" );
        return false;
    }

    //pRoleRes->fScale = pHeader->fScaleX;
    //pRoleRes->usShadowSize = pHeader->usShadowSize;
    //pRoleRes->usShadowOffset = pHeader->usShadowOffset;
    //pRoleRes->usBoundingBoxSize = pHeader->usBoundingBoxSize;
    //pRoleRes->usDeadBoundBoxSize = pHeader->usDeadBoundBoxSize;

    pData += sizeof( sRoleHeader );
	if(m_pNowAction)
		g_RoleMemoryMgr.FreeAction(m_pNowAction);
	m_pNowAction = g_RoleMemoryMgr.GetAction();
	sRoleAction* pAction = m_pNowAction;
    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    char szTexFile[50];
    D3DXVECTOR3 vScale, vPosition;

    //// �إ߰ʧ@���
    //for( unsigned short i=0; i<pHeader->usActCount; ++i )
    //{
    //    pAction = g_RoleMemoryMgr.GetAction();
    //    pRoleRes->mActionList.push_back( pAction );

        // ���o���
        strcpy( pAction->szActionName, (char*)pData );
        pData += 20;
        pAction->bPlayLoop = *(bool*)pData;
        pData += sizeof( bool );
        pAction->dwUpdateTime = *(DWORD*)pData;
        pData += sizeof( DWORD );
        pAction->bySoundKind = *(BYTE*)pData;
        pData += sizeof( BYTE );
        pAction->usHitTime1 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usHitTime2 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usPlaySoundTime1 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usPlaySoundTime2 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usPlaySoundTime3 = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usPlayParticleTime = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        strcpy( pAction->szSoundFile1, (char*)pData ); //���� ***
        pData += 50;
        strcpy( pAction->szSoundFile2, (char*)pData );   //���� ***
        pData += 50;
        strcpy( pAction->szSoundFile3, (char*)pData );   //���� ***
        pData += 50;
        strcpy( pAction->szParticleFile, (char*)pData );  //���� ***
        pData += 50;
        pData += 20;    // �w�d���

        // �s���`��
        unsigned short usComponentCount = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // �إ߹s����
        for( unsigned short j=0; j<usComponentCount; ++j )
        {
            pRoleComponent = g_RoleMemoryMgr.GetComponent();
            pAction->ComponentList->push_back( pRoleComponent );

            // ���o���
            strcpy( pRoleComponent->szComponentName, (char*)pData );
            pData += 20;

            for( unsigned short k=0; k<8; ++k )
            {
                // ���o���
                pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
                pData += sizeof( unsigned short );

                // �e���`��
                unsigned short usFrameCount = *(unsigned short*)pData;
                pData += sizeof( unsigned short );

                // �إߵe�����
                for( unsigned short l=0; l<usFrameCount; ++l )
                {
                    pFrame = g_RoleMemoryMgr.GetFrame();
					sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
					sFrameList& sfl = *scd.FrameList;
					sint nJCDebugCode_addr = (sint)scd.FrameList; 
					sfl.push_back(pFrame);

                    // ���o���
                    //pFrame->usUnitTime = *(unsigned short*)pData;
                    pData += sizeof( unsigned short );
                    pFrame->usPlayTime = *(unsigned short*)pData;
                    pData += sizeof( unsigned short );
                    pFrame->shLayer = *(short*)pData;
                    pData += sizeof( short );
                    vScale.z = *(float*)pData;
                    pData += sizeof( float );
                    vScale.x = *(float*)pData;
                    pData += sizeof( float );
                    vScale.y = *(float*)pData;
                    pData += sizeof( float );
                    vPosition = *(D3DXVECTOR3*)pData;
                    pData += sizeof( D3DXVECTOR3 );
                    pFrame->bTexInvert = *(bool*)pData;
                    pData += sizeof( bool );
                    pFrame->fWidth = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fHeight = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fTU1 = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fTV1 = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fTU2 = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fTV2 = *(float*)pData;
                    pData += sizeof( float );
                    strncpy( szTexFile, (char*)pData, 50 );
                    pData += 50;
                    if( pHeader->usVersion > 1 )
						pFrame->Color = *(DWORD*)pData;
					else
						pFrame->Color = 0xFFFFFFFF;
                    pData += sizeof( DWORD );

                    // Ū�������I��T
					if( pHeader->usVersion > 2 )
					{
						pFrame->fPercentX = *(float*)pData;
						pData += sizeof_float;
						pFrame->fPercentY = *(float*)pData;
						pData += sizeof_float;
					}
					else
					{
						pFrame->fPercentX = 50.0f;
						pData += sizeof_float;
						pFrame->fPercentY = 100.0f;
						pData += sizeof_float;
					}

                    // �u�ʰѼ�
                    pFrame->LinearFlag = *(DWORD*) pData;
                    pData += sizeof_DWORD;

                    // �w�d���
					pData += 4;

                    // Ū������
                    if( szTexFile[0] == '\0' )
                        pFrame->pImage = NULL;
                    else
                        pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );

                    // ��s�e���x�}
                    pFrame->vPosition = vPosition;
                    pFrame->vScale = vScale;
                    pFrame->RefreshMatrix( vScale, vPosition );
                }
            }
		}
   // }

    return true;
}


//----------------------------------------------------------------------------
//�q�O������J�H���ʧ@
//----------------------------------------------------------------------------
bool DisplayRole::LoadActionEx(const char* pFileName)
{
    memPack* pMem = LoadFromPacket( pFileName );

#ifdef RESMGR

	//cosiann
	if( pMem->Data == NULL ) return false;

#else
    if( pMem == NULL ) return false;

#endif


    BYTE* pData = pMem->Data;

    // Ū�����Y��
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->szFileID, "RAT" ) != 0 ) return false;

    pData += sizeof_RoleHeader;

    //sRoleComponent* pRoleComponent = NULL;
    //sFrame* pFrame = NULL;

    // ����귽
    RemoveAllAction();

	sRoleAction *pAction = NULL ; 

	//char tempActName[20] ; 

    // �إ߰ʧ@���
    for( unsigned short i=0; i< pHeader->usActCount; ++i )
    {

		pAction = g_RoleMemoryMgr.GetAction();

		m_actionVec.push_back( pAction );

		// ���o���
		strcpy( pAction->szActionName , (char*)pData );

		pData += 20;
		pAction->bPlayLoop = *(bool*)pData;
		pData += sizeof( bool );
		pAction->dwUpdateTime = *(DWORD*)pData;
		pData += sizeof( DWORD );
		pAction->bySoundKind = *(BYTE*)pData;
		pData += sizeof( BYTE );
		pAction->usHitTime1 = *(unsigned short*)pData;
		pData += sizeof( unsigned short );
		pAction->usHitTime2 = *(unsigned short*)pData;
		pData += sizeof( unsigned short );
		pAction->usPlaySoundTime1 = *(unsigned short*)pData;
		pData += sizeof( unsigned short );
		pAction->usPlaySoundTime2 = *(unsigned short*)pData;
		pData += sizeof( unsigned short );
		pAction->usPlaySoundTime3 = *(unsigned short*)pData;
		pData += sizeof( unsigned short );
		pAction->usPlayParticleTime = *(unsigned short*)pData;
		pData += sizeof( unsigned short );
		strcpy( pAction->szSoundFile1, (char*)pData );
		pData += 50;
		strcpy( pAction->szSoundFile2, (char*)pData );
		pData += 50;
		strcpy( pAction->szSoundFile3, (char*)pData );
		pData += 50;
		strcpy( pAction->szParticleFile, (char*)pData );
		pData += 50;
		pData += 20;    // �w�d���

		// �s���`��
		unsigned short usComponentCount = *(unsigned short*)pData;
		pData += sizeof( unsigned short );

		bool bBackAdornment = false;

		// �إ߹s����
		for( unsigned short j=0; j<usComponentCount; ++j )
		{
			sRoleComponent* pCom = g_RoleMemoryMgr.GetComponent();
			pCom->bShow = false;
			pAction->ComponentList->push_back( pCom );

			// ���o���
			strcpy( pCom->szComponentName, (char*)pData );
			pData += 20;

			// �P�_�O�_���I���s��
			//str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 863, CodeInlineText_Text ) );//�I��
			string str( "�I��" );
			if( strcmp( pCom->szComponentName, str.c_str() ) == 0 )
				bBackAdornment = true;
			else
				bBackAdornment = false;

			for( unsigned short k=0; k<8; ++k )
			{
				// ���o���
				sComponentDir& Dir = *(pCom->ComponentDirs[k]);
				Dir.usMaxFrameNum = *(unsigned short*)pData;
				pData += sizeof( unsigned short );

				// �e���`��
				unsigned short usFrameCount = *(unsigned short*)pData;
				pData += sizeof( unsigned short );

				// �إߵe�����
				for( unsigned short l=0; l<usFrameCount; ++l )
				{
					//sFrame* pFrame = g_RoleMemoryMgr.GetFrame();
					Dir.FrameList->push_back( g_RoleMemoryMgr.GetFrame() );
					sFrame* pFrame = Dir.FrameList->back();

					// ���o���
					//pFrame->usUnitTime = *(unsigned short*)pData;
					pData += sizeof( unsigned short );
					pFrame->usPlayTime = *(unsigned short*)pData;
					pData += sizeof( unsigned short );
					pFrame->shLayer = *(short*)pData;
					pData += sizeof( short );
					pFrame->vScale.z = *(float*)pData;
					pData += sizeof( float );
					pFrame->vScale.x = *(float*)pData;
					pData += sizeof( float );
					pFrame->vScale.y = *(float*)pData;
					pData += sizeof( float );
					pFrame->vPosition = *(D3DXVECTOR3*)pData;
					pData += sizeof( D3DXVECTOR3 );
					pFrame->bTexInvert = *(bool*)pData;
					pData += sizeof( bool );
					//pData += 9;    // �w�d���
					if( pHeader->usVersion > 1 )
					{
            			pFrame->fPercentX = *(float*)pData;
						pData += sizeof( float );
						pFrame->fPercentY = *(float*)pData;
						pData += sizeof( float );
						pFrame->LinearFlag = *(BYTE*) pData;
                        pData += sizeof_BYTE;
					}
					else
					{
            			pData += 9;    // �w�d���
					}
					pFrame->pImage = NULL;
					pFrame->Color = 0xFFFFFFFF;

					// ��s�e���x�}
					pFrame->RefreshMatrix( pFrame->vScale, pFrame->vPosition );
				}
			}
		}
		

	}

	//FilterAction() ; 

	// �إ߫ݾ��ʧ@���
	m_pNowAction = GetAction("Stand") ; 

    return true;
}

sRoleAction* DisplayRole::GetAction(const char *actionName)
{
	for(unsigned int i = 0 ; i < m_actionVec.size() ; i++)
	{
		sRoleAction* pAction = m_actionVec[i];
		
		if(pAction && strcmp(pAction->szActionName , actionName) == 0)
		{
			return m_actionVec[i] ; 
		}
	}

	return NULL ; 
}
void DisplayRole::PushParticle(const char *emtName , bool isStatic)
{
	delete m_pParticle ;
	m_pParticle = NULL ;
	
	m_pParticle = new ParticleObject() ; 
	
	m_pParticle->CreateParticle(emtName , isStatic) ;

}

void DisplayRole::SetAction(const char* actName)
{
	sRoleAction *tempAct = GetAction(actName) ; 
	if(m_pNowAction && tempAct)
	{
		if(strcmp(m_pNowAction->szActionName , tempAct->szActionName) == 0)
		{
			return ; 
		}

		m_pNowAction = tempAct ; 
	}
	
}


void DisplayRole::FilterAction() 
{
	for(unsigned int i = 0 ; i < m_actionVec.size() ; i++)
	{
		if(strcmp(m_actionVec[i]->szActionName , "Stand") == 0 || 
			strcmp(m_actionVec[i]->szActionName , "Walk") == 0)
			//strcmp(m_actionVec[i]->szActionName , "Atk_01") == 0 || 
			//strcmp(m_actionVec[i]->szActionName , "Atk_02") == 0 ||
			//strcmp(m_actionVec[i]->szActionName , "Atk_03") == 0 ||
			//strcmp(m_actionVec[i]->szActionName , "Atk_04") == 0 ||
			//strcmp(m_actionVec[i]->szActionName , "Atk_05") == 0 ||
			//strcmp(m_actionVec[i]->szActionName , "Atk_06") == 0 ||
			//strcmp(m_actionVec[i]->szActionName , "Atk_07") == 0 ||
			//strcmp(m_actionVec[i]->szActionName , "Atk_08") == 0 ||
			//strcmp(m_actionVec[i]->szActionName , "Atk_09") == 0 )
		{
			continue ; 
		}
		else
		{
			g_RoleMemoryMgr.FreeAction( m_actionVec[i] );

			m_actionVec[i] = NULL ;
		}


	}
}
//----------------------------------------------------------------------------
//�q�O������J�H���ʧ@
//----------------------------------------------------------------------------
bool DisplayRole::LoadAction( const string& strFileName )
{
    
#ifdef RESMGR
    memPack* pMem = LoadFromPacket( strFileName );

	//cosiann
	if( pMem->Data == NULL ) return false;

#else

	memPack* pMem = LoadFromPacket( strFileName );

    if( pMem == NULL ) return false;

#endif
        

    BYTE* pData = pMem->Data;

    // Ū�����Y��
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->szFileID, "RAT" ) != 0 )
    {
        /* str.assign("RAT");
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//�ɮ׮榡���~
        str.append( ",LoadAction" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//����
        Assert( 0, str.c_str() ); */
        return false;
    }

    pData += sizeof_RoleHeader;

    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    D3DXVECTOR3 vScale, vPosition;

    // ����귽
    RemoveAllAction();

    // �إ߫ݾ��ʧ@���
	if(m_pNowAction)
		g_RoleMemoryMgr.FreeAction(m_pNowAction);
    m_pNowAction = g_RoleMemoryMgr.GetAction();

    // ���o���
    strcpy( m_pNowAction->szActionName, (char*)pData );
    pData += 20;
    m_pNowAction->bPlayLoop = *(bool*)pData;
    pData += sizeof( bool );
    m_pNowAction->dwUpdateTime = *(DWORD*)pData;
    pData += sizeof( DWORD );
    m_pNowAction->bySoundKind = *(BYTE*)pData;
    pData += sizeof( BYTE );
    m_pNowAction->usHitTime1 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usHitTime2 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usPlaySoundTime1 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usPlaySoundTime2 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usPlaySoundTime3 = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    m_pNowAction->usPlayParticleTime = *(unsigned short*)pData;
    pData += sizeof( unsigned short );
    strcpy( m_pNowAction->szSoundFile1, (char*)pData );
    pData += 50;
    strcpy( m_pNowAction->szSoundFile2, (char*)pData );
    pData += 50;
    strcpy( m_pNowAction->szSoundFile3, (char*)pData );
    pData += 50;
    strcpy( m_pNowAction->szParticleFile, (char*)pData );
    pData += 50;
    pData += 20;    // �w�d���

    // �s���`��
    unsigned short usComponentCount = *(unsigned short*)pData;
    pData += sizeof( unsigned short );

    bool bBackAdornment = false;

    // �إ߹s����
    for( unsigned short j=0; j<usComponentCount; ++j )
    {
        pRoleComponent = g_RoleMemoryMgr.GetComponent();
        pRoleComponent->bShow = false;
        m_pNowAction->ComponentList->push_back( pRoleComponent );

        // ���o���
        strcpy( pRoleComponent->szComponentName, (char*)pData );
        pData += 20;

        // �P�_�O�_���I���s��
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 863, CodeInlineText_Text ) );//�I��
        string str( "�I��" );
        if( strcmp( pRoleComponent->szComponentName, str.c_str() ) == 0 )
            bBackAdornment = true;
        else
            bBackAdornment = false;

        for( unsigned short k=0; k<8; ++k )
        {
            // ���o���
            pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
            pData += sizeof( unsigned short );

            // �e���`��
            unsigned short usFrameCount = *(unsigned short*)pData;
            pData += sizeof( unsigned short );

            // �إߵe�����
            for( unsigned short l=0; l<usFrameCount; ++l )
            {
                pFrame = g_RoleMemoryMgr.GetFrame();
				sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
				sFrameList& sfl = *scd.FrameList;
				sint nJCDebugCode_addr = (sint)scd.FrameList; 
				sfl.push_back(pFrame);
/*
                // ���o���
                //pFrame->usUnitTime = *(unsigned short*)pData;
                    pData += sizeof( unsigned short );
                    pFrame->usPlayTime = *(unsigned short*)pData;
                    pData += sizeof( unsigned short );
                    pFrame->shLayer = *(short*)pData;
                    pData += sizeof( short );
                    pFrame->vScale.z = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->vScale.x = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->vScale.y = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->vPosition = *(D3DXVECTOR3*)pData;
                    pData += sizeof( D3DXVECTOR3 );
                    pFrame->bTexInvert = *(bool*)pData;
                    pData += sizeof( bool );
                    //pData += 9;    // �w�d���
                    if( pHeader->usVersion > 1 )
                    {
                    	pFrame->fPercentX = *(float*)pData;
                        pData += sizeof( float );
                        pFrame->fPercentY = *(float*)pData;
                        pData += sizeof( float );
                        pData += 1;
                    }
                    else
                    {
                    	pData += 9;    // �w�d���
                    }
                    pFrame->pImage = NULL;
                    pFrame->Color = 0xFFFFFFFF;*/

					// ��s�e���x�}
                    //pFrame->vPosition = vPosition;
                    //pFrame->vScale = vScale;
                    //pFrame->RefreshMatrix( vScale, vPosition );
                    // ��s�e���x�}
                    //pFrame->RefreshMatrix( pFrame->vScale, pFrame->vPosition );

				pData += sizeof( unsigned short );
                pFrame->usPlayTime = *(unsigned short*)pData;
                pData += sizeof( unsigned short );
                pFrame->shLayer = *(short*)pData;
                pData += sizeof( short );
                pData += sizeof( float ); // ����

                pFrame->matLocal._11 = *(float*)pData;
                pData += sizeof( float ); // �Y��X
                pFrame->matLocal._12 = *(float*)pData;
                pData += sizeof( float ); // �Y��Y

                pFrame->matLocal._13 = *(float*)pData;
                pData += sizeof( float ); // ��mX
                pFrame->matLocal._14 = *(float*)pData;
                pData += sizeof( float ); // ��mY

                pData += sizeof( float ); // ��mZ
                pFrame->bTexInvert = *(bool*)pData;
                pData += sizeof( bool );

                if (pHeader->usVersion > 1)
                {
                    pFrame->fPercentX = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->fPercentY = *(float*)pData;
                    pData += sizeof( float );
                    pFrame->LinearFlag = *(BYTE*) pData;
                    pData += sizeof_BYTE;
                }
                else
                {
                    pData += 9;    // �w�d���
                }

                pFrame->pImage = NULL;
                pFrame->Color = 0xFFFFFFFF;

                // �I���s��
                if( bBackAdornment )
                    pFrame->matLocal._21 = 1.0f;
                else
                    pFrame->matLocal._21 = -1.0f;
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------
// ���J�s��
//----------------------------------------------------------------------------
bool DisplayRole::LoadComponent( const string& strFileName )
{
    string str;
    if( m_pNowAction == NULL )
        return false;

    if( strFileName == "Null" )
        return true;

#ifdef RESMGR

	 memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;


#else
    // Ū���ʸ���
    memPack* pMem = LoadFromPacket( strFileName );
    if( pMem == NULL )
        return false;

    // �x�s�ʸ��ɸ��
    BYTE* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );
 
#endif



    // Ū�����Y��
    sComHeader* pHeader = (sComHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->szFileID, "RDL" ) != 0 )
    {
        /* str.assign("RDL");
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//�ɮ׮榡���~
        str.append( ",LoadComponent" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//����
        Assert( 0, str.c_str() ); */
        return false;
    }

    pData += sizeof( sComHeader );

    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    char szTexFile[50];
    char szActName[20];

    // ��s�������ʧ@���
    strcpy( szActName, (char*)pData );
    pData += 20;

    // �P�_�ʧ@�W��
    if( strcmp( m_pNowAction->szActionName, szActName ) != 0 )
    {
        /* str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 864, CodeInlineText_Text ) );//�ɮפ��Ĥ@���������O�ݾ����ʧ@�W��
        Assert( 0, str.c_str() ); */
        return false;
    }

    // ���o�s�����
    pRoleComponent = m_pNowAction->GetComponent( pHeader->szComponentName1 );

    if( pRoleComponent == NULL )
    {
        /* str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 865, CodeInlineText_Text ) );//�䤣��P�ɮפ������ŦX���s��W��
        Assert( 0, str.c_str() ); */
        return false;
    }

    // ��ܹs��
    pRoleComponent->bShow = true;

    for( unsigned short k=0; k<8; ++k )
    {
        // �M���e��
        pRoleComponent->ComponentDirs[k]->RemoveAllFrame();

        // ���o���
        pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // �e���`��
        unsigned short usFrameCount = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // �إߵe�����
        for( unsigned short l=0; l<usFrameCount; ++l )
        {
            pFrame = g_RoleMemoryMgr.GetFrame();
			sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
			sFrameList& sfl = *scd.FrameList;
			sint nJCDebugCode_addr = (sint)scd.FrameList; 
			sfl.push_back(pFrame);

            // ���o���
            //pFrame->usUnitTime = *(unsigned short*)pData;
            pData += sizeof( unsigned short );
            pFrame->usPlayTime = *(unsigned short*)pData;
            pData += sizeof( unsigned short );
            pFrame->shLayer = *(short*)pData;
            pData += sizeof( short );
            pData += sizeof( float ); // ����

            pFrame->matLocal._11 = *(float*)pData;
            pData += sizeof( float ); // �Y��X
            pFrame->matLocal._12 = *(float*)pData;
            pData += sizeof( float ); // �Y��Y

            pFrame->matLocal._13 = *(float*)pData;
            pData += sizeof( float ); // ��mX
            pFrame->matLocal._14 = *(float*)pData;
            pData += sizeof( float ); // ��mY

            pData += sizeof( float ); // ��mZ
            pFrame->bTexInvert = *(bool*)pData;
            pData += sizeof( bool );
            pFrame->fWidth = *(float*)pData;
            pData += sizeof( float );
            pFrame->fHeight = *(float*)pData;
            pData += sizeof( float );
            pFrame->fTU1 = *(float*)pData;
            pData += sizeof( float );
            pFrame->fTV1 = *(float*)pData;
            pData += sizeof( float );
            pFrame->fTU2 = *(float*)pData;
            pData += sizeof( float );
            pFrame->fTV2 = *(float*)pData;
            pData += sizeof( float );
            strncpy( szTexFile, (char*)pData, 50 );
            pData += 50;
            if( pHeader->usVersion > 1 )
                pFrame->Color = *(DWORD*)pData;
            else
                pFrame->Color = 0xFFFFFFFF;
            pData += 20;    // �w�d���

            // Ū������
            if( szTexFile[0] == '\0' )
            {
                pFrame->pImage = NULL;
            }
            else
            {
                pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool DisplayRole::LoadImageEx(const char* pFileName)
{
    if( strcmp( pFileName, "Null" ) == 0 ) return true;


#ifdef RESMGR

	memPack* pMem = LoadFromPacket( pFileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;


#else
    static MemDataMgr MemDataCacheMgr;

    BYTE* pData = MemDataCacheMgr.GetMemData( pFileName );
    if( pData == NULL )
    {
        memPack* pMem = LoadFromPacket( pFileName );

        if( pMem == NULL ) return false;

        // �x�s�O������
        pData = MemDataCacheMgr.AddMemData( pFileName, pMem->Data, pMem->DataSize );

        if( pData == NULL ) return false;
    }
 
#endif



    // Ū�����Y��
    sComHeader* pHeader = (sComHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->szFileID, "RPC" ) != 0 )
    {
        // string str;
        //str.assign( "RPC" );
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//�ɮ׮榡���~
        //str.append( ",LoadImage" );
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//����
        //Assert( 0, str.c_str() );
        return false;
    }

    pData += sizeof_ComHeader;

    char szTexFile[50];
    char szActName[20];
	bool isLoading = false ; 

	sRoleAction *pAction = NULL ; 

	for(unsigned int i = 0 ; i < pHeader->usActCount1 ; ++i)
	{
		// ��s�������ʧ@���
		strcpy( szActName, (char*)pData );
		pData += 20;

		pAction = GetAction(szActName) ; 

		if(pAction == NULL)
		{

			for( unsigned short k=0; k<8; ++k )
			{
				unsigned short usFrameCount = *(unsigned short*)pData;
				pData += sizeof( unsigned short );

				for(unsigned int frame = 0 ; frame < usFrameCount ; frame++)
				{
					pData += sizeof( bool );
					pData += sizeof( float );
					pData += sizeof( float );
					pData += sizeof( float );
					pData += sizeof( float );
					pData += sizeof( float );
					pData += sizeof( float );
					pData += 50;


					if( pHeader->usVersion>1 )
					{
            			
						pData += sizeof( DWORD );
						pData += 6;
					}
					else
					{
        				pData += 10;    // �w�d���
					}


				}


					continue ;
				}
		}	
		else
		{
		
			sRoleComponent* pRoleComponent = pAction->GetComponent( pHeader->szComponentName1 );
			if( pRoleComponent == NULL ) return false;

			pRoleComponent->bShow = true;

			for( unsigned short k=0; k<8; ++k )
			{
				unsigned short usFrameCount = *(unsigned short*)pData;
				pData += sizeof( unsigned short );

				// �e���`�Ƥ��
				unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();

				if(usFrameCount != usRoleFrameCount )
				{
					return false ; 
				}

				// ��s�e�����
				sFrameList::iterator itFraEnd( pRoleComponent->ComponentDirs[k]->FrameList->end() );
				for( sFrameList::iterator itFra = pRoleComponent->ComponentDirs[k]->FrameList->begin(); itFra != itFraEnd; ++itFra )
				{
					sFrame* pFrame = (*itFra);
					pData += sizeof( bool );
					pFrame->fWidth = *(float*)pData;
					pData += sizeof( float );
					pFrame->fHeight = *(float*)pData;
					pData += sizeof( float );
					pFrame->fTU1 = *(float*)pData;
					pData += sizeof( float );
					pFrame->fTV1 = *(float*)pData;
					pData += sizeof( float );
					pFrame->fTU2 = *(float*)pData;
					pData += sizeof( float );
					pFrame->fTV2 = *(float*)pData;
					pData += sizeof( float );
					strncpy( szTexFile, (char*)pData, 50 );
					pData += 50;
					//pData += 10;    // �w�d���
					if( pHeader->usVersion>1 )
					{
            			pFrame->Color = *(DWORD*)pData;
						pData += sizeof( DWORD );
						pData += 6;
					}
					else
					{
            			pData += 10;    // �w�d���
					}

					// Ū������
					if( szTexFile[0] == '\0' )
					{
						pFrame->pImage = NULL;
					}
					else
					{
						pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
					}
					pFrame->RefreshMatrix( pFrame->vScale, pFrame->vPosition );
				}

			}

		}
	}
	return true;
}
//----------------------------------------------------------------------------
//�q�O��������˳�
//----------------------------------------------------------------------------
bool DisplayRole::LoadImage( const string& strFileName )
{
    //string str;
    if( m_pNowAction == NULL ) return false;
    if( strFileName == "Null" ) return true;

#ifdef RESMGR

	memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;

#else
    // Ū���ʸ���
    memPack* pMem = LoadFromPacket( strFileName );
    if( pMem == NULL ) return false;

    // �x�s�ʸ��ɸ��
    BYTE* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );

#endif



    // Ū�����Y��
    sComHeader* pHeader = (sComHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->szFileID, "RPC" ) != 0 )
    {
        // str.assign("RPC");
        // str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//�ɮ׮榡���~
        // str.append( ",LoadImage" );
        // str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//����
        // Assert( 0, str.c_str() );
        return false;
    }

    pData += sizeof( sComHeader );

    sRoleComponent* pRoleComponent = NULL;
    char szTexFile[50];
    char szActName[20];

    // ��s�������ʧ@���
    strcpy( szActName, (char*)pData );
    pData += 20;

    // �P�_�ʧ@�W��
    if( strcmp( m_pNowAction->szActionName, szActName ) != 0 )
    {
        // str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 864, CodeInlineText_Text ) );//�ɮפ��Ĥ@���������@�N�����ʧ@�W��
        // Assert( 0, str.c_str() );
        return false;
    }

    // ���o�s�����
    pRoleComponent = m_pNowAction->GetComponent( pHeader->szComponentName1 );

    if( pRoleComponent == NULL )
    {
        // str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 865, CodeInlineText_Text ) );//�䤣��P�ɮפ������ŦX���s��W��
        // Assert( 0, str.c_str() );
        return false;
    }

    // ��ܹs��
    pRoleComponent->bShow = true;

    for( unsigned short k=0; k<8; ++k )
    {
        unsigned short usFrameCount = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // �e���`�Ƥ��
        unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();
        if( usFrameCount != usRoleFrameCount )
        {
            // str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 866, CodeInlineText_Text ) );//�e���ƻP�ɮפ�����������
            // Assert( 0, str.c_str() );
            return false;
        }

        // ��s�e�����
        sFrameList::iterator itFraEnd( pRoleComponent->ComponentDirs[k]->FrameList->end() );
        for( sFrameList::iterator itFra = pRoleComponent->ComponentDirs[k]->FrameList->begin(); itFra != itFraEnd; ++itFra )
        {
            sFrame* pFrame = (*itFra);
            //(*itFra)->bTexInvert = *(bool*)pData;
            pData += sizeof( bool );
            pFrame->fWidth = *(float*)pData;
            pData += sizeof( float );
            pFrame->fHeight = *(float*)pData;
            pData += sizeof( float );
            pFrame->fTU1 = *(float*)pData;
            pData += sizeof( float );
            pFrame->fTV1 = *(float*)pData;
            pData += sizeof( float );
            pFrame->fTU2 = *(float*)pData;
            pData += sizeof( float );
            pFrame->fTV2 = *(float*)pData;
            pData += sizeof( float );
            strncpy( szTexFile, (char*)pData, 50 );
            pData += 50;
            //pData += 10;    // �w�d���
            if( pHeader->usVersion>1 )		//add test
            {
            	pFrame->Color = *(DWORD*)pData;
                pData += sizeof( DWORD );
                pData += 6;
            }
            else
            {
            	pData += 10;    // �w�d���
            }								//add test

            // Ū������
            if( szTexFile[0] == '\0' )
            {
                pFrame->pImage = NULL;
            }
            else
            {
                pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
            }
        }
    }

    return true;

}
//----------------------------------------------------------------------------
//�M�������ɮ�
//----------------------------------------------------------------------------
bool DisplayRole::UnLoadPart( const string& strFileName )
{
	if (strFileName.c_str() == NULL) return false; // ���~�I�ǤJ�ū���
	if (strlen(strFileName.c_str()) == 0) return false; // ���~�I�ǤJ�Ŧr��
    if (strcmp(strFileName.c_str(),"Null") == 0) return false; // �������]�w�������J����ϧ�

#ifdef RESMGR

    memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return false;

#else

	memPack* pMem = LoadFromPacket( strFileName.c_str() );
    if (pMem==NULL) return false;

    vector<char> Data( pMem->DataSize+1 );
    char* pText = &Data.front();
    memset( pText, 0, Data.size() );
    memcpy( pText, pMem->Data, pMem->DataSize );

#endif



#ifdef RESMGR


	//cosiann �h�@�ӬO�O��\n

	unsigned int allsize = pMem->DataSize + 1 ;


	//cosiann
	char *ptemp = new char[allsize] ; 
	//cosiann
	ZeroMemory(ptemp , sizeof(char) * allsize) ;
	//cosiann

	memcpy(ptemp , pMem->Data , pMem->DataSize) ;

	
    //char* pToken = strtok( pText, "\n\r" );
	//cosiann
	char* pToken = strtok( ptemp, "\n\r" );

    while( pToken )
    {
        this->UnLoadImageEx( pToken );
        pToken = strtok( 0, "\n\r" );
    }

	//cosiann 
	delete[] ptemp ;
	ptemp = NULL ;

    return true;

#else

    char* pToken = strtok( pText, "\n\r" );
    while( pToken )
    {
        this->UnLoadImageEx( pToken );
        pToken = strtok( 0, "\n\r" );
    }
	return true;
#endif	
}
//----------------------------------------------------------------------------
//���J�����ɮ�
//----------------------------------------------------------------------------
bool DisplayRole::LoadPart( const string& strFileName )
{
	if (strFileName.c_str() == NULL) return false; // ���~�I�ǤJ�ū���
	if (strlen(strFileName.c_str()) == 0) return false; // ���~�I�ǤJ�Ŧr��
    if (strcmp(strFileName.c_str(),"Null") == 0) return false; // �������]�w�������J����ϧ�

#ifdef RESMGR

    memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return false;

#else

	memPack* pMem = LoadFromPacket( strFileName.c_str() );
    if (pMem==NULL) return false;

    vector<char> Data( pMem->DataSize+1 );
    char* pText = &Data.front();
    memset( pText, 0, Data.size() );
    memcpy( pText, pMem->Data, pMem->DataSize );

#endif






#ifdef RESMGR


	//cosiann �h�@�ӬO�O��\n

	unsigned int allsize = pMem->DataSize + 1 ;


	//cosiann
	char *ptemp = new char[allsize] ; 
	//cosiann
	ZeroMemory(ptemp , sizeof(char) * allsize) ;
	//cosiann

	memcpy(ptemp , pMem->Data , pMem->DataSize) ;

	
    //char* pToken = strtok( pText, "\n\r" );
	//cosiann
	char* pToken = strtok( ptemp, "\n\r" );

    while( pToken )
    {
        this->LoadImageEx( pToken );
        pToken = strtok( 0, "\n\r" );
    }

	//cosiann 
	delete[] ptemp ;
	ptemp = NULL ;

    return true;

#else

    char* pToken = strtok( pText, "\n\r" );
    while( pToken )
    {
        //this->LoadImage( pToken );
        this->LoadImageEx( pToken );
        pToken = strtok( 0, "\n\r" );
    }

	return true;
#endif

 
}

//----------------------------------------------------------------------------
// ���ùs��
//----------------------------------------------------------------------------
void DisplayRole::HideComponent( const char* pComName )
{
    if( m_pNowAction == NULL ) return;

    // ���o�s�����
    sRoleComponent* pRoleComponent = m_pNowAction->GetComponent( pComName );

    if( pRoleComponent != NULL )
        pRoleComponent->bShow = false;
}
void DisplayRole::PushActionStack(const char *actName)
{
	//m_playStack.push(actName) ; 

}
//----------------------------------------------------------------------------
// ���ùs��
//----------------------------------------------------------------------------
void DisplayRole::HideComponents()
{
	m_pNowAction->HideComponents();
}

void DisplayRole::RenderPipeline()
{
	RenderRole() ; 


}

//----------------------------------------------------------------------------
//ø�s�i�ܨ����2�h
//----------------------------------------------------------------------------
void DisplayRole::RenderRole()
{

    if( this->BeginRender() )
    {
		
		while( !m_RenderFrames.empty() )
        {
			RenderFrame( m_RenderFrames.front() );
            m_RenderFrames.pop_front();

			if(m_pParticle != NULL)
			{


				D3DXMATRIX mat ; 
				D3DXMatrixIdentity(&mat) ; 

				GetParticleSystemPtr()->SetViewMatrix(mat);
				GetParticleSystemPtr()->SetInvViewMatrix(mat);



				D3DXMatrixScaling(&mat , m_disScale ,  m_disScale , 1.0f) ; 
				mat._41 = 0.0f ; 
				mat._42 = 0.0f ;
				mat._43 = 1.0f ; 


				m_pParticle->SetPosition(mat._41 , mat._42 , mat._43 ) ; 
				m_pParticle->SetScale(mat._11 , mat._22 , mat._33) ; 
				m_pParticle->RenderParticle(0.0f , m_matView , false ) ; 
			}
        }


		this->EndRender();
    }
}
//----------------------------------------------------------------------------
bool DisplayRole::GetRenderFrames(float fElapsedTime)
{

	static bool isRePlay = true ; 

	//if(m_playStack.empty() )
	//{
	//	m_pNowAction = GetAction("Stand") ; 

	//}
	//else
	//if(!m_playStack.empty() )
	//{
	//	if(isRePlay)
	//		m_pNowAction = GetAction(m_playStack.top()) ; 

	//	isRePlay = false ; 

	//}


    if( m_pNowAction == NULL ) 
	{
		return true;
	}

	m_usFrameMax = 0  ; 

	m_passTime += (fElapsedTime * 1000.0f) ; 

	DWORD updateTime = m_pNowAction->dwUpdateTime ; 

	m_usFrameMax = 0 ; 

    sRoleComponentList::iterator itComEnd( m_pNowAction->ComponentList->end() );
    for( sRoleComponentList::iterator j = m_pNowAction->ComponentList->begin(); j != itComEnd; ++j )
    {
        if( m_usFrameMax < (*j)->ComponentDirs[m_eShowDir]->usMaxFrameNum )
            m_usFrameMax = (*j)->ComponentDirs[m_eShowDir]->usMaxFrameNum;
    }

    while (m_passTime >= updateTime)
    {
        ++m_usFrameNum;
		if (m_usFrameNum > m_usFrameMax )
        {
           
            m_usFrameNum = 1;

			//if(!m_playStack.empty())
			//	m_playStack.pop() ;

			//isRePlay = true ; 
        
        }

        m_passTime = 0;
    }

	itCom	pos = m_pNowAction->ComponentList->begin();
    citCom	end = m_pNowAction->ComponentList->end();

    for( pos; pos != end; ++pos )
    {
        sRoleComponent* pCom = (*pos);							// �ˬd�O�_��ܹs��
		sComponentDir&	Dir	 = *(pCom->ComponentDirs[m_eShowDir]);	// �ˬd�e����C�O�_����
        
		if (!pCom->bShow || Dir.FrameList->empty()) 
		{
			continue;
		}
		
		
		sFrameList::reverse_iterator iter(Dir.FrameList->rbegin()) ; 

		for( ; iter != Dir.FrameList->rend() ; ++iter )
		{
			sFrame* pFrame = (*iter);  

			if (pFrame->usPlayTime == m_usFrameNum)
            {
				if (pFrame && pFrame->pImage && pFrame->pImage->pTexture)
				{
					m_RenderFrames.push_back( pFrame );

				}
				else
				{
					return false ; 
				}
			}

		}
    }

    // �s��ϼh�Ƨ�
    m_RenderFrames.sort( FrameSort );
	return true;
}

void DisplayRole::InitLockParts()
{
   if( m_pNowAction == NULL ) 
	{
		return ;
	}

    m_lockPartVec.clear() ; 


	itCom	pos = m_pNowAction->ComponentList->begin();
    citCom	end = m_pNowAction->ComponentList->end();

    for( pos; pos != end; ++pos )
    {
        sRoleComponent* pCom = (*pos);							// �ˬd�O�_��ܹs��
		sComponentDir&	Dir	 = *(pCom->ComponentDirs[m_eShowDir]);	// �ˬd�e����C�O�_����
        
		if (Dir.FrameList->empty()) 
		{
			continue;
		}
	
		// ����Ĥ@�i�e��
		sFrame* pFrame = Dir.FrameList->front();

		m_lockPartVec.push_back(pFrame );
	
	


    }


}


void DisplayRole::InitLockParts(const std::vector<std::string> &partVec) 
{
   if( m_pNowAction == NULL ) 
	{
		return ;
	}

	itCom	pos = m_pNowAction->ComponentList->begin();
    citCom	end = m_pNowAction->ComponentList->end();

    for( pos; pos != end; ++pos )
    {
        sRoleComponent* pCom = (*pos);							// �ˬd�O�_��ܹs��
		sComponentDir&	Dir	 = *(pCom->ComponentDirs[m_eShowDir]);	// �ˬd�e����C�O�_����
        
		if (Dir.FrameList->empty()) 
		{
			continue;
		}
		for(unsigned int i = 0 ; i < partVec.size() ; ++i)
		{
			if(strcmp(pCom->szComponentName , partVec[i].c_str()) == 0)
			{
				// ����Ĥ@�i�e��
				sFrame* pFrame = Dir.FrameList->front();

				m_lockPartVec.push_back(pFrame );


			}
		}


    }

}
void DisplayRole::SetViewPosition(float left , float top)
{
	SetPosition(D3DXVECTOR2(left , top)) ; 

}
void DisplayRole::SetViewSize(unsigned int width , unsigned int height)
{
	SetSize(width , height);
}

void DisplayRole::Update(float fElapsedTime)
{



    bool bLoadDone = GetRenderFrames(fElapsedTime);

	if( m_bDynamicRender == false && bLoadDone == false)
	{		
		m_RenderFrames.clear();
		return ;	
	}

	if(m_pParticle != NULL)
	{
		m_pParticle->FrameMoveParticle(fElapsedTime) ; 
	}
}


void DisplayRole::SetDisplayScale(float scale)
{
	if(m_lockPartVec.empty())
	{
		return ; 
	}

	m_disScale = scale ; 

/* ���b�ݭn�H�U�ʧ@

//	D3DXMATRIX matWorld;
//	D3DXMATRIX scaleMat ; 

//	D3DXMatrixScaling(&scaleMat , scale , scale , 1.0f) ;

	float minHeight = FLT_MAX ; 
	float maxHeight = -FLT_MAX ; 

	float minX = FLT_MAX;
	float maxX = -FLT_MAX;
	for(unsigned int i = 0 ; i < m_lockPartVec.size() ; ++i)
	{
		sFrame* pFrame = m_lockPartVec[i] ; 
		
//		D3DXMatrixMultiply( &matWorld, &pFrame->matLocal , &scaleMat );

		D3DXVECTOR3 v[4] ; 

		v[0] = D3DXVECTOR3( -pFrame->fWidth, 0.0f, 0.0f );
		v[1] = D3DXVECTOR3( pFrame->fWidth, 0.0f, 0.0f );
		v[2] = D3DXVECTOR3( -pFrame->fWidth, pFrame->fHeight, 0.0f );
		v[3] = D3DXVECTOR3( pFrame->fWidth, pFrame->fHeight, 0.0f );

		D3DXVec3TransformCoord(&v[0] , &v[0] , &pFrame->matLocal) ;
		D3DXVec3TransformCoord(&v[1] , &v[1] , &pFrame->matLocal) ;
		D3DXVec3TransformCoord(&v[2] , &v[2] , &pFrame->matLocal) ;
		D3DXVec3TransformCoord(&v[3] , &v[3] , &pFrame->matLocal) ;		

		for(int j=0; j<4; ++j)
		{
			if(minHeight > v[j].y)
				minHeight = v[j].y ; 
			
			if(maxHeight < v[j].y)
				maxHeight = v[j].y ; 

			if(minX > v[j].x)
				minX = v[j].x ; 
			
			if(maxX < v[j].x)
				maxX = v[j].x ; 
		}
	}

	//���o�nlockview��Frame����������
	m_frameCenter = minHeight + (maxHeight - minHeight) * 0.5f  ; 

//	m_boundRectMin.x = minX;
//	m_boundRectMin.y = minHeight;

//	m_boundRectMax.x = maxX;
//	m_boundRectMax.y = maxHeight;
*/
}


void DisplayRole::LockView()
{
	if(m_lockPartVec.empty())
	{
	    //m_pd3dDevice->GetTransform( D3DTS_VIEW, &s_matView );
		//m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

		return ; 
	}

//	m_viewOffset.x = m_ViewPort.Width * 0.5f ; 
	//camera������0�A�O�bviewPort��������m
//	m_viewOffset.y = m_ViewPort.Height * 0.5f - m_frameCenter ; 

//	m_matView._41 = m_viewOffset.x ; 
//	m_matView._42 = m_viewOffset.y ; 
   // m_pd3dDevice->GetTransform( D3DTS_VIEW, &s_matView );
	//m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );


}
void DisplayRole::UnLockView(void)
{
	m_matView._41 = 0.0f ; 
	m_matView._42 = 0.0f ; 

//	m_pd3dDevice->SetTransform( D3DTS_VIEW, &s_matView );

}

//----------------------------------------------------------------------------
//ø�s�i�ܨ����3�h ø�s�X�C���H��
//----------------------------------------------------------------------------
void DisplayRole::RenderFrame(sFrame* pFrame)
{

#ifdef _RENDER_3D

	

	D3DXMATRIX matWorld;
	D3DXMATRIX scaleMat ; 
	D3DXMATRIX tempMat ; 

	D3DXMatrixScaling(&scaleMat , -m_disScale, m_disScale, 1.0f) ; 
	
	D3DXMATRIX offset;
	::D3DXMatrixTranslation(&offset, 0, -1.0f*(m_ViewPort.Height/2.0f), 10);

	D3DXMatrixMultiply( &tempMat, &pFrame->matLocal, &scaleMat );
	D3DXMatrixMultiply( &matWorld, &tempMat , &offset );

	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_pd3dDevice->SetTexture( 0, pFrame->pImage->pTexture );

	DrawRole( pFrame, pFrame->Color );

#else
    m_pd3dDevice->SetTexture( 0, pFrame->pImage->pTexture );

    float fOffsetX = 0.0f;
    float fOffsetY = 0.0f;

    // �I���s��
    if( pFrame->matLocal._21 > 0.0f )
    { // �p��ø�ϰϰ쪺������
        fOffsetX = pFrame->matLocal._13 * pFrame->matLocal._11 * m_vScale.x * -12.0f;
        fOffsetY = pFrame->matLocal._14 * pFrame->matLocal._12 * m_vScale.y *  12.0f;
    }
    else
    { // �p��ø�ϰϰ쪺������
        fOffsetX = pFrame->matLocal._13 * pFrame->matLocal._11 * m_vScale.x * -19.0f;
        fOffsetY = pFrame->matLocal._14 * pFrame->matLocal._12 * m_vScale.y * -19.0f;
    }

    //�j�Y���ק�
    RECT rcRect;
    if(pFrame->shLayer==2||pFrame->shLayer==4)
    {
        rcRect.left   = (LONG)(-pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x);
        rcRect.top    = (LONG)(-pFrame->fHeight *  9.5f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y);
        rcRect.right  = (LONG)( pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x);
        rcRect.bottom = (LONG)( pFrame->fHeight * 14.5f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y);
    }
    else if( pFrame->shLayer==-3 )
    {
        rcRect.left   = (LONG)(-pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x);
        rcRect.top    = (LONG)(-pFrame->fHeight * 15.0f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y);
        rcRect.right  = (LONG)( pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x);
        rcRect.bottom = (LONG)( pFrame->fHeight *  9.0f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y);
    }
    else if( pFrame->shLayer== 3 ) //����
    {
        rcRect.left   = (LONG)(-pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x);
        rcRect.top    = (LONG)(-pFrame->fHeight *  9.5f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y);
        rcRect.right  = (LONG)( pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x);
        rcRect.bottom = (LONG)( pFrame->fHeight * 14.5f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y);
    }
    else
    { // �p��ø�ϰϰ�d��
        rcRect.left   = (LONG)(-pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x );
        rcRect.top    = (LONG)(-pFrame->fHeight * 12.0f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y );
        rcRect.right  = (LONG)( pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x );
        rcRect.bottom = (LONG)( pFrame->fHeight * 12.0f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y );
    }

    DrawDisplayRole( rcRect, pFrame );
#endif
}

//----------------------------------------------------------------------------
void DisplayRole::SetSize(DWORD Width, DWORD Height)
{
    m_ViewPort.Width = Width;
    m_ViewPort.Height = Height;

	SetPosition(m_vPosition) ; 
}
//----------------------------------------------------------------------------
// �]�w��m
//---------------------------------------------------------------------------
void DisplayRole::SetPosition( const D3DXVECTOR2& vPos )
{
    m_vPosition = vPos;

    m_ViewPort.X = (DWORD)vPos.x;
    m_ViewPort.Y = (DWORD)vPos.y;    


    // �]�w���f��m
 //   m_ViewPort.X = static_cast<DWORD>(m_vPosition.x)-m_ViewPort.Width/2;
 //   m_ViewPort.Y = static_cast<DWORD>(m_vPosition.y)-m_ViewPort.Height/2;    
}

//----------------------------------------------------------------------------
// ���o��m
//---------------------------------------------------------------------------
const D3DXVECTOR2& DisplayRole::GetPosition() const
{
    return m_vPosition;
}

//----------------------------------------------------------------------------
// ���o�Y��
//---------------------------------------------------------------------------
const D3DXVECTOR2& DisplayRole::GetScale() const
{
    return m_vScale;
}

//----------------------------------------------------------------------------
// �U�Ӥ�V
//----------------------------------------------------------------------------
void DisplayRole::NextDirection()
{
    int iDir = (int)m_eShowDir;
    if( ++iDir > 7 )
        iDir = 0;

    m_eShowDir = (RoleDirect)iDir;
}

//----------------------------------------------------------------------------
// �W�Ӥ�V
//----------------------------------------------------------------------------
void DisplayRole::LastDirection()
{
    int iDir = (int)m_eShowDir;
    if( --iDir < 0 )
        iDir = 7;

    m_eShowDir = (RoleDirect)iDir;
}
//----------------------------------------------------------------------------
bool DisplayRole::BeginRender()
{

   // m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

    // �]�w Viewport
    //D3DVIEWPORT9 currViewPort;
   // m_pd3dDevice->GetViewport( &s_ViewPort );
   // m_pd3dDevice->SetViewport( &m_ViewPort );
	//m_pd3dDevice->Clear(0 , NULL , D3DCLEAR_TARGET , 0x00000000 , 1.0f , 0) ; 

    // �]�w��v�x�}
    //D3DXMATRIX matCurrProj;
   // m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &s_matProj );
   // m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // �]�w��v���x�}
   // D3DXMATRIX matCurrView ; 
	//D3DXMatrixIdentity(&matCurrView) ; 

	LockView() ; 

	// �ץ�viewport�H��projection matrix
	// : �w��viewport�W�X�ù����ץ��B�z

	::D3DVIEWPORT9 refineViewPort = m_ViewPort;

	int shiftLeft = (((int)refineViewPort.X) <0)?-(int)refineViewPort.X:0;
	int diffHeight = g_RenderSystem.GetBackBufferHeight() - (refineViewPort.Y+refineViewPort.Height);
	int shiftBottom = (diffHeight<0)?-diffHeight:0;

	if(shiftLeft>0)
		refineViewPort.X = 0;
	refineViewPort.Width -= shiftLeft;
	refineViewPort.Height -= shiftBottom;

	float hWidth = m_ViewPort.Width/2.0f;
	float hHeight = m_ViewPort.Height/2.0f;
	D3DXMatrixOrthoOffCenterLH(&m_matProj, -hWidth+shiftLeft, hWidth, -hHeight+shiftBottom, hHeight, 0.0f , 100.0f) ;

	BeginRender2D(m_pd3dDevice , s_ViewPort , s_matProj , s_matView ,  refineViewPort , m_matProj , m_matView) ; 

	// ø�s�f�ɰw��V���T����
    m_pd3dDevice->GetRenderState( D3DRS_CULLMODE, &m_oldCullState);
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

//	m_pd3dDevice->Clear(0 , NULL , D3DCLEAR_TARGET , 0x00000000 , 1.0f , 0) ; 

    return true;
}
//----------------------------------------------------------------------------
bool DisplayRole::EndRender()
{
    // �٭���v���x�}
 //   m_pd3dDevice->SetTransform( D3DTS_VIEW, &s_matView );
	UnLockView() ; 

	EndRender2D(m_pd3dDevice , s_ViewPort , s_matProj , s_matView) ; 

    // �٭��v�x�}
   // m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &s_matProj );
    // �٭� Viewport
   // m_pd3dDevice->SetViewport( &s_ViewPort );

	D3DXMATRIX mat ; 

	D3DXMatrixIdentity(&mat) ; 

	m_pd3dDevice->SetTransform(D3DTS_WORLD , &mat) ; 

	m_pd3dDevice->SetTexture(0 , NULL) ; 

   // m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, m_oldCullState );

    return true;
}
//----------------------------------------------------------------------------
void DisplayRole::UnLoadImageEx( const char* pFilename )
{
if( strcmp( pFilename, "Null" ) == 0 ) return;


#ifdef RESMGR

	memPack* pMem = LoadFromPacket( pFilename );
    
    if( pMem->Data == NULL ) return;
        

    BYTE* pData = pMem->Data;


#else
    static MemDataMgr MemDataCacheMgr;

    BYTE* pData = MemDataCacheMgr.GetMemData( pFileName );
    if( pData == NULL )
    {
        memPack* pMem = LoadFromPacket( pFileName );

        if( pMem == NULL ) return;

        // �x�s�O������
        pData = MemDataCacheMgr.AddMemData( pFileName, pMem->Data, pMem->DataSize );

        if( pData == NULL ) return;
    }
 
#endif



    // Ū�����Y��
    sComHeader* pHeader = (sComHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->szFileID, "RPC" ) != 0 )
    {
        // string str;
        //str.assign( "RPC" );
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//�ɮ׮榡���~
        //str.append( ",LoadImage" );
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//����
        //Assert( 0, str.c_str() );
        return;
    }

    pData += sizeof_ComHeader;

    char szTexFile[50];
    char szActName[20];
	bool isLoading = false ; 

	sRoleAction *pAction = NULL ; 

	for(unsigned int i = 0 ; i < pHeader->usActCount1 ; ++i)
	{
		// ��s�������ʧ@���
		strcpy( szActName, (char*)pData );
		pData += 20;

		pAction = GetAction(szActName) ; 

		if( pAction )
		{
		
			sRoleComponent* pRoleComponent = pAction->GetComponent( pHeader->szComponentName1 );
			if( pRoleComponent == NULL ) return;

			pRoleComponent->bShow = false;

			for( unsigned short k=0; k<8; ++k )
			{
				unsigned short usFrameCount = *(unsigned short*)pData;
				pData += sizeof( unsigned short );

				// �e���`�Ƥ��
				unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();

				if(usFrameCount != usRoleFrameCount )
				{
					return; 
				}

				// ��s�e�����
				sFrameList::iterator itFraEnd( pRoleComponent->ComponentDirs[k]->FrameList->end() );
				for( sFrameList::iterator itFra = pRoleComponent->ComponentDirs[k]->FrameList->begin(); itFra != itFraEnd; ++itFra )
				{
					sFrame* pFrame = (*itFra);
					pData += sizeof( bool );
					pFrame->fWidth = *(float*)pData;
					pData += sizeof( float );
					pFrame->fHeight = *(float*)pData;
					pData += sizeof( float );
					pFrame->fTU1 = *(float*)pData;
					pData += sizeof( float );
					pFrame->fTV1 = *(float*)pData;
					pData += sizeof( float );
					pFrame->fTU2 = *(float*)pData;
					pData += sizeof( float );
					pFrame->fTV2 = *(float*)pData;
					pData += sizeof( float );
					strncpy( szTexFile, (char*)pData, 50 );
					pData += 50;
					//pData += 10;    // �w�d���
					if( pHeader->usVersion>1 )
					{
            			pFrame->Color = *(DWORD*)pData;
						pData += sizeof( DWORD );
						pData += 6;
					}
					else
					{
            			pData += 10;    // �w�d���
					}

					// Ū������
					if( szTexFile[0] == '\0' )
					{
						pFrame->pImage = NULL;
					}
					else
					{
						pFrame->pImage = NULL;
					}
				}

			}

		}
	}
	return;
}
//----------------------------------------------------------------------------
void DisplayRole::UnLoadImage( const char* pFilename )
{
#ifdef RESMGR

	sRoleComponent* pRoleComponent;

    memPack* pMem = LoadFromPacket( pFilename );

	BYTE* pData = pMem->Data;
    
    if( pData )
    {
        // Ū�����Y��
        sComHeader* pHeader = (sComHeader*)pData;        
        // �T�{�ɮסI
        if( stricmp( pHeader->szFileID, "RPC" ) != 0 ) return;
        // �M�����
		if( m_pNowAction == NULL )
			return;

		pRoleComponent = m_pNowAction->GetComponent( pHeader->szComponentName1 );
		UnLoadComponentTexture( pRoleComponent );

		pRoleComponent = m_pNowAction->GetComponent( pHeader->szComponentName2 );
		UnLoadComponentTexture( pRoleComponent );
    }

#else

	sRoleComponent* pRoleComponent;

    memPack* pMem = LoadFromPacket( pFilename );
    
	if( pMem )
    {
        // Ū�����Y��
        sComHeader* pHeader = (sComHeader*)pMem->Data;        
        // �T�{�ɮסI
        if( stricmp( pHeader->szFileID, "RPC" ) != 0 ) 
			return;
        // �M�����
		if( m_pNowAction == NULL )
			return;

		sRoleComponent* pRoleComponent = m_pNowAction->GetComponent( pHeader->szComponentName1 );
        
		pRoleComponent = m_pNowAction->GetComponent( pHeader->szComponentName1 );
		UnLoadComponentTexture( pRoleComponent );

		pRoleComponent = m_pNowAction->GetComponent( pHeader->szComponentName2 );
		UnLoadComponentTexture( pRoleComponent );
    }

#endif
}
//----------------------------------------------------------------------------
void DisplayRole::UnLoadComponentTexture( sRoleComponent* pRoleComponent )
{
	if( pRoleComponent == NULL )
			return;

	for( unsigned short i = 0 ; i < 8 ; i++ )
	{
		sFrameList::iterator itFraEnd( pRoleComponent->ComponentDirs[i]->FrameList->end() );
		
		for( sFrameList::iterator itFra = pRoleComponent->ComponentDirs[i]->FrameList->begin() ; itFra != itFraEnd ; itFra++ )
		{
			sFrame* pFrame = (*itFra);
			pFrame->pImage = NULL;
		}
	}

	pRoleComponent->bShow = false;
}
//----------------------------------------------------------------------------
void DisplayRole::HideImage(const char* pFilename)
{
#ifdef RESMGR

    memPack* pMem = LoadFromPacket( pFilename );

	BYTE* pData = pMem->Data;

    
    if (pData)
    {
        // Ū�����Y��
        sComHeader* pHeader = (sComHeader*)pData;        
        // �T�{�ɮסI
        if( stricmp( pHeader->szFileID, "RPC" ) != 0 ) return;
        // ���ùs��    
        pData += sizeof_ComHeader;

		char szTexFile[50];
		char szActName[20];
		bool isLoading = false ; 

		sRoleAction *pAction = NULL ; 

		for(unsigned int i = 0 ; i < pHeader->usActCount1 ; ++i)
		{
			// ��s�������ʧ@���
			strcpy( szActName, (char*)pData );
			pData += 20;

			pAction = GetAction(szActName) ; 

			if( pAction )
			{
			
				sRoleComponent* pRoleComponent = pAction->GetComponent( pHeader->szComponentName1 );
				if( pRoleComponent == NULL ) return;

				pRoleComponent->bShow = false;

				for( unsigned short k=0; k<8; ++k )
				{
					unsigned short usFrameCount = *(unsigned short*)pData;
					pData += sizeof( unsigned short );

					// �e���`�Ƥ��
					unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();

					if(usFrameCount != usRoleFrameCount )
					{
						return; 
					}

					// ��s�e�����
					sFrameList::iterator itFraEnd( pRoleComponent->ComponentDirs[k]->FrameList->end() );
					for( sFrameList::iterator itFra = pRoleComponent->ComponentDirs[k]->FrameList->begin(); itFra != itFraEnd; ++itFra )
					{
						sFrame* pFrame = (*itFra);
						pData += sizeof( bool );
						pFrame->fWidth = *(float*)pData;
						pData += sizeof( float );
						pFrame->fHeight = *(float*)pData;
						pData += sizeof( float );
						pFrame->fTU1 = *(float*)pData;
						pData += sizeof( float );
						pFrame->fTV1 = *(float*)pData;
						pData += sizeof( float );
						pFrame->fTU2 = *(float*)pData;
						pData += sizeof( float );
						pFrame->fTV2 = *(float*)pData;
						pData += sizeof( float );
						strncpy( szTexFile, (char*)pData, 50 );
						pData += 50;
						//pData += 10;    // �w�d���
						if( pHeader->usVersion>1 )
						{
            				pFrame->Color = *(DWORD*)pData;
							pData += sizeof( DWORD );
							pData += 6;
						}
						else
						{
            				pData += 10;    // �w�d���
						}

						// Ū������
						if( szTexFile[0] == '\0' )
						{
							pFrame->pImage = NULL;
						}
						else
						{
							pFrame->pImage = NULL;
						}
					}

				}

			}
		}
    }

#else

    memPack* pMem = LoadFromPacket( pFilename );
    if (pMem)
    {
        // Ū�����Y��
        sComHeader* pHeader = (sComHeader*)pMem->Data;        
        // �T�{�ɮסI
        if( stricmp( pHeader->szFileID, "RPC" ) != 0 ) return;
        // ���ùs��    
        pData += sizeof_ComHeader;

		char szTexFile[50];
		char szActName[20];
		bool isLoading = false ; 

		sRoleAction *pAction = NULL ; 

		for(unsigned int i = 0 ; i < pHeader->usActCount1 ; ++i)
		{
			// ��s�������ʧ@���
			strcpy( szActName, (char*)pData );
			pData += 20;

			pAction = GetAction(szActName) ; 

			if( pAction )
			{
			
				sRoleComponent* pRoleComponent = pAction->GetComponent( pHeader->szComponentName1 );
				if( pRoleComponent == NULL ) return;

				pRoleComponent->bShow = false;

				for( unsigned short k=0; k<8; ++k )
				{
					unsigned short usFrameCount = *(unsigned short*)pData;
					pData += sizeof( unsigned short );

					// �e���`�Ƥ��
					unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();

					if(usFrameCount != usRoleFrameCount )
					{
						return; 
					}

					// ��s�e�����
					sFrameList::iterator itFraEnd( pRoleComponent->ComponentDirs[k]->FrameList->end() );
					for( sFrameList::iterator itFra = pRoleComponent->ComponentDirs[k]->FrameList->begin(); itFra != itFraEnd; ++itFra )
					{
						sFrame* pFrame = (*itFra);
						pData += sizeof( bool );
						pFrame->fWidth = *(float*)pData;
						pData += sizeof( float );
						pFrame->fHeight = *(float*)pData;
						pData += sizeof( float );
						pFrame->fTU1 = *(float*)pData;
						pData += sizeof( float );
						pFrame->fTV1 = *(float*)pData;
						pData += sizeof( float );
						pFrame->fTU2 = *(float*)pData;
						pData += sizeof( float );
						pFrame->fTV2 = *(float*)pData;
						pData += sizeof( float );
						strncpy( szTexFile, (char*)pData, 50 );
						pData += 50;
						//pData += 10;    // �w�d���
						if( pHeader->usVersion>1 )
						{
            				pFrame->Color = *(DWORD*)pData;
							pData += sizeof( DWORD );
							pData += 6;
						}
						else
						{
            				pData += 10;    // �w�d���
						}

						// Ū������
						if( szTexFile[0] == '\0' )
						{
							pFrame->pImage = NULL;
						}
						else
						{
							pFrame->pImage = NULL;
						}
					}

				}

			}
		}
    }

#endif
}
//----------------------------------------------------------------------------
bool DisplayRole::HidePart(const char* pFilename)
{
    if (pFilename==NULL) 
	{
		return false; // ���~�I�ǤJ�ū���
	}
    if (strlen(pFilename)==0) 
	{
		return false; // ���~�I�ǤJ�Ŧr��
	}
    if (strcmp(pFilename,"Null")==0) 
	{
		return false; // �������]�w�������J����ϧ�   
	}


#ifdef RESMGR

	char* pText = NULL ;

    memPack* pMem = LoadFromPacket( pFilename );
    
    if( pMem->Data == NULL ) 
	{
		return false;
	}

#else
    memPack* pMem = LoadFromPacket( pFilename );
    if (pMem==NULL) return false;
    vector<char> Data( pMem->DataSize );
    char* pText = &Data.front();
    memcpy( pText, pMem->Data, pMem->DataSize );


#endif


#ifdef RESMGR

	//cosiann �h�@�ӬO�O��\n
	unsigned int allsize = pMem->DataSize + 1  ;

	//cosiann
	char *ptemp = new char[allsize] ; 
	//cosiann
	ZeroMemory(ptemp , sizeof(char) * allsize) ;
	//cosiann

	memcpy(ptemp , pMem->Data , pMem->DataSize ) ;

	
    //char* pToken = strtok( pText, "\n\r" );
	//cosiann
	char* pToken = strtok( ptemp, "\n\r" );

    while( pToken )
    {
        this->HideImage( pToken );        
        pToken = strtok( 0, "\n\r" );
    }

	//cosiann
	delete[] ptemp ;
	ptemp = NULL ;

    return true;

#else
    char* pToken = strtok( pText, "\n\r" );
    while( pToken )
    {
        this->HideImage( pToken );        
        pToken = strtok( 0, "\n\r" );
    }

    return true;
#endif

}

// enum BinEQPartList
// {
// 	//��ƪ�̪����Ǹ�ser�ʥ]���@��
// 	WEAPON = 1,	// �k��(���˪Z��)
// 	SHIELD,		// ����(���ˬ�)
// 	TWO_HAND,	// ����Z��
// 	HELMET,		// �Y��
// 	ARMOR,		// �W��
// 	PANTS,		// �@�L(�U��)
// 	GLOVE,		// ��M
// 	SHOES,		// �c
// 	FINGER1,	// ���~1
// 	FINGER2,	// ���~2(�٫�)
// 	CLOAK,		// �ܭ�
// 	ADOM,		// ����
// 	PET,		// ���F
// 
// 	EQPART_MAX	//�`�ƶq
// };

//////////////////////////////////////////////////////////////////////////
//���o���w���쪺���
//////////////////////////////////////////////////////////////////////////
int DisplayRole::GetDesignatePartItemID(int part)
{
	return PDArray[part].nItemID;
}

//////////////////////////////////////////////////////////////////////////
//�]�w���w���쪺���
//////////////////////////////////////////////////////////////////////////
void DisplayRole::SetDesignatePartItemID(int part, int itemID)
{
	PDArray[part].nItemID = itemID;
}
////----------------------------------------------------------
////�x�s�˳Ƴ��쪺�D��id
////----------------------------------------------------------
//void DisplayRole::SetEQItemID(int EQpartNum, int itemID)
//{
//	switch(EQpartNum)
//	{
//	case WEAPON:
//		SetWeaponID(itemID);
//		break;
//	case SHIELD:
//		SetShieldID(itemID);	
//		break;
//	case TWO_HAND:
//		break;
//	case HELMET:
//		SetHelmetID(itemID);	
//		break;
//	case ARMOR:
//		SetArmorID(itemID);
//		break;
//	case PANTS:
//		SetPantsID(itemID);
//		break;
//	case GLOVE:
//		SetGloveID(itemID);
//		break;
//	case SHOES:
//		SetShoesID(itemID);
//		break;
//	case FINGER1:
//		break;
//	case FINGER2:
//		break;
//	case CLOAK:		
//		SetCloakID(itemID);
//		break;
//	case ADOM:		
//		SetAdomID(itemID);
//		break;
//	case PET:
//		break;
//	}
//}

////----------------------------------------------------------
////�ǤJ�˳Ƴ��� ���o�˳Ƴ��쪺�D��id
////----------------------------------------------------------
//const int DisplayRole::GetItemID(const int EQpartNum)
//{
//	switch(EQpartNum)
//	{
//	case WEAPON:
//		return GetWeaponID();
//	case SHIELD:
//		return GetShieldID();	
//	case TWO_HAND:
//		break;
//	case HELMET:
//		return GetHelmetID();	
//	case ARMOR:
//		return GetArmorID();
//	case PANTS:
//		return GetPantsID();
//	case GLOVE:
//		return GetGloveID();
//	case SHOES:
//		return GetShoesID();
//	case FINGER1:
//		break;
//	case FINGER2:
//		break;
//	case CLOAK:		
//		return GetCloakID();
//	case ADOM:		
//		return GetAdomID();
//	case PET:
//		break;
//	}
//	return 0;
//}


void DisplayRole::ClearDisplayRoleAllInfo()
{
	for(int i = nHelmet; i < EQPART_MAX; ++ i)
	{
		PDArray[i].nItemID = -1;
	}
// 	SetHelmetID(0);	
// 	SetAdomID(0);	
// 	SetArmorID(0);	
// 	SetPantsID(0);	
// 	SetWeaponID(0);	
// 	SetCloakID(0);	
// 	SetGloveID(0);	
// 	SetShoesID(0);	
}



//----------------------------------------------------------------------------
} // namespace FDO
//----------------------------------------------------------------------------