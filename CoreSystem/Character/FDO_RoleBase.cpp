#include "stdafx.h"
//#pragma hdrstop
//----------------------------------------------------------------------------
#include "J_SoundManager.h"
#include "PacketReader.h"
#include "ResourceManager.h"
#include "DrawPlane.h"
#include "PhenixD3D9RenderSystem.h"
#include "PhenixAssert.h"
#include "FDOUT.h"
//#include "FDO_RoleMemoryMgr.h"
//#include "GameMapManager.h"
//#include "JLCamera.h"
#include "Common.h"
//#include "HUISystemSet.h"
#include "CFDOXML.h"
//#include "Global.h"
#include "ParticleSystem.h"
#include "CDXUTListBox.h"
#include "FDO_RoleSkin.h"
#include "RoleResourceManage.h"
#pragma hdrstop
#include "FDO_RoleBase.h"
#include "fdo_thread_utility.h"
//////////////////////								////////////////////////////
#include "CRATFileFactory.h"
#include "CRPCFileFactory.h"
#include "CRoleSkinFactory.h"
#include "CRoleSkinStruct.h"
#include "CRoleRenderer.h"
#include "Debug.h"
#include "IGDRole.h"
#include "R_BaseType.h"
#include "GameMessageEvent.h"
#include "GameMessageEventid.h"
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
	bool ActorSort(RoleBase* i, RoleBase* j){ return (i->m_pNowAction < j->m_pNowAction); }

	bool RoleID::operator == (const RoleID& k) const 
	{
		return k.nID == nID;
	}

	bool RoleID::operator != (const RoleID& k) const 
	{
		return k.nID != nID;
	}

	bool RoleID::operator < (const RoleID& k) const 
	{
		return  nID < k.nID;
	}

	void RoleID::operator =	(uint nId) 
	{
		nServerID = nId;			
	}

	void RoleID::operator =	(const RoleID& nId)	
	{		
		nID = nId.nID;

		//assert(nType);
	}

	RoleID::RoleID(uint t,uint i) : nType(t) ,nServerID(i)
	{

	}

	RoleID::RoleID	(uint t) : nType(t) ,nServerID(0)
	{
		//assert(nType);
	}

	RoleID::RoleID(const RoleID& k) : nID(k.nID)
	{
		//assert(nType);
	}

	RoleID::RoleID() : nID(0)
	{
		
	}

	RoleID::~RoleID	() 
	{

	}


//----------------------------------------------------------------------------
const D3DXVECTOR3 g_vSouth( 0.0f, 0.0f, -1.0f );
const D3DXVECTOR3 g_vESouth( -0.707f, 0.0f, -0.707f );
const D3DXVECTOR3 g_vEast( -1.0f, 0.0f , 0.0f );
const D3DXVECTOR3 g_vENorth( -0.707f, 0.0f , 0.707f );
const D3DXVECTOR3 g_vNorth( 0.0f ,0.0f , 1.0f );
const D3DXVECTOR3 g_vWNorth( 0.707f, 0.0f , 0.707f );
const D3DXVECTOR3 g_vWest( 1.0f , 0.0f , 0.0f );
const D3DXVECTOR3 g_vWSouth( 0.707f, 0.0f , -0.707f );

const D3DXVECTOR3 g_PosOffset( 1.0f, 0.0f, 1.0f );          // 角色位置偏移量
const D3DXVECTOR3 g_HeightOffset( 0.0f, 2.5f, 0.0f );       // 角色高度位置偏移量

// 掉落物角色 Bounding Box
const D3DXVECTOR3 g_01_BBX0( -0.8f, 0.0f, 0.0f );
const D3DXVECTOR3 g_01_BBX1( -0.8f, 1.6f, 0.0f );
const D3DXVECTOR3 g_01_BBX2(  0.8f, 0.0f, 0.0f );
const D3DXVECTOR3 g_01_BBX3(  0.8f, 1.6f, 0.0f );

// 陸地小型角色 Bounding Box
const D3DXVECTOR3 g_02_BBX0( -1.0f, 0.0f, 0.0f );
const D3DXVECTOR3 g_02_BBX1( -1.0f, 2.0f, 0.0f );
const D3DXVECTOR3 g_02_BBX2(  1.0f, 0.0f, 0.0f );
const D3DXVECTOR3 g_02_BBX3(  1.0f, 2.0f, 0.0f );

// 陸地中小型角色 Bounding Box
const D3DXVECTOR3 g_03_BBX0( -1.1f, 0.0f, 0.0f );
const D3DXVECTOR3 g_03_BBX1( -1.1f, 3.5f, 0.0f );
const D3DXVECTOR3 g_03_BBX2(  1.1f, 0.0f, 0.0f );
const D3DXVECTOR3 g_03_BBX3(  1.1f, 3.5f, 0.0f );

// 陸地中型角色 Bounding Box
const D3DXVECTOR3 g_04_BBX0( -1.2f, 0.0f, 0.0f );
const D3DXVECTOR3 g_04_BBX1( -1.2f, 5.2f, 0.0f );
const D3DXVECTOR3 g_04_BBX2(  1.2f, 0.0f, 0.0f );
const D3DXVECTOR3 g_04_BBX3(  1.2f, 5.2f, 0.0f );

// 陸地大型角色 Bounding Box
const D3DXVECTOR3 g_05_BBX0( -1.6f, 0.0f, 0.0f );
const D3DXVECTOR3 g_05_BBX1( -1.6f, 7.0f, 0.0f );
const D3DXVECTOR3 g_05_BBX2(  1.6f, 0.0f, 0.0f );
const D3DXVECTOR3 g_05_BBX3(  1.6f, 7.0f, 0.0f );

// 陸地超大型角色 Bounding Box
const D3DXVECTOR3 g_06_BBX0( -2.0f, 0.0f, 0.0f );
const D3DXVECTOR3 g_06_BBX1( -2.0f, 8.0f, 0.0f );
const D3DXVECTOR3 g_06_BBX2(  2.0f, 0.0f, 0.0f );
const D3DXVECTOR3 g_06_BBX3(  2.0f, 8.0f, 0.0f );

// 空中小型角色 Bounding Box
const D3DXVECTOR3 g_07_BBX0( -1.0f, 3.2f, 0.0f );
const D3DXVECTOR3 g_07_BBX1( -1.0f, 5.2f, 0.0f );
const D3DXVECTOR3 g_07_BBX2(  1.0f, 3.2f, 0.0f );
const D3DXVECTOR3 g_07_BBX3(  1.0f, 5.2f, 0.0f );

// 空中中型角色 Bounding Box
const D3DXVECTOR3 g_08_BBX0( -1.2f, 3.0f, 0.0f );
const D3DXVECTOR3 g_08_BBX1( -1.2f, 6.0f, 0.0f );
const D3DXVECTOR3 g_08_BBX2(  1.2f, 3.0f, 0.0f );
const D3DXVECTOR3 g_08_BBX3(  1.2f, 6.0f, 0.0f );

// 空中大型角色 Bounding Box
const D3DXVECTOR3 g_09_BBX0( -1.6f, 2.5f, 0.0f );
const D3DXVECTOR3 g_09_BBX1( -1.6f, 7.5f, 0.0f );
const D3DXVECTOR3 g_09_BBX2(  1.6f, 2.5f, 0.0f );
const D3DXVECTOR3 g_09_BBX3(  1.6f, 7.5f, 0.0f );

// 空中超大型角色 Bounding Box
const D3DXVECTOR3 g_10_BBX0( -2.0f, 2.0f, 0.0f );
const D3DXVECTOR3 g_10_BBX1( -2.0f, 8.0f, 0.0f );
const D3DXVECTOR3 g_10_BBX2(  2.0f, 2.0f, 0.0f );
const D3DXVECTOR3 g_10_BBX3(  2.0f, 8.0f, 0.0f );

// 橫向小型角色 Bounding Box
const D3DXVECTOR3 g_11_BBX0( -1.6f,  0.0f, 0.0f );
const D3DXVECTOR3 g_11_BBX1( -1.6f,  2.5f, 0.0f );
const D3DXVECTOR3 g_11_BBX2(  1.6f,  0.0f, 0.0f );
const D3DXVECTOR3 g_11_BBX3(  1.6f,  2.5f, 0.0f );

// 橫向中型角色 Bounding Box
const D3DXVECTOR3 g_12_BBX0( -2.0f,  0.0f, 0.0f );
const D3DXVECTOR3 g_12_BBX1( -2.0f,  3.0f, 0.0f );
const D3DXVECTOR3 g_12_BBX2(  2.0f,  0.0f, 0.0f );
const D3DXVECTOR3 g_12_BBX3(  2.0f,  3.0f, 0.0f );

// 空中精靈角色 Bounding Box
const D3DXVECTOR3 g_13_BBX0( -2.0f,  0.0f, 0.0f );
const D3DXVECTOR3 g_13_BBX1( -2.0f,  4.0f, 0.0f );
const D3DXVECTOR3 g_13_BBX2(  2.0f,  0.0f, 0.0f );
const D3DXVECTOR3 g_13_BBX3(  2.0f,  4.0f, 0.0f );

// 橫向大型角色 Bounding Box
const D3DXVECTOR3 g_14_BBX0( -2.5f,  0.0f, 0.0f );
const D3DXVECTOR3 g_14_BBX1( -2.5f,  4.0f, 0.0f );
const D3DXVECTOR3 g_14_BBX2(  2.5f,  0.0f, 0.0f );
const D3DXVECTOR3 g_14_BBX3(  2.5f,  4.0f, 0.0f );

// 大象級的那種要偏移的超高角色 Bounding Box
const D3DXVECTOR3 g_15_BBX0( -2.5f, 2.0f, 0.0f );
const D3DXVECTOR3 g_15_BBX1( -2.5f, 11.0f, 0.0f );
const D3DXVECTOR3 g_15_BBX2(  2.5f, 2.0f, 0.0f );
const D3DXVECTOR3 g_15_BBX3(  2.5f, 11.0f, 0.0f );

// 鬼頭級的那種要偏移的飛行橫向大型角色 Bounding Box
const D3DXVECTOR3 g_16_BBX0( -3.5f, 3.0f, 0.0f );
const D3DXVECTOR3 g_16_BBX1( -3.5f, 10.0f, 0.0f );
const D3DXVECTOR3 g_16_BBX2(  3.5f, 3.0f, 0.0f );
const D3DXVECTOR3 g_16_BBX3(  3.5f, 10.0f, 0.0f );

// 麒麟級的那種要偏移的去頭去腳大型角色 Bounding Box
const D3DXVECTOR3 g_17_BBX0( -3.5f, 2.0f, 0.0f );
const D3DXVECTOR3 g_17_BBX1( -3.5f, 9.0f, 0.0f );
const D3DXVECTOR3 g_17_BBX2(  3.5f, 2.0f, 0.0f );
const D3DXVECTOR3 g_17_BBX3(  3.5f, 9.0f, 0.0f );

// 箭塔 Bounding Box
const D3DXVECTOR3 g_18_BBX0( -3.0f, 0.0f, 0.0f );
const D3DXVECTOR3 g_18_BBX1( -3.0f, 16.0f, 0.0f );
const D3DXVECTOR3 g_18_BBX2(  3.0f, 0.0f, 0.0f );
const D3DXVECTOR3 g_18_BBX3(  3.0f, 16.0f, 0.0f );

// 公會石那種超大金寶塔 Bounding Box
const D3DXVECTOR3 g_19_BBX0( -2.5f, 0.0f, 0.0f );
const D3DXVECTOR3 g_19_BBX1( -2.5f, 18.0f, 0.0f );
const D3DXVECTOR3 g_19_BBX2(  2.5f, 0.0f, 0.0f );
const D3DXVECTOR3 g_19_BBX3(  2.5f, 18.0f, 0.0f );

const DWORD g_shakeTime = 200 ; //撥完震動的時間
const DWORD g_DropTime = 1500 ; 
// BEGIN_GD_VALUE_MAP(RoleBase,IGameData)
// END_GD_VALUE_MAP
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//一開始就new
//////////////////////////////////////////////////////////////////////////
RoleBase::RoleBase(uint nType, PropertyPtr pBase , PropertyPtr pStatus)
: mFrameListCacheOutOfData(true) , m_bRage(false) , RoleBaseData(nType , pBase,pStatus) , m_RoleId(RoleBaseData::mRoleId)
{
    m_pDevice           = g_RenderSystem.GetD3DDevice();
    m_pActionList       = NULL;
    m_pNowAction        = NULL;
	m_pCurrRATAct		= 0;
    m_Color             = 0xFFFFFFFF;
	m_pickColor			= 0xFFFFFFFF ; 
    m_bVisible          = false;
    m_RoleKind          = Role_None;
    m_usBoundingBoxSize = 0;
    m_usDeadBoundBoxSize= 0;
    m_vPosition         = D3DXVECTOR3( 255.0f, 0.0f, 255.0f );
    m_vScale            = D3DXVECTOR3( 0.9f, 0.9f, 0.9f );  //修改
    m_ScreenPos         = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_shakeDir			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_bMatrixChange     = true;
	m_bShake			= false ;
	m_bDrop				= false ; 
	m_bDivide			= false ; 
	m_bPickColor		= false ; 
    m_usFrameNum        = 1;
    m_usFrameMax        = 1;
    m_PlayStatus        = Status_Stop;
    m_bPlayLoop         = false;
    m_dwUpdateTime      = 60;
    m_dwPassTime        = 0;
    m_eRoleDir          = RD_South;
    m_eShowDir          = RD_South;
    m_szNowActName[0]   = '\0';
    m_bPlaySound        = true;

    m_hDC               = GetDC(NULL);
    m_NameWidth         = 0;
    m_bShowName         = false;
    m_NameColor         = 0xFFFFFFFF;
    SetRect( &m_NameRect, 0, 0, 0, 0 );
	SetRect( &m_IDRect, 0, 0, 0, 0 );

    m_iCellPosX         = 0;
    m_iCellPosY         = 0;
    m_fShadowSize       = 0.5f;
    m_fShadowOffset     = 0;
    m_bShowShadow       = true;
    m_iNowAction        = ACT_Stand;
	NPCDifference		= 0;
	m_angleLoop			= 0.0f ; 
	m_DropDownLoop		= PI * 0.3f ; 
	m_DropScale			= 0.0f ; 
	m_DropRoation		= 0.0f ; 
	m_dropDir			= D3DXVECTOR3(0.0f , 0.0f , 0.0f) ; 
	m_pTextObject = NULL;
    	
	m_TalkPassTime = 0;
    m_bShowTalk = false;
	m_bUseList = false;
	m_iTalkWaitTime = 0;
	m_iTalkWaitNowTime = 0;
	m_TalkShowTime = 3000;
	m_bNpcTalkSelf = false;
	m_NpcTalkScreenPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_NpcEventScreenPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

    // <yuming>
    m_pRoleRenderer = new CRoleRenderer();
    // </yuming>

	m_BaseData = NULL ; 

	m_shareCatch = false ; 

}
//----------------------------------------------------------------------------
RoleBase::~RoleBase()
{
    RemoveAllAction();

    ReleaseDC( NULL, m_hDC );

    delete m_pTextObject;
    m_pTextObject = NULL;

    /// <yuming>    
    delete m_pRoleRenderer; 
	m_pRoleRenderer = NULL;

	//if(!m_shareCatch)
	{
		_FOREACH(sFrameList, mFrameListCache, pos)
		{
		   g_RoleMemoryMgr.ReleaseTempFrame(*pos) ; // delete (*pos);
		}
	}

	mFrameListCache.clear() ;
    /// </yuming>
}
//----------------------------------------------------------------------------
void RoleBase::ResetBase()
{
    UnRegisterObject( this );

	D3DXMatrixIdentity(&m_matShake) ; 
 	D3DXMatrixIdentity(&m_matDrop) ; 
 
    m_pActionList       = NULL;
    m_pNowAction        = NULL;
	m_pCurrRATAct		= 0;
    m_Color             = 0xFFFFFFFF;
	m_pickColor			= 0xFFFFFFFF ; 
    m_bVisible          = false;
	m_bPickColor		= false ; 
	//我要確保m_RoleKind
    //m_RoleKind          = Role_None;
    m_usBoundingBoxSize = 0;
    m_usDeadBoundBoxSize= 0;

    m_vPosition         = D3DXVECTOR3( 255.0f, 0.0f, 255.0f );
    m_vScale            = D3DXVECTOR3( 0.9f, 0.9f, 0.9f );    //(修改)
    m_ScreenPos         = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_bMatrixChange     = true;

    m_usFrameNum        = 1;
    m_usFrameMax        = 1;
    m_PlayStatus        = Status_Stop;
    m_bPlayLoop         = false;
    m_dwUpdateTime      = 60;
    m_dwPassTime        = 0;
    m_eRoleDir          = RD_South;
    m_eShowDir          = RD_South;
    m_szNowActName[0]   = '\0';
    m_bPlaySound        = true;
	NPCDifference		= 0;

    m_NameWidth         = 0;
    m_bShowName        = false;
    m_NameColor         = 0xFFFFFFFF;
    SetRect( &m_NameRect, 0, 0, 0, 0 );
	SetRect( &m_IDRect, 0, 0, 0, 0 );

    m_iCellPosX         = 0;
    m_iCellPosY         = 0;
    m_fShadowSize       = 0.5f;
    m_fShadowOffset     = 0;
    m_bShowShadow       = true;
    m_iNowAction        = ACT_Stand;
	
	m_TalkPassTime = 0;
    m_bShowTalk = false;
	m_bUseList = false;
	m_iTalkWaitTime = 0;
	m_iTalkWaitNowTime = 0;
	m_TalkShowTime = 3000;
	m_bNpcTalkSelf =false;

	//if (m_pTextObject) {
        delete m_pTextObject;
        m_pTextObject = NULL;
    //}

    mFrameListCacheOutOfData = true;

	m_BaseData = NULL ; 

    RemoveAllAction();
}
//----------------------------------------------------------------------------
//移除裝備動作
//----------------------------------------------------------------------------
void RoleBase::RemoveAllAction()
{

#ifdef RESMGR

/*
	if ( !m_ActionList.empty() && !m_actname.empty())
	{

		//丟入Action mempool
		g_RoleMemoryMgr.Push(m_actname , m_ActionList );

		//if (!g_RoleMemoryMgr.Push(m_actname , m_ActionList ))
		//{

		//	//滿了的話，直接刪除他
		//	sRoleActionList::iterator itEnd( m_ActionList.end() );
		//	for( sRoleActionList::iterator it = m_ActionList.begin(); it != itEnd; ++it )
		//	{
		//		g_RoleMemoryMgr.FreeAction( *it );

		//	}


		//	m_ActionList.clear();

		//	return ; 
		//}
		//沒有滿，直接將他清空
		m_ActionList.clear() ;
	}
	else
	{
			
			sRoleActionList::iterator itEnd( m_ActionList.end() );
			for( sRoleActionList::iterator it = m_ActionList.begin(); it != itEnd; ++it )
			{
				g_RoleMemoryMgr.FreeAction( *it );

			}

			m_ActionList.clear();
	}

*/


#else
    sRoleActionList::iterator itEnd( m_ActionList.end() );
    for( sRoleActionList::iterator it = m_ActionList.begin(); it != itEnd; ++it )
    {
        g_RoleMemoryMgr.FreeAction( *it );

    }
	
	m_pNowAction = 0;
    m_ActionList.clear();
#endif

// <yuming>
    //if( m_spSkinRAT.get() )
    //{
    //    GetRATManage()->Release( m_spSkinRAT->strFilename, m_spSkinRAT );
    //}

    //if( m_pRoleSkin )
    //{
    //    m_pRoleSkin->SkinActs.clear();
    //}

    //while( !m_SkinRPCs.empty() )
    //{
    //    GetRPCManage()->Release( m_SkinRPCs.front()->strFilename, m_SkinRPCs.front() );
    //    m_SkinRPCs.pop_front();
    //}
    m_pRoleRenderer->UnloadAllRPCFile();
// </yuming>
}

//----------------------------------------------------------------------------
// 載入整個角色
//----------------------------------------------------------------------------
bool RoleBase::LoadRoleAll( const string& strFileName )
{

#ifdef RESMGR

    memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;

#else
    // 讀取封裝檔
    memPack* pMem = LoadFromPacket( strFileName );
    if( pMem == NULL )
        return false;

    BYTE* pData = NULL;

    if( m_RoleKind == Role_Leader )
    {
        // 儲存封裝檔資料
        pData = GetNewMemory( pMem->DataSize );
        memcpy( pData, pMem->Data, pMem->DataSize );
    }
    else
    {
        pData = pMem->Data;
    }

#endif


    // 讀取標頭檔
    sRoleHeader* pHeader = (sRoleHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "ALL" ) != 0 )
    {
        /* string str;
        str.assign( "ALL" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//檔案格式錯誤
        str.append( ",LoadRoleAll" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        Assert( 0, str.c_str() ); */
        return false;
    }

    m_vScale.x = pHeader->fScaleX*0.92f;
    m_vScale.y = pHeader->fScaleX*0.92f;
    m_vScale.z = pHeader->fScaleX*0.92f;

    m_fShadowSize = pHeader->usShadowSize * 0.15f;
    m_fShadowOffset = pHeader->usShadowOffset * 0.1f;
    m_usBoundingBoxSize = pHeader->usBoundingBoxSize;
    m_usDeadBoundBoxSize = pHeader->usDeadBoundBoxSize;
	m_pActionList        = &m_ActionList;

    pData += sizeof( sRoleHeader );

    sRoleAction* pAction = NULL;
    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    char szTexFile[50];
    D3DXVECTOR3 vScale, vPosition;

    // 釋放資源
    RemoveAllAction();

    // 建立動作資料
    for( unsigned short i=0; i<pHeader->usActCount; ++i )
    {
        pAction = g_RoleMemoryMgr.GetAction();
        m_ActionList.push_back( pAction );

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

        // 建立零件資料
        for( unsigned short j=0; j<usComponentCount; ++j )
        {
            pRoleComponent = g_RoleMemoryMgr.GetComponent();
            pRoleComponent->bShow = true;
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

					pData += 4; // 預留欄位

                    // 讀取材質
                    if( szTexFile[0] == '\0' )
                    {
                        pFrame->pImage = NULL;
                    }
                    else
                    {
                        if( m_RoleKind == Role_Leader )
                            pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
                        else
                            pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
                    }

                    // 更新畫面矩陣
                    //pFrame->vPosition = vPosition;
                    //pFrame->vScale = vScale;
                    pFrame->RefreshMatrix( pFrame->vScale, pFrame->vPosition );
                }
            }
        }
    }

    // 更新矩陣
    m_bMatrixChange = true;

    return true;
}

//////////////////////////////////////////////////////////////////////////
//讀取rat檔 (動作骨架)
//////////////////////////////////////////////////////////////////////////
bool RoleBase::LoadRATFile(const char* pFileName, CRATFactory* pRATFactory)
{
	// 所有動作都交給 RoleRenderer 去做，這樣一來移植比較方便
    m_pRoleRenderer->SetRATFactory( pRATFactory );
    if( !m_pRoleRenderer->LoadRATFile( pFileName ) )
	{
		return false;
	}
	SP_RATFile& spRATFile = m_pRoleRenderer->GetRATFile();

	//設定rolebase參數
	m_vScale.x = spRATFile->fScaleX;
	m_vScale.y = spRATFile->fScaleY;
	m_vScale.z = spRATFile->fScaleZ;
	m_fShadowSize = spRATFile->fShadowSize;
	m_fShadowOffset = spRATFile->fShadowOffset;
	m_usBoundingBoxSize = spRATFile->usBoundingBoxSize;
	m_usDeadBoundBoxSize = spRATFile->usDeadBoundBoxSize;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//讀取rpc檔 (動作圖片)
//////////////////////////////////////////////////////////////////////////
bool RoleBase::LoadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory)
{
    // 所有動作都交給 RoleRenderer 去做，這樣一來移植比較方便
    m_pRoleRenderer->SetRPCFactory( pRPCFactory );

	//return m_pRoleRenderer->LoadRPCFile( pFileName, m_RoleKind );


	return m_pRoleRenderer->LoadRPCFileThread(pFileName , m_RoleId.nServerID ) ; 
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void RoleBase::UnloadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory)
{
    m_pRoleRenderer->SetRPCFactory( pRPCFactory );
    m_pRoleRenderer->UnloadRPCFile( pFileName );
}

//----------------------------------------------------------------------------
// 載入零件
//----------------------------------------------------------------------------
bool RoleBase::LoadComponent( const string& strFileName )
{
    if( strFileName == "Null" )
        return true;


#ifdef RESMGR

    memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;

#else
    static MemDataMgr MemDataCacheMgr;

    BYTE* pData = MemDataCacheMgr.GetMemData( strFileName );
    if( pData == NULL )
    {
        memPack* pMem = LoadFromPacket( strFileName );

        if( pMem == NULL )
            return false;

        // 儲存記憶體資料
        pData = MemDataCacheMgr.AddMemData( strFileName, pMem->Data, pMem->DataSize );

        if( pData == NULL )
            return false;
    }

#endif




    // 讀取標頭檔
    sComHeader* pHeader = (sComHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->szFileID, "RDL" ) != 0 )
    {
        /* string str;
        str.assign( "RDL" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ) );//檔案格式錯誤
        str.append( ",LoadComponent" );
        str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        Assert( 0, str.c_str() ); */
        return false;
    }

    pData += sizeof( sComHeader );

    sRoleAction* pAction = NULL;
    sRoleComponent* pRoleComponent = NULL;
    sFrame* pFrame = NULL;
    char szTexFile[50];
    char szActName[20];
    D3DXVECTOR3 vScale, vPosition;

    // 更新紀錄的動作資料
    for( unsigned short i=0; i<pHeader->usActCount1; ++i )
    {
        strcpy( szActName, (char*)pData );
        pData += 20;

        // 取得動作指標
        pAction = GetAction( szActName );

        if( pAction == NULL )
        {
            /* string str;
            str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 874, CodeInlineText_Text ) );//找不到與檔案中紀錄符合的動作名稱
            Assert( 0, str.c_str() ); */
            return false;
        }

        // 取得零件指標
        pRoleComponent = pAction->GetComponent( pHeader->szComponentName1 );

        if( pRoleComponent == NULL )
        {
            /* string str;
            str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 865, CodeInlineText_Text ) );//找不到與檔案中紀錄符合的零件名稱
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
                pData += 20;    // 預留欄位

                // 讀取材質
                if( szTexFile[0] == '\0' )
                {
                    pFrame->pImage = NULL;
                }
                else
                {
                    if( m_RoleKind == Role_Leader )
                        pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
                    else
                        pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
                }

                // 更新畫面矩陣
                pFrame->RefreshMatrix( vScale, vPosition );
            }
        }
    }

    // 更新紀錄的動作資料
    for( unsigned short i=0; i<pHeader->usActCount2; ++i )
    {
        strcpy( szActName, (char*)pData );
        pData += 20;

        // 取得動作指標
        pAction = GetAction( szActName );

        if( pAction == NULL )
        {
            /* string str;
            str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 874, CodeInlineText_Text ) );//找不到與檔案中紀錄符合的動作名稱
            Assert( 0, str.c_str() ); */
            return false;
        }

        // 取得零件指標
        pRoleComponent = pAction->GetComponent( pHeader->szComponentName2 );

        if( pRoleComponent == NULL )
        {
            /* string str;
            str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 865, CodeInlineText_Text ) );//找不到與檔案中紀錄符合的零件名稱
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
                pData += 20;    // 預留欄位

                // 讀取材質
                if( szTexFile[0] == '\0' )
                {
                    pFrame->pImage = NULL;
                }
                else
                {
                    if( m_RoleKind == Role_Leader )
                        pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
                    else
                        pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );
                }

                // 更新畫面矩陣
                pFrame->RefreshMatrix( vScale, vPosition );
            }
        }
    }

    bool bImportSound = *(bool*)pData;
    pData += sizeof( bool );

    // 更新動作的聲音
    if( bImportSound )
    {
        strcpy( szActName, (char*)pData );
        pData += 20;

        // 取得動作指標
        pAction = GetAction( szActName );
        if( pAction == NULL )
        {
            /* string str;
            str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 874, CodeInlineText_Text ) );//找不到與檔案中紀錄符合的動作名稱
            Assert( 0, str.c_str() ); */
            return false;
        }

        strcpy( pAction->szSoundFile1, (char*)pData );
        pData += 50;
        strcpy( pAction->szSoundFile2, (char*)pData );
        pData += 50;
        strcpy( pAction->szSoundFile3, (char*)pData );
        pData += 50;
    }

    return true;
}
//----------------------------------------------------------------------------
// 卸載部位檔案
//----------------------------------------------------------------------------
bool RoleBase::UnloadPart(const char* pFilename, CRPCFactory* pRPCFactory)
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
    
    if( pMem->Data == NULL ) return false;

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

	memcpy(ptemp , pMem->Data , pMem->DataSize) ;

	
    //char* pToken = strtok( pText, "\n\r" );
	//cosiann
	char* pToken = strtok( ptemp, "\n\r" );

    while( pToken )
    {
		m_pRoleRenderer->UnloadRPCFile( pToken );
        //this->HideImage( pToken );        
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
        // TODO: 更改為使用 LoadRPCFile
        //this->HideImage( pToken );
        this->UnloadRPCFile( pToken, pRPCFactory );

        pToken = strtok( 0, "\n\r" );
    }

    return true;
#endif


}
//----------------------------------------------------------------------------
// 載入部位檔案
//----------------------------------------------------------------------------
bool RoleBase::LoadPart(const char* pFilename, CRPCFactory* pRPCFactory)
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
    
    if( pMem->Data == NULL ) return false;
        

	try
	{
		pText = (char*)(pMem->Data);

	}catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what() ) ;
	}

#else

    memPack* pMem = LoadFromPacket( pFilename );
    if (pMem==NULL) return false;

    vector<char> Data( pMem->DataSize+1 );
    char* pText = &Data.front();
    memset( pText, 0, Data.size() );
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
	memcpy(ptemp , pMem->Data , pMem->DataSize) ;

	//cosiann
	char* pToken = strtok( ptemp, "\n\r" );

    while( pToken )
    {
        this->LoadRPCFile( pToken, pRPCFactory );

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
        this->LoadRPCFile( pToken, pRPCFactory );

        pToken = strtok( 0, "\n\r" );
    }

    return true;
#endif



}

//----------------------------------------------------------------------------
// 取得動作指標
//----------------------------------------------------------------------------
sRoleAction* RoleBase::GetAction( const char* pActName )
{
    sRoleActionList::iterator itEnd ( m_ActionList.end() );
    for( sRoleActionList::iterator i = m_ActionList.begin(); i != itEnd; ++i )
    {
        if( strcmp( (*i)->szActionName, pActName ) == 0 )
            return *i;
    }

    return NULL;
}

//----------------------------------------------------------------------------
// 設定顏色
//----------------------------------------------------------------------------
void RoleBase::SetColor( DWORD Color )
{
	
	m_Color = Color;

}
void RoleBase::SetPickColor(bool bPick , DWORD Color )
{
	m_bPickColor = bPick ; 
	m_pickColor = Color;

}
//----------------------------------------------------------------------------
// 取得顏色
//----------------------------------------------------------------------------
const D3DCOLOR& RoleBase::GetColor()
{
	return m_Color;
}

//----------------------------------------------------------------------------
// 設定顯示判斷子
//----------------------------------------------------------------------------
void RoleBase::SetVisible( bool bValue)
{
	if(m_bVisible != bValue )
	{
		if(bValue)
		{			
			TDG_GAMEEVENTSYSTEM_POST( GameEvent::ROLE_SHOW , GERoleInScreen( m_RoleId, true ) );
		}
		else
		{
			TDG_GAMEEVENTSYSTEM_POST( GameEvent::ROLE_HIDE , GERoleInScreen( m_RoleId, false ) );
		}
	}
	m_bVisible = bValue;
}

//----------------------------------------------------------------------------
// 取得顯示判斷子
//----------------------------------------------------------------------------
bool RoleBase::GetVisible()
{
    return m_bVisible;
}

//----------------------------------------------------------------------------
// 設定方向
//----------------------------------------------------------------------------
void RoleBase::SetDirection( RoleDirect eDirect )
{
    m_eRoleDir = eDirect;
}

//----------------------------------------------------------------------------
// 取得方向
//----------------------------------------------------------------------------
RoleDirect RoleBase::GetDirection()
{
    return m_eRoleDir;
}

//----------------------------------------------------------------------------
// 設定位置
//---------------------------------------------------------------------------
void RoleBase::SetPosition( const D3DXVECTOR3& vPos )
{
    m_vPosition = vPos;
    m_bMatrixChange = true;
}

//----------------------------------------------------------------------------
// 設定縮放
//---------------------------------------------------------------------------
void RoleBase::SetScale( const D3DXVECTOR3& vScale )
{
    m_vScale = vScale;
    m_bMatrixChange = true;
}

//----------------------------------------------------------------------------
// 取得位置
//---------------------------------------------------------------------------
const D3DXVECTOR3& RoleBase::GetPosition() const
{
    return m_vPosition;
}

//----------------------------------------------------------------------------
// 取得縮放
//---------------------------------------------------------------------------
const D3DXVECTOR3& RoleBase::GetScale() const
{
	return m_vScale;
}

//----------------------------------------------------------------------------
// 設定含偏移量位置
//---------------------------------------------------------------------------
/* void RoleBase::SetPositoinWithOffset( const D3DXVECTOR3& vPos )
{
    m_vPosition = vPos + g_PosOffset;
    m_vPosition.y = g_GameMapManager.GetHeight( m_vPosition.x, m_vPosition.z )+0.45f;
    m_bMatrixChange = true;

    //moder
    if( m_RoleKind == Role_Leader )
    {
        g_SoundManager._SetListenerPos( m_vPosition );

        m_iCellPosX = (int)(( vPos.x - g_GameMapManager.GetLeft()   ) * 0.5f);
        m_iCellPosY = (int)(( vPos.z - g_GameMapManager.GetBottom() ) * 0.5f);
    }
} */

//----------------------------------------------------------------------------
// 取得減掉偏移量位置
//---------------------------------------------------------------------------
D3DXVECTOR3 RoleBase::GetPositoinWithoutOffset()
{
    return m_vPosition - g_PosOffset;
}
//----------------------------------------------------------------------------
// 取得角色中心點位置
//---------------------------------------------------------------------------
D3DXVECTOR3 RoleBase::GetCenterPosition()
{
    return m_vPosition + g_HeightOffset;
}

//----------------------------------------------------------------------------
// 取得角色種類
//---------------------------------------------------------------------------
RoleKind RoleBase::GetRoleKind()
{
    return m_RoleKind;
}

//----------------------------------------------------------------------------
// 設定更新時間
//---------------------------------------------------------------------------
void RoleBase::SetUpdateTime( const DWORD& dwUpdateTime )
{
    m_dwUpdateTime = dwUpdateTime;
}

//----------------------------------------------------------------------------
// 設定顯示影子判斷子
//---------------------------------------------------------------------------
void RoleBase::SetShowShadow( bool bValue )
{
    m_bShowShadow = bValue;
}

void RoleBase::SetRoleData(sRoleAllRes *pRoleData)
{
	m_BaseData = pRoleData ; 

	m_vScale.x = m_BaseData->fScale ; 
	m_vScale.y = m_BaseData->fScale ; 
	m_vScale.z = m_BaseData->fScale ; 

    m_fShadowSize        = m_BaseData->usShadowSize ;
    m_fShadowOffset      = m_BaseData->usShadowOffset ;
    m_usBoundingBoxSize  = m_BaseData->usBoundingBoxSize;
    m_usDeadBoundBoxSize = m_BaseData->usDeadBoundBoxSize;

}

//----------------------------------------------------------------------------
// 取得顯示影子判斷子
//---------------------------------------------------------------------------
bool RoleBase::GetShowShadow()
{
    return m_bShowShadow;
}

//----------------------------------------------------------------------------
// 更新螢幕位置
//----------------------------------------------------------------------------
void RoleBase::FrameMoveScreenPos( const D3DXMATRIX& matInv )
{
    if( m_bShowName )
    {
        D3DXMATRIX matView, matProj;
        D3DVIEWPORT9 ViewPort;

        
        m_pDevice->GetViewport( &ViewPort );
        m_pDevice->GetTransform( D3DTS_VIEW, &matView );
        m_pDevice->GetTransform( D3DTS_PROJECTION, &matProj );

		D3DXVec3TransformCoord( &m_ScreenPos, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &matInv );
        D3DXVec3Project( &m_ScreenPos, &m_ScreenPos, &ViewPort, &matProj, &matView, &m_matWorld );

		D3DXVec3TransformCoord( &m_NpcEventScreenPos, &D3DXVECTOR3(0.0f, 6.2f, 0.0f), &matInv );
        D3DXVec3Project( &m_NpcEventScreenPos, &m_NpcEventScreenPos, &ViewPort, &matProj, &matView, &m_matWorld );

		//對話框
		if( m_bShowTalk )
        {
            D3DXVec3TransformCoord( &m_NpcTalkScreenPos, &D3DXVECTOR3(0.0f, 6.2f, 0.0f), &matInv );
            D3DXVec3Project( &m_NpcTalkScreenPos, &m_NpcTalkScreenPos, &ViewPort, &matProj, &matView, &m_matWorld );
        }
    }
}
//----------------------------------------------------------------------------
//更新角色其他需要顯示的介面相關位置
void RoleBase::FrameMove2DInfo(const DWORD dwPassTime , const D3DXVECTOR4 &rectRange)
{
	// 越南專用
    // 更新稱號位置
    if( m_bShowName )
       FrameMoveNameTitle();

	// 更新NPC對話
	FrameMoveNPCTalk( dwPassTime );
}
/**
 * @brief 影格移動時的事件
 */
void RoleBase::OnFrameMove()
{
    char* particleFileName = NULL;
    vector<char*> soundFileNames;
    
    switch (m_RoleKind)
    {
    case Role_Leader:
    case Role_Costar:
        // 播放音效
        if (m_pCurrRATAct != NULL)
        {
            if (m_pCurrRATAct->usPlaySoundTime1 == m_usFrameNum) soundFileNames.push_back(m_pCurrRATAct->szSoundFile1);
            if (m_pCurrRATAct->usPlaySoundTime2 == m_usFrameNum) soundFileNames.push_back(m_pCurrRATAct->szSoundFile2);
            if (m_pCurrRATAct->usPlaySoundTime3 == m_usFrameNum) soundFileNames.push_back(m_pCurrRATAct->szSoundFile3);
            // 播放分子
            if (m_pCurrRATAct->usPlayParticleTime == m_usFrameNum) particleFileName = m_pCurrRATAct->szParticleFile;
        }
        break;
    default:
        // 播放音效
        if (m_pNowAction != NULL)
        {
            if (m_pNowAction->usPlaySoundTime1 == m_usFrameNum) soundFileNames.push_back(m_pNowAction->szSoundFile1);
            if (m_pNowAction->usPlaySoundTime2 == m_usFrameNum) soundFileNames.push_back(m_pNowAction->szSoundFile2);
            if (m_pNowAction->usPlaySoundTime3 == m_usFrameNum) soundFileNames.push_back(m_pNowAction->szSoundFile3);
            // 播放分子
            if (m_pNowAction->usPlayParticleTime == m_usFrameNum) particleFileName = m_pNowAction->szParticleFile;
        }
    }
    // 播放分子
    if ((particleFileName != NULL) && (strlen(particleFileName) > 0))
    {
        AddParticle(this, particleFileName);
    }
    // 播放音效
    if (!soundFileNames.empty())
    {
        unsigned int iSoundNum = rand() % soundFileNames.size();
        char* soundFileName = soundFileNames.at(iSoundNum);
        if ((soundFileName != NULL) && (strlen(soundFileName) > 0))
        {
            g_SoundManager._Play3DSoundInMemory(soundFileName, m_vPosition);
        }
    }
}
//----------------------------------------------------------------------------
// 狀態更新
//----------------------------------------------------------------------------
void RoleBase::FrameMoveRole(const DWORD dwPassTime, 
                             const D3DXMATRIX& matInv, 
                             const D3DXVECTOR3& vDir, 
                             const float fScale, 
                             bool bAngleChange,
                             const D3DXVECTOR3& vPosOffset, 
							 const D3DXVECTOR4 &rectRange)
{
    unsigned short usPassFrame = 0;

    // 更新動畫顯示張數
    if( m_PlayStatus == Status_Play )
    {
        m_dwPassTime += dwPassTime;
        mFrameListCacheOutOfData = true;
        //usPassFrame = static_cast<unsigned short>(m_dwPassTime/m_dwUpdateTime);
        //if( usPassFrame > 0 )
        //    m_bPlaySound = true;

        //m_usFrameNum += usPassFrame;

        //if( m_usFrameNum > m_usFrameMax )
        //{
        //    m_usFrameNum %= m_usFrameMax;
        //    if( m_usFrameNum == 0 )
        //        m_usFrameNum = 1;
        //    if( !m_bPlayLoop )
        //    {
        //        m_usFrameNum = m_usFrameMax;
        //        m_PlayStatus = Status_Pause;
        //        m_bPlaySound = false;
        //    }
        //}
        //m_dwPassTime = (m_dwPassTime%m_dwUpdateTime);

        while (m_dwPassTime >= m_dwUpdateTime)
        {
            ++m_usFrameNum;
            if (m_usFrameNum > m_usFrameMax)
            {
                if (m_bPlayLoop)
                {
                    m_usFrameNum = 1;
                }
                else
                {
                    m_usFrameNum = m_usFrameMax;
                    m_PlayStatus = Status_Pause;
                }
            }
            OnFrameMove(); // 影格移動時的事件
            m_dwPassTime -= m_dwUpdateTime;
        }
    }


    //  檢查攝影機垂直角度是否改變
    if( bAngleChange )
        m_bMatrixChange = true;
    m_vPosOffset = vPosOffset;
    //if( m_bMatrixChange )
    //{
        // 縮放
        //float fScaleY = m_vScale.y;

        //if( fScale != 1.0f )
            //fScaleY = fScale * m_vScale.y;

        // 更新位置矩陣
        D3DXVECTOR3 vPos( m_vPosition + m_vPosOffset );
        // 計算世界矩陣
        D3DXMatrixScaling( &m_matWorld, m_vScale.x, m_vScale.y*fScale, m_vScale.z );
        D3DXMatrixTranslation( &m_matPos, vPos.x, vPos.y, vPos.z );
        D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &m_matPos );

		if(m_RoleKind == Role_Item && !GetDrop())
		{

			D3DXMATRIX rotMat ; 


			float scale = sin(m_DropScale) * 0.5f ; 

			BYTE Alpha = 255 - (BYTE)(sin(m_DropScale) * 155.0f) ; 

			D3DXVECTOR3 tempScale(m_vScale) ; 
			tempScale.x += scale;
			tempScale.y += scale;
			tempScale.z += scale;

			m_DropScale += PI * dwPassTime / g_DropTime ;

			m_Color |= 0xFF000000 ;

			m_Color &= D3DCOLOR_ARGB( Alpha, 255, 255, 255 );

			if(m_DropScale > PI)
			{
				m_DropScale = 0.0f ; 
				m_Color |= 0xFF000000 ; 

			}



			D3DXMATRIX tempWorldMat ; 
			D3DXMATRIX tempScaleMat ; 

			D3DXMatrixScaling(&tempScaleMat , tempScale.x , tempScale.y , tempScale.z) ; 
			D3DXMatrixTranslation( &m_matPos, vPos.x, vPos.y, vPos.z );
			D3DXMatrixMultiply( &tempWorldMat, &tempScaleMat, &m_matPos );

			 D3DXMatrixMultiply( &m_matDrop, &matInv, &tempWorldMat );
		}
		else
		if(GetDrop())
		{
			D3DXVECTOR3 dir(0.0f , 1.0f , 0.0f) ; 

			vPos += sin(m_DropDownLoop) * dir * 5.0f ; 

			m_DropDownLoop += PI * dwPassTime / g_DropTime ; 

			if(m_DropDownLoop > PI)
			{
				m_DropDownLoop = PI * 0.3f ; 
				SetDrop(false) ; 
			}

			D3DXMATRIX scaleMat ; 
			D3DXMATRIX posMat ; 
			D3DXMATRIX tempWorldMat ; 
			// 計算世界矩陣
			D3DXMatrixScaling( &scaleMat, m_vScale.x, m_vScale.y*fScale, m_vScale.z );
			D3DXMatrixTranslation( &posMat, vPos.x, vPos.y, vPos.z );

			D3DXMatrixMultiply( &tempWorldMat, &scaleMat, &posMat );

			 D3DXMatrixMultiply( &m_matDrop, &matInv, &tempWorldMat );


		}
		else
		if(GetShake())
		{
			
			vPos += sin(m_angleLoop) * m_shakeDir * 1.5f ; 

			m_angleLoop += PI_2 * dwPassTime / g_shakeTime ; 

			if(m_angleLoop > PI_2)
			{
				m_angleLoop = 0.0f ; 
				SetShake(false , D3DXVECTOR3(0.0f , 0.0f , 0.0f)) ; 
			}
			
			D3DXMATRIX scaleMat ; 
			D3DXMATRIX posMat ; 
			D3DXMATRIX tempWorldMat ; 
			// 計算世界矩陣
			D3DXMatrixScaling( &scaleMat, m_vScale.x, m_vScale.y*fScale, m_vScale.z );
			D3DXMatrixTranslation( &posMat, vPos.x, vPos.y, vPos.z );

			D3DXMatrixMultiply( &tempWorldMat, &scaleMat, &posMat );

			 D3DXMatrixMultiply( &m_matShake, &matInv, &tempWorldMat );

		}

        m_bMatrixChange = false;



  //  }

    // 計算反向矩陣
    m_matInv = matInv;
    D3DXMatrixMultiply( &m_matInvWorld, &m_matInv, &m_matWorld );
    D3DXMatrixMultiply( &m_matInvPos, &m_matInv, &m_matPos );
	
	// 更新角色在螢幕上的位置(2D平面座標)
    FrameMoveScreenPos( matInv );
	{
		D3DXMATRIX matView, matProj;
		D3DVIEWPORT9 ViewPort;


		m_pDevice->GetViewport( &ViewPort );
		m_pDevice->GetTransform( D3DTS_VIEW, &matView );
		m_pDevice->GetTransform( D3DTS_PROJECTION, &matProj );
		RECT& rcBody = m_rcBody;

		D3DXVECTOR3 vrcBody[8];
		static D3DXVECTOR3 bbxs[] = 
		{
			D3DXVECTOR3( -1.2f, 0.0f, 0.0f ),
			D3DXVECTOR3( -1.2f, 5.2f, 0.0f ),
			D3DXVECTOR3(  1.2f, 0.0f, 0.0f ),
			D3DXVECTOR3(  1.2f, 5.2f, 0.0f ),			

			D3DXVECTOR3( -1.2f, 0.0f, 0.0f ) + g_HeightOffset,
			D3DXVECTOR3( -1.2f, 5.2f, 0.0f ) + g_HeightOffset,
			D3DXVECTOR3(  1.2f, 0.0f, 0.0f ) + g_HeightOffset,
			D3DXVECTOR3(  1.2f, 5.2f, 0.0f ) + g_HeightOffset			
		};
		for (uint i = 0 ; i < 8 ; ++i)
		{
			D3DXVec3TransformCoord( &vrcBody[i], &bbxs[i], &matInv );
			D3DXVec3Project( &vrcBody[i], &vrcBody[i], &ViewPort, &matProj, &matView, &m_matWorld );
		}

/*		rcBody.left = vrcBody[0].x;
		rcBody.right = vrcBody[0].x;
		rcBody.top = vrcBody[0].y;
		rcBody.bottom = vrcBody[0].y;
		for (uint i = 1 ; i < 8 ; ++i)
		{
			if(rcBody.left > vrcBody[i].x)
				rcBody.left = vrcBody[i].x;

			if(rcBody.right < vrcBody[i].x)
				rcBody.right = vrcBody[i].x;

			if(rcBody.top > vrcBody[i].y)
				rcBody.top = vrcBody[i].y;

			if(rcBody.bottom < vrcBody[i].y)
				rcBody.bottom = vrcBody[i].y;
		}
*/
		//因為rcBody.XXXX的形別是LONG, 但是vrcBody.XXX的型別是float
		//改成以下寫法可以減少LONG和float轉換的次數
		//
		D3DXVECTOR2 minP, maxP;
		minP.x = maxP.x = vrcBody[0].x;
		minP.y = maxP.y = vrcBody[0].y;

		for (uint i = 1 ; i < 8 ; ++i)
		{
			if(minP.x > vrcBody[i].x)
				minP.x = vrcBody[i].x;

			if(maxP.x < vrcBody[i].x)
				maxP.x = vrcBody[i].x;

			if(minP.y > vrcBody[i].y)
				minP.y = vrcBody[i].y;

			if(maxP.y < vrcBody[i].y)
				maxP.y = vrcBody[i].y;
		}
	
		rcBody.left = (LONG)minP.x;
		rcBody.right = (LONG)maxP.x;
		rcBody.top = (LONG)minP.y;
		rcBody.bottom = (LONG)maxP.y;
	}
	// 更新角色 其他需要顯示的2D介面相關位置
	// p.s m_ScreenPos 已被更新
	FrameMove2DInfo( dwPassTime , rectRange);
	

    D3DXVECTOR3 vCross;
    float fDotRet = 0.0f;

    //判斷主角方向
    if( m_eRoleDir == RD_South )
    {
        D3DXVec3Cross( &vCross, &g_vSouth, &vDir );
        fDotRet = D3DXVec3Dot( &g_vSouth, &vDir );
    }
    else if( m_eRoleDir == RD_ESouth )
    {
        D3DXVec3Cross( &vCross, &g_vESouth, &vDir );
        fDotRet = D3DXVec3Dot( &g_vESouth, &vDir );
    }
    else if( m_eRoleDir == RD_East )
    {
        D3DXVec3Cross( &vCross, &g_vEast, &vDir );
        fDotRet = D3DXVec3Dot( &g_vEast, &vDir );
    }
    else if( m_eRoleDir == RD_ENorth )
    {
        D3DXVec3Cross( &vCross, &g_vENorth, &vDir );
        fDotRet = D3DXVec3Dot( &g_vENorth, &vDir );
    }
    else if( m_eRoleDir == RD_North )
    {
        D3DXVec3Cross( &vCross, &g_vNorth, &vDir );
        fDotRet = D3DXVec3Dot( &g_vNorth, &vDir );
    }
    else if( m_eRoleDir == RD_WNorth )
    {
        D3DXVec3Cross( &vCross, &g_vWNorth, &vDir );
        fDotRet = D3DXVec3Dot( &g_vWNorth, &vDir );
    }
    else if( m_eRoleDir == RD_West )
    {
        D3DXVec3Cross( &vCross, &g_vWest, &vDir );
        fDotRet = D3DXVec3Dot( &g_vWest, &vDir );
    }
    else if( m_eRoleDir == RD_WSouth )
    {
        D3DXVec3Cross( &vCross, &g_vWSouth, &vDir );
        fDotRet = D3DXVec3Dot( &g_vWSouth, &vDir );
    }

    float fAngleBuf = 0.0f;

    if( (fDotRet <= 1.0f) && (fDotRet >= -1.0f) )
    {
        if( vCross.y > 0.0f )
            fAngleBuf = acos( fDotRet );
        else
            fAngleBuf = 6.283185482f - acos( fDotRet );
    }

         if((fAngleBuf < 0.392699093f) || (fAngleBuf > 5.890486389f))    m_eShowDir = RD_North;
    else if((fAngleBuf > 0.392699093f) && (fAngleBuf < 1.178097278f))    m_eShowDir = RD_ENorth;
    else if((fAngleBuf > 1.178097278f) && (fAngleBuf < 1.963495463f))    m_eShowDir = RD_East;
    else if((fAngleBuf > 1.963495463f) && (fAngleBuf < 2.748893648f))    m_eShowDir = RD_ESouth;
    else if((fAngleBuf > 2.748893648f) && (fAngleBuf < 3.534291834f))    m_eShowDir = RD_South;
    else if((fAngleBuf > 3.534291834f) && (fAngleBuf < 4.319690019f))    m_eShowDir = RD_WSouth;
    else if((fAngleBuf > 4.319690019f) && (fAngleBuf < 5.105088204f))    m_eShowDir = RD_West;
    else if((fAngleBuf > 5.105088204f) && (fAngleBuf < 5.890486389f))    m_eShowDir = RD_WNorth;

    // 更新分子
    ControlParticles( this, dwPassTime*0.001f, &m_matWorld, m_vPosition , rectRange);

	//////////////////////////////////////////////////////////////////////////
	// rage

		static int preAction = 0;
		static DWORD divideCount = 0 ; 
		static int iCellPosX = 0;
		static int iCellPosY = 0;

		if(m_bRage)
		{
			divideCount += dwPassTime ; 

			if(divideCount >= 50 && (m_iNowAction != preAction || 
				(m_iCellPosX != iCellPosX || m_iCellPosY != iCellPosY)))
			{
				SetVidide(true) ; 
				divideCount = 0 ; 
				preAction = m_iNowAction;
				iCellPosX = m_iCellPosX;
				iCellPosY = m_iCellPosY;
			
			}
			else
			{
				SetVidide(false) ; 
			}
		}



}

//////////////////////////////////////////////////////////////////////////
/**
 * @brief 播放動作聲音
 * @note useless, an empty function
 * @todo remove it!
 */
void RoleBase::PlayActionSound( RoleKind iRoleKind )
{
//    // 因為主角跟配角是新的作法 所以連動作指標都被換掉了  故分開處裡
//	switch( iRoleKind )
//	{
//		case Role_Leader:
//		case Role_Costar:
//		{
//			//----------------------------------------------------
//			if( m_pCurrRATAct == NULL )
//				return;
//
//			// 播放音效
//			if( ( m_pCurrRATAct->bySoundKind != 0 ) && ( m_bPlaySound ) )
//			{
//				if( m_pCurrRATAct->bySoundKind == 1 )
//				{
//					if( m_pCurrRATAct->usPlaySoundTime1 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pCurrRATAct->szSoundFile1[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile1, m_vPosition );
//					}
//				}
//				else if( m_pCurrRATAct->bySoundKind == 2 )
//				{
//					if( m_pCurrRATAct->usPlaySoundTime1 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pCurrRATAct->szSoundFile1[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile1, m_vPosition );
//					}
//					else if( m_pCurrRATAct->usPlaySoundTime2 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pCurrRATAct->szSoundFile2[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile2, m_vPosition );
//					}
//				}
//				else if( m_pCurrRATAct->bySoundKind == 3 )
//				{
//					if( m_pCurrRATAct->usPlaySoundTime1 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pCurrRATAct->szSoundFile1[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile1, m_vPosition );
//					}
//					else if( m_pCurrRATAct->usPlaySoundTime2 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pCurrRATAct->szSoundFile2[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile2, m_vPosition );
//					}
//					else if( m_pCurrRATAct->usPlaySoundTime3 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pCurrRATAct->szSoundFile3[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile3, m_vPosition );
//					}
//				}
//				else if( m_pCurrRATAct->bySoundKind == 4 )
//				{
//					if( m_pCurrRATAct->usPlaySoundTime1 == m_usFrameNum )
//					{
//						int iSoundNum = rand() % 2;
//						if( iSoundNum == 0 )
//						{
//							if( m_pCurrRATAct->szSoundFile1[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile1, m_vPosition );
//						}
//						else
//						{
//							if( m_pCurrRATAct->szSoundFile2[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile2, m_vPosition );
//						}
//
//						m_bPlaySound = false;
//					}
//				}
//				else if( m_pCurrRATAct->bySoundKind == 5 )
//				{
//					if( m_pCurrRATAct->usPlaySoundTime1 == m_usFrameNum )
//					{
//						int iSoundNum = rand() % 3;
//						if( iSoundNum == 0 )
//						{
//							if( m_pCurrRATAct->szSoundFile1[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile1, m_vPosition );
//						}
//						else if( iSoundNum == 1 )
//						{
//							if( m_pCurrRATAct->szSoundFile2[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile2, m_vPosition );
//						}
//						else
//						{
//							if( m_pCurrRATAct->szSoundFile3[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pCurrRATAct->szSoundFile3, m_vPosition );
//						}
//
//						m_bPlaySound = false;
//					}
//				}
//			}
//			//----------------------------------------------------
//		}// end case
//		break;
//	
//		case Role_Monster:
//		case Role_Pet:
//		case Role_NPC:
//		case Role_Item:
//		{
//			//----------------------------------------------------
//			if( m_pNowAction == NULL )
//				return;
//
//			// 播放音效
//			if( ( m_pNowAction->bySoundKind != 0 ) && ( m_bPlaySound ) )
//			{
//				if( m_pNowAction->bySoundKind == 1 )
//				{
//					if( m_pNowAction->usPlaySoundTime1 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pNowAction->szSoundFile1[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile1, m_vPosition );
//					}
//				}
//				else if( m_pNowAction->bySoundKind == 2 )
//				{
//					if( m_pNowAction->usPlaySoundTime1 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pNowAction->szSoundFile1[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile1, m_vPosition );
//					}
//					else if( m_pNowAction->usPlaySoundTime2 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pNowAction->szSoundFile2[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile2, m_vPosition );
//					}
//				}
//				else if( m_pNowAction->bySoundKind == 3 )
//				{
//					if( m_pNowAction->usPlaySoundTime1 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pNowAction->szSoundFile1[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile1, m_vPosition );
//					}
//					else if( m_pNowAction->usPlaySoundTime2 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pNowAction->szSoundFile2[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile2, m_vPosition );
//					}
//					else if( m_pNowAction->usPlaySoundTime3 == m_usFrameNum )
//					{
//						m_bPlaySound = false;
//
//						if( m_pNowAction->szSoundFile3[0] != '\0' )
//							g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile3, m_vPosition );
//					}
//				}
//				else if( m_pNowAction->bySoundKind == 4 )
//				{
//					if( m_pNowAction->usPlaySoundTime1 == m_usFrameNum )
//					{
//						int iSoundNum = rand() % 2;
//						if( iSoundNum == 0 )
//						{
//							if( m_pNowAction->szSoundFile1[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile1, m_vPosition );
//						}
//						else
//						{
//							if( m_pNowAction->szSoundFile2[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile2, m_vPosition );
//						}
//
//						m_bPlaySound = false;
//					}
//				}
//				else if( m_pNowAction->bySoundKind == 5 )
//				{
//					if( m_pNowAction->usPlaySoundTime1 == m_usFrameNum )
//					{
//						int iSoundNum = rand() % 3;
//						if( iSoundNum == 0 )
//						{
//							if( m_pNowAction->szSoundFile1[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile1, m_vPosition );
//						}
//						else if( iSoundNum == 1 )
//						{
//							if( m_pNowAction->szSoundFile2[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile2, m_vPosition );
//						}
//						else
//						{
//							if( m_pNowAction->szSoundFile3[0] != '\0' )
//								g_SoundManager._Play3DSoundInMemory( m_pNowAction->szSoundFile3, m_vPosition );
//						}
//
//						m_bPlaySound = false;
//					}
//				}
//			}
//			//----------------------------------------------------
//		}// end case
//		break;
//
//		default :
//			return;
//	
//	}// end switch
}

//////////////////////////////////////////////////////////////////////////
//新的渲染繪製
//////////////////////////////////////////////////////////////////////////
void RoleBase::RenderRoleSkin(RoleBase *pRole , CRenderer *pRender)
{
    float slope = static_cast<float>(m_dwPassTime) / static_cast<float>(m_dwUpdateTime);
	D3DCOLOR color ; 


	if(IsPickColor())
	{
		color = m_pickColor ; 
	}
	else
	{
		color = m_Color ; 
	}

	if( !m_pRoleRenderer->Render( m_pDevice, m_matInvWorld, color, m_eShowDir, m_usFrameNum, m_RoleKind , pRender , slope, m_bDivide) )
	{
		
		//失敗就不是要畫主角配角
		RenderFramesOriginal(pRole , pRender);

		//FlushFrameOriginal() ; 
	}
}

//////////////////////////////////////////////////////////////////////////
//舊的畫法，npc、怪物、道具使用，
//////////////////////////////////////////////////////////////////////////

void RoleBase::FlushFrameOriginal()
{
	// 繪製圖形
	sFrameList::iterator iter(mFrameListCache.begin()) ; 

	for(; iter != mFrameListCache.end() ; ++iter)
	{
		sFrame* pFrame = *iter;

		D3DXMATRIX matWorld;

		D3DXMatrixMultiply( &matWorld, &pFrame->matLocal, &m_matInvWorld );  

	
		m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );
		m_pDevice->SetTexture( 0, pFrame->pImage->pTexture );

		//if( m_Color == 0xFFFFFFFF )

		DrawRole( pFrame, m_Color&pFrame->Color );
		//else
		//DrawRole( pFrame, m_Color );
	}

}

void RoleBase::RenderFramesOriginal(RoleBase *pRole , CRenderer *pRender)
{
	if(m_pNowAction == NULL)
	{
		if(strcmp(m_szNowActName , "") != 0)
		{
			//因為thread的關係，server丟來的封包(0xC050)裡面會指定哪一種動作(PushOrderStack)
			//資料上會在最後的SharePlay得到一個初始，但是ActionList還尚未初始完畢（經由一個thread），
			//而此封包只會丟一次，如：稻草人或卡點中的怪物。所以這邊要重新check一下。
			//針對怪物部分。
			SharePlay(m_szNowActName , m_bPlayLoop) ;

			if(m_pNowAction == NULL)
			{
				return ; 
			}
		}
		else
		{
			return ; 
		}

	}

	D3DXMATRIX matWorld;

	//道具部分，不能作此優化！
	if( m_RoleKind != Role_Item && pRole != NULL && m_pNowAction == pRole->m_pNowAction && m_eShowDir == pRole->m_eShowDir)
	{
 	//	m_shareCatch = true ; 

       _FOREACH(sFrameList, mFrameListCache, pos)
        {
           g_RoleMemoryMgr.ReleaseTempFrame(*pos) ; // delete (*pos);
        }
		mFrameListCache.clear() ; 

		sFrameList::iterator iter(pRole->mFrameListCache.begin()) ; 

		for(; iter != pRole->mFrameListCache.end() ; ++iter)
		{
			sFrame* pFrame = (*iter); 

            /// <yuming> add linear attribute
            RoleFrmPtr pNewFrame = g_RoleMemoryMgr.GetTempFrame() ; //new sFrame();
         //   *pNewFrame = *pFrame;
			pNewFrame->Clone(pFrame) ; 
            pFrame = pNewFrame;
            /// </yuming>


			if(GetDrop())
			{
				D3DXMatrixMultiply( &matWorld, &pFrame->matLocal, &m_matDrop );  

			}
			else
			if(GetShake())
			{
				D3DXMatrixMultiply( &matWorld, &pFrame->matLocal, &m_matShake );  
				
			}
			else
			{
				D3DXMatrixMultiply( &matWorld, &pFrame->matLocal, &m_matInvWorld );  

			}
			
			if(IsPickColor())
			{
				pRender->CreateFrameOp(pFrame , matWorld , m_pickColor&pFrame->Color) ; 

			}
			else
			{
				pRender->CreateFrameOp(pFrame , matWorld , m_Color&pFrame->Color) ; 
			}
			


			mFrameListCache.push_back(pFrame) ; 
		}
		return ; 
	}
		

	//m_shareCatch = false ; 

   // if (mFrameListCacheOutOfData)
    {
        // clear current frames container
        _FOREACH(sFrameList, mFrameListCache, pos)
        {
           g_RoleMemoryMgr.ReleaseTempFrame(*pos) ; // delete (*pos);
        }
        mFrameListCache.clear();

        float slope = static_cast<float>(m_dwPassTime) / static_cast<float>(m_dwUpdateTime);

		sRoleComponentList::iterator iter(m_pNowAction->ComponentList->begin()) ; 

		
       // _FOREACH(sRoleComponentList, m_pNowAction->ComponentList, ComPos)
		for(; iter != m_pNowAction->ComponentList->end() ; ++iter)
	    {
		    sRoleComponent* pCom = *iter;// (*ComPos);

		    // 檢查是否顯示零件
		    if( pCom->bShow == false )
			    continue;
		    // 取得方向
		    sComponentDir& Dir = *(pCom->ComponentDirs[m_eShowDir]);
		    //  檢查畫面串列是否為空
		    //if( Dir.FrameList.empty() )
		    //continue;            
		    // 判斷目前播放畫面張數

			sFrameList::reverse_iterator iter(Dir.FrameList->rbegin()) ; 

            RoleFrmPtr pNextFrame = NULL;

			 for( ; iter != Dir.FrameList->rend() ; ++iter )
			 {
				sFrame* pFrame = (*iter); 
            
			    if (pFrame->usPlayTime <= m_usFrameNum)
                {
                    /// <yuming> add linear attribute
                    RoleFrmPtr pNewFrame = g_RoleMemoryMgr.GetTempFrame() ; //new sFrame();

					pNewFrame->Clone(pFrame) ; 
                   // *pNewFrame = *pFrame;

                    if (pNextFrame == NULL)
                    {
                        if (m_bPlayLoop)
                        {
                            pNextFrame = Dir.FrameList->front();
                        }
                    }
                    pNewFrame->Lerp(slope, pNextFrame);
					pFrame = pNewFrame;
                    /// </yuming>

		            // 判斷是否已讀取材質
		            if (pFrame && pFrame->pImage && pFrame->pImage->pTexture)
		            {
						if(m_RoleKind == Role_Item && !GetDrop())
						{
							D3DXMatrixMultiply( &matWorld, &pFrame->matLocal, &m_matDrop );  

						}else
						if(GetDrop())
						{
							D3DXMatrixMultiply( &matWorld, &pFrame->matLocal, &m_matDrop );  

						}
						else							
						if(GetShake())
						{
							D3DXMatrixMultiply( &matWorld, &pFrame->matLocal, &m_matShake );  
							
						}
						else
						{
							D3DXMatrixMultiply( &matWorld, &pFrame->matLocal, &m_matInvWorld );  

						}
						if(IsPickColor())
						{
							pRender->CreateFrameOp(pFrame , matWorld , m_pickColor&pFrame->Color) ; 

						}
						else
						{
							pRender->CreateFrameOp(pFrame , matWorld , m_Color&pFrame->Color) ; 
						}
						mFrameListCache.push_back(pFrame);
                        break;
		            }
                }
                else
                {
                    pNextFrame = pFrame;
                }
			 }
       //     _RFOREACH(sFrameList, Dir.FrameList, FrmPos)
       //     {
       //         sFrame* pFrame = (*FrmPos);            
			    //if (pFrame->usPlayTime <= m_usFrameNum)
       //         {
		     //       // 判斷是否已讀取材質
		     //       if (pFrame && pFrame->pImage && pFrame->pImage->pTexture)
		     //       {
			    //        mFrameListCache.push_back(pFrame);
       //                 break;
		     //       }
       //         }
       //     }
	    }

	    // 零件圖層排序
	 //   mFrameListCache.sort( FrameSort );

   //     mFrameListCacheOutOfData = false;
    }

	
}

/**
 * @brief an empty function, do not use anymore!!
 * @todo remove it!
 */
void RoleBase::RenderRole()
{
    //if( m_pNowAction == NULL ) return;        

    // 播放動作聲音
    //PlayActionSound( m_RoleKind );

    //RenderFrames();

    // 繪製分子
    //RenderParticles( this, g_Camera.GetViewInverseMatrix() );
    //RenderParticles( this, m_matInv );

    // 繪製稱號
    //RenderNameTitle();

	// 繪製對話
	//RenderNPCTalk();

	// 其他
	//RenderOther();
}
//----------------------------------------------------------------------------
void RoleBase::RenderOther()
{

}
//----------------------------------------------------------------------------
// 取得繪製畫面
//----------------------------------------------------------------------------
//short RoleBase::GetRenderFrame( sFrame* ppFrameArray[], short shSize )
//{
//    if( m_pNowAction == NULL )
//        return 0;
//
//    // 播放動作聲音
//    PlayActionSound( m_RoleKind );
//
//    sRoleComponent* pComponent = NULL;
//    sFrameList::iterator itKey, itNowKey, itEnd;
//    short shFrameCount = 0;
//
//    sRoleComponentList::iterator end ( m_pNowAction->ComponentList.end() );
//    for( sRoleComponentList::iterator k=m_pNowAction->ComponentList.begin(); k!=end; ++k )
//    {
//        pComponent = *k;
//
//        // 檢查是否顯示零件
//        if( pComponent->bShow == false )
//            continue;
//
//        //  檢查畫面串列是否為空
//        if( pComponent->ComponentDirs[m_eShowDir].FrameList.empty() )
//            continue;
//
//        // 判斷目前播放畫面張數
//        itEnd = pComponent->ComponentDirs[m_eShowDir].FrameList.end();
//        for( itKey = pComponent->ComponentDirs[m_eShowDir].FrameList.begin(); itKey != itEnd; ++itKey )
//        {
//            if( (*itKey)->usPlayTime <= m_usFrameNum )
//                itNowKey = itKey;
//            else
//                break;
//        }
//
//        // 判斷是否已讀取材質
//        if( ( (*itNowKey)->pImage != NULL ) && ( (*itNowKey)->pImage->pTexture != NULL ) )
//        {
//            if( shFrameCount >= shSize )
//            {
//                /* string str;
//                str.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 867, CodeInlineText_Text ));//陣列可用空間不足
//                Assert( 0, str.c_str() ); */
//                return shFrameCount;
//            }
//
//            ppFrameArray[shFrameCount] = (*itNowKey);
//            ++shFrameCount;
//        }
//    }
//
//    // 繪製稱號
//    RenderNameTitle();
//
//    return shFrameCount;
//}


//----------------------------------------------------------------------------
// 繪製陰影
//----------------------------------------------------------------------------
void RoleBase::RenderShadow(CRenderer *prender , const float fOffsetX, const float fOffsetY, const float fOffsetZ)
{
    // 陰影偏移植
    //D3DXVECTOR3 v;
    //g_Camera.GetViewDistance(&v);
    //float vx = v.x / v.y;
    //float vz = v.z / v.y;
    //float vy = 0.1f;
    //D3DXVECTOR3 vOffset( fOffsetX*m_fShadowOffset-vx, fOffsetY-vy, fOffsetZ*m_fShadowOffset-vz );
    D3DXVECTOR3 vOffset( fOffsetX*m_fShadowOffset, fOffsetY, fOffsetZ*m_fShadowOffset );
    vOffset -= m_vPosOffset;

	prender->CreateShadowOp(m_vPosition , vOffset , m_fShadowSize) ; 

    // 計算陰影大小與位置
    //D3DXVECTOR3 v1, v2, v3, v4;
    //v1 = m_vPosition + D3DXVECTOR3( -m_fShadowSize, 0.0f, -m_fShadowSize ) - vOffset;
    //v2 = m_vPosition + D3DXVECTOR3( -m_fShadowSize, 0.0f,  m_fShadowSize ) - vOffset;
    //v3 = m_vPosition + D3DXVECTOR3(  m_fShadowSize, 0.0f, -m_fShadowSize ) - vOffset;
    //v4 = m_vPosition + D3DXVECTOR3(  m_fShadowSize, 0.0f,  m_fShadowSize ) - vOffset;

    // 計算陰影高度
    /*if( ( m_RoleKind == Role_Costar ) || ( m_RoleKind == Role_Leader ) )
    {
        v1.y = g_GameMapManager.GetHeight( v1.x, v1.z ) + 0.85f;
        v2.y = g_GameMapManager.GetHeight( v2.x, v2.z ) + 0.85f;
        v3.y = g_GameMapManager.GetHeight( v3.x, v3.z ) + 0.85f;
        v4.y = g_GameMapManager.GetHeight( v4.x, v4.z ) + 0.85f;
        
        v1.y = m_vPosition.y;
        v2.y = m_vPosition.y;
        v3.y = m_vPosition.y;
        v4.y = m_vPosition.y;
    }*/

    // 繪製陰影
    //DrawShadow( v1, v2, v3, v4 );
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
// 播放
//----------------------------------------------------------------------------
void RoleBase::Play( const char* szActionName, bool bLoop )
{
	PlaySkinAct( szActionName, bLoop );
}

//////////////////////////////////////////////////////////////////////////
//檢查目前動作是否有更新，新的
//////////////////////////////////////////////////////////////////////////
void RoleBase::PlaySkinAct(const char* szActionName, bool bLoop )
{
	m_bPlayLoop = bLoop;
	m_PlayStatus = Status_Play;
	m_usFrameNum = 1;
	m_bPlaySound = true;
	//檢查播放動作是否與現在動作相同
	if( strcmp( m_szNowActName, szActionName ) == 0 )
	{
		return;
	}
	//設定動作名稱
	strcpy( m_szNowActName, szActionName );
	m_usFrameMax = 1;

	//取得角色動作陣列
	//SRATActFile* pRATAct = m_pRoleRenderer->Play(szActionName, bLoop );
	m_pCurrRATAct = m_pRoleRenderer->Play(szActionName, bLoop );
	if(m_pCurrRATAct)
	{
		m_dwUpdateTime = m_pCurrRATAct->dwUpdateTime;

		for(size_t i(0); i < m_pCurrRATAct->vRATComList.size(); ++i)
		{
			SRATComFile& rRATCom = m_pCurrRATAct->vRATComList.at(i);

            /// <yuming date="20100506"> get max frame num in all direction
            for (int j = 0; j < RD_Count; ++j) {
                if (m_usFrameMax < rRATCom.RATDir[j].usMaxFrameNum)
                    m_usFrameMax = rRATCom.RATDir[j].usMaxFrameNum;
            }
			//if( m_usFrameMax < rRATCom.RATDir[m_eShowDir].usMaxFrameNum )
			//{
			//	m_usFrameMax = rRATCom.RATDir[m_eShowDir].usMaxFrameNum;
			//}
            /// </yuming>
		}
	}
	
}

//----------------------------------------------------------------------------
// 共享模式播放
//----------------------------------------------------------------------------
void RoleBase::SharePlay( const char* szActionName, bool bLoop, bool bIsNPC )
{
    if( m_pActionList == NULL )
        return;

    m_bPlayLoop = bLoop;
    m_PlayStatus = Status_Play;
    m_bPlaySound = true;

	//因為會在npc update過程中，會一直更新
	if( !bIsNPC )
		m_usFrameNum = 1;

	//檢查播放動作是否與現在動作相同。
	//thread在進行時，當m_pNowAction != NULL時m_pNowAction裡面的資料一定會完整的！
	if(m_pNowAction != NULL)
	{
		if( strcmp( m_szNowActName, szActionName ) == 0 )
			return;
	}
	



    strcpy( m_szNowActName, szActionName );
    m_usFrameMax = 1;
    m_pNowAction = NULL;

    // 尋找動作名稱並設定最大播放張數
    sRoleActionList::iterator itActEnd( m_pActionList->end() );
    for( sRoleActionList::iterator i = m_pActionList->begin(); i != itActEnd; ++i )
    {
		sRoleAction* pRoleAct = (*i);
		if(pRoleAct == 0 || pRoleAct->szActionName)
        if( strcmp( (*i)->szActionName, m_szNowActName ) == 0 )
        {
            m_pNowAction = (*i);
			
			if( bIsNPC )
			{
				m_bPlayLoop = m_pNowAction->bPlayLoop;
			}
            
			m_dwUpdateTime = m_pNowAction->dwUpdateTime;

            sRoleComponentList::iterator itComEnd( m_pNowAction->ComponentList->end() );
            for( sRoleComponentList::iterator j = m_pNowAction->ComponentList->begin(); j != itComEnd; ++j )
            {
                if( m_usFrameMax < (*j)->ComponentDirs[m_eShowDir]->usMaxFrameNum )
                    m_usFrameMax = (*j)->ComponentDirs[m_eShowDir]->usMaxFrameNum;
            }

            break;
        }
    }
}

//----------------------------------------------------------------------------
// 停止(after Stop position goto top)
//----------------------------------------------------------------------------
void RoleBase::Stop()
{
    m_PlayStatus = Status_Stop;
    m_usFrameNum = 1;
    m_bPlaySound = false;
}

//----------------------------------------------------------------------------
// 暫停
//----------------------------------------------------------------------------
void RoleBase::Pause( unsigned short usFrameNum )
{
    if( ( usFrameNum < 1 ) || ( usFrameNum > m_usFrameMax ) )
        return;

    m_PlayStatus = Status_Pause;
    m_usFrameNum = usFrameNum;
}

//----------------------------------------------------------------------------
//計算稱號長度
//----------------------------------------------------------------------------
void RoleBase::SetNameTitle( const char16* szName )
{
	

    m_NameTitle = szName ;

	
	const LONG cxMaxExtent = 1024;
	//CTextObject* pTextObject = g_FDOXML.MakeTextObject(
		//g_pD3DDevice, g_FDOXML.GetBoldFont(), D3DXCOLOR(0xFFFFFFFF), m_NameTitle.c_str(), -1, cxMaxExtent);
	CTextObject* pTextObject = g_FDOXML.MakeTextObject(
		g_pD3DDevice, g_FDOXML.GetNormalFont(), D3DXCOLOR(0xFFFFFFFF), m_NameTitle.c_str(), -1, cxMaxExtent);
	assert(pTextObject);
	m_TextObjectNameTitle = *pTextObject;
	delete pTextObject;

    // TextObject


    // 計算文字區域大小
    SIZE size;
    m_TextObjectNameTitle.GetExtent(size);
    m_NameWidth  = size.cx / 2;
    m_NameRect.right = m_NameRect.left + size.cx;
    m_NameRect.bottom = m_NameRect.top + size.cy;
}

//----------------------------------------------------------------------------
//計算稱號長度
//----------------------------------------------------------------------------
void RoleBase::SetNameTitleHV( const WCHAR* szName )
{
    m_NameTitle =  szName;

    // TextObject
    const LONG cxMaxExtent = 1024;
    CTextObject* pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, g_FDOXML.GetBoldFont(), D3DXCOLOR(0xFFFFFFFF), m_NameTitle.c_str(), -1, cxMaxExtent);
    assert(pTextObject);
    m_TextObjectNameTitle = *pTextObject;
    delete pTextObject;

    // 計算文字區域大小
    SIZE size;
    m_TextObjectNameTitle.GetExtent(size);
    m_NameWidth  = size.cx / 2;
    m_NameRect.right = m_NameRect.left + size.cx;
    m_NameRect.bottom = m_NameRect.top + size.cy;
}

//----------------------------------------------------------------------------
//事件狀態
//----------------------------------------------------------------------------
void RoleBase::SetCondition( int SP )
{
    wstring m_IDTitle;
    std::wstring str;

    switch( SP )
    {
        case 0:
            str = L"";
            m_IDTitle = str;
            break;

        case 1:
            str = L"<emo49/>";
            m_IDTitle = str;
            break;

        case 2:
            str = L"<emo48/>";
            m_IDTitle = str;
            break;
    }//end switch

    const LONG cxMaxExtent = 1024;
    CTextObject* pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, g_FDOXML.GetBoldFont(), D3DXCOLOR(0xFFFFFFFF), m_IDTitle.c_str(), -1, cxMaxExtent);
    assert(pTextObject);
    m_TextObjectIDTitle = *pTextObject;
    delete pTextObject;

    // 計算文字區域大小
    SIZE size;
    m_TextObjectIDTitle.GetExtent(size);
    m_IDWidth  = size.cx / 2;
	m_IDRect.right  = (LONG)( m_NpcEventScreenPos.x + m_IDWidth );
	m_IDRect.bottom = (LONG)( m_NpcEventScreenPos.y + m_IDWidth );
    //m_IDRect.right = m_IDRect.left + size.cx;
    //m_IDRect.bottom = m_IDRect.top + size.cy;
}

//----------------------------------------------------------------------------
//設定顯示稱號
//----------------------------------------------------------------------------
void RoleBase::SetShowNameTitle( bool bShow )
{
    m_bShowName = bShow;
}

//----------------------------------------------------------------------------
//設定稱號顏色
//----------------------------------------------------------------------------
void RoleBase::SetNameTitleColor( D3DCOLOR clrColor )
{
    m_NameColor = clrColor;
}

//----------------------------------------------------------------------------
//更新稱號
//----------------------------------------------------------------------------
void RoleBase::FrameMoveNameTitle()
{
    //計算稱號顯示位置
    int cx = RectWidth(m_NameRect);
    int cy = RectHeight(m_NameRect);
    int Ix = RectWidth(m_IDRect);
    int Iy = RectHeight(m_IDRect);

    m_NameRect.left = (LONG)(m_ScreenPos.x - m_NameWidth);
    m_NameRect.right = (LONG)(m_NameRect.left + cx);
	m_IDRect.left   = (LONG)( m_NpcEventScreenPos.x - m_IDWidth);
	m_TalkRect.right  = (LONG)( m_NpcEventScreenPos.x + Ix);

    if( m_RoleKind == Role_Leader )
    {
        m_NameRect.top = (LONG)(m_ScreenPos.y + 20.0f);
        m_NameRect.bottom = (LONG)(m_NameRect.top + cy);
        m_IDRect.top = (LONG)( m_NpcEventScreenPos.y - NPCDifference );
        m_IDRect.bottom = (LONG)( m_NpcEventScreenPos.y + Iy + NPCDifference);
    }
    else
    {
        m_NameRect.top = (LONG)(m_ScreenPos.y + 4.0f);
        m_NameRect.bottom = (LONG)(m_NameRect.top + cy);
		m_IDRect.top = (LONG)( m_NpcEventScreenPos.y - NPCDifference );
        m_IDRect.bottom = (LONG)( m_NpcEventScreenPos.y + Iy + NPCDifference);
    }
}

//----------------------------------------------------------------------------
//顯示稱號
//----------------------------------------------------------------------------
void RoleBase::RenderNameTitle()
{

    if( m_bShowName ) 
	{
		
		// Render
		//g_RenderSystem.EndFrame();
// 
		CDXUTDialogResourceManager* pManager = GetDialogResourceManager();

// 		pManager->m_pStateBlock->Capture();       
// 		pManager->m_pSprite->Begin( D3DXSPRITE_DONOTSAVESTATE );
// 		
// 		m_TextObjectNameTitle.ChangeDefaultColor(m_NameColor);
// 		m_TextObjectNameTitle.DrawText(0, &m_NameRect, NULL, true);
// 		m_TextObjectNameTitle.ChangeDefaultColor(m_NameColor);
// 		m_TextObjectIDTitle.DrawText(0, &m_IDRect, NULL, true);
		
// 		pManager->m_pSprite->Flush();
// 		pManager->m_pSprite->End();
// 		pManager->m_pStateBlock->Apply();
// 		
		//g_RenderSystem.BeginFrame();
		
		
        
        //void DrawText(DWORD dwFormat, const RECT* pRectView, ID3DXSprite* pSprite, bool bShadow);
        //g_RenderSystem.DrawText( m_NameTitle.c_str(), &m_NameRect, m_NameColor );
    } // end if


}

//----------------------------------------------------------------------------
void RoleBase::AddMessage( const string &Message )// 未完成
{
	//m_pListBox->AddItem( Message, WHITE );
}
//---------------------------------------------------------------------------
void RoleBase::AddList()// 未完成
{
   /* HRESULT hr = S_OK;
	m_pListBox = new CDXUTListBox();

	m_pListBox->SetID( 1 );
	m_pListBox->SetLocation( 0, 0 );
    m_pListBox->SetSize( 180, 20 );
    m_pListBox->SetStyle( 0 );
	m_pListBox->SetVisible( false );
	m_pListBox->SetEnabled( false );
	m_pListBox->SetScrollBarWidth( 0 );*/
}

//---------------------------------------------------------------------------
void RoleBase::SetNpcTalk( DWORD dwPassTime )	// 實驗成功但未使用
{
	if( !m_bNpcTalkSelf )
		return;

	if( false == m_bShowTalk )
	{
		m_iTalkWaitNowTime += dwPassTime;
	}
	if( m_iTalkWaitNowTime >= m_iTalkWaitTime )
	{
		if( m_RoleKind == Role_NPC )
		{
			SYSTEMTIME st;
			GetLocalTime( &st );
			int p = ( random(99) + st.wMilliseconds ) % 5;
			if( p == 0 )
				SetNPCTalkText("噁~~~我的腳好臭");
			else if( p == 1 )
				SetNPCTalkText("嗯~~~~~~~\n呃阿!!(放屁)");
			else if( p == 2 )
				SetNPCTalkText("唉...肚子好餓...");
			else if( p == 3 )
				SetNPCTalkText("唔...屁股好癢\n(抓抓抓)");
			else
				SetNPCTalkText("看!!!有飛碟!!");
		}
		else if( m_RoleKind == Role_Item )
			SetNPCTalkText("我掉到地上了！！！\n快!快!快撿我！！");

		m_iTalkWaitNowTime = 0;
	}	
}

//---------------------------------------------------------------------------
void RoleBase::SetNPCTalkList()// 未完成
{
	// 開始顯示對話框
    m_TalkPassTime = 0;
    m_bShowTalk = false;	
	m_bUseList = true;
	m_pListBox->SetVisible( true );

}
//----------------------------------------------------------------------------
//設定對話訊息
//----------------------------------------------------------------------------
void RoleBase::SetNPCTalkText( const char* szText )
{
    
    m_TextString.assign(AnsiToUnicode( szText ) );
   
    // 計算 TextObject.
    if (m_pTextObject)
        delete m_pTextObject;
    m_pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, NULL, D3DXCOLOR(0xFFFFFFFF), // 文字預設顏色：白色。
        m_TextString.c_str(), -1, 180);
    
    // 開始顯示對話框
    m_TalkPassTime = 0;
    m_bShowTalk = true;
	m_bUseList = false;

}

//----------------------------------------------------------------------------
//更新對話訊息
//----------------------------------------------------------------------------
void RoleBase::FrameMoveNPCTalk( const DWORD& dwPassTime )
{
    if( !m_bShowTalk && !m_bUseList )
        return;

    m_TalkPassTime += dwPassTime;

    if( m_TalkPassTime >= m_TalkShowTime )
    {
        if( m_bUseList )
		{
			int iListSelIndex = m_pListBox->GetSelected();
			int iMaxSize = m_pListBox->GetItemSize();
			if( iListSelIndex = iMaxSize - 1)
			{
				m_TalkPassTime = 0;
				m_bUseList = false;
				m_pListBox->SetVisible( false );
			}
			else
			{
				m_pListBox->SetSelected( iListSelIndex += m_pListBox->m_ScrollBar.GetPageSize() - 1 );
				m_TalkPassTime = 0;
			}
		}
		else
		{
			m_TalkPassTime = 0;
			m_bShowTalk = false;
			//BtnResult = Wait;
		}
        return;
    }
}

//----------------------------------------------------------------------------
//渲染對話訊息
//----------------------------------------------------------------------------

void RoleBase::RenderNPCTalk()
// Program procedure, called by RoleManager::DrawGameRole().
// RenderTalk with TextObject.
{
	
    if (!m_bShowTalk && !m_bUseList)
        return;
    if (!m_pTextObject)
        return;
        
    if( m_bUseList )
	{
		m_pListBox->SetLocation( m_IDRect.left + m_IDWidth/2 - 90, m_IDRect.top - 20 );
	}
	
	if( m_bShowTalk )
	{
		// 設定文字預設屬性。
		const DWORD dwFormat = DT_SINGLELINE|DT_LEFT|DT_TOP|DT_NOCLIP; // DrawText() format
		g_FDOXML.SetFormat(dwFormat);
	    
		// 取得對話方框的大小。
		SIZE sizeText;
		m_pTextObject->GetExtent(sizeText);
		LONG cxTalkRect = sizeText.cx + 10;
		LONG cyTalkRect = sizeText.cy + 10;
	    
		// 計算對話框顯示位置
		m_TalkRect.left   = (LONG)( m_NpcTalkScreenPos.x - cxTalkRect / 2);
		m_TalkRect.right  = (LONG)( m_NpcTalkScreenPos.x + cxTalkRect / 2);
		m_TalkRect.bottom = (LONG)( m_NpcTalkScreenPos.y);
		m_TalkRect.top    = (LONG)( m_NpcTalkScreenPos.y - cyTalkRect );

	    
		// 繪製對話框背景。
		m_pDevice->SetTexture(0, NULL);
		DrawBackground(m_TalkRect);
	    
		// 繪製對話內容。
		RECT rcRender;
		CopyRect(&rcRender, &m_TalkRect);
		rcRender.left += 5;
		rcRender.top += 5;
	    
		// Begin
		CDXUTDialogResourceManager* pManager = GetDialogResourceManager();
		pManager->m_pStateBlock->Capture();       
		pManager->m_pSprite->Begin( D3DXSPRITE_DONOTSAVESTATE );
		// Render
		m_pTextObject->DrawText(dwFormat, &rcRender, NULL, false);  
		// End
		pManager->m_pSprite->End();
		pManager->m_pStateBlock->Apply();
	}
}
//----------------------------------------------------------------------------
//點擊偵測函式
//----------------------------------------------------------------------------

bool RoleBase::PickRole(D3DXVECTOR3 &vPickRayDir , D3DXVECTOR3 &vPickRayOrig)
{
    D3DXVECTOR3 v0, v1, v2, v3;

	
	
    if( m_iNowAction == ACT_Dead )
    {
        if( m_usDeadBoundBoxSize == 1 )
        {
            D3DXVec3TransformCoord( &v0, &g_01_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_01_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_01_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_01_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 2 )
        {
            D3DXVec3TransformCoord( &v0, &g_02_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_02_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_02_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_02_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 3 )
        {
            D3DXVec3TransformCoord( &v0, &g_03_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_03_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_03_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_03_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 4 )
        {
            D3DXVec3TransformCoord( &v0, &g_04_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_04_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_04_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_04_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 5 )
        {
            D3DXVec3TransformCoord( &v0, &g_05_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_05_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_05_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_05_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 6 )
        {
            D3DXVec3TransformCoord( &v0, &g_06_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_06_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_06_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_06_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 7 )
        {
            D3DXVec3TransformCoord( &v0, &g_07_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_07_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_07_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_07_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 8 )
        {
            D3DXVec3TransformCoord( &v0, &g_08_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_08_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_08_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_08_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 9 )
        {
            D3DXVec3TransformCoord( &v0, &g_09_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_09_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_09_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_09_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 10 )
        {
            D3DXVec3TransformCoord( &v0, &g_10_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_10_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_10_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_10_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 11 )
        {
            D3DXVec3TransformCoord( &v0, &g_11_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_11_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_11_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_11_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 12 )
        {
            D3DXVec3TransformCoord( &v0, &g_12_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_12_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_12_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_12_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 13 )
        {
            D3DXVec3TransformCoord( &v0, &g_13_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_13_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_13_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_13_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 14 )
        {
            D3DXVec3TransformCoord( &v0, &g_14_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_14_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_14_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_14_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 15 )
        {
            D3DXVec3TransformCoord( &v0, &g_15_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_15_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_15_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_15_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 16 )
        {
            D3DXVec3TransformCoord( &v0, &g_16_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_16_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_16_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_16_BBX3, &m_matInvPos );
        }
		else if( m_usDeadBoundBoxSize == 17 )
        {
            D3DXVec3TransformCoord( &v0, &g_17_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_17_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_17_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_17_BBX3, &m_matInvPos );
        }
		else if( m_usDeadBoundBoxSize == 18 )
        {
            D3DXVec3TransformCoord( &v0, &g_18_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_18_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_18_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_18_BBX3, &m_matInvPos );
        }
		else if( m_usDeadBoundBoxSize == 19 )
        {
            D3DXVec3TransformCoord( &v0, &g_19_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_19_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_19_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_19_BBX3, &m_matInvPos );
        }
        else
        {
            return false;
        }
    }
    else
    {
        if( m_usBoundingBoxSize == 1 )
        {
            D3DXVec3TransformCoord( &v0, &g_01_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_01_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_01_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_01_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 2 )
        {
            D3DXVec3TransformCoord( &v0, &g_02_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_02_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_02_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_02_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 3 )
        {
            D3DXVec3TransformCoord( &v0, &g_03_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_03_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_03_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_03_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 4 )
        {
            D3DXVec3TransformCoord( &v0, &g_04_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_04_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_04_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_04_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 5 )
        {
            D3DXVec3TransformCoord( &v0, &g_05_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_05_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_05_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_05_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 6 )
        {
            D3DXVec3TransformCoord( &v0, &g_06_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_06_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_06_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_06_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 7 )
        {
            D3DXVec3TransformCoord( &v0, &g_07_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_07_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_07_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_07_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 8 )
        {
            D3DXVec3TransformCoord( &v0, &g_08_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_08_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_08_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_08_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 9 )
        {
            D3DXVec3TransformCoord( &v0, &g_09_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_09_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_09_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_09_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 10 )
        {
            D3DXVec3TransformCoord( &v0, &g_10_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_10_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_10_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_10_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 11 )
        {
            D3DXVec3TransformCoord( &v0, &g_11_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_11_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_11_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_11_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 12 )
        {
            D3DXVec3TransformCoord( &v0, &g_12_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_12_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_12_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_12_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 13 )
        {
            D3DXVec3TransformCoord( &v0, &g_13_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_13_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_13_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_13_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 14 )
        {
            D3DXVec3TransformCoord( &v0, &g_14_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_14_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_14_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_14_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 15 )
        {
            D3DXVec3TransformCoord( &v0, &g_15_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_15_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_15_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_15_BBX3, &m_matInvPos );
        }
		else if( m_usBoundingBoxSize == 16 )
        {
            D3DXVec3TransformCoord( &v0, &g_16_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_16_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_16_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_16_BBX3, &m_matInvPos );
        }
		else if( m_usBoundingBoxSize == 17 )
        {
            D3DXVec3TransformCoord( &v0, &g_17_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_17_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_17_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_17_BBX3, &m_matInvPos );
        }
		else if( m_usBoundingBoxSize == 18 )
        {
            D3DXVec3TransformCoord( &v0, &g_18_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_18_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_18_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_18_BBX3, &m_matInvPos );
        }
		else if( m_usBoundingBoxSize == 19 )
        {
            D3DXVec3TransformCoord( &v0, &g_19_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_19_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_19_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_19_BBX3, &m_matInvPos );
        }
        else
        {
            return false;
        }
    }

	if(Pick(vPickRayDir , vPickRayOrig , v0, v1, v2) || Pick(vPickRayDir , vPickRayOrig , v1, v3, v2))
		return true ; 
  
    return false;
}

bool RoleBase::PickRole( const int& iX, const int& iY )
{
    D3DXVECTOR3 v0, v1, v2, v3;

	
	
    if( m_iNowAction == ACT_Dead )
    {
        if( m_usDeadBoundBoxSize == 1 )
        {
            D3DXVec3TransformCoord( &v0, &g_01_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_01_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_01_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_01_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 2 )
        {
            D3DXVec3TransformCoord( &v0, &g_02_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_02_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_02_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_02_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 3 )
        {
            D3DXVec3TransformCoord( &v0, &g_03_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_03_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_03_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_03_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 4 )
        {
            D3DXVec3TransformCoord( &v0, &g_04_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_04_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_04_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_04_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 5 )
        {
            D3DXVec3TransformCoord( &v0, &g_05_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_05_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_05_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_05_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 6 )
        {
            D3DXVec3TransformCoord( &v0, &g_06_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_06_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_06_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_06_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 7 )
        {
            D3DXVec3TransformCoord( &v0, &g_07_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_07_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_07_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_07_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 8 )
        {
            D3DXVec3TransformCoord( &v0, &g_08_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_08_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_08_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_08_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 9 )
        {
            D3DXVec3TransformCoord( &v0, &g_09_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_09_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_09_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_09_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 10 )
        {
            D3DXVec3TransformCoord( &v0, &g_10_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_10_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_10_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_10_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 11 )
        {
            D3DXVec3TransformCoord( &v0, &g_11_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_11_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_11_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_11_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 12 )
        {
            D3DXVec3TransformCoord( &v0, &g_12_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_12_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_12_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_12_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 13 )
        {
            D3DXVec3TransformCoord( &v0, &g_13_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_13_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_13_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_13_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 14 )
        {
            D3DXVec3TransformCoord( &v0, &g_14_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_14_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_14_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_14_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 15 )
        {
            D3DXVec3TransformCoord( &v0, &g_15_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_15_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_15_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_15_BBX3, &m_matInvPos );
        }
        else if( m_usDeadBoundBoxSize == 16 )
        {
            D3DXVec3TransformCoord( &v0, &g_16_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_16_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_16_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_16_BBX3, &m_matInvPos );
        }
		else if( m_usDeadBoundBoxSize == 17 )
        {
            D3DXVec3TransformCoord( &v0, &g_17_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_17_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_17_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_17_BBX3, &m_matInvPos );
        }
		else if( m_usDeadBoundBoxSize == 18 )
        {
            D3DXVec3TransformCoord( &v0, &g_18_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_18_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_18_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_18_BBX3, &m_matInvPos );
        }
		else if( m_usDeadBoundBoxSize == 19 )
        {
            D3DXVec3TransformCoord( &v0, &g_19_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_19_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_19_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_19_BBX3, &m_matInvPos );
        }
        else
        {
            return false;
        }
    }
    else
    {
        if( m_usBoundingBoxSize == 1 )
        {
            D3DXVec3TransformCoord( &v0, &g_01_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_01_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_01_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_01_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 2 )
        {
            D3DXVec3TransformCoord( &v0, &g_02_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_02_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_02_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_02_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 3 )
        {
            D3DXVec3TransformCoord( &v0, &g_03_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_03_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_03_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_03_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 4 )
        {
            D3DXVec3TransformCoord( &v0, &g_04_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_04_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_04_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_04_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 5 )
        {
            D3DXVec3TransformCoord( &v0, &g_05_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_05_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_05_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_05_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 6 )
        {
            D3DXVec3TransformCoord( &v0, &g_06_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_06_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_06_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_06_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 7 )
        {
            D3DXVec3TransformCoord( &v0, &g_07_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_07_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_07_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_07_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 8 )
        {
            D3DXVec3TransformCoord( &v0, &g_08_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_08_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_08_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_08_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 9 )
        {
            D3DXVec3TransformCoord( &v0, &g_09_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_09_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_09_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_09_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 10 )
        {
            D3DXVec3TransformCoord( &v0, &g_10_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_10_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_10_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_10_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 11 )
        {
            D3DXVec3TransformCoord( &v0, &g_11_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_11_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_11_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_11_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 12 )
        {
            D3DXVec3TransformCoord( &v0, &g_12_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_12_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_12_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_12_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 13 )
        {
            D3DXVec3TransformCoord( &v0, &g_13_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_13_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_13_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_13_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 14 )
        {
            D3DXVec3TransformCoord( &v0, &g_14_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_14_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_14_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_14_BBX3, &m_matInvPos );
        }
        else if( m_usBoundingBoxSize == 15 )
        {
            D3DXVec3TransformCoord( &v0, &g_15_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_15_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_15_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_15_BBX3, &m_matInvPos );
        }
		else if( m_usBoundingBoxSize == 16 )
        {
            D3DXVec3TransformCoord( &v0, &g_16_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_16_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_16_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_16_BBX3, &m_matInvPos );
        }
		else if( m_usBoundingBoxSize == 17 )
        {
            D3DXVec3TransformCoord( &v0, &g_17_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_17_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_17_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_17_BBX3, &m_matInvPos );
        }
		else if( m_usBoundingBoxSize == 18 )
        {
            D3DXVec3TransformCoord( &v0, &g_18_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_18_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_18_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_18_BBX3, &m_matInvPos );
        }
		else if( m_usBoundingBoxSize == 19 )
        {
            D3DXVec3TransformCoord( &v0, &g_19_BBX0, &m_matInvPos );
            D3DXVec3TransformCoord( &v1, &g_19_BBX1, &m_matInvPos );
            D3DXVec3TransformCoord( &v2, &g_19_BBX2, &m_matInvPos );
            D3DXVec3TransformCoord( &v3, &g_19_BBX3, &m_matInvPos );
        }
        else
        {
            return false;
        }
    }

    if( Pick( iX, iY, v0, v1, v2 ) || Pick( iX, iY, v1, v3, v2 ) )
        return true;

    return false;
}

float RoleBase::GetBoundingBoxTop()
{
    switch (m_usBoundingBoxSize)
    {
        case 1:  return (g_01_BBX1.y);
        case 2:  return (g_02_BBX1.y);
        case 3:  return (g_03_BBX1.y);
        case 4:  return (g_04_BBX1.y);
        case 5:  return (g_05_BBX1.y);
        case 6:  return (g_06_BBX1.y);
        case 7:  return (g_07_BBX1.y);
        case 8:  return (g_08_BBX1.y);
        case 9:  return (g_09_BBX1.y);
        case 10: return (g_10_BBX1.y);
        case 11: return (g_11_BBX1.y);
        case 12: return (g_12_BBX1.y);
        case 13: return (g_13_BBX1.y);
        case 14: return (g_14_BBX1.y);
    }
    return 0.0f;
}

//----------------------------------------------------------------------------
// 取得相對格子位置
//----------------------------------------------------------------------------
void RoleBase::GetCellPosition( int& iPosX, int& iPosY )
{
    // if( m_RoleKind == Role_Costar )
    //{
        //m_iCellPosX = (int)(( m_vPosition.x - g_GameMapManager.GetLeft()   ) * 0.5f);
        //m_iCellPosY = (int)(( m_vPosition.z - g_GameMapManager.GetBottom() ) * 0.5f);
    //}

    iPosX = m_iCellPosX;
    iPosY = m_iCellPosY;
} 

//----------------------------------------------------------------------------
// 設定相對格子位置
//----------------------------------------------------------------------------
void RoleBase::SetCellPosition( const int& iPosX, const int& iPosY )
{
    m_iCellPosX = iPosX;
    m_iCellPosY = iPosY;
}

//----------------------------------------------------------------------------
// 取得螢幕上的位置
//----------------------------------------------------------------------------
const D3DXVECTOR3& RoleBase::GetScreenPos() const
{
    return m_ScreenPos;
}
//----------------------------------------------------------------------------
const RECT& RoleBase::GetScreenRect() const
{
	return m_rcBody;
}
// const RoleID& RoleBase::GetClientID() const 
// {
// 	return m_RoleId;
// }
//----------------------------------------------------------------------------
void RoleBase::UnloadAllRPCFile()
{
	//sRoleActionList::iterator ActPos(m_ActionList.begin());
 //   sRoleActionList::const_iterator ActEnd( m_ActionList.end() );
 //   for( ActPos; ActPos != ActEnd; ++ActPos )
 //   {
 //       sRoleAction* pAct = (*ActPos);
	//	pAct->HideComponents();
 //   }
    m_pRoleRenderer->UnloadAllRPCFile();
}
//----------------------------------------------------------------------------
//bool RoleBase::UnloadImage(const char* pFilename)
//{
//    list< boost::shared_ptr<SkinRPC> >::const_iterator end( m_SkinRPCs.end() );
//    for( list< boost::shared_ptr<SkinRPC> >::iterator pos( m_SkinRPCs.begin() ); pos!=end; ++pos )
//    {
//        if( (*pos)->strFilename == pFilename )
//        {
//            // TODO:
//            GetRPCManage()->Release( (*pos)->strFilename, (*pos) );
//
//            m_SkinRPCs.erase( pos );
//
//            return true;
//        }
//    }
//    return false;
//}
//----------------------------------------------------------------------------
} // namespace FDO
//----------------------------------------------------------------------------
