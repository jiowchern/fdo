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
#pragma message("管理模式啟動") 
#else
#pragma message("一般模式啟動") 
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

float CNumber::s_fixCount = 1.0f ; //一秒
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
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), // 攝影機位置
        &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), // 注視點
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
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), // 攝影機位置
        &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), // 注視點
        &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ); // up vector

    D3DXMATRIX matBillboard;
    D3DXMatrixLookAtLH( &matBillboard,
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
        &D3DXVECTOR3( 0.0f, 0.0f, -1.0f ),
        &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    D3DXMatrixInverse(&matBillboard, NULL, &matBillboard); // 逆矩陣(反矩陣)

    // 設定物件位置
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
//移除裝備動作
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
// 載入整個角色
//----------------------------------------------------------------------------
bool DisplayRole::LoadRoleAll( const string& strFileName )
{

#ifdef RESMGR

    memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;


	string str;

#else
    // 讀取封裝檔
    memPack* pMem = LoadFromPacket( strFileName );
    if( pMem == NULL ) return false;

    string str;

    // 儲存封裝檔資料
    BYTE* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );

#endif




    // 讀取標頭檔
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "ALL" ) != 0 )
    {
        /* str.assign("ALL");
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//檔案格式錯誤
        str.append( ",LoadRoleAll" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        Assert( 0, str.c_str() ); */
        return false;
    }

    pData += sizeof( sRoleHeader );

    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    D3DXVECTOR3 vScale, vPosition;
    char szTexFile[50];

    // 釋放資源
    RemoveAllAction();

	

    // 建立待機動作資料
	if(m_pNowAction)
		g_RoleMemoryMgr.FreeAction(m_pNowAction);
    m_pNowAction = g_RoleMemoryMgr.GetAction();

    // 取得資料
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
    strcpy( m_pNowAction->szSoundFile1, (char*)pData );    //音效 ***
    pData += 50;
    strcpy( m_pNowAction->szSoundFile2, (char*)pData );   //音效 ***
    pData += 50;
    strcpy( m_pNowAction->szSoundFile3, (char*)pData );   //音效 ***
    pData += 50;
    strcpy( m_pNowAction->szParticleFile, (char*)pData );  //音效 ***
    pData += 50;
    pData += 20;    // 預留欄位

    // 零件總數
    unsigned short usComponentCount = *(unsigned short*)pData;
    pData += sizeof( unsigned short );

    bool bBackAdornment = false;

    // 建立零件資料
    for( unsigned short j=0; j<usComponentCount; ++j )
    {
        pRoleComponent = g_RoleMemoryMgr.GetComponent();
        pRoleComponent->bShow = true;
        m_pNowAction->ComponentList->push_back( pRoleComponent );

        // 取得資料
        strcpy( pRoleComponent->szComponentName, (char*)pData );
        pData += 20;

        // 判斷是否為背飾零件
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 863, CodeInlineText_Text ) );//背飾
        str.assign( "背飾" );
        if( strcmp( pRoleComponent->szComponentName, str.c_str() ) == 0 )
            bBackAdornment = true;
        else
            bBackAdornment = false;

        for( unsigned short k=0; k<8; ++k )
        {
            // 取得資料
            pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
            pData += sizeof( unsigned short );

            // 畫面總數
            unsigned short usFrameCount = *(unsigned short*)pData;
            pData += sizeof( unsigned short );

            // 建立畫面資料
            for( unsigned short l=0; l<usFrameCount; ++l )
            {
                pFrame = g_RoleMemoryMgr.GetFrame();
				sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
				sFrameList& sfl = *scd.FrameList;
				sint nJCDebugCode_addr = (sint)scd.FrameList; 
				sfl.push_back(pFrame);

                // 取得資料
                //pFrame->usUnitTime = *(unsigned short*)pData;
                pData += sizeof( unsigned short );
                pFrame->usPlayTime = *(unsigned short*)pData;
                pData += sizeof( unsigned short );
                pFrame->shLayer = *(short*)pData;
                pData += sizeof( short );
                pData += sizeof( float ); // 自轉

                pFrame->matLocal._11 = *(float*)pData;
                pData += sizeof( float ); // 縮放X
                pFrame->matLocal._12 = *(float*)pData;
                pData += sizeof( float ); // 縮放Y

                pFrame->matLocal._13 = *(float*)pData;
                pData += sizeof( float ); // 位置X
                pFrame->matLocal._14 = *(float*)pData;
                pData += sizeof( float ); // 位置Y

                pData += sizeof( float ); // 位置Z
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

                // 讀取中心點資訊
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

                // 線性參數
                pFrame->LinearFlag = *(DWORD*) pData;
                pData += sizeof_DWORD;
                // 預留欄位
                pData += 4;

                // 讀取材質
                if( szTexFile[0] == '\0' )
                {
                    pFrame->pImage = NULL;
                }
                else
                {
                    pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
                }

                // 背飾零件
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
  // 讀取封裝檔
    memPack* pMem = LoadFromPacket( strFileName );
	
	if( pMem->Data == NULL )
        return false;

    BYTE* pData = pMem->Data;

    // 讀取標頭檔
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // 確認檔案
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

    //// 建立動作資料
    for( unsigned short i=0; i<pHeader->usActCount; ++i )
    {
        pAction = g_RoleMemoryMgr.GetAction();
      //  pRoleRes->mActionList.push_back( pAction );

        // 取得資料
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
        strcpy( pAction->szSoundFile1, (char*)pData ); //音效 ***
        pData += 50;
        strcpy( pAction->szSoundFile2, (char*)pData );   //音效 ***
        pData += 50;
        strcpy( pAction->szSoundFile3, (char*)pData );   //音效 ***
        pData += 50;
        strcpy( pAction->szParticleFile, (char*)pData );  //音效 ***
        pData += 50;
        pData += 20;    // 預留欄位

        // 零件總數
        unsigned short usComponentCount = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // 建立零件資料
        for( unsigned short j=0; j<usComponentCount; ++j )
        {
            pRoleComponent = g_RoleMemoryMgr.GetComponent();
            pAction->ComponentList->push_back( pRoleComponent );

            // 取得資料
            strcpy( pRoleComponent->szComponentName, (char*)pData );
            pData += 20;

            for( unsigned short k=0; k<8; ++k )
            {
                // 取得資料
                pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
                pData += sizeof( unsigned short );

                // 畫面總數
                unsigned short usFrameCount = *(unsigned short*)pData;
                pData += sizeof( unsigned short );

                // 建立畫面資料
                for( unsigned short l=0; l<usFrameCount; ++l )
                {
                    pFrame = g_RoleMemoryMgr.GetFrame();                    

					sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
					sFrameList& sfl = *scd.FrameList;
					sint nJCDebugCode_addr = (sint)scd.FrameList; 
					sfl.push_back(pFrame);


                    // 取得資料
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

                    // 讀取中心點資訊
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

                    // 線性參數
                    pFrame->LinearFlag = *(DWORD*) pData;
                    pData += sizeof_DWORD;
                    // 預留欄位
					pData += 4;

                    // 讀取材質
                    if( szTexFile[0] == '\0' )
                        pFrame->pImage = NULL;
                    else
                        pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );

                    // 更新畫面矩陣
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
   // 讀取封裝檔
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

    // 讀取標頭檔
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "ALL" ) != 0 )
    {
  //      string str;
  //      str.assign( "ALL" );
  //      str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//檔案格式錯誤
  //      str.append( ",LoadRoleAll" );
  //      str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
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

    //// 建立動作資料
    //for( unsigned short i=0; i<pHeader->usActCount; ++i )
    //{
    //    pAction = g_RoleMemoryMgr.GetAction();
    //    pRoleRes->mActionList.push_back( pAction );

        // 取得資料
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
        strcpy( pAction->szSoundFile1, (char*)pData ); //音效 ***
        pData += 50;
        strcpy( pAction->szSoundFile2, (char*)pData );   //音效 ***
        pData += 50;
        strcpy( pAction->szSoundFile3, (char*)pData );   //音效 ***
        pData += 50;
        strcpy( pAction->szParticleFile, (char*)pData );  //音效 ***
        pData += 50;
        pData += 20;    // 預留欄位

        // 零件總數
        unsigned short usComponentCount = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // 建立零件資料
        for( unsigned short j=0; j<usComponentCount; ++j )
        {
            pRoleComponent = g_RoleMemoryMgr.GetComponent();
            pAction->ComponentList->push_back( pRoleComponent );

            // 取得資料
            strcpy( pRoleComponent->szComponentName, (char*)pData );
            pData += 20;

            for( unsigned short k=0; k<8; ++k )
            {
                // 取得資料
                pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
                pData += sizeof( unsigned short );

                // 畫面總數
                unsigned short usFrameCount = *(unsigned short*)pData;
                pData += sizeof( unsigned short );

                // 建立畫面資料
                for( unsigned short l=0; l<usFrameCount; ++l )
                {
                    pFrame = g_RoleMemoryMgr.GetFrame();
					sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
					sFrameList& sfl = *scd.FrameList;
					sint nJCDebugCode_addr = (sint)scd.FrameList; 
					sfl.push_back(pFrame);

                    // 取得資料
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

                    // 讀取中心點資訊
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

                    // 線性參數
                    pFrame->LinearFlag = *(DWORD*) pData;
                    pData += sizeof_DWORD;

                    // 預留欄位
					pData += 4;

                    // 讀取材質
                    if( szTexFile[0] == '\0' )
                        pFrame->pImage = NULL;
                    else
                        pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );

                    // 更新畫面矩陣
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
//從記憶體載入人物動作
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

    // 讀取標頭檔
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "RAT" ) != 0 ) return false;

    pData += sizeof_RoleHeader;

    //sRoleComponent* pRoleComponent = NULL;
    //sFrame* pFrame = NULL;

    // 釋放資源
    RemoveAllAction();

	sRoleAction *pAction = NULL ; 

	//char tempActName[20] ; 

    // 建立動作資料
    for( unsigned short i=0; i< pHeader->usActCount; ++i )
    {

		pAction = g_RoleMemoryMgr.GetAction();

		m_actionVec.push_back( pAction );

		// 取得資料
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
		pData += 20;    // 預留欄位

		// 零件總數
		unsigned short usComponentCount = *(unsigned short*)pData;
		pData += sizeof( unsigned short );

		bool bBackAdornment = false;

		// 建立零件資料
		for( unsigned short j=0; j<usComponentCount; ++j )
		{
			sRoleComponent* pCom = g_RoleMemoryMgr.GetComponent();
			pCom->bShow = false;
			pAction->ComponentList->push_back( pCom );

			// 取得資料
			strcpy( pCom->szComponentName, (char*)pData );
			pData += 20;

			// 判斷是否為背飾零件
			//str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 863, CodeInlineText_Text ) );//背飾
			string str( "背飾" );
			if( strcmp( pCom->szComponentName, str.c_str() ) == 0 )
				bBackAdornment = true;
			else
				bBackAdornment = false;

			for( unsigned short k=0; k<8; ++k )
			{
				// 取得資料
				sComponentDir& Dir = *(pCom->ComponentDirs[k]);
				Dir.usMaxFrameNum = *(unsigned short*)pData;
				pData += sizeof( unsigned short );

				// 畫面總數
				unsigned short usFrameCount = *(unsigned short*)pData;
				pData += sizeof( unsigned short );

				// 建立畫面資料
				for( unsigned short l=0; l<usFrameCount; ++l )
				{
					//sFrame* pFrame = g_RoleMemoryMgr.GetFrame();
					Dir.FrameList->push_back( g_RoleMemoryMgr.GetFrame() );
					sFrame* pFrame = Dir.FrameList->back();

					// 取得資料
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
					//pData += 9;    // 預留欄位
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
            			pData += 9;    // 預留欄位
					}
					pFrame->pImage = NULL;
					pFrame->Color = 0xFFFFFFFF;

					// 更新畫面矩陣
					pFrame->RefreshMatrix( pFrame->vScale, pFrame->vPosition );
				}
			}
		}
		

	}

	//FilterAction() ; 

	// 建立待機動作資料
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
//從記憶體載入人物動作
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

    // 讀取標頭檔
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "RAT" ) != 0 )
    {
        /* str.assign("RAT");
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//檔案格式錯誤
        str.append( ",LoadAction" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        Assert( 0, str.c_str() ); */
        return false;
    }

    pData += sizeof_RoleHeader;

    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    D3DXVECTOR3 vScale, vPosition;

    // 釋放資源
    RemoveAllAction();

    // 建立待機動作資料
	if(m_pNowAction)
		g_RoleMemoryMgr.FreeAction(m_pNowAction);
    m_pNowAction = g_RoleMemoryMgr.GetAction();

    // 取得資料
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
    pData += 20;    // 預留欄位

    // 零件總數
    unsigned short usComponentCount = *(unsigned short*)pData;
    pData += sizeof( unsigned short );

    bool bBackAdornment = false;

    // 建立零件資料
    for( unsigned short j=0; j<usComponentCount; ++j )
    {
        pRoleComponent = g_RoleMemoryMgr.GetComponent();
        pRoleComponent->bShow = false;
        m_pNowAction->ComponentList->push_back( pRoleComponent );

        // 取得資料
        strcpy( pRoleComponent->szComponentName, (char*)pData );
        pData += 20;

        // 判斷是否為背飾零件
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 863, CodeInlineText_Text ) );//背飾
        string str( "背飾" );
        if( strcmp( pRoleComponent->szComponentName, str.c_str() ) == 0 )
            bBackAdornment = true;
        else
            bBackAdornment = false;

        for( unsigned short k=0; k<8; ++k )
        {
            // 取得資料
            pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
            pData += sizeof( unsigned short );

            // 畫面總數
            unsigned short usFrameCount = *(unsigned short*)pData;
            pData += sizeof( unsigned short );

            // 建立畫面資料
            for( unsigned short l=0; l<usFrameCount; ++l )
            {
                pFrame = g_RoleMemoryMgr.GetFrame();
				sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
				sFrameList& sfl = *scd.FrameList;
				sint nJCDebugCode_addr = (sint)scd.FrameList; 
				sfl.push_back(pFrame);
/*
                // 取得資料
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
                    //pData += 9;    // 預留欄位
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
                    	pData += 9;    // 預留欄位
                    }
                    pFrame->pImage = NULL;
                    pFrame->Color = 0xFFFFFFFF;*/

					// 更新畫面矩陣
                    //pFrame->vPosition = vPosition;
                    //pFrame->vScale = vScale;
                    //pFrame->RefreshMatrix( vScale, vPosition );
                    // 更新畫面矩陣
                    //pFrame->RefreshMatrix( pFrame->vScale, pFrame->vPosition );

				pData += sizeof( unsigned short );
                pFrame->usPlayTime = *(unsigned short*)pData;
                pData += sizeof( unsigned short );
                pFrame->shLayer = *(short*)pData;
                pData += sizeof( short );
                pData += sizeof( float ); // 自轉

                pFrame->matLocal._11 = *(float*)pData;
                pData += sizeof( float ); // 縮放X
                pFrame->matLocal._12 = *(float*)pData;
                pData += sizeof( float ); // 縮放Y

                pFrame->matLocal._13 = *(float*)pData;
                pData += sizeof( float ); // 位置X
                pFrame->matLocal._14 = *(float*)pData;
                pData += sizeof( float ); // 位置Y

                pData += sizeof( float ); // 位置Z
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
                    pData += 9;    // 預留欄位
                }

                pFrame->pImage = NULL;
                pFrame->Color = 0xFFFFFFFF;

                // 背飾零件
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
// 載入零件
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
    // 讀取封裝檔
    memPack* pMem = LoadFromPacket( strFileName );
    if( pMem == NULL )
        return false;

    // 儲存封裝檔資料
    BYTE* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );
 
#endif



    // 讀取標頭檔
    sComHeader* pHeader = (sComHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "RDL" ) != 0 )
    {
        /* str.assign("RDL");
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//檔案格式錯誤
        str.append( ",LoadComponent" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        Assert( 0, str.c_str() ); */
        return false;
    }

    pData += sizeof( sComHeader );

    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    char szTexFile[50];
    char szActName[20];

    // 更新紀錄的動作資料
    strcpy( szActName, (char*)pData );
    pData += 20;

    // 判斷動作名稱
    if( strcmp( m_pNowAction->szActionName, szActName ) != 0 )
    {
        /* str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 864, CodeInlineText_Text ) );//檔案中第一項紀錄不是待機的動作名稱
        Assert( 0, str.c_str() ); */
        return false;
    }

    // 取得零件指標
    pRoleComponent = m_pNowAction->GetComponent( pHeader->szComponentName1 );

    if( pRoleComponent == NULL )
    {
        /* str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 865, CodeInlineText_Text ) );//找不到與檔案中紀錄符合的零件名稱
        Assert( 0, str.c_str() ); */
        return false;
    }

    // 顯示零件
    pRoleComponent->bShow = true;

    for( unsigned short k=0; k<8; ++k )
    {
        // 清除畫面
        pRoleComponent->ComponentDirs[k]->RemoveAllFrame();

        // 取得資料
        pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // 畫面總數
        unsigned short usFrameCount = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // 建立畫面資料
        for( unsigned short l=0; l<usFrameCount; ++l )
        {
            pFrame = g_RoleMemoryMgr.GetFrame();
			sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
			sFrameList& sfl = *scd.FrameList;
			sint nJCDebugCode_addr = (sint)scd.FrameList; 
			sfl.push_back(pFrame);

            // 取得資料
            //pFrame->usUnitTime = *(unsigned short*)pData;
            pData += sizeof( unsigned short );
            pFrame->usPlayTime = *(unsigned short*)pData;
            pData += sizeof( unsigned short );
            pFrame->shLayer = *(short*)pData;
            pData += sizeof( short );
            pData += sizeof( float ); // 自轉

            pFrame->matLocal._11 = *(float*)pData;
            pData += sizeof( float ); // 縮放X
            pFrame->matLocal._12 = *(float*)pData;
            pData += sizeof( float ); // 縮放Y

            pFrame->matLocal._13 = *(float*)pData;
            pData += sizeof( float ); // 位置X
            pFrame->matLocal._14 = *(float*)pData;
            pData += sizeof( float ); // 位置Y

            pData += sizeof( float ); // 位置Z
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
            pData += 20;    // 預留欄位

            // 讀取材質
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

        // 儲存記憶體資料
        pData = MemDataCacheMgr.AddMemData( pFileName, pMem->Data, pMem->DataSize );

        if( pData == NULL ) return false;
    }
 
#endif



    // 讀取標頭檔
    sComHeader* pHeader = (sComHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "RPC" ) != 0 )
    {
        // string str;
        //str.assign( "RPC" );
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//檔案格式錯誤
        //str.append( ",LoadImage" );
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
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
		// 更新紀錄的動作資料
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
        				pData += 10;    // 預留欄位
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

				// 畫面總數比較
				unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();

				if(usFrameCount != usRoleFrameCount )
				{
					return false ; 
				}

				// 更新畫面資料
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
					//pData += 10;    // 預留欄位
					if( pHeader->usVersion>1 )
					{
            			pFrame->Color = *(DWORD*)pData;
						pData += sizeof( DWORD );
						pData += 6;
					}
					else
					{
            			pData += 10;    // 預留欄位
					}

					// 讀取材質
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
//從記憶體卸除裝備
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
    // 讀取封裝檔
    memPack* pMem = LoadFromPacket( strFileName );
    if( pMem == NULL ) return false;

    // 儲存封裝檔資料
    BYTE* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );

#endif



    // 讀取標頭檔
    sComHeader* pHeader = (sComHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "RPC" ) != 0 )
    {
        // str.assign("RPC");
        // str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//檔案格式錯誤
        // str.append( ",LoadImage" );
        // str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        // Assert( 0, str.c_str() );
        return false;
    }

    pData += sizeof( sComHeader );

    sRoleComponent* pRoleComponent = NULL;
    char szTexFile[50];
    char szActName[20];

    // 更新紀錄的動作資料
    strcpy( szActName, (char*)pData );
    pData += 20;

    // 判斷動作名稱
    if( strcmp( m_pNowAction->szActionName, szActName ) != 0 )
    {
        // str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 864, CodeInlineText_Text ) );//檔案中第一項紀錄不世代機的動作名稱
        // Assert( 0, str.c_str() );
        return false;
    }

    // 取得零件指標
    pRoleComponent = m_pNowAction->GetComponent( pHeader->szComponentName1 );

    if( pRoleComponent == NULL )
    {
        // str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 865, CodeInlineText_Text ) );//找不到與檔案中紀錄符合的零件名稱
        // Assert( 0, str.c_str() );
        return false;
    }

    // 顯示零件
    pRoleComponent->bShow = true;

    for( unsigned short k=0; k<8; ++k )
    {
        unsigned short usFrameCount = *(unsigned short*)pData;
        pData += sizeof( unsigned short );

        // 畫面總數比較
        unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();
        if( usFrameCount != usRoleFrameCount )
        {
            // str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 866, CodeInlineText_Text ) );//畫面數與檔案中的紀錄不符
            // Assert( 0, str.c_str() );
            return false;
        }

        // 更新畫面資料
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
            //pData += 10;    // 預留欄位
            if( pHeader->usVersion>1 )		//add test
            {
            	pFrame->Color = *(DWORD*)pData;
                pData += sizeof( DWORD );
                pData += 6;
            }
            else
            {
            	pData += 10;    // 預留欄位
            }								//add test

            // 讀取材質
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
//清除部位檔案
//----------------------------------------------------------------------------
bool DisplayRole::UnLoadPart( const string& strFileName )
{
	if (strFileName.c_str() == NULL) return false; // 錯誤！傳入空指標
	if (strlen(strFileName.c_str()) == 0) return false; // 錯誤！傳入空字串
    if (strcmp(strFileName.c_str(),"Null") == 0) return false; // 企劃表格設定為不載入任何圖形

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


	//cosiann 多一個是記錄\n

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
//載入部位檔案
//----------------------------------------------------------------------------
bool DisplayRole::LoadPart( const string& strFileName )
{
	if (strFileName.c_str() == NULL) return false; // 錯誤！傳入空指標
	if (strlen(strFileName.c_str()) == 0) return false; // 錯誤！傳入空字串
    if (strcmp(strFileName.c_str(),"Null") == 0) return false; // 企劃表格設定為不載入任何圖形

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


	//cosiann 多一個是記錄\n

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
// 隱藏零件
//----------------------------------------------------------------------------
void DisplayRole::HideComponent( const char* pComName )
{
    if( m_pNowAction == NULL ) return;

    // 取得零件指標
    sRoleComponent* pRoleComponent = m_pNowAction->GetComponent( pComName );

    if( pRoleComponent != NULL )
        pRoleComponent->bShow = false;
}
void DisplayRole::PushActionStack(const char *actName)
{
	//m_playStack.push(actName) ; 

}
//----------------------------------------------------------------------------
// 隱藏零件
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
//繪製展示角色第2層
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
        sRoleComponent* pCom = (*pos);							// 檢查是否顯示零件
		sComponentDir&	Dir	 = *(pCom->ComponentDirs[m_eShowDir]);	// 檢查畫面串列是否為空
        
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

    // 零件圖層排序
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
        sRoleComponent* pCom = (*pos);							// 檢查是否顯示零件
		sComponentDir&	Dir	 = *(pCom->ComponentDirs[m_eShowDir]);	// 檢查畫面串列是否為空
        
		if (Dir.FrameList->empty()) 
		{
			continue;
		}
	
		// 播放第一張畫面
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
        sRoleComponent* pCom = (*pos);							// 檢查是否顯示零件
		sComponentDir&	Dir	 = *(pCom->ComponentDirs[m_eShowDir]);	// 檢查畫面串列是否為空
        
		if (Dir.FrameList->empty()) 
		{
			continue;
		}
		for(unsigned int i = 0 ; i < partVec.size() ; ++i)
		{
			if(strcmp(pCom->szComponentName , partVec[i].c_str()) == 0)
			{
				// 播放第一張畫面
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

/* 不在需要以下動作

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

	//先得要lockview的Frame的中間高度
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
	//camera的高度0，是在viewPort的中間位置
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
//繪製展示角色第3層 繪製出遊戲人物
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

    // 背飾零件
    if( pFrame->matLocal._21 > 0.0f )
    { // 計算繪圖區域的偏移值
        fOffsetX = pFrame->matLocal._13 * pFrame->matLocal._11 * m_vScale.x * -12.0f;
        fOffsetY = pFrame->matLocal._14 * pFrame->matLocal._12 * m_vScale.y *  12.0f;
    }
    else
    { // 計算繪圖區域的偏移值
        fOffsetX = pFrame->matLocal._13 * pFrame->matLocal._11 * m_vScale.x * -19.0f;
        fOffsetY = pFrame->matLocal._14 * pFrame->matLocal._12 * m_vScale.y * -19.0f;
    }

    //大頭版修改
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
    else if( pFrame->shLayer== 3 ) //面飾
    {
        rcRect.left   = (LONG)(-pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x);
        rcRect.top    = (LONG)(-pFrame->fHeight *  9.5f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y);
        rcRect.right  = (LONG)( pFrame->fWidth  * 24.0f * pFrame->matLocal._11 * m_vScale.x + fOffsetX + m_vPosition.x);
        rcRect.bottom = (LONG)( pFrame->fHeight * 14.5f * pFrame->matLocal._12 * m_vScale.y + fOffsetY + m_vPosition.y);
    }
    else
    { // 計算繪圖區域範圍
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
// 設定位置
//---------------------------------------------------------------------------
void DisplayRole::SetPosition( const D3DXVECTOR2& vPos )
{
    m_vPosition = vPos;

    m_ViewPort.X = (DWORD)vPos.x;
    m_ViewPort.Y = (DWORD)vPos.y;    


    // 設定視口位置
 //   m_ViewPort.X = static_cast<DWORD>(m_vPosition.x)-m_ViewPort.Width/2;
 //   m_ViewPort.Y = static_cast<DWORD>(m_vPosition.y)-m_ViewPort.Height/2;    
}

//----------------------------------------------------------------------------
// 取得位置
//---------------------------------------------------------------------------
const D3DXVECTOR2& DisplayRole::GetPosition() const
{
    return m_vPosition;
}

//----------------------------------------------------------------------------
// 取得縮放
//---------------------------------------------------------------------------
const D3DXVECTOR2& DisplayRole::GetScale() const
{
    return m_vScale;
}

//----------------------------------------------------------------------------
// 下個方向
//----------------------------------------------------------------------------
void DisplayRole::NextDirection()
{
    int iDir = (int)m_eShowDir;
    if( ++iDir > 7 )
        iDir = 0;

    m_eShowDir = (RoleDirect)iDir;
}

//----------------------------------------------------------------------------
// 上個方向
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

    // 設定 Viewport
    //D3DVIEWPORT9 currViewPort;
   // m_pd3dDevice->GetViewport( &s_ViewPort );
   // m_pd3dDevice->SetViewport( &m_ViewPort );
	//m_pd3dDevice->Clear(0 , NULL , D3DCLEAR_TARGET , 0x00000000 , 1.0f , 0) ; 

    // 設定投影矩陣
    //D3DXMATRIX matCurrProj;
   // m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &s_matProj );
   // m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // 設定攝影機矩陣
   // D3DXMATRIX matCurrView ; 
	//D3DXMatrixIdentity(&matCurrView) ; 

	LockView() ; 

	// 修正viewport以及projection matrix
	// : 針對viewport超出螢幕的修正處理

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

	// 繪製逆時針方向的三角形
    m_pd3dDevice->GetRenderState( D3DRS_CULLMODE, &m_oldCullState);
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

//	m_pd3dDevice->Clear(0 , NULL , D3DCLEAR_TARGET , 0x00000000 , 1.0f , 0) ; 

    return true;
}
//----------------------------------------------------------------------------
bool DisplayRole::EndRender()
{
    // 還原攝影機矩陣
 //   m_pd3dDevice->SetTransform( D3DTS_VIEW, &s_matView );
	UnLockView() ; 

	EndRender2D(m_pd3dDevice , s_ViewPort , s_matProj , s_matView) ; 

    // 還原投影矩陣
   // m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &s_matProj );
    // 還原 Viewport
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

        // 儲存記憶體資料
        pData = MemDataCacheMgr.AddMemData( pFileName, pMem->Data, pMem->DataSize );

        if( pData == NULL ) return;
    }
 
#endif



    // 讀取標頭檔
    sComHeader* pHeader = (sComHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "RPC" ) != 0 )
    {
        // string str;
        //str.assign( "RPC" );
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//檔案格式錯誤
        //str.append( ",LoadImage" );
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
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
		// 更新紀錄的動作資料
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

				// 畫面總數比較
				unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();

				if(usFrameCount != usRoleFrameCount )
				{
					return; 
				}

				// 更新畫面資料
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
					//pData += 10;    // 預留欄位
					if( pHeader->usVersion>1 )
					{
            			pFrame->Color = *(DWORD*)pData;
						pData += sizeof( DWORD );
						pData += 6;
					}
					else
					{
            			pData += 10;    // 預留欄位
					}

					// 讀取材質
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
        // 讀取標頭檔
        sComHeader* pHeader = (sComHeader*)pData;        
        // 確認檔案！
        if( stricmp( pHeader->szFileID, "RPC" ) != 0 ) return;
        // 清除資料
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
        // 讀取標頭檔
        sComHeader* pHeader = (sComHeader*)pMem->Data;        
        // 確認檔案！
        if( stricmp( pHeader->szFileID, "RPC" ) != 0 ) 
			return;
        // 清除資料
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
        // 讀取標頭檔
        sComHeader* pHeader = (sComHeader*)pData;        
        // 確認檔案！
        if( stricmp( pHeader->szFileID, "RPC" ) != 0 ) return;
        // 隱藏零件    
        pData += sizeof_ComHeader;

		char szTexFile[50];
		char szActName[20];
		bool isLoading = false ; 

		sRoleAction *pAction = NULL ; 

		for(unsigned int i = 0 ; i < pHeader->usActCount1 ; ++i)
		{
			// 更新紀錄的動作資料
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

					// 畫面總數比較
					unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();

					if(usFrameCount != usRoleFrameCount )
					{
						return; 
					}

					// 更新畫面資料
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
						//pData += 10;    // 預留欄位
						if( pHeader->usVersion>1 )
						{
            				pFrame->Color = *(DWORD*)pData;
							pData += sizeof( DWORD );
							pData += 6;
						}
						else
						{
            				pData += 10;    // 預留欄位
						}

						// 讀取材質
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
        // 讀取標頭檔
        sComHeader* pHeader = (sComHeader*)pMem->Data;        
        // 確認檔案！
        if( stricmp( pHeader->szFileID, "RPC" ) != 0 ) return;
        // 隱藏零件    
        pData += sizeof_ComHeader;

		char szTexFile[50];
		char szActName[20];
		bool isLoading = false ; 

		sRoleAction *pAction = NULL ; 

		for(unsigned int i = 0 ; i < pHeader->usActCount1 ; ++i)
		{
			// 更新紀錄的動作資料
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

					// 畫面總數比較
					unsigned short usRoleFrameCount = pRoleComponent->ComponentDirs[k]->FrameList->size();

					if(usFrameCount != usRoleFrameCount )
					{
						return; 
					}

					// 更新畫面資料
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
						//pData += 10;    // 預留欄位
						if( pHeader->usVersion>1 )
						{
            				pFrame->Color = *(DWORD*)pData;
							pData += sizeof( DWORD );
							pData += 6;
						}
						else
						{
            				pData += 10;    // 預留欄位
						}

						// 讀取材質
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
		return false; // 錯誤！傳入空指標
	}
    if (strlen(pFilename)==0) 
	{
		return false; // 錯誤！傳入空字串
	}
    if (strcmp(pFilename,"Null")==0) 
	{
		return false; // 企劃表格設定為不載入任何圖形   
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

	//cosiann 多一個是記錄\n
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
// 	//資料表裡的順序跟ser封包不一樣
// 	WEAPON = 1,	// 右手(都裝武器)
// 	SHIELD,		// 左手(都裝盾)
// 	TWO_HAND,	// 雙手武器
// 	HELMET,		// 頭部
// 	ARMOR,		// 上裝
// 	PANTS,		// 護腿(下裝)
// 	GLOVE,		// 手套
// 	SHOES,		// 鞋
// 	FINGER1,	// 飾品1
// 	FINGER2,	// 飾品2(戒指)
// 	CLOAK,		// 披風
// 	ADOM,		// 面飾
// 	PET,		// 精靈
// 
// 	EQPART_MAX	//總數量
// };

//////////////////////////////////////////////////////////////////////////
//取得指定部位的資料
//////////////////////////////////////////////////////////////////////////
int DisplayRole::GetDesignatePartItemID(int part)
{
	return PDArray[part].nItemID;
}

//////////////////////////////////////////////////////////////////////////
//設定指定部位的資料
//////////////////////////////////////////////////////////////////////////
void DisplayRole::SetDesignatePartItemID(int part, int itemID)
{
	PDArray[part].nItemID = itemID;
}
////----------------------------------------------------------
////儲存裝備部位的道具id
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
////傳入裝備部位 取得裝備部位的道具id
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