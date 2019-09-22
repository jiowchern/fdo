//
// CDXUTDialog.cpp
//
#include "StdAfx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
#include <strsafe.h>
#include "CDXUTTimer.h"
#include "FDOUT.h"
#include "PacketReader.h"
#include "JLAnimation2DManager.h"
//#include "HUISystemSet.h"
#include <cmath>
#include "J_SoundManager.h"
#include <stdio.h>
#include "iniReader.h"
#include "CFDOXML.h"
#include "DialogType.h"
#include "TextObject.h"
//#include "Global.h"
//#include "HUITalkBar.h"
#include "dxerr9.h"
#include "Mousetrap.h"
#include "Common.h"
#include "DrawPlane.h"
//#include "BinaryTableManager.h"



using namespace FDO;

#include "CDXUTDialogResourceManager.h"
#include "CDXUTStatic.h"
#include "CDXUTButton.h"
#include "CDXUTEditBox.h"
#include "CDXUTIMEEditBox.h"
#include "CDXUTListBox.h"
#include "CDXUTComboBox.h"
#include "CDXUTIconListBox.h"
#include "VariableHeightListBox.h"
#include "SmoothScrollingListBox.h"
#include "ImageControls.h"
#include "mapcontrol.h"
#include "CDXUTPuzzle.h"
#include "CDXUTTabPage.h"
#include "CDXUTTable.h"
#pragma hdrstop
#include "CDXUTDialog.h"
#include "TalkBarImageControl.h"
#include "Debug.h"
namespace UIEvent
{
	EventManager::EventManager(UIControlPtr pUI) : mpUI(pUI)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	EventManager::~EventManager()
	{
		ReleaseAllListener();
	}
	//////////////////////////////////////////////////////////////////////////
	void EventManager::ReleaseAllListener()
	{
		EventMap::iterator it = EventMap::begin();
		for ( ;it != EventMap::end() ; ++it)
		{
			delete it->second.pFncSlot;
		}
		EventMap::clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void EventManager::RiseListener	(uint nId,uint nEvent)
	{
		EventKey key;
		key.nId = nId;
		key.nEvent = nEvent;
		EventMap::iterator it = EventMap::find(key);
		if(it == EventMap::end())
			return ;

		delete it->second.pFncSlot;
		EventMap::erase(it);
	}
	//////////////////////////////////////////////////////////////////////////
	uint EventManager::SendToListener(uint nId,uint nEvent,uint nData)
	{
		EventKey key;
		key.nId = nId;
		key.nEvent = nEvent;

		std::pair<EventMap::iterator, EventMap::iterator> itp = EventMap::equal_range(key);
		EventMap::iterator it(itp.first) ;
		for (; it != itp.second ; ++it)		
		{			
			EventData data;
			data.nCtrlId = nId;
			data.nEventId = nEvent;
			data.nParam = it->second.nData;
			data.nEventData = nData;
			data.pUI = mpUI;
			if((*it->second.pFncSlot)(data) == DONE)
				return DONE;
		}
		return DONE;
	}
	
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CDXUTDialogDropTarget::CDXUTDialogDropTarget(CDXUTDialog* pOwner) : mpOwner(pOwner)
{

}
//////////////////////////////////////////////////////////////////////////
CDXUTDialogDropTarget::~CDXUTDialogDropTarget()
{

}
//////////////////////////////////////////////////////////////////////////
void CDXUTDialogDropTarget::OnDrop(DragDropInfo* pInfo) 
{
	mpOwner->SendEvent(EVENT_DIALOG_DROP,true,0,(UINT)pInfo);
	//OutputDebugString(L"==EVENT_DIALOG_DROP==\n");
}
//////////////////////////////////////////////////////////////////////////
void CDXUTDialogDropTarget::OnMouseOver(DragDropInfo* pInfo) 
{

}
//////////////////////////////////////////////////////////////////////////
void CDXUTDialogDropTarget::OnDropNotTarget(DragDropInfo* pInfo) 
{
	mpOwner->SendEvent(EVENT_DIALOG_DROP_NOT_TARGET,true,0,(UINT)pInfo);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define _OBJ_FILE_PATH	"UI\\obj.tga"
#define L_OBJ_FILE_PATH	L"UI\\obj.tga"
//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
double        CDXUTDialog::s_fTimeRefresh = 0.0f;
bool			CDXUTDialog::s_bEnableDebugInfo	=	false;
CDXUTControl* CDXUTDialog::s_pControlFocus = NULL;        // The control which has focus
CDXUTControl* CDXUTDialog::s_pControlPressed = NULL;      // The control currently pressed

CDXUTDialog*  CDXUTDialog::s_pDefFocusDialog = NULL;
CDXUTDialog*  CDXUTDialog::s_pFocusDialog = NULL;
CDXUTDialog*  CDXUTDialog::s_pMouseDialog = NULL;
static CDXUTDialog* s_pDialogDragging = NULL;

void CDXUTDialog::BeginDragDrop()
{
	
}

void CDXUTDialog::EndDragDrop()
{
	if(s_pDialogDragging)
	{
		s_pDialogDragging->m_bLButtonDown = false;
		s_pDialogDragging->m_bDragging = false;
	}
	s_pDialogDragging = 0;
}


CDXUTDialog::eSkinStyleType CDXUTDialog::sm_skinStyle = CDXUTDialog::sstHVO; //預設採用HVO的UI形式
std::string CDXUTDialog::sm_resizeMouseAnim; //resize的mouse anim
std::wstring CDXUTDialog::sm_defaultFont = L"Arial"; //預設字型
std::wstring CDXUTDialog::sm_hintTextMinimum=L"最小化"; //最小化	// Darkness Edit
std::wstring CDXUTDialog::sm_hintTextMaximum; //最大化				// Darkness Edit
std::wstring CDXUTDialog::sm_hintTextClose=L"關閉"; //關閉			// Darkness Edit
std::wstring CDXUTDialog::sm_hintTextOrigin; //復原					// Darkness Edit	
bool CDXUTDialog::sm_bShowHint = true;
HandleHookManager CDXUTDialog::m_HookMgr;

VOID CALLBACK fnDefaultEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	DialogEventUserContext* pD = (DialogEventUserContext*)pUserContext;
	pD->nRet = pD->pDlg->SendToListener(nControlID,nEvent,pD->nEventData);	
}



FDO::CRenderer* CDXUTDialog::m_pRender = NULL ; 
//////////////////////////////////////////////////////////////////////////
bool fnIsParent(CDXUTDialog* pParentDlg ,CDXUTDialog* pDlg)
{
	const CDXUTSubDialog* pCtrl = pDlg->GetParent();	
	while(pCtrl)
	{		
		const CDXUTDialog* pParent = pCtrl->GetParent();
		if(pParent == pParentDlg )
		{
			return true;
		}
		else if(pParent)
		{
			pCtrl = pParent->GetParent();
		}
		else if(pParent == 0)
		{
			return false;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------
// CDXUTDialog class
//--------------------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
CDXUTDialog::CDXUTDialog() 
: m_bControlOrder(false) , UIEventManager(this) ,m_pCallbackEvent(fnDefaultEvent),m_pCallbackEventUserContext(&mDEUC) 
, mHnd(new ResoueceSystem::HANDLEIMPL(0,0)), m_ID(-1) , mCDXUTDialogDropTarget(this) , m_pPipeVec()
,m_pParent(0) , mbUseCloseBtn(false)
, m_BackgroundColor((DWORD)0) 
{
	Register(mHnd,this);

	mDEUC.nEventData = 0;
	mDEUC.pDlg = this;
    Initialize();

}
//--------------------------------------------------------------------------------------
CDXUTDialog::CDXUTDialog(list<CDXUTDialog*> *pDialogList)// : m_pDialogList( pDialogList )
: m_bControlOrder(false) , UIEventManager(this),m_pCallbackEvent(fnDefaultEvent),m_pCallbackEventUserContext(&mDEUC)
, mHnd(new ResoueceSystem::HANDLEIMPL(0,0)), m_ID(-1), mCDXUTDialogDropTarget(this) , m_pPipeVec()
,m_pParent(0) , mbUseCloseBtn(false)
,m_BackgroundColor((DWORD)0)
{
	Register(mHnd,this);

	mDEUC.nEventData = 0;
	mDEUC.pDlg = this;
    Initialize();
    JoinDialogList(pDialogList);

}
//--------------------------------------------------------------------------------------
void CDXUTDialog::JoinDialogList(list<CDXUTDialog*> *pDialogList)
{
    if (!pDialogList)
        return;

    if (m_pDialogList) {
        m_pDialogList->remove(this);
    } // end if
    
    m_pDialogList = pDialogList;
	if(m_pDialogList)
		m_pDialogList->push_back(this);

	
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SeparateDialogList() //脫離List ,意味這不被Manager掌控
{	
	
	if(m_pDialogList)
	{		
		fnDisablePublicState(this);
		m_pDialogList->remove(this);
		m_pDialogList = 0;
		m_pParent = 0;
	}
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::Initialize()
{
    m_pDialogList = NULL;
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
    m_dxDrag = 0;
    m_dyDrag = 0;

    // Darkness Add
    m_x2 = 0;
    m_y2 = 0;
    m_x_r = 0;
    m_y_b = 0;
    m_width2 = 0;
    m_height2 = 0;
    m_width2 = 0;
    m_height2 = 0;
    m_iSubX = 0;
    m_iSubY = 0;
    m_ColorRed = 255;
    m_ColorGreen = 255;
    m_ColorBlue = 255;
    m_ColorAlpha = 255;
	m_iALLHintAlpha = 300;// 這個300只是一個判斷欲的數字,無意義

    m_bMouseOver = false; // ming add
    
    m_bVisible = true;
    m_bCaption = false;
    m_bSizeChange = false;
    m_bCanSizeChange = false;
    m_bCanMinimized = false;
    m_bMinimized = false;
    m_bUseSideFrame = false;
    m_bAptoticSize = false;
    m_wszCaption[0] = L'\0';
    m_nCaptionHeight = 18;

    // Darkness Add
    m_iChangeSizeMode = 0;
    m_iFrameGauge = 5;
    m_SideGauge = 5;
    m_iMaxWidth = 0;
    m_iMaxHeight = 0;
    m_iMinWidth = 125;
    m_iMinHeight = 100;
    m_iOriginWidth = 0;
    m_iOriginHeight = 0;
	m_bCanChColor = false;
	m_bSet9BarSelf = false;
	m_i9Bar_R = 0;								
	m_i9Bar_T = 0;								
	m_i9Bar_L = 0;								
	m_i9Bar_B = 0;

    m_colorTopLeft = 0xffffffff;
    m_colorTopRight = 0xffffffff;
    m_colorBottomLeft = 0xffffffff;
    m_colorBottomRight = 0xffffffff;

    m_pCallbackEvent = fnDefaultEvent;
//    m_pCallbackEventUserContext = this;

    m_fTimeLastRefresh = 0;

    m_pControlMouseOver = NULL;

    m_pNextDialog = this;
    m_pPrevDialog = this;

    m_nDefaultControlID = 0xffff;
    m_bNonUserEvents = false;
    m_bKeyboardInput = false;
    m_bMouseInput = true;
    m_bDragToMove = false;
    m_bDragging = false;
    m_bLButtonDown = false;
	m_bNOChColorAndAlpha = false;
	for( int i=0; i<10; i++ )
		m_SidElement[i].iTexture = -1;
    
    // ming add >>
    SetRectEmpty( &m_rcBoundingBox );
    SetRectEmpty( &m_rcDraw );
    SetRectEmpty( &m_rcCaption );
    SetRectEmpty( &m_rcCapText );
	SetRectEmpty( &m_rcCaption1 );
	SetRectEmpty( &m_rcCaption2 );
    
    m_MinBtn = NULL;
    m_CloseBtn = NULL;
    m_OrigBtn = NULL;
    m_bAlwaysFocus = false;
    // ming add <<
    mousestr = "\0";
    
    m_pTextObjectCaption = NULL;
	//-------------------------------------
	// Element for the dialog
	//-------------------------------------
	m_DiaElement.TextureColor.Init( 0, 0, 0, 0 );
	// Darkness add
	for(int i=0;i<10;i++)
		m_SidElement[i].TextureColor.Init( 0, 0, 0, 0 );
	// ------------

}
//--------------------------------------------------------------------------------------
void CDXUTDialog::fnDisablePublicState(CDXUTDialog* pDlg)
{
	if(s_pDefFocusDialog == pDlg )
		s_pDefFocusDialog = 0;

	if(s_pFocusDialog == pDlg)
		s_pFocusDialog = 0;

	if(s_pMouseDialog == pDlg)
		s_pMouseDialog = 0;

}
//--------------------------------------------------------------------------------------

CDXUTDialog::~CDXUTDialog()
{

	fnDisablePublicState(this);
	
    int i=0;
    if( m_pTextObjectCaption )
    {
        delete m_pTextObjectCaption;
        m_pTextObjectCaption = NULL;
    }

    RemoveAllControls();

    m_Fonts.RemoveAll();
    m_Textures.RemoveAll();

    for( i=0; i < m_DefaultElements.GetSize(); i++ )
    {
        DXUTElementHolder* pElementHolder = m_DefaultElements.GetAt( i );
        SAFE_DELETE( pElementHolder );
    }

    m_DefaultElements.RemoveAll();

	Unregister(mHnd);

}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetSystemButtonHintText(eSystemButtonType type, const WCHAR *text)
{
	switch(type)
	{
	case sbtMINIMUM:
		sm_hintTextMinimum = text;
		break;
	case sbtMAXIMUM:
		sm_hintTextMaximum = text;
		break;
	case sbtCLOSE:
		sm_hintTextClose = text;
		break;
	case sbtORIGIN:
		sm_hintTextOrigin = text;
		break;
	default:
		assert(0);
	}
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::Init( CDXUTDialogResourceManager* pManager )
{

	assert(s_pManager);
	if(sm_skinStyle == sstFDO)
	    InitDefaultElements_FdoStyle(); // requires a CDXUTDialogResourceManager to store textures/fonts
	else
	    InitDefaultElements_HvoStyle(); // requires a CDXUTDialogResourceManager to store textures/fonts

    // ming add >> 增加最小化、關閉鈕
    AddButton( 0, L"", 0, 1-m_nCaptionHeight, 14, 14, 0, false, &m_MinBtn ); // 最小化　
    AddButton( 0, L"", 0, 1-m_nCaptionHeight, 14, 14, 0, false, &m_CloseBtn ); // 關閉鈕
    AddButton( 0, L"", 0, 1-m_nCaptionHeight, 14, 14, 0, false, &m_OrigBtn ); // 復原鈕

    /*string str;
    str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 894, CodeInlineText_Text ) );//最小化
    m_MinBtn->SetHint( str.c_str() );  //音效 *** modify
    str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 56, CodeInlineText_Text ) );//關閉
    m_CloseBtn->SetHint( str.c_str() ); //音效 *** modify
    str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 1235, CodeInlineText_Text ) );//復原
    m_OrigBtn->SetHint( str.c_str() );
*/
	//最小化
    m_MinBtn->SetHint( sm_hintTextMinimum.c_str() );  //音效 *** modify
    //關閉
	//m_CloseBtn->SetHint( sm_hintTextClose.c_str() ); //音效 *** modify
	//復原
    m_OrigBtn->SetHint( sm_hintTextOrigin.c_str() );

    // 設定最小化紐材質
    RECT rcNormal, rcFocus;
    SetRect( &rcNormal, 56, 3, 69, 16 );
    SetRect( &rcFocus, 56, 3, 69, 16 );
    SetMinBtuttonTexture( _OBJ_FILE_PATH, _OBJ_FILE_PATH,  &rcNormal, &rcFocus );

    // 設定復原紐材質
    SetRect( &rcNormal, 73, 3, 86, 16 );
    SetRect( &rcFocus, 73, 3, 86, 16 );
    SetOrigBtuttonTexture( _OBJ_FILE_PATH, _OBJ_FILE_PATH,  &rcNormal, &rcFocus );

    // 設定關閉紐材質
    SetRect( &rcNormal, 59, 21, 72, 34 );
    SetRect( &rcFocus, 59, 21, 72, 34 );
    SetCloseBtuttonTexture( _OBJ_FILE_PATH, _OBJ_FILE_PATH, &rcNormal, &rcFocus );

    // 初始時設為不可視
    m_OrigBtn->SetVisible( false );
    m_MinBtn->SetVisible( false );
    m_CloseBtn->SetVisible( false );
    // ming add <<
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::SetLocation( int x, int y, bool bAutoCorrect )
{
    // Dimension check, to prevent corrupted UIPos.log.
	if(m_x!=x || m_y!=y)
	{
		HWND hWnd = DXUTGetHWND();
		RECT rcClient;
		GetClientRect(hWnd, &rcClient);

		if (bAutoCorrect) {
			if (x + m_width/2 <= 0 || x + m_width/2 >= rcClient.right)
				return;
			if (y < 0 || y + 24 >= rcClient.bottom)
				return;
		} // end if

		// Set dimension.
		m_x = x; 
		m_y = y; 
		UpdateRects(); 
	}
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::SetSize( int width, int height )
{
    m_width = width;
    m_height = height;
    UpdateRects();

    // 設定關閉鈕、最小化鈕位置
    //m_MinBtn->SetLocation( width-40, 2-m_nCaptionHeight );
    //m_CloseBtn->SetLocation( width-22, 2-m_nCaptionHeight );
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::GetRect(RECT& rcOut)
{
    rcOut.left = m_x;
    rcOut.top = m_y;
    rcOut.right = m_x + m_width;
    rcOut.bottom = m_y + m_height;
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::EnableDragToMove(bool bEnable)
{
    m_bDragToMove = bEnable;
    // Below is a bug should be fixed.
    if (bEnable) {
        EnableCaption(true, &m_rcCaption, false);
    } // end if
}
//--------------------------------------------------------------------------------------
// 啟用渲染外框, 外框厚度(不使用可不填或填0)
//--------------------------------------------------------------------------------------
void CDXUTDialog::EnableUseSideFrame( bool bEnable, int iFrameGauge )
{
    // 是否使用外框(如有開啟下面的縮放功能視同啟用外框)
    m_bUseSideFrame = bEnable;

    if( m_bUseSideFrame )
        if( iFrameGauge > 0 )   m_iFrameGauge = iFrameGauge;

}
//--------------------------------------------------------------------------------------
// 固定大小縮放
//--------------------------------------------------------------------------------------
void CDXUTDialog::EnableAptoticSize( bool bEnable, int iWidth, int iHeight )
{
    m_bAptoticSize = bEnable;

    if( m_bAptoticSize )
    {
        if( iWidth>0 )  m_iSubX = iWidth;
        else    m_iSubX = 0;             // 0表示在此方向上沒有影響
        
        if( iHeight>0 ) m_iSubY = iHeight;
        else    m_iSubY = 0;             // 0表示在此方向上沒有影響
    }
}
//--------------------------------------------------------------------------------------
// 開啟本視窗的縮放功能( 開關, 可判斷滑鼠之邊框厚度(不使用填0,則預設5), 模式(不使用可不填或填0,))
//--------------------------------------------------------------------------------------
void CDXUTDialog::EnableSizeChange( bool bEnable, int iSideGauge , int iChSzMode )
{
    // 是否開啟視窗縮放功能
    m_bCanSizeChange = bEnable;

    if( m_bCanSizeChange )
    {
        // 模式：0:八方向都可拖曳(預設)  1:四個對角可拖曳  2:四個邊可拖曳
        //       3:僅"右,右下,下"可拖曳  4:僅"下"可拖曳
        if( iChSzMode != 0)   m_iChangeSizeMode = iChSzMode;
        // 厚度 預設5
        if( iSideGauge > 0)  m_SideGauge = iSideGauge;
    }

}
//--------------------------------------------------------------------------------------
// 最大寬度(不使用就填0), 最大高度(不使用就填0)
void CDXUTDialog::SetMaxSize( int iMaxWidth, int iMaxHeight )
{
    if( m_bCanSizeChange )
    {
        if( iMaxWidth > 0 )   m_iMaxWidth = iMaxWidth;

        if( iMaxHeight > 0 )  m_iMaxHeight = iMaxHeight;
    }
}
//--------------------------------------------------------------------------------------
// 最小寬度(不使用就填0), 最小高度(不使用就填0)
void CDXUTDialog::SetMinSize( int iMinWidth, int iMinHeight )
{
    if( m_bCanSizeChange )
    {
        if( iMinWidth > 0 )   m_iMinWidth = iMinWidth;
        else    m_iMinWidth = 125;

        if( iMinHeight > 0 )  m_iMinHeight = iMinHeight;
        else    m_iMinHeight = 100;
    }
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::EnableCaption( bool bEnable, RECT* prcCaption, bool bDrawCap )
{
    m_bCaption = bEnable;

    if (prcCaption) {
        m_rcCaption = *prcCaption;
		m_rcCaption1 = *prcCaption;
		m_rcCaption2 = *prcCaption;
		m_rcCaption2.right = 120;
        m_nCaptionHeight = RectHeight(*prcCaption) + prcCaption->top;
		
    }

    m_bDrawCap = bDrawCap;
    //m_MinBtn->SetVisible( bEnable );
    //m_CloseBtn->SetVisible( bEnable );
}
//--------------------------------------------------------------------------------------
// 設定9號BAR的大小
//--------------------------------------------------------------------------------------
void CDXUTDialog::Set9Bar( int iW, int iH ) 
{	
	if( iW>0 && iH>0 )
		m_bSet9BarSelf = true;
	
	if( iW > 0 )
	{
		m_i9Bar_L = m_width/2 - iW/2;	
		m_i9Bar_R = m_i9Bar_L;//m_i9Bar_R = m_i9Bar_L + iW;
		m_i9Bar_L = m_i9Bar_L - 5;
		m_i9Bar_R = m_i9Bar_R - 4;
	}
	else
	{
		//m_i9Bar_R = m_width-4;
		//m_i9Bar_L = 5;
	}

	if( iH > 0 )
	{
		m_i9Bar_T = m_nCaptionHeight - iH/2;
		m_i9Bar_B = m_i9Bar_T + iH;
	}
	else
	{
		//m_i9Bar_B = m_nCaptionHeight+12;
		//m_i9Bar_T = m_nCaptionHeight+7;
	}

	/*
	if( iX > 0 )
		m_i9Bar_X = iX;
	else
		m_i9Bar_X = 5;

	if( iY > 0 )
		m_i9Bar_Y = iY;
	else
		m_i9Bar_Y = m_nCaptionHeight+7;
	*/	

}
//--------------------------------------------------------------------------------------
// 設定縮小鈕的位置大小
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetMinimizeBtutton( int x, int y, int width, int height )
{
    m_MinBtn->SetVisible( true );

    if( x>=0 && y>=0 )
    {
        m_MinBtn->SetLocation( x, y-m_nCaptionHeight );
    }

    if( width>0 && height>0 )
    {
        m_MinBtn->SetSize( width, height );
    }

    m_bCanMinimized = true;
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::SetMinBtnVisible( bool bVisible )
{
	m_MinBtn->SetVisible( bVisible );	
}
//--------------------------------------------------------------------------------------
// 設定復原鈕的位置大小
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetOriginBtutton( int x, int y, int width, int height )
{
    m_OrigBtn->SetVisible( true );

    if( x>=0 && y>=0 )
    {
        m_OrigBtn->SetLocation( x, y-m_nCaptionHeight );
    }

    if( width>0 && height>0 )
    {
        m_OrigBtn->SetSize( width, height );
    }
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::SetOriBtnVisible( bool bVisible )
{
	m_OrigBtn->SetVisible( bVisible );	
}
//--------------------------------------------------------------------------------------
// 設定關閉鈕的位置大小
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetCloseBtutton( int x, int y, int width, int height )
{
    m_CloseBtn->SetVisible( true );
	mbUseCloseBtn = true;
    if( x>=0 && y>=0 )
    {
        m_CloseBtn->SetLocation( x, y-m_nCaptionHeight );
    }

    if( width>0 && height>0 )
    {
        m_CloseBtn->SetSize( width, height );
    }
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::SetClsBtnVisible( bool bVisible )
{
	m_CloseBtn->SetVisible( bVisible );	
	mbUseCloseBtn = bVisible;
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::Move3Btn()
{
    m_CloseBtn->SetLocation( m_width-36, 7-m_nCaptionHeight );
    m_MinBtn->SetLocation( m_width-66, 7-m_nCaptionHeight );
    m_OrigBtn->SetLocation( m_width-51, 7-m_nCaptionHeight );
}
//--------------------------------------------------------------------------------------
// 設定關閉鈕的位置大小 (AUTO)
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetCloseBtuttonAuto()
{
    m_CloseBtn->SetVisible( true );
    m_CloseBtn->SetLocation( m_width-36, -(m_nCaptionHeight/2 + 14/2) );
    m_CloseBtn->SetSize( 14, 14 );
}
//--------------------------------------------------------------------------------------
// 設定縮小鈕的位置大小(AUTO)
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetMinimizeBtuttonAuto()
{
    m_MinBtn->SetVisible( true );
    m_MinBtn->SetLocation( m_width-66, -(m_nCaptionHeight/2 + 14/2) );
    m_MinBtn->SetSize( 14, 14 );
    m_bCanMinimized = true;
}
//--------------------------------------------------------------------------------------
// 設定復原鈕的位置大小(AUTO)
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetOriginBtuttonAuto()
{
    m_OrigBtn->SetVisible( true );
    m_OrigBtn->SetLocation( m_width-51, -(m_nCaptionHeight/2 + 14/2) );
    m_OrigBtn->SetSize( 14, 14 );
}
//--------------------------------------------------------------------------------------
// 設定最小化鈕的材質
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetMinBtuttonTexture( const string &strNormal,
      const string &strFocus, RECT* prcNormal, RECT* prcFocus )
{
    m_MinBtn->SetNormalTexture( strNormal, prcNormal );
    m_MinBtn->SetFocusTexture( strFocus, prcFocus );
}
//--------------------------------------------------------------------------------------
// 設定復原鈕的材質
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetOrigBtuttonTexture( const string &strNormal,
      const string &strFocus, RECT* prcNormal, RECT* prcFocus )
{
    m_OrigBtn->SetNormalTexture( strNormal, prcNormal );
    m_OrigBtn->SetFocusTexture( strFocus, prcFocus );
}
//--------------------------------------------------------------------------------------
// 設定關閉鈕的材質
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetCloseBtuttonTexture( const string &strNormal,
      const string &strFocus, RECT* prcNormal, RECT* prcFocus )
{
    m_CloseBtn->SetNormalTexture( strNormal, prcNormal );
    m_CloseBtn->SetFocusTexture( strFocus, prcFocus );
}
//--------------------------------------------------------------------------------------
// 設定預設位置,大小
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetOriginalSize( int x, int y, int Width, int Height )
{
    if( Width>0 && Height>0 )
    {
        m_iOriginX = x;
        m_iOriginY = y;
        m_iOriginWidth = Width;
        m_iOriginHeight = Height;
    }
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetCallback( PCALLBACKDXUTGUIEVENT pCallback, void* pUserContext )
{
    // If this assert triggers, you need to call CDXUTDialog::Init() first.  This change
    // was made so that the DXUT's GUI could become seperate and optional from DXUT's core.  The 
    // creation and interfacing with CDXUTDialogResourceManager is now the responsibility 
    // of the application if it wishes to use DXUT's GUI.
    assert( s_pManager != NULL && L"To fix call CDXUTDialog::Init() first.  See comments for details." ); 

    m_pCallbackEvent = pCallback; 
    m_pCallbackEventUserContext = pUserContext; 
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::RemoveControl( int ID )
{
    for( int i=0; i < m_Controls.GetSize(); i++ )
    {
        CDXUTControl* pControl = m_Controls.GetAt( i );
        if( pControl->GetID() == ID )
        {
            // Clean focus first
            ClearFocus();

            // Clear references to this control

            if( s_pControlFocus == pControl )
			{
				s_pControlFocus->OnFocusOut();
                s_pControlFocus = NULL;
			}
            if( s_pControlPressed == pControl )
                s_pControlPressed = NULL;
            if( m_pControlMouseOver == pControl )
                m_pControlMouseOver = NULL;

            SAFE_DELETE( pControl );
            m_Controls.Remove( i );
			mControlCache.erase(ID);
            return;
        }
    }
}

//--------------------------------------------------------------------------------------
bool CDXUTDialog::RemoveControl( CDXUTControl* pControl )
{
    for( int i=0; i < m_Controls.GetSize(); ++i )
    {
        if( pControl == m_Controls.GetAt( i ) )
        {
            

			// Clear references to this control
			if( s_pControlFocus == pControl )
			{
				s_pControlFocus->OnFocusOut();
				s_pControlFocus = NULL;
				// Clean focus first
				ClearFocus();
			}
			if( s_pControlPressed == pControl )
				s_pControlPressed = NULL;
			if( m_pControlMouseOver == pControl )
				m_pControlMouseOver = NULL;
            
			mControlCache.erase(pControl->GetID());
            SAFE_DELETE( pControl );
            m_Controls.Remove( i );
            
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::RemoveAllControls()
{
    if( s_pControlFocus && s_pControlFocus->m_pDialog == this )
	{
		s_pControlFocus->OnFocusOut();
        s_pControlFocus = NULL;
	}
    if( s_pControlPressed && s_pControlPressed->m_pDialog == this )
        s_pControlPressed = NULL;
    m_pControlMouseOver = NULL;

    for( int i=0; i < m_Controls.GetSize(); i++ )
    {
        CDXUTControl* pControl = m_Controls.GetAt( i );
        SAFE_DELETE( pControl );
    }

    m_Controls.RemoveAll();
	mControlCache.clear();
}

//--------------------------------------------------------------------------------------
// For temporarily hiding all UI for PrintScreen.
// See UIManager::SaveVisibility(), UIManager::RestoreVisibility().

void CDXUTDialog::SaveVisibility()
{ 
    PushVis(m_bVisible);
}

bool CDXUTDialog::RestoreVisibility()  
{ 
    return PopVis(m_bVisible);
}

void CDXUTDialog::PushVis(bool bVal)
{
    m_listVis.push_front(bVal);
}

bool CDXUTDialog::PopVis(bool& bValOut)
{
    if (true == m_listVis.empty())
        return false;
    bValOut = m_listVis.front();
    m_listVis.pop_front();
    return true;
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::OnMouseEnter()
{
    if (s_pMouseDialog && s_pMouseDialog != this )
        s_pMouseDialog->OnMouseLeave();
    m_bMouseOver = true;
    s_pMouseDialog = this;
    //CDXUTScrollBar::bDrag = false;
    // 如果面有可以改變顏色透明度功能時,連同這三顆按鈕一起設定透明度
    if( m_bCanChColor )
    {
        m_MinBtn->SetNormalStateAlpha( 255 );
        m_OrigBtn->SetNormalStateAlpha( 255 );
        m_CloseBtn->SetNormalStateAlpha( 255 );
    }
	SendEvent( EVENT_DIALOG_MOUSEENTER ,true,0,0 );
    //m_pCallbackEvent( EVENT_DIALOG_MOUSEENTER, -1, NULL, m_pCallbackEventUserContext );
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::OnMouseLeave()
{
	CDXUTScrollBar::ResetDragState();
    m_bMouseOver = false;
    s_pMouseDialog = NULL;

    // 不呼叫 Refresh()，只處理 m_pControlMouseOver。
    if (m_pControlMouseOver) {
        m_pControlMouseOver->OnMouseLeave();
        m_pControlMouseOver = NULL;
    }
    if( m_bCanChColor )
    {
        m_MinBtn->SetNormalStateAlpha( m_ColorAlpha );
        m_OrigBtn->SetNormalStateAlpha( m_ColorAlpha );
        m_CloseBtn->SetNormalStateAlpha( m_ColorAlpha );
    }
	SendEvent(EVENT_DIALOG_MOUSELEAVE,true,0,0);
// 	EVENT_DIALOG_FOCUSIN
//     m_pCallbackEvent( EVENT_DIALOG_MOUSELEAVE, -1, NULL, m_pCallbackEventUserContext );
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::Refresh()
{
    // 處理 m_pControlMouseOver
    if (m_pControlMouseOver) {
        m_pControlMouseOver->OnMouseLeave();
        m_pControlMouseOver = NULL;
    }
    // 處理 s_pControlFocus
    if (s_pControlFocus && s_pControlFocus->m_pDialog == this) 
	{
        s_pControlFocus->OnFocusOut();
        s_pControlFocus = NULL;
    }
    // 處理 s_pControlPressed
    s_pControlPressed = NULL;

    /* ming mark 不需要的效果
    for( int i=0; i < m_Controls.GetSize(); i++ )
    {
        CDXUTControl* pControl = m_Controls.GetAt(i);
        pControl->Refresh();
    }
    */

    //if( m_bKeyboardInput )
    //    FocusDefaultControl();
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetVisible( bool bVisible )
{
	//<gino/> Skip if m_bVisible == bVisible
	if(m_bVisible!=bVisible)
	{
		m_bVisible = bVisible;
// 		if(m_MinBtn)
// 			m_MinBtn->SetVisible(m_bVisible);
		if(m_CloseBtn && mbUseCloseBtn)
			m_CloseBtn->SetVisible(m_bVisible);
// 		if(m_OrigBtn)
// 			m_OrigBtn->SetVisible(m_bVisible);
		if( !m_bVisible )
		{
			if (s_pMouseDialog == this)
				s_pMouseDialog = NULL;
			OnDialogFocusOut();
			m_DiaElement.TextureColor.Current = D3DCOLOR_ARGB(0, 255, 255, 255);
			// Darkness add
			for( int i=0; i<10; i++ )
				m_SidElement[i].TextureColor.Current = D3DCOLOR_ARGB(0, 255, 255, 255);
			// ------------
			CDXUTControl* pControl = GetControlAtPoint( m_LastMouse ) ;
			if( pControl )
			{
				SendEvent( EVENT_CONTROL_MOUSE_OUT , true , pControl ) ;				
			}
			SendEvent(EVENT_DIALOG_HIDE,true , 0 ,0);
			//m_pCallbackEvent( EVENT_DIALOG_HIDE, -1, NULL, m_pCallbackEventUserContext );

			//關掉則解除所有的hook
			this->m_HookMgr.ClearSpecifyHook(this->mHnd);

			if(s_pControlFocus && s_pControlFocus->GetParent() == this)
			{
				s_pControlFocus->OnFocusOut();
				s_pControlFocus = 0;
			}
		}

		
		for (sint i = 0 ; i < m_Controls.GetSize() ; ++i)
		{
			CDXUTControl* pCtrl = m_Controls.GetAt(i);
			
			if( fnIsSubDialogType(pCtrl->GetType()) == false)
				continue;

			CDXUTSubDialog* pSub = (CDXUTSubDialog*)pCtrl;
			//if(pSub->GetDialog()->GetVisible())
			pSub->SetVisible(bVisible);
		}

		if(m_bVisible)
			SendEvent(EVENT_DIALOG_SHOW,true , 0,0 );
		//m_pCallbackEvent( EVENT_DIALOG_SHOW, -1, NULL, m_pCallbackEventUserContext );
	}
	if( m_bVisible )
	{
		Raise();	
		CDXUTDialog::push_front();		

		


	}
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetColors( D3DCOLOR color )
{
    if(m_bNOChColorAndAlpha)
		return;
	
	if(m_bCanChColor)
        return;

    // Darkness add
	m_ColorAlpha = color>>24 & 0xFF ;

    if( m_DiaElement.iTexture>0 )
    {
		m_DiaElement.TextureColor.States[ DXUT_STATE_NORMAL ] = color;
        m_DiaElement.TextureColor.States[ DXUT_STATE_UNFOCUS ] = color;
    }

    for( int i=0; i<10; i++ )
	{
        if( m_SidElement[i].iTexture>=0 )
		{
			m_SidElement[i].TextureColor.States[ DXUT_STATE_NORMAL ] = color;
            m_SidElement[i].TextureColor.States[ DXUT_STATE_UNFOCUS ] = color;
		}
	}
    // ------------
    //m_DiaElement.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = color;
    //m_DiaElement.TextureColor.States[ DXUT_STATE_NORMAL ] = color;
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetColors_RGBA( int Red, int Green, int Blue, int Alpha )
{   // Darkness add
	if(m_bNOChColorAndAlpha)
		return;

    m_ColorRed = Red;
    m_ColorGreen = Green;
    m_ColorBlue = Blue;
    //m_ColorAlpha = Alpha;

    // 一般的Dialog
    if( m_DiaElement.iTexture>0 && m_bCanChColor )
    {
        m_DiaElement.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB( m_ColorAlpha,m_ColorRed,m_ColorGreen,m_ColorBlue );
        m_DiaElement.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB( m_ColorAlpha,m_ColorRed,m_ColorGreen,m_ColorBlue );
        m_DiaElement.TextureColor.States[ DXUT_STATE_UNFOCUS ] = D3DCOLOR_ARGB( m_ColorAlpha,m_ColorRed,m_ColorGreen,m_ColorBlue );
    }

    // 有邊框可變動大小的Dialog
    for( int i=0; i<10; i++ )
        if( m_SidElement[i].iTexture>=0 && m_bCanChColor )
        {
            //if( i == 9 )// Cpation那一張
            //{
            //    m_SidElement[9].TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB( 255,m_ColorRed,m_ColorGreen,m_ColorBlue );
            //    m_SidElement[9].TextureColor.States[ DXUT_STATE_UNFOCUS ] = D3DCOLOR_ARGB( 255,m_ColorRed,m_ColorGreen,m_ColorBlue );
            //}
            //else
            //{
                m_SidElement[i].TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB( m_ColorAlpha,m_ColorRed,m_ColorGreen,m_ColorBlue );
                m_SidElement[i].TextureColor.States[ DXUT_STATE_UNFOCUS ] = D3DCOLOR_ARGB( m_ColorAlpha,m_ColorRed,m_ColorGreen,m_ColorBlue );
            //}
        }
    // ------------
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetAlpha(int Alpha )
{   // Darkness add
    if(m_bNOChColorAndAlpha)
		return;
	
	m_ColorAlpha = Alpha;

    if( m_DiaElement.iTexture>0 )
    {
        m_DiaElement.TextureColor.States[ DXUT_STATE_UNFOCUS ] = D3DCOLOR_ARGB( m_ColorAlpha,m_ColorRed,m_ColorGreen,m_ColorBlue );
		SendEvent(EVENT_DIALOG_CHANGECOLOR ,true , 0,0 );
        //m_pCallbackEvent( EVENT_DIALOG_MOVE, -1, NULL, m_pCallbackEventUserContext );
    }
    for( int i=0; i<10; i++ )
        if( m_SidElement[i].iTexture>=0 && m_bCanChColor )
        {
            //if(i!=9)
            m_SidElement[i].TextureColor.States[ DXUT_STATE_UNFOCUS ] = D3DCOLOR_ARGB(m_ColorAlpha,m_ColorRed,m_ColorGreen,m_ColorBlue);

        }
    if( m_bCanChColor )
    {
        m_MinBtn->SetNormalStateAlpha( m_ColorAlpha );
        m_OrigBtn->SetNormalStateAlpha( m_ColorAlpha );
        m_CloseBtn->SetNormalStateAlpha( m_ColorAlpha );
		SendEvent(EVENT_DIALOG_CHANGECOLOR,true , 0,0 );
        //m_pCallbackEvent( EVENT_DIALOG_CHANGECOLOR, -1, NULL, m_pCallbackEventUserContext );
    }
}
//--------------------------------------------------------------------------------------
DXUT_CONTROL_STATE CDXUTDialog::GetCurrentState()
{
    DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

    if (m_bCaption || m_bDragToMove) { // 有Caption的Dialog才有fade效果
        iState = DXUT_STATE_UNFOCUS;
        if (m_bMouseOver)
            iState = DXUT_STATE_MOUSEOVER;
    }
    return iState;
} 
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::OnRender( float fElapsedTime )
{   
    // If this assert triggers, you need to call CDXUTDialogResourceManager::On*Device() from inside
    // the application's device callbacks.  See the SDK samples for an example of how to do this.
    assert( s_pManager->GetD3DDevice() && s_pManager->m_pStateBlock && L"To fix hook up CDXUTDialogResourceManager to device callbacks.  See comments for details" );

    // See if the dialog needs to be refreshed


    // For invisible dialog, out now.
    if( !m_bVisible )
        return S_OK;

    //if( m_fTimeLastRefresh < s_fTimeRefresh )
    //{
    //    m_fTimeLastRefresh = DXUTGetTime();
    //    Refresh();
    //} 
  //  s_pManager->m_pSprite->Begin( D3DXSPRITE_DONOTSAVESTATE );
    RenderCore(fElapsedTime);
   // s_pManager->m_pSprite->End();

	
	
	if(s_bEnableDebugInfo)
	{
		RECT rcScreen(m_rcBoundingBox);
		OffsetRect(&rcScreen,-m_x,-m_y);

		srand((int)this);
		
		DrawRect(&rcScreen,(D3DCOLOR(random(256) * 0x100 + random(256) * 0x10000 + random(256) * 0x1) & 0x00ffffff) | 0x3000000);

		if(m_DebugLayoutPath.length())
		{		
			RECT rcScreen(m_rcBoundingBox);
			SIZE szText = s_pManager->GetStringSize(0,m_DebugLayoutPath.c_str());										
			SetRect(&rcScreen ,rcScreen.left , rcScreen.top ,rcScreen.left + szText.cx , rcScreen.top + szText.cy);						

			s_pManager->DrawFont(0,m_DebugLayoutPath.c_str(),rcScreen,0,0xffffffff,2);
		}
	}

	


    return S_OK;
}
//--------------------------------------------------------------------------------------
// RenderCore() = RenerBackground() + RenderControls()
//--------------------------------------------------------------------------------------

void fnDialogRenderPerformance(const char8* caption, float fTime)
{	
	static float		fMaxTime = 0.0f;
	static std::map<string , float> Records;
	Records[caption] = fTime;
	if(fMaxTime < fTime )
	{
		fMaxTime = fTime;
		string str;
		StringFormat8(str,"更新 Dialog繪製 位置:%s 最大時間:%f \n",caption,fMaxTime);
		OutputDebugStringA(str.c_str());
		float fTotal = 0.0f;
		std::map<string , float>::iterator it = Records.begin();
		for (; it != Records.end() ; ++it)
		{	 
			fTotal += it->second;
			string str;
			StringFormat8(str,"Dialog繪製 位置:%s 時間:%f \n",it->first.c_str(),it->second);
			OutputDebugStringA(str.c_str());
		}		
		StringFormat8(str,"Dialog繪製 總合時間 %f \n",fTotal);
		OutputDebugStringA(str.c_str());
		Records.clear();
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTDialog::RenderCore(float fElapsedTime)
{
// 	PerformanceMonitor pm(fnDialogRenderPerformance);
// 	string str;
// 	StringFormat8(str,"%x",this);
// 	pm.Begin(str.c_str());

    RenderBackground(fElapsedTime);
    RenderBackground2(fElapsedTime);
    RenderControls(fElapsedTime);

	if(!m_pPipeVec.empty())
	{
		s_pManager->m_pSprite->Flush() ;

		for(unsigned int i = 0 ; i < m_pPipeVec.size() ; ++i)
		{
			m_pPipeVec[i]->Update(fElapsedTime) ; 
			m_pPipeVec[i]->RenderPipeline() ; 
		}
	}


//	pm.End();
    
    // ming add >> 再最小化時畫出最小化及關閉鈕
//    else
//    {
//        if( m_bCaption )
//        {
//            m_MinBtn->Render( pd3dDevice, fElapsedTime );
//            m_CloseBtn->Render( pd3dDevice, fElapsedTime );
//        }
//    }
    // ming add <<
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::RenderBackground(float fElapsedTime)
{
    //if( !m_bMinimized ) // 畫出Dialog本體
    {
        DXUT_CONTROL_STATE iState = GetCurrentState();
        m_DiaElement.TextureColor.Blend(iState, fElapsedTime);

        RECT rc = { 0, 0, m_width, m_height };
        if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
            rc = m_rcDraw;

        if( m_bCaption )
            OffsetRect( &rc, 0, -m_nCaptionHeight );

		//DrawRect(&rc , m_BackgroundColor);
				
		D3DXCOLOR destColor(m_BackgroundColor);
		//destColor.a = (float)m_ColorAlpha / 255;
		
		DrawRect(&rc , (DWORD)destColor);
		


//		DrawRect(&rc , (DWORD)currentColor);
//        DrawSprite( &m_DiaElement, &rc ); // 畫出Dialog的材質
    }

    // Render the caption if it's enabled.
    //DrawCaption();
}
//------------------------------------------------------------------------------
void CDXUTDialog::RenderBackground2(float fElapsedTime)  //針對Dialog本體+邊框分開的部分
{
    if ( m_bUseSideFrame )
    {
        DXUT_CONTROL_STATE iState = GetCurrentState();

        for( int i=0; i<10; i++ )
        {
//            m_SidElement[i].TextureColor.Blend(iState, fElapsedTime);			
			m_SidElement[i].TextureColor.Current.a = (float)m_ColorAlpha / 255;
			m_SidElement[i].TextureColor.Current.r = 1.0f;
			m_SidElement[i].TextureColor.Current.g = 1.0f;
			m_SidElement[i].TextureColor.Current.b = 1.0f;


			if( m_SidElement[i].iTexture == -1 )
				return;

            if(i == 0)
            {   // 背景的位置
                RECT rc={m_iFrameGauge, m_iFrameGauge, m_width-m_iFrameGauge, m_height-m_iFrameGauge };

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[0], &rc ); // 畫出Dialog的材質
            }
            if(i == 1)
            {   // LEFTTOP
                RECT rc={0, 0, m_iFrameGauge, m_iFrameGauge};

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[1], &rc ); // 畫出左上的材質
            }
            if(i == 2)
            {   // TOP
                RECT rc={m_iFrameGauge, 0, m_width-m_iFrameGauge, m_iFrameGauge};

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[2], &rc ); // 畫出上邊的材質
            }
            if(i == 3)
            {   // RIGHTTOP
                RECT rc={m_width-m_iFrameGauge, 0, m_width, m_iFrameGauge};

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[3], &rc ); // 畫出右上的材質
            }
            if(i == 4)
            {   // RIGHT
                RECT rc={m_width-m_iFrameGauge, m_iFrameGauge, m_width, m_height-m_iFrameGauge};

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[4], &rc ); // 畫出右邊的材質
            }
            if(i == 5)
            {   // RIGHTBOTTON
                RECT rc={m_width-m_iFrameGauge, m_height-m_iFrameGauge, m_width, m_height};

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[5], &rc ); // 畫出右下的材質
            }
            if(i == 6)
            {   // BOTTON
                RECT rc={m_iFrameGauge, m_height-m_iFrameGauge, m_width-m_iFrameGauge, m_height};

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[6], &rc ); // 畫出下邊的材質
            }
            if(i == 7)
            {   // LEFTBOTTON
                RECT rc={0, m_height-m_iFrameGauge, m_iFrameGauge, m_height};

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[7], &rc ); // 畫出左下的材質
            }
            if(i == 8)
            {   // LEFT
                RECT rc={0, m_iFrameGauge, m_iFrameGauge, m_height-m_iFrameGauge};

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[8], &rc ); // 畫出左邊的材質
            }
            if(i == 9 && !m_bMinimized )
            {   // CAPTION
                
				RECT rc={5, m_nCaptionHeight+0, m_width-4, m_nCaptionHeight+5};
				if( m_bSet9BarSelf )
				{
					SetRect(&rc, 5 + m_i9Bar_L, m_i9Bar_T, m_width - 4 - m_i9Bar_R, m_i9Bar_B);
				}

                if( RectWidth(m_rcDraw)>0 && RectHeight(m_rcDraw)>0 )
                    rc = m_rcDraw;

                if( m_bCaption )
                    OffsetRect( &rc, 0, -m_nCaptionHeight );

                DrawSprite( &m_SidElement[9], &rc ); // 畫出Caption的材質
            }
        }
    }
    DrawCaption();
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::RenderControls(float fElapsedTime)
{
    // If the dialog is minimized, skip rendering its controls.
    //if( !m_bMinimized )
    {
        IDirect3DDevice9* pd3dDevice = s_pManager->GetD3DDevice();     

		int count = m_Controls.GetSize() ; 

        for( int i=0; i < count; ++i )
        {

            CDXUTControl* pControl = m_Controls.GetAt(i);  

			if(!pControl->GetVisible())
			{
				continue ; 
			}

            // Focused control is drawn last
            if( pControl == s_pControlFocus )
                continue;

			pControl->RenderControl( pd3dDevice , fElapsedTime ) ;
            //pControl->Render( pd3dDevice, fElapsedTime );
			if(s_bEnableDebugInfo)
			{				
				RECT rcScreen(pControl->GetBoundingBox());
				
				srand((int)pControl);
				DrawRect(&rcScreen,(D3DCOLOR(random(256) * 0x100 + random(256) * 0x10000 + random(256) * 0x1) & 0x00ffffff) | 0x30000000);				


				std::wstring str;
				StringFormat16(str , L"%d", pControl->GetID()) ;
				SIZE szText = s_pManager->GetStringSize(0,str.c_str());								
				OffsetRect(&rcScreen,m_x,m_y);
				SetRect(&rcScreen ,rcScreen.left , rcScreen.top ,rcScreen.left + szText.cx , rcScreen.top + szText.cy);				
				s_pManager->DrawFont(0,str.c_str(),rcScreen,0,0xffff0000,2);
			}
        }

        if( s_pControlFocus != NULL && (s_pControlFocus->m_pDialog == this || fnIsParent(s_pControlFocus->m_pDialog ,this)))
			s_pControlFocus->RenderControl( pd3dDevice , fElapsedTime ) ;
			//s_pControlFocus->Render( pd3dDevice, fElapsedTime );

//         if( sm_bShowHint ) // 提示開關
//         {
//             // 畫出元件提示
//             for( int i=0; i < count ; ++i )
// 			{
// 				CDXUTControl* pControl = m_Controls.GetAt(i);  
// 
// 				if(pControl->GetVisible())
// 				{
// 					pControl->RenderHint( pd3dDevice, fElapsedTime );					
// 				}
// 
// 			}
//         }
    }
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::RenderHintAll( int iWord_Max_Width, D3DCOLOR TextColor, D3DCOLOR FrameColor, D3DCOLOR BackgroundColor, CDXUTElement* pElement, POINT &MousePoint, const WCHAR* strHint, const WCHAR* strHintImgPath, bool bHintTurnRight )
{
	RECT rcText, rcTexture, rcBack, rcLine, rcClient;
	int iTexture;

	HWND hWnd = DXUTGetHWND();
    GetClientRect( hWnd, &rcClient );

    SetRectEmpty( &rcText );
    SetRectEmpty( &rcTexture );
    SetRectEmpty( &rcBack );

	if( m_iALLHintAlpha != 300 )
	{
		int iRed   = ( BackgroundColor<<8  & 0xFF );
		int iGreen = ( BackgroundColor<<16 & 0xFF );
		int iBlue  = ( BackgroundColor<<24 & 0xFF );

		D3DCOLOR color = D3DCOLOR_ARGB( m_iALLHintAlpha, iRed, iGreen, iBlue );
		BackgroundColor = color;
	}

	if( wcslen( strHintImgPath ) )//********* 有圖片與文字的HINT（暫時不使用 ************************）
    {
		RECT *rcCatch = 0;
        iTexture = GetManager()->AddTexture( strHintImgPath );
        pTextureElement.SetTexture( iTexture, rcCatch );
        pTextureElement.TextureColor.Current = D3DCOLOR_ARGB( 255, 255, 255, 255 );

		// 過濾每行第一個字元為/n
		WCHAR temp[MAX_PATH] ;
		ZeroMemory( &temp, sizeof( temp ) );
		StringCchCopy( temp, MAX_PATH, strHint );
		//RemoveFirst_SlashN( temp );

		// 產生 wszText 的 TextObject，並指定給該 Item。
        LONG cxMaxExtent = iWord_Max_Width; 
        DWORD m_dwDefaultColor = TextColor;
        g_FDOXML.SetDefaultColor(m_dwDefaultColor);
        CTextObject* pTextObject = g_FDOXML.MakeTextObject( g_pD3DDevice, NULL, TextColor, temp, -1, cxMaxExtent);
        assert(pTextObject);

		SIZE Extent;
        pTextObject->GetExtent( Extent );
						
        SetRect( &rcText, 5, 5, Extent.cx+5, Extent.cy+5 );

		if( bHintTurnRight )
			OffsetRect( &rcText, MousePoint.x + 10, MousePoint.y - RectHeight( rcText ) - 10 );
		else
			OffsetRect( &rcText, MousePoint.x - RectWidth( rcText ) - 10, MousePoint.y - RectHeight( rcText ) - 10 );

		SetRect( &rcTexture, 43, 43, 107, 107 );
		OffsetRect( &rcTexture, rcText.left + 43, rcText.top - 45 );

		SetRect( &rcBack, rcText.left - 5, rcTexture.top - 5, rcText.right + 5, rcText.bottom + 5 );
                
		if( bHintTurnRight )
		{
			if( rcBack.right + m_x > rcClient.right )
			{
				OffsetRect( &rcText,	-( RectWidth( rcBack ) + 10 ) , 0 );
				OffsetRect( &rcBack,	-( RectWidth( rcBack ) + 10 ) , 0 );
				OffsetRect( &rcTexture, -( RectHeight( rcBack ) + 20 ), 0 );
			}
		}
		else
		{
			if( rcBack.left + m_x < rcClient.left )
			{
				OffsetRect( &rcText,	RectWidth( rcBack ) + 20 , 0 );
				OffsetRect( &rcBack,	RectWidth( rcBack ) + 20 , 0 );
				OffsetRect( &rcTexture, RectHeight( rcBack ) + 20, 0 );
			}
		}
		if( rcBack.top + m_y < rcClient.top )
        {
            OffsetRect( &rcText,	0, RectHeight( rcBack ) + 20 );
            OffsetRect( &rcBack,	0, RectHeight( rcBack ) + 20 );
			OffsetRect( &rcTexture, 0, RectHeight( rcBack ) + 20 );
        }
		if( rcBack.bottom > rcClient.bottom )
		{
            OffsetRect( &rcText,	0, rcClient.bottom - ( rcBack.bottom + m_y + 20 )  );
            OffsetRect( &rcBack,	0, rcClient.bottom - ( rcBack.bottom + m_y + 20 )  );
			OffsetRect( &rcTexture, 0, rcClient.bottom - ( rcBack.bottom + m_y + 20 )  );
        }
                        
        DrawRect( &rcBack, BackgroundColor );
        DrawSprite( &pTextureElement, &rcTexture );
        DrawTextObject( pTextObject, pElement, &rcText, false );
        delete pTextObject;
    }
	else//*********** 純文字的HINT ******************************
    {
		// 過濾每行第一個字元為/n
		WCHAR temp[MAX_PATH] ;
		ZeroMemory( &temp, sizeof( temp ) );
		StringCchCopy( temp, MAX_PATH, strHint );
		//RemoveFirst_SlashN( temp );

		// 產生 wszText 的 TextObject，並指定給該 Item。
        LONG cxMaxExtent = iWord_Max_Width; 
        DWORD m_dwDefaultColor = TextColor;
        g_FDOXML.SetDefaultColor(m_dwDefaultColor);
        CTextObject* pTextObject = g_FDOXML.MakeTextObject( g_pD3DDevice, NULL, TextColor, temp, -1, cxMaxExtent);
        assert(pTextObject);

		SIZE Extent;
        pTextObject->GetExtent( Extent );
						
		//m_pDialog->CalcTextRect( MultipleHint[ nClicked ].m_strHint, pElement, &rcText );
        //wstring str = MultipleHint[ nClicked ].m_strHint;
        //int StrLong = str.length();
        //int W = ((RectWidth( rcText )/120)+1)*12;
        SetRect( &rcText, 5, 5, Extent.cx+5, Extent.cy+5 );
        //InflateRect( &rcText, 5, 5 );
        
		if( bHintTurnRight )
			OffsetRect( &rcText, MousePoint.x + 10, MousePoint.y + RectHeight( rcText ) - 10 + 22 );
		else
			OffsetRect( &rcText, MousePoint.x /*- RectWidth( rcText ) - 10*/, MousePoint.y + RectHeight( rcText ) - 10 + 22);

		SetRect( &rcBack, rcText.left - 5, rcText.top - 5, rcText.right + 5, rcText.bottom + 5 );
        //OffsetRect( &rcBack, MousePoint.x - RectWidth( rcText ) - 5, MousePoint.y - RectHeight( rcText )/2 - 5 );

		if( rcBack.right + m_x > rcClient.right )
		{
			OffsetRect( &rcText,	-(rcBack.right + m_x - rcClient.right) , 0 );
			OffsetRect( &rcBack,	-(rcBack.right + m_x - rcClient.right) , 0 );
		}
		if( rcBack.left + m_x < rcClient.left )
		{
			OffsetRect( &rcText,	RectWidth( rcBack ) + 20 , 0 );
			OffsetRect( &rcBack,	RectWidth( rcBack ) + 20 , 0 );
		}
// 		if( bHintTurnRight )
// 		{
// 			if( rcBack.right + m_x > rcClient.right )
// 			{
// 				OffsetRect( &rcText,	-( RectWidth( rcBack ) + 10 ) , 0 );
// 				OffsetRect( &rcBack,	-( RectWidth( rcBack ) + 10 ) , 0 );
// 			}
// 		}
// 		else
// 		{
// 			if( rcBack.left + m_x < rcClient.left )
// 			{
// 				OffsetRect( &rcText,	RectWidth( rcBack ) + 20 , 0 );
// 				OffsetRect( &rcBack,	RectWidth( rcBack ) + 20 , 0 );
// 			}
// 		}
		if( rcBack.top + m_y < rcClient.top )
        {
            OffsetRect( &rcText,	0, RectHeight( rcBack ) + 20 );
            OffsetRect( &rcBack,	0, RectHeight( rcBack ) + 20 );
        }
		if( rcBack.bottom + m_y > rcClient.bottom )
		{
            OffsetRect( &rcText,	0, rcClient.bottom - ( rcBack.bottom + m_y + 20 ) );
            OffsetRect( &rcBack,	0, rcClient.bottom - ( rcBack.bottom + m_y + 20 ) );
        }
                        
        SetRect( &rcLine, rcBack.left+1, rcBack.top+1, rcBack.right-2, rcBack.bottom-2 );

        DrawRect( &rcBack, BackgroundColor );
        DrawTextObject( pTextObject, pElement, &rcText, false );
		//DrawFrameRect( &rcLine, FrameColor );	
		delete pTextObject;
	}
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::RemoveFirst_SlashN( WCHAR *temp )
{
	WCHAR temp2[MAX_PATH] ;
	ZeroMemory( &temp2, sizeof( temp2 ) );
	StringCchCopy( temp2, MAX_PATH, temp );
	int count = 0; 
	for(int i=1;count < MAX_PATH;i++)
	{
		if(i==1 && temp2[count] == L'\n')
		{
			for(int j=count;j < MAX_PATH;j++)
			{
				temp2[j] = temp2[j+1];
			}
			i = 0;
		} 
		else if(i==12 || temp2[count] == L'\n' )	
			i = 0;

		count++;
	}
	StringCchCopy( temp, MAX_PATH, temp2 );
	//return temp;
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::DrawCaption()
{
    if (!m_bCaption)
        return;

    // DrawSprite will offset the rect down by
    // m_nCaptionHeight, so adjust the rect higher
    // here to negate the effect.
    //RECT rc = { 0, -m_nCaptionHeight, m_width, 0 };//0, -m_nCaptionHeight, m_width, 0
    //DrawSprite( &m_CapElement, &rc ); // 畫出Caption的材質

    RECT rc = m_rcCaption;
    OffsetRect( &rc, 0, 1-m_nCaptionHeight );
    // debug
    if( m_bDrawCap )
        DrawRect( &rc, D3DCOLOR_ARGB(100, 255, 0, 0) );

    rc = m_rcCapText;
    OffsetRect( &rc, 0, 1-m_nCaptionHeight );
    rc.left += 5; // Make a left margin
/*    WCHAR wszOutput[256];
    StringCchCopy( wszOutput, 256, m_wszCaption );
    if( m_bMinimized )
    {
        string str;
        str.assign(" (");
        str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 894, CodeInlineText_Text));//最小化
        str.append(")");
        StringCchCat( wszOutput, 256, AnsiToUnicode(str.c_str()) );
    }
  */      
    // Create a new TextObject for caption.
    if (m_pTextObjectCaption) {
        DrawTextObject(m_pTextObjectCaption, &m_CapElement, &rc, false);
    }
}
//--------------------------------------------------------------------------------------
uint CDXUTDialog::SendEvent( UINT nEvent, bool bTriggeredByUser, CDXUTControl* pControl ,UINT nData)
{
    // If no callback has been registered there's nowhere to send the event to
    if( m_pCallbackEvent == NULL )
        return 0;

    // Discard events triggered programatically if these types of events haven't been
    // enabled
    if( !bTriggeredByUser && !m_bNonUserEvents )
        return 0;

	if(&mDEUC == m_pCallbackEventUserContext)
	{
		DialogEventUserContext deuc;
		deuc.nEventData = nData;
		deuc.pDlg = this;
		deuc.nRet = 0;
		if(!pControl)
		{
			m_pCallbackEvent( nEvent, m_ID , 0, &deuc );
		}
		else
		{
			m_pCallbackEvent( nEvent, pControl->GetID(), pControl, &deuc );
		}
		
		return deuc.nRet;
	}
	else
	{
		if( !pControl )
		{
			m_pCallbackEvent( nEvent, -1, 0, m_pCallbackEventUserContext );
		}
		else
			m_pCallbackEvent( nEvent, pControl->GetID(), pControl, m_pCallbackEventUserContext );
	}
    return 0;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight )
{
    // If this assert triggers, you need to call CDXUTDialog::Init() first.  This change
    // was made so that the DXUT's GUI could become seperate and optional from DXUT's core.  The 
    // creation and interfacing with CDXUTDialogResourceManager is now the responsibility 
    // of the application if it wishes to use DXUT's GUI.
    assert( s_pManager != NULL && L"To fix call CDXUTDialog::Init() first.  See comments for details." );

    // Make sure the list is at least as large as the index being set
    UINT i;
    for( i=m_Fonts.GetSize(); i <= index; i++ )
    {
        m_Fonts.Add( -1 );
    }

    int iFont = s_pManager->AddFont( strFaceName, height, weight );
    m_Fonts.SetAt( index, iFont );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::SetFont( UINT index, string strFaceName, LONG height, LONG weight )
{
    return SetFont( index, ANSItoUNICODE(strFaceName), height, weight );
}
//--------------------------------------------------------------------------------------
DXUTFontNode* CDXUTDialog::GetFont( UINT index )
{
    if( NULL == s_pManager )
        return NULL;

    //return s_pManager->GetFontNode( m_Fonts.GetAt( index ) );
    return s_pManager->GetFontNode( index );
}

//--------------------------------------------------------------------------------------
// ming add
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetCaptionText( const WCHAR *pwszText, RECT* prcCapText )
{
    StringCchCopy( m_wszCaption, sizeof(m_wszCaption)/sizeof(m_wszCaption[0]), pwszText);

    if( prcCapText )
        m_rcCapText = *prcCapText;

    // Create a new TextObject for caption.
    if (m_pTextObjectCaption) 
	{
        delete m_pTextObjectCaption;
    }
    m_pTextObjectCaption = g_FDOXML.MakeTextObject(g_pD3DDevice, &m_CapElement, m_wszCaption, -1, RectWidth(m_rcCapText));
    m_pTextObjectCaption->ChangeDefaultColor(m_CapElement.FontColor.Current);
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::SetCaptionText( const string &szText, RECT* prcCapText )
{
    SetCaptionText( ANSItoUNICODE(szText), prcCapText );
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetCaptionTextColor( DWORD dwColor )
{
    m_pTextObjectCaption->ChangeDefaultColor( dwColor );
}
//--------------------------------------------------------------------------------------
// ming add 計算form的範圍
//--------------------------------------------------------------------------------------
void CDXUTDialog::UpdateRects()
{
    SetRect( &m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height );
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::SetTexture( UINT index, LPCWSTR strFilename )
{
    // If this assert triggers, you need to call CDXUTDialog::Init() first.  This change
    // was made so that the DXUT's GUI could become seperate and optional from DXUT's core.  The 
    // creation and interfacing with CDXUTDialogResourceManager is now the responsibility 
    // of the application if it wishes to use DXUT's GUI.
    assert( s_pManager != NULL && L"To fix call CDXUTDialog::Init() first.  See comments for details." ); 

    // Make sure the list is at least as large as the index being set
    for( UINT i=m_Textures.GetSize(); i <= index; i++ )
    {
        m_Textures.Add( -1 );
    }

    int iTexture = s_pManager->AddTexture( strFilename );

    m_Textures.SetAt( index, iTexture );
    return S_OK;
}
//--------------------------------------------------------------------------------------
int CDXUTDialog::GetCaptionHeight() const 
{ 
	if(m_bCaption)
		return m_nCaptionHeight; 
	return 0;
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::GetCaptionRect(RECT& rcOut) const
{ 
	if(m_bCaption)
		rcOut = m_rcCaption; 
	else
		SetRect(&rcOut,0,0,0,0);
}
//--------------------------------------------------------------------------------------
DXUTTextureNode* CDXUTDialog::GetTexture( UINT index )
{
    if( NULL == s_pManager )
        return NULL;

    // ming edit >>
    //return s_pManager->GetTextureNode( m_Textures.GetAt( index ) );
    return s_pManager->GetTextureNode( index );
    /// ming edit <<
}    

//--------------------------------------------------------------------------------------
void CDXUTDialog::SetPressedControl(CDXUTControl* pControl)
{
    s_pControlPressed = pControl;
}
//======================================================================================
// MsgProc
//======================================================================================
bool CDXUTDialog::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// Returns bNoFurtherProcessing.
// (1) Non-client area 處理：處理 Caption 的滑鼠訊息。
// (2) Client area mouse message 處理。
{

	
    // For invisible dialog, do not handle anything.
    if( !m_bVisible )
        return false;

	
    // If the dialog is minimized, don't send any messages to controls.
    //if( m_bMinimized )
    //    return false;
	

    // If automation command-line switch is on, enable this dialog's keyboard input
    // upon any key press or mouse click.
    if( //DXUTGetAutomation() && ming unknow
        ( WM_LBUTTONDOWN == uMsg || WM_LBUTTONDBLCLK == uMsg || WM_KEYDOWN == uMsg ) )
    {
        EnableKeyboardInput(true);
    }

    //----------------------------------------------------------------------------
    // 計算滑鼠的客戶端位置。
    //----------------------------------------------------------------------------
    POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
    mousePoint.x -= m_x;
    mousePoint.y -= m_y;
    // If caption is enabled, offset the Y coordinate by the negative of its height.
    if (m_bCaption)
        mousePoint.y -= m_nCaptionHeight;
    if (true == g_Mousetrap.ControlTrapProc(uMsg, mousePoint, wParam, lParam))
        return true;

    // If a control is in focus, it belongs to this dialog, and it's enabled, then give
    // it the first chance at handling the message.
	if( s_pControlFocus && 
		s_pControlFocus->GetEnabled() )
	{
		if(fnIsParent(s_pControlFocus->m_pDialog , this))
			return false;
		if(s_pControlFocus->m_pDialog == this)
		{			
			// If the control MsgProc handles it, then we don't.
			if( s_pControlFocus->MsgProc( uMsg, wParam, lParam ) )
				return true;
		}
    
	}
    if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) {
        return MouseProc(hWnd, uMsg, wParam, lParam);
    }
    // Keyboard Messages
    if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) {
        return KeyboardProc(hWnd, uMsg, wParam, lParam);
    }
    // Other messages
    switch( uMsg )
    {
        case WM_SIZE:
        case WM_MOVE:
        {
            // Handle sizing and moving messages so that in case the mouse cursor is moved out
            // of an UI control because of the window adjustment, we can properly
            // unhighlight the highlighted control.
            POINT pt = { -1, -1 };
            OnMouseMove( pt );
            break;
        }

        case WM_ACTIVATEAPP:
            // Call OnFocusIn()/OnFocusOut() of the control that currently has the focus
            // as the application is activated/deactivated.  This matches the Windows
            // behavior.
            if( s_pControlFocus && 
                s_pControlFocus->m_pDialog == this && 
                s_pControlFocus->GetEnabled() )
            {
                if( wParam )
				{
                    s_pControlFocus->OnFocusIn();
				}
                else
				{
                    s_pControlFocus->OnFocusOut();
					s_pControlFocus = 0;
				}
            }
            break;
    } // end switch (uMsg)

    return false;
}
//======================================================================================
// MouseProc
//======================================================================================
bool CDXUTDialog::MouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// return bStopProcessing
{
	
	if(s_pDialogDragging != 0)
	{
		if(uMsg == WM_LBUTTONUP)
		{
			EndDragDrop();						
			//return true;
		}	
	}

	if(s_pDialogDragging != 0 && s_pDialogDragging != this)
	{
		// 拖曳訊息 發生時候不是自己的都pass
		return false;
	}


	

	bool bStopOuter;
    if (PreprocessMouseMessage(uMsg, wParam, lParam, bStopOuter))
        return bStopOuter;
    
	if (SideMouseProc(hWnd, uMsg, wParam, lParam))      // Darkness add
        return true;

	if (CaptionMouseProc(hWnd, uMsg, wParam, lParam))
		return true;

	
	
	if (ClientMouseProc(hWnd, uMsg, wParam, lParam))
		return true;

	return false;
}

bool CDXUTDialog::CheckCanPush(UINT uMsg)
{
	if(uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)

	{
		//檢查有hook拉取成功訊息就不往下送。	
		DialogHandle hnd = m_HookMgr.CheckHook(mHnd);		
		CDXUTDialog* pPreDlg = this;
		bool bFindAny = false;
		for(;;)
		{
			if(CDXUTDialog* pDlg = CDXUTDialog::Query(hnd))
			{		
				pPreDlg->OnDialogFocusOut();
				bFindAny = true;				
				pDlg->push_front();
				hnd = m_HookMgr.CheckHook(pDlg->GetHandle());
				pPreDlg = pDlg;
			}
			else
				break;
		}
		
		return !(bFindAny);
	}
	return true;
}

//--------------------------------------------------------------------------------------
bool CDXUTDialog::PreprocessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bStopOuter)
// return bStopProcessing
{
    bStopOuter = false;
    
    switch (uMsg) 
    {
        case WM_LBUTTONDOWN:
        {
            POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
            if (ContainsPoint(mousePoint)) 
			{
                m_bLButtonDown = true;
				
            } // end if
            break;
        }
        case WM_LBUTTONUP:
        {
            
            //if (s_pDialogDragging == this)
            EndDragDrop();
			m_bLButtonDown = false;
			
            break;
        }
        case WM_MOUSEMOVE:
        {
            // 讓拖拉中的視窗可以從底下經過上層視窗。
//             if (MK_LBUTTON == wParam) {
//                 if (s_pDialogDragging)
//                 if (s_pDialogDragging != this)
//                 if (s_pMouseDialog != this)
//                     return true;
//             }
            // 檢查是否為拖拉中的視窗。
            if (m_bLButtonDown /*&& (MK_LBUTTON & wParam)*/)
			{	
				if(s_pDialogDragging == 0)
				{
					s_pDialogDragging = this;
					BeginDragDrop();
					SendEvent( EVENT_DIALOG_DRAG , true , 0 , (UINT)&m_LastMouse) ; 
					
				}
				else
				{
					//assert(0);
					
				}
				 
                
				
			}
            break;
        }
    } // end switch
    
    return false;
}
//--------------------------------------------------------------------------------------
//拖曳縮放Dialog的大小
//--------------------------------------------------------------------------------------
bool CDXUTDialog::SideMouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    if ( !m_bCanSizeChange )
        return false;
    if( m_bMinimized )    // 視窗最小化的時候不執行
        return false;

    // 判斷位置
    MouseArea( wParam, lParam );


    if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK )
    {
        if(m_iArea != 0)
        {
            POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
            m_LastMouse = mousePoint;
            RECT rc = m_rcCaption;
            OffsetRect( &rc, m_x, m_y );
            if( m_bAptoticSize && !m_bSizeChange )
            {
                m_x2 = m_x;
                m_y2 = m_y;
                m_width2 = m_width;
                m_height2 = m_height;
            }
            push_front();
            m_bSizeChange = true;
            m_dxDrag = 0;
            m_dyDrag = 0;
            g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
			return true;
        }
    }

    else if (uMsg == WM_LBUTTONUP && m_bSizeChange)
    {
		m_bLButtonDown = false;
        m_bSizeChange = false;
        m_dxDrag = 0;
        m_dyDrag = 0;

        // 固定大小控制
        AptoticChangeSize( hWnd );
        Move3Btn();
		SendEvent(EVENT_DIALOG_MOVE_STOP,true ,0 ,0);
        //m_pCallbackEvent( EVENT_DIALOG_MOVE_STOP, -1, NULL, m_pCallbackEventUserContext );
        return true;
    }

    else if( uMsg == WM_MOUSEMOVE && m_bSizeChange ) // 拖曳
    {
        if( LOWORD(wParam) == MK_LBUTTON )
        {
            // 拖曳控制
            DragToChnageSize( hWnd, uMsg, wParam, lParam );

            // 送出Dialog移動事件
            Move3Btn();
			SendEvent(EVENT_DIALOG_MOVE,true ,0 ,0);
            //m_pCallbackEvent( EVENT_DIALOG_MOVE, -1, NULL, m_pCallbackEventUserContext );
            return true;
        }
        return false;
    } // end else if

    return false;

}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::CaptionMouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// Returns bNoFurtherProcessing.
{
    // If caption is enable, check for clicks in the caption area.
    if (!m_bCaption)
        return false;

	

    // 先送訊息給 最小化 及 關閉鈕 及 還原鈕
    POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

	if(uMsg == WM_MOUSEWHEEL )
	{
		::ScreenToClient(hWnd,&mousePoint);
	}

	// Mouse outside of dialog.
	if (ContainsPoint(mousePoint))
		if( !CheckCanPush(uMsg) )
		{
			return false;
		}

    mousePoint.x -= m_x;
    mousePoint.y -= m_y + m_nCaptionHeight; // shift


	{
		//----------------------------------------------------------------------------
		// If a control is in focus, it belongs to this dialog, and it's enabled, then give
		// it the first chance at handling the message.
		//----------------------------------------------------------------------------
		if (s_pControlFocus)
		{
			if(fnIsParent(s_pControlFocus->m_pDialog, this))
				return false;
			if( s_pControlFocus->m_pDialog == this ) 
				//if (s_pControlFocus->m_pDialog == this)
				if (s_pControlFocus->GetEnabled())
				{					
					if (s_pControlFocus->HandleMouse(uMsg, mousePoint, wParam, lParam))
						return true;
				}
		}
	}

// 	if( !CheckCanPush(uMsg) )
// 	{
// 		return false;
// 	}
    // 送訊息給最小化鈕     Darkness add
    if( m_MinBtn->GetVisible() )
    {
        if( m_MinBtn->HandleMouse( uMsg, mousePoint, wParam, lParam ) )
        {
            if( uMsg == WM_LBUTTONUP && m_MinBtn->ContainsPoint(mousePoint) )
            {
                MiniChange( hWnd );
            }

            return true;
        }
    }

    // 送訊息給還原鈕   Darkness add
    if( m_OrigBtn->GetVisible() && m_OrigBtn->ContainsPoint(mousePoint) )
    {
        if( m_OrigBtn->HandleMouse( uMsg, mousePoint, wParam, lParam ) )
        {
            if( uMsg == WM_LBUTTONUP && m_OrigBtn->ContainsPoint(mousePoint) )
            {
                if( m_bMinimized )
                {
                    //string str;
                    //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 894, CodeInlineText_Text ) );//最小化
                    m_MinBtn->SetHint( sm_hintTextMinimum.c_str() );
                }
                m_bMinimized = false;
                m_x = m_iOriginX;
                m_y = m_iOriginY;
                m_width = m_iOriginWidth;
                m_height = m_iOriginHeight;
                SetLocation( m_x, m_y );
                SetSize( m_width, m_height );
				m_rcCaption = m_rcCaption1;
                Move3Btn();
                SendEvent( EVENT_DIALOG_CHANGEBIG, true, m_OrigBtn );
            }
            return true;
        }
    }

    // 送訊息給關閉鈕
    if( m_CloseBtn->GetVisible() && m_CloseBtn->ContainsPoint(mousePoint) )
    {
        if( m_CloseBtn->HandleMouse( uMsg, mousePoint, wParam, lParam ) )
        {
            if( uMsg == WM_LBUTTONUP && m_CloseBtn->ContainsPoint(mousePoint) )
            {
                SendEvent( EVENT_DIALOG_CLOSE, true, m_CloseBtn );
                Hide();
            }
            return true;
        }
    }

    if (DragToMove(m_rcCaption, hWnd, uMsg, wParam, lParam))
        return true;
    
    return false;
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::ClientMouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// Returns bNoFurtherProcessing.
{
    // If not accepting mouse input, return false to indicate the message should still 
    // be handled by the application (usually to move the camera).
    if( !m_bMouseInput )
        return false;

    POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

	if(uMsg == WM_MOUSEWHEEL )
	{
		::ScreenToClient(hWnd,&mousePoint);
	}

    // Mouse outside of dialog.
    if (!ContainsPoint(mousePoint))
        return HandleMouseOutside(hWnd, uMsg, wParam, lParam);
	
    // Mouse inside of dialog.
	
    if (HandleMouseInside(hWnd, uMsg, wParam, lParam))
        return true;

	

// 	if( !CheckCanPush(uMsg) )
// 	{
// 		return false;
// 	}

    // Still not handled, hand this off to the dialog. Return false to indicate the
    // message should still be handled by the application (usually to move the camera).

    // DragToMove
	if (m_bDragToMove) {
        if (m_pDialogList && m_pDialogList->front() == this) 
		{
            RECT rcDragToMove = { 0, 0, GetWidth(), GetHeight() };
            if (DragToMove(rcDragToMove, hWnd, uMsg, wParam, lParam))
                return true;
        } // end if
    } // end if

    return true;
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::HandleMouseOutside(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// return bStopOuter
{
	{
		//----------------------------------------------------------------------------
		// 計算滑鼠的客戶端位置。
		//----------------------------------------------------------------------------
		POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
		if(uMsg == WM_MOUSEWHEEL )
		{
			::ScreenToClient(hWnd,&mousePoint);
		}
		mousePoint.x -= m_x;
		mousePoint.y -= m_y;

		// If caption is enabled, offset the Y coordinate by the negative of its height.
		if (m_bCaption)
			mousePoint.y -= m_nCaptionHeight;
		//----------------------------------------------------------------------------
		// If a control is in focus, it belongs to this dialog, and it's enabled, then give
		// it the first chance at handling the message.
		//----------------------------------------------------------------------------
		if (s_pControlFocus)
		{
			if(fnIsParent(s_pControlFocus->m_pDialog, this))
				return false;
			if (s_pControlFocus->m_pDialog == this)
				if (s_pControlFocus->GetEnabled())
				{
					
					if (s_pControlFocus->HandleMouse(uMsg, mousePoint, wParam, lParam))
						return true;
				}
		}
	}
	

    switch( uMsg )
    {
        // 在 Dialog 外面點選滑鼠左鍵：這個 Dialog 失焦，外面被點選的 Dialog 得到焦點。
        case WM_LBUTTONDOWN:
        {
            if (s_pFocusDialog==this && !s_pFocusDialog->GetAlwaysFocus()) {
                s_pFocusDialog->OnMouseLeave();
                OnDialogFocusOut();
            }
            break;
        }
        // 鼠標移動到 Dialog 外面。
        case WM_MOUSEMOVE:
        {
            if (s_pMouseDialog==this && !s_pMouseDialog->GetAlwaysFocus()) {
                s_pMouseDialog->OnMouseLeave();
            }
            // 如果之前有 Button 被按下，然後滑鼠鍵移出了該 Button。
            if (s_pControlPressed && s_pControlPressed->GetParent() == this ) 
			{
                POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
                s_pControlPressed->HandleMouse(uMsg, mousePoint, wParam, lParam);
                s_pControlPressed = NULL;
            } 
            if (m_bLButtonDown && MK_LBUTTON == wParam) {
                return true;
            }
            break;
        }
    }        
    return false;
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::HandleMouseInside(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	if( !CheckCanPush(uMsg) )
	{
		_LOG(0,L"Normal_Forbidden if( !CheckCanPush(uMsg) )");
		g_Animation2DManager.GetMouseAnimation()->Play( "Normal_Forbidden" );
		return true;
	}
	else
	{
		g_Animation2DManager.GetMouseAnimation()->Play( "Normal" );
	}
	//----------------------------------------------------------------------------
	// 計算滑鼠的客戶端位置。
	//----------------------------------------------------------------------------
	POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
	if(uMsg == WM_MOUSEWHEEL )
	{
		::ScreenToClient(hWnd,&mousePoint);
	}
	mousePoint.x -= m_x;
	mousePoint.y -= m_y;

	// If caption is enabled, offset the Y coordinate by the negative of its height.
	if (m_bCaption)
		mousePoint.y -= m_nCaptionHeight;

    //--------------------------------------------------------------------------
    // Pre-processing of mouse messages.
    //--------------------------------------------------------------------------
    switch (uMsg) {
        case WM_MOUSEMOVE: 
		{
            // Switch s_pMouseDialog. 某些 dialog 的 control 正好位於邊界。
            if (s_pMouseDialog != this)
                OnMouseEnter();
            SwitchMouseOverControl(mousePoint);
			break;
        } // end case WM_MOUSEMOVE

		case WM_MOUSEWHEEL : 
		{
			UINT uLines;
			SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
			
			SendEvent( EVENT_DIALOG_MOUSEWHEEL , true , 0 , nScrollAmount ) ; 
			break;
		}
    } // end switch

	
	if (HandleMouseOnDialog(uMsg, mousePoint, wParam, lParam))
		return true;
    //--------------------------------------------------------------------------
    // Mouse on controls.
    //--------------------------------------------------------------------------
    if (HandleMouseOnControls(uMsg, mousePoint, wParam, lParam))
        return true;
    //--------------------------------------------------------------------------
    // Mouse on non-control area.
    //--------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------
    // Post-processing of mouse messages.
    //--------------------------------------------------------------------------
    switch (uMsg) {
        case WM_MOUSEMOVE: {
            // Dragging
            if (m_bLButtonDown && MK_LBUTTON == wParam && s_pDialogDragging == this) 
                return true;
        } // end case WM_MOUSEMOVE
    } // end switch
    
    return false;
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::HandleMouseOnControls(UINT uMsg, POINT mousePoint, WPARAM wParam, LPARAM lParam)
// mousePoint is in client-area. See HandleMouseInside().
{
    
	//----------------------------------------------------------------------------
	// If a control is in focus, it belongs to this dialog, and it's enabled, then give
	// it the first chance at handling the message.
	//----------------------------------------------------------------------------
	if (s_pControlFocus)
	{
		if(fnIsParent(s_pControlFocus->m_pDialog, this))
			return false;
		if(s_pControlFocus->m_pDialog == this && s_pControlFocus->GetEnabled())						
		{			
			if (s_pControlFocus->HandleMouse(uMsg, mousePoint, wParam, lParam))
				return true;
		}
	}
    //----------------------------------------------------------------------------
    // No control in focus handle this mouse message, see if the mouse is over any control.
    //----------------------------------------------------------------------------
    CDXUTControl* pControl = GetControlAtPoint(mousePoint);
    if (pControl != NULL)
    if (pControl->GetEnabled())
    if (pControl->HandleMouse( uMsg, mousePoint, wParam, lParam ))
        return true;

    //----------------------------------------------------------------------------
    // Post-processing of mouse messages.
    // Mouse not over any controls in this dialog, if there was a control
    // which had focus it just lost it
    //----------------------------------------------------------------------------
    if( uMsg == WM_LBUTTONDOWN && 
        s_pControlFocus && 
        s_pControlFocus->m_pDialog == this )
    {
        s_pControlFocus->OnFocusOut();
        s_pControlFocus = NULL;
    }

    return false;
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::HandleMouseOnDialog(UINT uMsg, POINT mousePoint, WPARAM wParam, LPARAM lParam)
// mousePoint is in client-area. See HandleMouseInside().
{
    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
		{
			Raise();
			CDXUTControl* pControl = GetControlAtPoint(mousePoint);
			if (pControl != NULL && pControl->GetEnabled()) {
				s_pControlPressed = pControl;
			}
			SendEvent( EVENT_DIALOG_MOUSE_CLICKED, true, 0 , (UINT)&mousePoint);
			break;
		}
        case WM_LBUTTONDBLCLK:
        {
            Raise();
            CDXUTControl* pControl = GetControlAtPoint(mousePoint);
            if (pControl != NULL && pControl->GetEnabled()) {
                s_pControlPressed = pControl;
            }
			SendEvent( EVENT_DIALOG_MOUSE_DOUBLECLICKED, true, 0 , (UINT)&mousePoint);
            break;
        }
		case WM_LBUTTONUP:
		{
			m_bLButtonDown = false;

			Raise();
            CDXUTControl* pControl = GetControlAtPoint(mousePoint);
            if (pControl != NULL && pControl->GetEnabled()) {
                s_pControlPressed = pControl;
            }
			SendEvent( EVENT_DIALOG_MOUSE_LCLICKED, true, 0 , (UINT)&mousePoint);
            break;
		}
		case WM_RBUTTONUP:
		{
			Raise();
			CDXUTControl* pControl = GetControlAtPoint(mousePoint);
			if (pControl != NULL && pControl->GetEnabled()) {
				s_pControlPressed = pControl;
			}
			SendEvent( EVENT_DIALOG_MOUSE_RCLICKED, true, 0 , (UINT)&mousePoint);
			break;
		}
        case WM_MOUSEMOVE:
        {
            // 如果之前有 Button 被按下，然後滑鼠鍵移出了該 Button。
            if (s_pControlPressed) {
                if (!s_pControlPressed->ContainsPoint(mousePoint)) {
                    s_pControlPressed->HandleMouse(uMsg, mousePoint, wParam, lParam);
                    s_pControlPressed = NULL;
                } // end if
            } // end if

            if( s_pMouseDialog == this )
                break;
            if( s_pMouseDialog && s_pMouseDialog!=s_pFocusDialog && !s_pMouseDialog->GetAlwaysFocus() )
                s_pMouseDialog->OnMouseLeave(); // 顯示效果 fade out

            OnMouseEnter();
            break;
        }
    } // end switch

    return false;
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::DragToMove(const RECT& rcDrag, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // 在可以變動SIZE的DIALOG的CAPTION上按兩下滑鼠右鍵可恢復預設的DIALOG大小
    /*if( uMsg == WM_RBUTTONDBLCLK && m_bCanSizeChange )
    {
        POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
        RECT rc = rcDrag;
        OffsetRect( &rc, m_x, m_y );
        if( PtInRect( &rc, mousePoint ) )
        {
           if( m_bMinimized )
                {
                    string str;
                    str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 894, CodeInlineText_Text ) );//最小化
                    m_MinBtn->SetHint( str.c_str() );
                }
                m_bMinimized = false;
                m_x = m_iOriginX;
                m_y = m_iOriginY;
                m_width = m_iOriginWidth;
                m_height = m_iOriginHeight;
                SetLocation( m_x, m_y );
                SetSize( m_width, m_height );
                m_pCallbackEvent( EVENT_DIALOG_MOVE, -1, NULL, m_pCallbackEventUserContext );
                return true;
        }
    }*/
    if( uMsg == WM_LBUTTONDBLCLK && m_bCanMinimized )
    {
        POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
        m_LastMouse = mousePoint;
        RECT rc = rcDrag;
        OffsetRect( &rc, m_x, m_y );
        if( PtInRect( &rc, mousePoint ) )
        {
            MiniChange( hWnd );
            return true;
        }
    }
    if( uMsg == WM_LBUTTONDOWN )
    {
        POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
        m_LastMouse = mousePoint;
        RECT rc = rcDrag;
        OffsetRect( &rc, m_x, m_y );
        if( PtInRect( &rc, mousePoint ) )
        {
            push_front();
            m_bDragging = true;
            m_dxDrag = 0;
            m_dyDrag = 0;
            return true;
        }
    }
    else if (uMsg == WM_LBUTTONUP && m_bDragging)
    {
        m_bDragging = false;
        m_dxDrag = 0;
        m_dyDrag = 0;
		SendEvent(EVENT_DIALOG_MOVE_STOP , true , 0 , 0);
        //m_pCallbackEvent( EVENT_DIALOG_MOVE_STOP, -1, NULL, m_pCallbackEventUserContext );
        return true;
    }
    else if( uMsg == WM_MOUSEMOVE && m_bDragging ) // 拖曳
    {
        if( LOWORD(wParam) == MK_LBUTTON )
        {
            POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
            m_dxDrag = mousePoint.x - m_LastMouse.x;
            m_dyDrag = mousePoint.y - m_LastMouse.y;
            SetLocation(m_x + m_dxDrag, m_y + m_dyDrag);
            m_LastMouse = mousePoint; // 記錄最後點
            // 送出Dialog移動事件
			SendEvent(EVENT_DIALOG_MOVE , true , 0 , 0);
            //m_pCallbackEvent( EVENT_DIALOG_MOVE, -1, NULL, m_pCallbackEventUserContext );
        }
        return true;
    }

    return false;
}
//--------------------------------------------------------------------------------------
// 最小化控制
//--------------------------------------------------------------------------------------
void CDXUTDialog::MiniChange( HWND hWnd )
{
    if( m_bCanMinimized )
    {
        m_bMinimized = !m_bMinimized;
        push_front();

        if( m_bMinimized )
        {
            //string str;
            //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 1236, CodeInlineText_Text ) );//放大
            m_MinBtn->SetHint( sm_hintTextMaximum.c_str() );
            // 記憶最小化之前的SIZE
            m_x_r = m_width;
            m_y_b = m_height;
            int a = m_iFrameGauge*2;
            SetSize( 125, a );
            if( m_x < 0)
            {
                m_x = 0;
                m_y = 0;
                SetLocation( m_x, m_y );
            }
			m_rcCaption = m_rcCaption2;
            Move3Btn();
            SendEvent( EVENT_DIALOG_CHANGEMINI, true, m_MinBtn );
        }
        else
        {   
            //string str;
            //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 894, CodeInlineText_Text ) );//最小化
            m_MinBtn->SetHint( sm_hintTextMinimum.c_str() );
            // 復原最小化之前的SIZE
            m_width = m_x_r;
            m_height = m_y_b;

            SetSize( m_width, m_height );
			m_rcCaption = m_rcCaption1;
            Move3Btn();
            SendEvent( EVENT_DIALOG_CHANGEBIG, true, m_MinBtn );
        }
    }
}
//--------------------------------------------------------------------------------------
// 判斷滑鼠所在( 預設整個可調整個SIZE的外框左上角是X=0, Y=0-m_nCaptionHeight, 厚度是m_SideGauge(=5), 寬=m_with, 高=m_height )
//--------------------------------------------------------------------------------------
void CDXUTDialog::MouseArea( WPARAM wParam, LPARAM lParam )
{
    if(!m_bSizeChange)
    {
        POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
        mousePoint.x -= m_x;
        mousePoint.y -= m_y + m_nCaptionHeight; // shift
        if( ( mousePoint.x >= 0 ) && ( mousePoint.x <= m_SideGauge ) && ( mousePoint.y >= 0-m_nCaptionHeight ) && ( mousePoint.y <= 0-m_nCaptionHeight+m_SideGauge ) && ( m_iChangeSizeMode == 0 || m_iChangeSizeMode == 1 ) )
        {
            m_iArea = 1;    // 左上
            //mousestr.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 75, CodeInlineText_Text ));//
			mousestr = sm_resizeMouseAnim;
            g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
        }
        else if( ( mousePoint.x > m_SideGauge ) && ( mousePoint.x < m_width-m_SideGauge ) && ( mousePoint.y >= 0-m_nCaptionHeight ) && ( mousePoint.y <= 0-m_nCaptionHeight+m_SideGauge ) && ( m_iChangeSizeMode == 0 || m_iChangeSizeMode == 2 ) )
        {
            m_iArea = 2;    //   上
            //mousestr.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 75, CodeInlineText_Text ));//
            mousestr = sm_resizeMouseAnim;
			g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
        }
        else if( ( mousePoint.x >= m_width-m_SideGauge ) && ( mousePoint.x < m_width ) && ( mousePoint.y >= 0-m_nCaptionHeight ) && ( mousePoint.y <= 0-m_nCaptionHeight+m_SideGauge ) && ( m_iChangeSizeMode == 0 || m_iChangeSizeMode == 1 ) )
        {
            m_iArea = 3;    // 右上
            //mousestr.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 75, CodeInlineText_Text ));//
            mousestr = sm_resizeMouseAnim;
			g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
        }
        else if( ( mousePoint.x >= m_width-m_SideGauge ) && ( mousePoint.x < m_width ) && ( mousePoint.y > 0-m_nCaptionHeight+m_SideGauge ) && ( mousePoint.y < m_height-m_nCaptionHeight-m_SideGauge ) && ( m_iChangeSizeMode == 0 || m_iChangeSizeMode == 2  || m_iChangeSizeMode == 3 ) )
        {
            m_iArea = 4;    // 右
            //mousestr.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 75, CodeInlineText_Text ));//
            mousestr = sm_resizeMouseAnim;
			g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
        }
        else if( ( mousePoint.x >= m_width-m_SideGauge ) && ( mousePoint.x < m_width ) && ( mousePoint.y >= m_height-m_nCaptionHeight-m_SideGauge ) && ( mousePoint.y < m_height-m_nCaptionHeight ) && ( m_iChangeSizeMode == 0 || m_iChangeSizeMode == 1  || m_iChangeSizeMode == 3 ) )
        {
            m_iArea = 5;    // 右下
            //mousestr.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 75, CodeInlineText_Text ));//
            mousestr = sm_resizeMouseAnim;
			g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
        }
        else if( ( mousePoint.x > m_SideGauge ) && ( mousePoint.x < m_width-m_SideGauge ) && ( mousePoint.y >= m_height-m_nCaptionHeight-m_SideGauge ) && ( mousePoint.y < m_height-m_nCaptionHeight ) && ( m_iChangeSizeMode == 0 || m_iChangeSizeMode == 2  || m_iChangeSizeMode == 3  || m_iChangeSizeMode == 4 ) )
        {
            m_iArea = 6;    //   下
            //mousestr.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 75, CodeInlineText_Text ));//
            mousestr = sm_resizeMouseAnim;
			g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
        }
        else if( ( mousePoint.x >= 0 ) && ( mousePoint.x <= m_SideGauge ) && ( mousePoint.y >= m_height-m_nCaptionHeight-m_SideGauge ) && ( mousePoint.y < m_height-m_nCaptionHeight ) && ( m_iChangeSizeMode == 0 || m_iChangeSizeMode == 1 ) )
        {
            m_iArea = 7;    // 左下
            //mousestr.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 75, CodeInlineText_Text ));//
            mousestr = sm_resizeMouseAnim;
			g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
        }
        else if( ( mousePoint.x >= 0 ) && ( mousePoint.x <= m_SideGauge ) && ( mousePoint.y > 0-m_nCaptionHeight+m_SideGauge ) && ( mousePoint.y < m_height-m_nCaptionHeight-m_SideGauge ) && ( m_iChangeSizeMode == 0 || m_iChangeSizeMode == 2 ) )
        {
            m_iArea = 8;    // 左
            //mousestr.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 75, CodeInlineText_Text ));//
            mousestr = sm_resizeMouseAnim;
			g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
        }
        else
            m_iArea = 0;
            //mousestr.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 884, CodeInlineText_Text ));//
    }
}
//--------------------------------------------------------------------------------------
// 固定SIZE控制(放掉左鍵後產生變化)
//--------------------------------------------------------------------------------------
void CDXUTDialog::AptoticChangeSize( HWND hWnd )
{
        if( m_bAptoticSize )
        {
            // X
            if( m_iSubX>0 && m_x-m_x2!=0 && (m_x-m_x2)%m_iSubX <= m_iSubX/2 )
            {
                if( m_x-m_x2>0 )
                    m_x2 += ((m_x-m_x2)/m_iSubX)*m_iSubX;
                else if( m_x-m_x2<0 && (m_x-m_x2)%m_iSubX < -m_iSubX/2 )
                    m_x2 += (((m_x-m_x2)/m_iSubX)-1)*m_iSubX;
                else
                    m_x2 += ((m_x-m_x2)/m_iSubX)*m_iSubX;
            }
            else if( m_iSubX>0 && m_x-m_x2!=0 && (m_x-m_x2)%m_iSubX > m_iSubX/2 )
                m_x2 += (((m_x-m_x2)/m_iSubX)+1)*m_iSubX;
             // Y
            if( m_iSubY>0 && m_y-m_y2!=0 && (m_y-m_y2)%m_iSubY <= m_iSubY/2 )
            {
                if( m_y-m_y2>0 )
                    m_y2 += ((m_y-m_y2)/m_iSubY)*m_iSubY;
                else if( m_y-m_y2<0 && (m_y-m_y2)%m_iSubY < -m_iSubY/2)
                    m_y2 += (((m_y-m_y2)/m_iSubY)-1)*m_iSubY;
                else
                    m_y2 += ((m_y-m_y2)/m_iSubY)*m_iSubY;
            }
            else if( m_iSubY>0 && m_y-m_y2!=0 && (m_y-m_y2)%m_iSubY > m_iSubY/2 )
                m_y2 += (((m_y-m_y2)/m_iSubY)+1)*m_iSubY;
            // WIDTH
            if( m_iSubX>0 && m_width-m_width2!=0 && (m_width-m_width2)%m_iSubX <= m_iSubX/2 )
            {
                if( m_width-m_width2>0 )
                    m_width2 += ((m_width - m_width2)/m_iSubX)*m_iSubX;
                else if( m_width-m_width2<0 && (m_width-m_width2)%m_iSubX < -m_iSubX/2 )
                    m_width2 += (((m_width - m_width2)/m_iSubX)-1)*m_iSubX;
                else
                    m_width2 += ((m_width - m_width2)/m_iSubX)*m_iSubX;
            }
            else if( m_iSubX>0 && m_width-m_width2!=0 && (m_width-m_width2)%m_iSubX > m_iSubX/2 )
                m_width2 += (((m_width - m_width2)/m_iSubX)+1)*m_iSubX;
            // HEIGHT
            if( m_iSubY>0 && m_height-m_height2!=0 && (m_height-m_height2)%m_iSubY <= m_iSubY/2 )
            {
                if( m_height-m_height2>0 )
                    m_height2 += ((m_height-m_height2)/m_iSubY)*m_iSubY;
                else if( m_height-m_height2<0 && (m_height-m_height2)%m_iSubY < m_iSubY/2 )
                    m_height2 += (((m_height-m_height2)/m_iSubY)-1)*m_iSubY;
                else
                    m_height2 += ((m_height-m_height2)/m_iSubY)*m_iSubY;
            }
            else if( m_iSubY>0 && m_height-m_height2!=0 && (m_height-m_height2)%m_iSubY > m_iSubY/2 )
                m_height2 += (((m_height-m_height2)/m_iSubY)+1)*m_iSubY;
            // *****************************************************************
            if( m_iSubX==0 )
            {   m_width2 = m_width;     m_x2 = m_x; }
            if( m_iSubY==0 )
            {   m_height2 = m_height;   m_y2 = m_y; }
            // *****************************************************************
            // 大小限制
            RECT rcClient;
            GetClientRect( hWnd, &rcClient );
            if( m_x2 < 0 )
            {   m_x2 += m_iSubX;    m_width2 -= m_iSubX;    }
            if( m_y2 < 0 )
            {   m_y2 += m_iSubY;    m_height2 -= m_iSubY;    }
            if( m_width2+m_x2 > rcClient.right )
                m_width2 -= m_iSubX;
            if( m_height2+m_y2 > rcClient.bottom )
                m_height2 -= m_iSubY;
            if( m_iMaxWidth > 0 && m_width2 > m_iMaxWidth )
                m_width2 -= m_iSubX;
            if( m_iMaxHeight > 0 && m_height2 > m_iMaxHeight )
                m_height2 -= m_iSubY;
            // *****************************************************************

            SetLocation( m_x2, m_y2 );
            SetSize( m_width2 , m_height2 );

            m_x2 = 0;
            m_y2 = 0;
            m_width2 = 0;
            m_height2 = 0;
            // 送出Dialog移動事件
            Move3Btn();
			SendEvent(EVENT_DIALOG_MOVE , true , 0 , 0);
            //m_pCallbackEvent( EVENT_DIALOG_MOVE, -1, NULL, m_pCallbackEventUserContext );
        }
}
//--------------------------------------------------------------------------------------
// 拖曳變更Dialog大小
//--------------------------------------------------------------------------------------
void CDXUTDialog::DragToChnageSize( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
            m_x_r = m_x + m_width;
            m_y_b = m_y + m_height;
            POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
            g_Animation2DManager.GetMouseAnimation()->Play( mousestr.c_str() );
            //******************************************************************
            // 依照點選的位置不同,用不同方式計算(基本上向外擴張都是>0)
            if( m_iArea == 1 || m_iArea == 7 || m_iArea == 8 )
            {
                m_dxDrag = m_LastMouse.x - mousePoint.x;
                m_x -= m_dxDrag;
            }
            if( m_iArea == 1 || m_iArea == 2 || m_iArea == 3 )
            {
                m_dyDrag = m_LastMouse.y - mousePoint.y;
                m_y -= m_dyDrag;
            }
            if( m_iArea == 3 || m_iArea == 4 || m_iArea == 5 )
                m_dxDrag = mousePoint.x - m_LastMouse.x;
            if( m_iArea == 5 || m_iArea == 6 || m_iArea == 7 )
                m_dyDrag = mousePoint.y - m_LastMouse.y;

            m_width += m_dxDrag;
            m_height += m_dyDrag;

            // 縮小界限(預設125x100)*********************************************
            // 有設定最小SIZE時會以設定的最小數值為準
            if( m_width <= m_iMinWidth && m_dxDrag < 0 )
            {
                m_width = m_iMinWidth;
                if( m_iArea == 1 || m_iArea == 7 || m_iArea == 8 )
                    m_x = m_x_r - m_width;
                m_dxDrag = 0;
            }
            if( m_height <= m_iMinHeight && m_dyDrag < 0 )
            {
                m_height = m_iMinHeight;
                if( m_iArea == 1 || m_iArea == 2 || m_iArea == 3 )
                    m_y = m_y_b - m_height;
                m_dyDrag = 0;
            }

            // 放大不可超過玩家畫面*********************************************
            RECT rcClient;
            GetClientRect( hWnd, &rcClient );
            // 左邊界
            if( m_x <= 0  && m_dxDrag > 0 && ( m_iArea == 1 || m_iArea == 7 || m_iArea == 8 ) )
            {
                m_x = 0;
                m_dxDrag = 0;
                m_width = m_x_r - m_x;
            }

            // 上邊界
            if( m_y <= 0  && m_dyDrag > 0 &&( m_iArea == 1 || m_iArea == 2 || m_iArea == 3 ) )
            {
                m_y = 0;
                m_dyDrag = 0;
                m_height = m_y_b - m_y;
            }

            // 寬度限制(不得大於視窗)
            if( m_width > rcClient.right )
                m_width = rcClient.right;

            // 高度限制(不得大於視窗)
            if( m_height > rcClient.bottom )
                m_height = rcClient.bottom;

            // 右邊界
            if( m_x + m_width >= rcClient.right && ( m_x == 0 || m_iArea == 3 || m_iArea == 4 || m_iArea == 5 ) )
                m_width = rcClient.right - m_x;

            // 下邊界
            if( m_y + m_height >= rcClient.bottom && ( m_y == 0 || m_iArea == 5 || m_iArea == 6 || m_iArea == 7 ) )
                m_height = rcClient.bottom - m_y;

            // 有設定最大SIZE的時候
            if( m_iMaxWidth > 0 && m_width >= m_iMaxWidth && m_dxDrag > 0 )
            {
                m_width = m_iMaxWidth;
                if( m_iArea == 1 || m_iArea == 7 || m_iArea == 8 )
                    m_x = m_x_r - m_width ;
                m_dxDrag= 0;
            }
            if( m_iMaxHeight > 0 && m_height >= m_iMaxHeight && m_dyDrag > 0 )
            {
                m_height = m_iMaxHeight;
                if( m_iArea == 1 || m_iArea == 2 || m_iArea == 3 )
                    m_y = m_y_b - m_height;
                m_dyDrag= 0;
            }
            SetLocation( m_x, m_y );
            SetSize( m_width , m_height );
            m_LastMouse = mousePoint; // 記錄最後點
            
}
//======================================================================================
// KeyboardProc
//======================================================================================
bool CDXUTDialog::KeyboardProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        // Keyboard messages
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            // If a control is in focus, it belongs to this dialog, and it's enabled, then give
            // it the first chance at handling the message.
            if( s_pControlFocus && s_pControlFocus->GetEnabled())
            {
				if(fnIsParent(s_pControlFocus->m_pDialog, this))
					return false;
				if(s_pControlFocus->m_pDialog == this )
					if( s_pControlFocus->HandleKeyboard( uMsg, wParam, lParam ) )
						return true;
            }

            // Not yet handled, see if this matches a control's hotkey
            // Activate the hotkey if the focus doesn't belong to an
            // edit box.
            if( uMsg == WM_KEYDOWN && ( !s_pControlFocus ||
                                      ( s_pControlFocus->GetType() != DXUT_CONTROL_EDITBOX
                                     && s_pControlFocus->GetType() != DXUT_CONTROL_IMEEDITBOX ) ) )
            {
                for( int i=0; i < m_Controls.GetSize(); i++ )
                {
                    CDXUTControl* pControl = m_Controls.GetAt( i );
                    if( pControl->GetHotkey() == wParam )
                    {
                        pControl->OnHotkey();
                        return true;
                    } // end if
                } // end for
            } // end if

            // Not yet handled, check for focus messages
            if( uMsg == WM_KEYDOWN )
            {
                // If keyboard input is not enabled, this message should be ignored
                if( !m_bKeyboardInput )
                    return false;

                switch( wParam )
                {
                    case VK_RIGHT:
                    case VK_DOWN:
                        if( s_pControlFocus != NULL )
                        {
                            return OnCycleFocus( true );
                        }
                        break;

                    case VK_LEFT:
                    case VK_UP:
                        if( s_pControlFocus != NULL )
                        {
                            return OnCycleFocus( false );
                        }
                        break;

                    case VK_TAB: 
                        if( s_pControlFocus == NULL )
                        {
                            FocusDefaultControl();
                        }
                        else
                        {
                            bool bShiftDown = ((GetAsyncKeyState( VK_SHIFT ) & 0x8000) != 0);
                            return OnCycleFocus( !bShiftDown );
                        }
                        return true; 
                } // end switch
            } // end if WM_KEYDOWN
            break;
        } // end case keyboard messages
    } // end switch (uMsg)

    return false;
}
//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetControlAtPoint( POINT pt )
{
    // Search through all child controls for the first one which
    // contains the mouse point
	if(m_bControlOrder == true)
	{		
		for( int i=m_Controls.GetSize()-1; i >= 0 ; --i )
		{
			CDXUTControl* pControl = m_Controls.GetAt(i);

			if( pControl == NULL )
			{
				continue;
			}

			// We only return the current control if it is visible
			// and enabled.  Because GetControlAtPoint() is used to do mouse
			// hittest, it makes sense to perform this filtering.
			if( pControl->ContainsPoint( pt ) )
				if( pControl->GetEnabled() )
					if( pControl->GetVisible() )
						return pControl;
		}
	}
	else
	{
		for( int i=0; i < m_Controls.GetSize(); i++ )
			//for( int i=m_Controls.GetSize()-1; i >= 0 ; --i )
		{
			CDXUTControl* pControl = m_Controls.GetAt(i);

			if( pControl == NULL )
			{
				continue;
			}

			// We only return the current control if it is visible
			// and enabled.  Because GetControlAtPoint() is used to do mouse
			// hittest, it makes sense to perform this filtering.
			if( pControl->ContainsPoint( pt ) )
				if( pControl->GetEnabled() )
					if( pControl->GetVisible() )
						return pControl;
		}
	}
    
    return NULL;
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::GetControlEnabled( int ID )
{
    CDXUTControl* pControl = GetControl( ID );
    if( pControl == NULL )
        return false;

    return pControl->GetEnabled();
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetControlEnabled( int ID, bool bEnabled )
{
    CDXUTControl* pControl = GetControl( ID );
    if( pControl == NULL )
        return;

    pControl->SetEnabled( bEnabled );
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::OnMouseUp( POINT pt )
{
    s_pControlPressed = NULL;
    m_pControlMouseOver = NULL;
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SwitchMouseOverControl( POINT pt )
{
    // Figure out which control the mouse is over now
    CDXUTControl* pControl = GetControlAtPoint( pt );

    // If the mouse is still over the same control, nothing needs to be done
    if( pControl == m_pControlMouseOver )
        return;

    // Handle mouse leaving the old control
    if( m_pControlMouseOver )
        m_pControlMouseOver->OnMouseLeave();

    // Handle mouse entering the new control
    m_pControlMouseOver = pControl;
    if( pControl != NULL )
        m_pControlMouseOver->OnMouseEnter();
}

void CDXUTDialog::OnMouseMove( POINT pt )
{
    SwitchMouseOverControl(pt);
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::SetDefaultElement( UINT nControlType, UINT iElement, CDXUTElement* pElement )
{
    // If this Element type already exist in the list, simply update the stored Element
    for( int i=0; i < m_DefaultElements.GetSize(); i++ )
    {
        DXUTElementHolder* pElementHolder = m_DefaultElements.GetAt( i );
        
        if( pElementHolder->nControlType == nControlType &&
            pElementHolder->iElement == iElement )
        {
            pElementHolder->Element = *pElement;
            return S_OK;
        }
    }

    // Otherwise, add a new entry
    DXUTElementHolder* pNewHolder;
    pNewHolder = new DXUTElementHolder;
    if( pNewHolder == NULL )
        return E_OUTOFMEMORY;

    pNewHolder->nControlType = nControlType;
    pNewHolder->iElement = iElement;
    pNewHolder->Element = *pElement;

    m_DefaultElements.Add( pNewHolder );
    return S_OK;
}   

//--------------------------------------------------------------------------------------
CDXUTElement* CDXUTDialog::GetDefaultElement( UINT nControlType, UINT iElement )
{
    for( int i=0; i < m_DefaultElements.GetSize(); i++ )
    {
        DXUTElementHolder* pElementHolder = m_DefaultElements.GetAt( i );
        
        if( pElementHolder->nControlType == nControlType &&
            pElementHolder->iElement == iElement )
        {
            return &pElementHolder->Element;
        }
    }
    
    return NULL;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddStatic( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault, CDXUTStatic** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTStatic* pStatic = new CDXUTStatic( this );

    if( ppCreated != NULL )
        *ppCreated = pStatic;

    if( pStatic == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pStatic,ID,true );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pStatic->SetID( ID ); 
    pStatic->SetText( strText );
    pStatic->SetLocation( x, y );
    pStatic->SetSize( width, height );
    pStatic->m_bIsDefault = bIsDefault;

    return S_OK;
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::SetButtonText( int ID, LPCWSTR strText )
{
    CDXUTButton* pButton = (CDXUTButton*)GetControl( ID, DXUT_CONTROL_BUTTON );

    if( pButton != NULL )
    {
        pButton->SetText( strText );
        return S_OK;
    }
    else
    {
        return E_OUTOFMEMORY;
    }
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddButton( int ID, string strText, int x, int y, int width,
      int height, UINT nHotkey, bool bIsDefault, CDXUTButton** ppCreated )
{
    return AddButton( ID, ANSItoUNICODE(strText), x, y, width, height, nHotkey,
                bIsDefault, ppCreated );
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddButton( int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault, CDXUTButton** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTButton* pButton = new CDXUTButton( this );

    if( ppCreated != NULL )
        *ppCreated = pButton;

    if( pButton == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pButton ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pButton->SetID( ID ); 
    pButton->SetText( strText );
    pButton->SetLocation( x, y );
    pButton->SetSize( width, height );
    pButton->SetHotkey( nHotkey );
    pButton->m_bIsDefault = bIsDefault;

    return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault, CDXUTCheckBox** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTCheckBox* pCheckBox = new CDXUTCheckBox( this );

    if( ppCreated != NULL )
        *ppCreated = pCheckBox;

    if( pCheckBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pCheckBox,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pCheckBox->SetID( ID ); 
    pCheckBox->SetText( strText );
    pCheckBox->SetLocation( x, y );
    pCheckBox->SetSize( width, height );
    pCheckBox->SetHotkey( nHotkey );
    pCheckBox->m_bIsDefault = bIsDefault;
    pCheckBox->SetChecked( bChecked );
    
    return S_OK;
}

//--------------------------------------------------------------------------------------
// ming add
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddRadioButton( int ID, UINT nButtonGroup, string strText,
    int x, int y, int width, int height, bool bChecked, UINT nHotkey,
    bool bIsDefault, CDXUTRadioButton** ppCreated )
{
    return AddRadioButton( ID, nButtonGroup, ANSItoUNICODE(strText), x, y, width, height, bChecked, nHotkey, bIsDefault, ppCreated );
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault, CDXUTRadioButton** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTRadioButton* pRadioButton = new CDXUTRadioButton( this );

    if( ppCreated != NULL )
        *ppCreated = pRadioButton;

    if( pRadioButton == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pRadioButton ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pRadioButton->SetID( ID );
    pRadioButton->SetText( strText );
    pRadioButton->SetButtonGroup( nButtonGroup );
    pRadioButton->SetLocation( x, y );
    pRadioButton->SetSize( (int)(width), (int)(height/1.5) );
    pRadioButton->SetHotkey( nHotkey );
    pRadioButton->SetChecked( bChecked , true);
    pRadioButton->m_bIsDefault = bIsDefault;
    pRadioButton->SetChecked( bChecked , true);

    return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddComboBox( int ID, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault, CDXUTComboBox** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTComboBox* pComboBox = new CDXUTComboBox( this );

    if( ppCreated != NULL )
        *ppCreated = pComboBox;

    if( pComboBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pComboBox, ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pComboBox->SetID( ID ); 
    pComboBox->SetLocation( x, y );
    pComboBox->SetSize( width, height );
    pComboBox->SetHotkey( nHotkey );
    pComboBox->m_bIsDefault = bIsDefault;

    return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddSlider( int ID, int x, int y, int width, int height, int min, int max, int value, bool bIsDefault, CDXUTSlider** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTSlider* pSlider = new CDXUTSlider( this );

    if( ppCreated != NULL )
        *ppCreated = pSlider;

    if( pSlider == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pSlider ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pSlider->SetID( ID ); 
    pSlider->SetLocation( x, y );
    pSlider->SetSize( width, height );
    pSlider->m_bIsDefault = bIsDefault;
    pSlider->SetRange( min, max );
    pSlider->SetValue( value );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddEditBox( int ID, string strText, int x, int y,
      int width, int height, bool bIsDefault, CDXUTEditBox** ppCreated )
{
    return AddEditBox( ID, ANSItoUNICODE(strText), x, y, width, height, bIsDefault, ppCreated );
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault, CDXUTEditBox** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTEditBox *pEditBox = new CDXUTEditBox( this );

    if( ppCreated != NULL )
        *ppCreated = pEditBox;

    if( pEditBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pEditBox, ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pEditBox->SetID( ID ); 
    pEditBox->SetLocation( x, y );
    pEditBox->SetSize( width, height );
    pEditBox->m_bIsDefault = bIsDefault;

    if( strText )
        pEditBox->SetText( strText );

    return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddIMEEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault, CDXUTIMEEditBox** ppCreated )
{
    HRESULT hr = S_OK;
    CDXUTIMEEditBox *pEditBox = new CDXUTIMEEditBox( this );

    if( ppCreated != NULL )
        *ppCreated = pEditBox;

    if( pEditBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pEditBox ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pEditBox->SetID( ID ); 
    pEditBox->SetLocation( x, y );
    pEditBox->SetSize( width, height );
    pEditBox->m_bIsDefault = bIsDefault;

    if( strText )
        pEditBox->SetText( strText );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddListBox( int ID, int x, int y, int width, int height, DWORD dwStyle, CDXUTListBox** ppCreated )
{
    HRESULT hr = S_OK;
    CDXUTListBox *pListBox = new CDXUTListBox( this );

    if( ppCreated != NULL )
        *ppCreated = pListBox;

    if( pListBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pListBox,ID,true );
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pListBox->SetID( ID );
    pListBox->SetLocation( x, y );
    pListBox->SetSize( width, height );
    pListBox->SetStyle( dwStyle );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddVariableHeightListBox( int ID, int x, int y, int width, int height, DWORD dwStyle, CVariableHeightListBox** ppCreated )
{
    HRESULT hr = S_OK;
    CVariableHeightListBox *pListBox = new CVariableHeightListBox( this );

    if( ppCreated != NULL )
        *ppCreated = pListBox;

    if( pListBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pListBox ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pListBox->SetID( ID );
    pListBox->SetLocation( x, y );
    pListBox->SetSize( width, height );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddSmoothScrollingListBox( int ID, int x, int y, int width, int height, DWORD dwStyle, CSmoothScrollingListBox** ppCreated )
{
    HRESULT hr = S_OK;
    CSmoothScrollingListBox *pListBox = new CSmoothScrollingListBox( this );

    if( ppCreated != NULL )
        *ppCreated = pListBox;

    if( pListBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pListBox ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pListBox->SetID( ID );
    pListBox->SetLocation( x, y );
    pListBox->SetSize( width, height );
    pListBox->SetStyle( dwStyle );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddBtnListBox( int ID, int x, int y, int width, int height, DWORD dwStyle, moCDXUTBtnListBox** ppCreated)
{
    HRESULT hr = S_OK;
    moCDXUTBtnListBox *pListBox = new moCDXUTBtnListBox( this );

    if( ppCreated != NULL )
        *ppCreated = pListBox;

    if( pListBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pListBox ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pListBox->SetID( ID );
    pListBox->SetLocation( x, y );
    pListBox->SetSize( width, height );
    pListBox->SetStyle( dwStyle );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// 增加ImageListBox ming add
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddImageListBox( int ID, int x, int y, int width, int height,
      DWORD dwStyle, CDXUTImageListBox** ppCreated )
{
    HRESULT hr = S_OK;
    CDXUTImageListBox *pImageListBox = new CDXUTImageListBox( this );

    if( ppCreated != NULL )
        *ppCreated = pImageListBox;

    if( pImageListBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pImageListBox ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pImageListBox->SetID( ID );
    pImageListBox->SetLocation( x, y );
    pImageListBox->SetSize( width, height );
    pImageListBox->SetStyle( dwStyle );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// 增加BtnImageListBox   JL add
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddBtnImageListBox( int ID, int x, int y, int width, int height,
      DWORD dwStyle, CDXUTBtnImageListBox** ppCreated )
{
    HRESULT hr = S_OK;
    CDXUTBtnImageListBox *pBtnImageListBox = new CDXUTBtnImageListBox( this );

    if( ppCreated != NULL )
        *ppCreated = pBtnImageListBox;

    if( pBtnImageListBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pBtnImageListBox,ID,true );
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pBtnImageListBox->SetID( ID );
    pBtnImageListBox->SetLocation( x, y );
    pBtnImageListBox->SetSize( width, height );
    pBtnImageListBox->SetStyle( dwStyle );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// 增加 Image ming add
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddImage( int ID, int x, int y, int width, int height,
      bool bIsDefault, CDXUTImage** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTImage* pImage = new CDXUTImage(this);

    if( ppCreated != NULL )
        *ppCreated = pImage;

    if( pImage == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pImage ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pImage->SetID( ID );
    pImage->SetLocation( x, y );
    pImage->SetSize( width, height );
    pImage->m_bIsDefault = bIsDefault;

    return S_OK;
}
//--------------------------------------------------------------------------------------
// 增加 Icon Image
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddIconImage( int ID, int x, int y, int width, int height,
      bool bIsDefault, CDXUTIconImage** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTIconImage* pImage = new CDXUTIconImage(this);

    if( ppCreated != NULL )
        *ppCreated = pImage;

    if( pImage == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pImage,ID,true );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pImage->SetID( ID );
    pImage->SetLocation( x, y );
    pImage->SetSize( width, height );
    pImage->m_bIsDefault = bIsDefault;

    return S_OK;
}
//--------------------------------------------------------------------------------------
// 增加 TalkBarImageControl
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddTalkBarImageControl( int ID, int x, int y, int width, int height, 
    bool bIsDefault, CTalkBarImageControl** ppCreated )
{
    HRESULT hr = S_OK;

    CTalkBarImageControl* pImage = new CTalkBarImageControl(this);

    if( ppCreated != NULL )
        *ppCreated = pImage;

    if( pImage == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pImage,ID,true );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pImage->SetID( ID );
    pImage->SetLocation( x, y );
    pImage->SetSize( width, height );
    pImage->m_bIsDefault = bIsDefault;

    return S_OK;
}
//--------------------------------------------------------------------------------------
// 增加 -- 裝Image的Box
// 用於快捷列等...
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddImageBox( int ID, int x, int y, int width, int height, 
      bool bIsDefault, CDXUTImageBox** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTImageBox* pImageBox = new CDXUTImageBox(this);

    if( ppCreated != NULL )
        *ppCreated = pImageBox;

    if( pImageBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pImageBox ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pImageBox->SetID( ID );
    pImageBox->SetLocation( x, y );
    pImageBox->SetSize( width, height );
    pImageBox->m_bIsDefault = bIsDefault;

    return S_OK;
}
//--------------------------------------------------------------------------------------
// 增加 -- 裝IconImage的Box
// 用於快捷列等...
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddIconImageBox( int ID, int x, int y, int width, int height,
      bool bIsDefault, CDXUTIconImageBox** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTIconImageBox* pIconImageBox = new CDXUTIconImageBox(this);

    if( ppCreated != NULL )
        *ppCreated = pIconImageBox;

    if( pIconImageBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pIconImageBox,ID,true );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pIconImageBox->SetID( ID );
    pIconImageBox->SetLocation( x, y );
    pIconImageBox->SetSize( width, height );
    pIconImageBox->m_bIsDefault = bIsDefault;

    return S_OK;
}

//--------------------------------------------------------------------------------------
// 增加自訂元件 ImageBoxList - List中只有ImageBox
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddImageBoxList( int ID, int x, int y,
      int numCellsPerRow, int numCellsPerCol, CDXUTImageBoxList** ppCreated )
{
    HRESULT hr = S_OK;
    CDXUTImageBoxList *pImageBoxList = new CDXUTImageBoxList( this );

    if( ppCreated != NULL )
        *ppCreated = pImageBoxList;

    if( pImageBoxList == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pImageBoxList,ID,true );
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pImageBoxList->SetID( ID );
    pImageBoxList->SetLocation( x, y );
    //pImageBoxList->SetSize( width, height );
    pImageBoxList->SetSize( numCellsPerRow*32, numCellsPerCol*32 );
    pImageBoxList->SetStyle( 0 ); // 不可多選

    return S_OK;
}
//--------------------------------------------------------------------------------------
// 增加自訂元件 IconListBox - List中只有 ImageIcon
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddIconListBox( int ID, int x, int y,
      int Row, int Col, CDXUTIconListBox** ppCreated )
{
    HRESULT hr = S_OK;
    CDXUTIconListBox *pIconListBox = new CDXUTIconListBox( this );

    if( ppCreated != NULL )
        *ppCreated = pIconListBox;

    if( pIconListBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pIconListBox ,ID,true);
    if( FAILED(hr) )
        return hr;

	pIconListBox->SetRow( Row ) ;
    pIconListBox->SetCol( Col ) ;

    // Set the ID and position
    pIconListBox->SetID( ID );
    pIconListBox->SetLocation( x, y ) ;
    pIconListBox->SetStyle( 0 ); // 不可多選

    return S_OK;
}
//--------------------------------------------------------------------------------------
// 增加自訂元件 IconListBox - List中只有 ImageIcon  這個是越南用的 避免跟路板有衝突
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddIconListBoxVN( int ID, int x, int y,
      int Row, int Col, CDXUTIconListBox** ppCreated )
{
    HRESULT hr = S_OK;
    CDXUTIconListBox *pIconListBox = new CDXUTIconListBox( this );

    if( ppCreated != NULL )
        *ppCreated = pIconListBox;

    if( pIconListBox == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pIconListBox ,ID,true);
    if( FAILED(hr) )
        return hr;

	pIconListBox->SetRow( Row ) ;
	pIconListBox->SetCol( Col ) ;

    // Set the ID and position
    pIconListBox->SetID( ID );
    pIconListBox->SetLocation( x, y );
    pIconListBox->SetStyle( 0 ); // 不可多選

    return S_OK;
}
//--------------------------------------------------------------------------------------
// 增加自訂元件 ProgressBar
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddProgressBar( int ID, int x, int y, int width, int height,
      bool bIsDefault, CDXUTProgressBar** ppCreated )
{
    HRESULT hr = S_OK;
    CDXUTProgressBar *pProgressBar = new CDXUTProgressBar( this );

    if( ppCreated != NULL )
        *ppCreated = pProgressBar;

    if( pProgressBar == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pProgressBar ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pProgressBar->SetID( ID );
    pProgressBar->SetLocation( x, y );
    pProgressBar->SetSize( width, height );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddSpecialButton( int ID, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault, CDXUTSpecialButton** ppCreated )
{
    HRESULT hr = S_OK;

    CDXUTSpecialButton* pButton = new CDXUTSpecialButton( this );

    if( ppCreated != NULL )
        *ppCreated = pButton;

    if( pButton == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pButton,ID,true );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pButton->SetID( ID );
    pButton->SetLocation( x, y );
    pButton->SetSize( width, height );
    pButton->SetHotkey( nHotkey );
    pButton->m_bIsDefault = bIsDefault;

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddMapControl( int ID, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault, CMapControl** ppCreated )
{

	HRESULT hr = S_OK;

    CMapControl* pmap = new CMapControl( this );

    if( ppCreated != NULL )
        *ppCreated = pmap;

    if( pmap == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( pmap ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pmap->SetID( ID );
	pmap->SetWindowSize( width, height ); //必須先設定Size，在設定頂點位置
	pmap->SetWindowPos( x, y );
    pmap->SetHotkey( nHotkey );
    pmap->m_bIsDefault = bIsDefault;


	return S_OK;
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddPuzzle( int ID, int x, int y, int cellwidth, int cellheight, UINT nHotke, bool bIsDefault , CDXUTPuzzle** ppCreated )
{
	HRESULT hr = S_OK;

    CDXUTPuzzle* puzzle = new CDXUTPuzzle( this );

    if( ppCreated != NULL )
        *ppCreated = puzzle ;

    if( puzzle == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( puzzle ,ID,true);
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    puzzle->SetID( ID );
	puzzle->SetLocation( x , y ) ;
	puzzle->SetCellSize( cellwidth , cellheight ) ;
    puzzle->m_bIsDefault = bIsDefault;


	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddTabPage( int ID, int x, int y, int tabwidth, int tabheight, UINT nHotkey , bool bIsDefault , CDXUTTabPage** ppCreated )
{
	HRESULT hr = S_OK;

    CDXUTTabPage* TabPage = new CDXUTTabPage( this );

    if( ppCreated != NULL )
        *ppCreated = TabPage ;

    if( TabPage == NULL )
        return E_OUTOFMEMORY;

    hr = AddControl( TabPage,ID,true );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    TabPage->SetID( ID );
	TabPage->SetLocation( x , y ) ;
	TabPage->SetTabSize( tabwidth , tabheight ) ;
    TabPage->m_bIsDefault = bIsDefault;


	return S_OK;

}
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddControl(CDXUTControl* pControl, int ID, int x, int y, int width, int height)
{
    assert(pControl);

    HRESULT hr = S_OK;

    hr = AddControl( pControl ,ID,true);
    if (FAILED(hr))
        return hr;

    // Set the ID and list index
    pControl->SetID( ID );
    pControl->SetLocation( x, y );
    pControl->SetSize( width, height );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddControl(CDXUTControl* pControl, int ID, const RECT& rcControl)
{
    assert(pControl);

    HRESULT hr = S_OK;


	// Set the ID and list index
	pControl->SetID( ID );
	pControl->SetLocation( rcControl.left, rcControl.top );
	pControl->SetSize( RectWidth(rcControl), RectHeight(rcControl) );


    hr = AddControl( pControl ,ID,true);
    if (FAILED(hr))
        return hr;

    

	
    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::AddControl( CDXUTControl* pControl , int ID , bool bInitControl /*= true*/)
{
    HRESULT hr = S_OK;

	pControl->m_Index = m_Controls.GetSize();
	if (bInitControl)
	{
		hr = InitControl( pControl );
	}    
    if( FAILED(hr) )
        return DXTRACE_ERR( L"CDXUTDialog::InitControl", hr );

    // Add to the list
    hr = m_Controls.Add( pControl );
	
    if( FAILED(hr) )
    {
        return DXTRACE_ERR( L"CGrowableArray::Add", hr );
    }

	if(ID != 0 && ID != -1)
	{
		pControl->SetID( ID ) ;
		ControlCache::iterator it = mControlCache.find(ID);
		if(it != mControlCache.end())
		{
#ifdef _DEBUG
			assert(0 && "重複的Control ID");

#endif
			it->second = pControl;
		}
		else
			mControlCache.insert( make_pair( ID , pControl) );

	}
	
    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::InitControl( CDXUTControl* pControl )
{
    HRESULT hr;

    if( pControl == NULL )
        return E_INVALIDARG;

    // Look for a default Element entries
    for( int i=0; i < m_DefaultElements.GetSize(); i++ )
    {
        DXUTElementHolder* pElementHolder = m_DefaultElements.GetAt( i );
        if( pElementHolder->nControlType == pControl->GetType() )
            pControl->SetElement( pElementHolder->iElement, &pElementHolder->Element );
    }

    V_RETURN( pControl->OnInit() );

    return S_OK;
}

//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetControl( int ID )
{
	ControlCache::iterator it = mControlCache.find(ID);
	if(it != mControlCache.end())
	{
		return it->second;
	}
    // Not found
// 	wstring msg ;
// 	StringFormat16( msg , L"Error Path : %s , ID : %d" , m_DebugLayoutPath.c_str() , ID ) ;
// 	_LOG( 0 , msg.c_str() ) ;
	//Assert( 0 , msg.c_str() ) ;
    return NULL;
}

CDXUTControl* CDXUTDialog::GetSubDialogControl( int iSubDlgID , int iCtrlID , CDXUTDialog* pDialog )
{
	CDXUTControl* pControl = pDialog->GetControl( iSubDlgID ) ;
	
	if( pControl && fnIsSubDialogType(pControl->GetType()) )
	{
		CDXUTDialog* pDialog = ( ( CDXUTSubDialog* )pControl )->GetDialog() ;
		if( pDialog )
		{
			return pDialog->GetControl( iCtrlID ) ;
		}
	}
	return 0 ;
}

CDXUTControl* CDXUTDialog::GetSubDialogControl( int iSubDlgID1 , int iSubDlgID2 , ... )
{
	va_list args ;
	va_start( args , iSubDlgID2 ) ;
	int iNowSubDlgID = iSubDlgID1 ;
	int iNextSubDlgID = iSubDlgID2 ;
	CDXUTControl* pControl = GetSubDialogControl( iNowSubDlgID , iNextSubDlgID , this ) ;
	if( pControl )
	{
		while( iNextSubDlgID != -1 && fnIsSubDialogType(pControl->GetType()) )
		{
			iNowSubDlgID = iNextSubDlgID ;
			iNextSubDlgID = va_arg( args , int ) ;
			pControl = GetSubDialogControl( iNowSubDlgID , iNextSubDlgID ,  ( ( CDXUTSubDialog* )pControl )->GetDialog() ) ;
		}
	}
	va_end(args);
	return pControl ;
}


//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetControl( int ID, UINT nControlType )
{
	CDXUTControl* pCtrl = GetControl(ID);
	if(pCtrl)
	{
		if(pCtrl->GetType() ==nControlType)
			return pCtrl;
	}
	return 0;
    // Try to find the control with the given ID
    for( int i=0; i < m_Controls.GetSize(); i++ )
    {
        CDXUTControl* pControl = m_Controls.GetAt( i );

        if( pControl->GetID() == ID && pControl->GetType() == nControlType )
        {
            return pControl;
        }
    }

    // Not found
    return NULL;
}



//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetNextControl( CDXUTControl* pControl )
{
    int index = pControl->m_Index + 1;

    CDXUTDialog* pDialog = pControl->m_pDialog;
    
    // Cycle through dialogs in the loop to find the next control. Note
    // that if only one control exists in all looped dialogs it will
    // be the returned 'next' control.
    while( index >= (int) pDialog->m_Controls.GetSize() )
    {
        pDialog = pDialog->m_pNextDialog;
        index = 0;
    }
    
    return pDialog->m_Controls.GetAt( index );    
}

//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTDialog::GetPrevControl( CDXUTControl* pControl )
{
    int index = pControl->m_Index - 1;

    CDXUTDialog* pDialog = pControl->m_pDialog;
    
    // Cycle through dialogs in the loop to find the next control. Note
    // that if only one control exists in all looped dialogs it will
    // be the returned 'previous' control.
    while( index < 0 )
    {
        pDialog = pDialog->m_pPrevDialog;
        if( pDialog == NULL )
            pDialog = pControl->m_pDialog;

        index = pDialog->m_Controls.GetSize() - 1;
    }
    
    return pDialog->m_Controls.GetAt( index );    
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::ClearRadioButtonGroup( UINT nButtonGroup )
{
    // Find all radio buttons with the given group number
    for( int i=0; i < m_Controls.GetSize(); i++ )
    {
        CDXUTControl* pControl = m_Controls.GetAt( i );

        if( pControl->GetType() == DXUT_CONTROL_RADIOBUTTON )
        {
            CDXUTRadioButton* pRadioButton = (CDXUTRadioButton*) pControl;

            if( pRadioButton->GetButtonGroup() == nButtonGroup )
                pRadioButton->SetChecked( false, false );
        }
    }
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::DeleteRadioButtonGroup( UINT nButtonGroup )
{
    // Find all radio buttons with the given group number
    for( int i=0; i < m_Controls.GetSize(); i++ )
    {
        CDXUTControl* pControl = m_Controls.GetAt( i );

        if( pControl->GetType() == DXUT_CONTROL_RADIOBUTTON )
        {
            CDXUTRadioButton* pRadioButton = (CDXUTRadioButton*) pControl;

            if( pRadioButton->GetButtonGroup() == nButtonGroup )
            {
                pRadioButton->SetChecked( false, false );
                RemoveControl( pRadioButton );
                i = -1;
            }
        }
    }
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::ClearComboBox( int ID )
{
    CDXUTComboBox* pComboBox = GetComboBox( ID );
    if( pComboBox == NULL )
        return;

    pComboBox->RemoveAllItems();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTDialog::RequestDefaultFocus()
{
	m_CloseBtn->OnFocusOut();
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::EnableDebugInfo(bool bEnable)
{
	s_bEnableDebugInfo = bEnable;
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::GetEnableDebugInfo()
{
	return s_bEnableDebugInfo;
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::RequestFocus( CDXUTControl* pControl )
{
    if( s_pControlFocus == pControl )
        return;

    if( pControl && !pControl->CanHaveFocus() )
        return;

    if( s_pControlFocus )
        s_pControlFocus->OnFocusOut();

	if(pControl)
		pControl->OnFocusIn();

    s_pControlFocus = pControl;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::DrawRect(const RECT* pRect, D3DCOLOR color )
{
    RECT rcScreen = *pRect;
    OffsetRect( &rcScreen, m_x, m_y );

    // If caption is enabled, offset the Y position by its height.
    if( m_bCaption )
        OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    DXUT_SCREEN_VERTEX vertices[4] =
    {
        (float) rcScreen.left -0.5f,  (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 0, 0,
        (float) rcScreen.right -0.5f, (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 0, 0, 
        (float) rcScreen.right -0.5f, (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0, 
        (float) rcScreen.left -0.5f,  (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0,
    };

    IDirect3DDevice9* pd3dDevice = s_pManager->GetD3DDevice();

    // Since we're doing our own drawing here we need to flush the sprites
    s_pManager->m_pSprite->Flush();
	//if(m_pRender != NULL)
	//{
	//	m_pRender->FlushUI() ; 
	//}

    IDirect3DVertexDeclaration9 *pDecl = NULL;
    pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
    pd3dDevice->SetFVF( DXUT_SCREEN_VERTEX::FVF );

    // Preserve previous texture stage states.
    DWORD dwOldColorOp, dwOldAlphaOp;
    pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &dwOldColorOp );
    pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP, &dwOldAlphaOp );

    // Set new TSSs.
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

    pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(DXUT_SCREEN_VERTEX) );

    // Restore previous TSSs.
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, dwOldColorOp );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, dwOldAlphaOp );

    // Restore the vertex decl
    pd3dDevice->SetVertexDeclaration( pDecl );
    pDecl->Release();

    return S_OK;
}

HRESULT CDXUTDialog::DrawFrameRect(const RECT* pRect, D3DCOLOR color )
{
    RECT rcScreen = *pRect;
    OffsetRect( &rcScreen, m_x, m_y );

    // If caption is enabled, offset the Y position by its height.
    if( m_bCaption )
        OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    DXUT_SCREEN_VERTEX vertices[8] =
    {
        (float) rcScreen.left,  (float) rcScreen.top,    0.0f, 1.0f, color, 0.0f, 0.0f,
        (float) rcScreen.right, (float) rcScreen.top,    0.0f, 1.0f, color, 0.0f, 0.0f,
        
        (float) rcScreen.right, (float) rcScreen.top,    0.0f, 1.0f, color, 0.0f, 0.0f,
        (float) rcScreen.right, (float) rcScreen.bottom, 0.0f, 1.0f, color, 0.0f, 0.0f,

        (float) rcScreen.right, (float) rcScreen.bottom, 0.0f, 1.0f, color, 0.0f, 0.0f,
        (float) rcScreen.left,  (float) rcScreen.bottom, 0.0f, 1.0f, color, 0.0f, 0.0f,

        (float) rcScreen.left,  (float) rcScreen.bottom, 0.0f, 1.0f, color, 0.0f, 0.0f,
        (float) rcScreen.left,  (float) rcScreen.top,    0.0f, 1.0f, color, 0.0f, 0.0f,
    };

    IDirect3DDevice9* pd3dDevice = s_pManager->GetD3DDevice();

    // Since we're doing our own drawing here we need to flush the sprites
    s_pManager->m_pSprite->Flush();
    IDirect3DVertexDeclaration9 *pDecl = NULL;
    pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
	DWORD oldFVF;
    if(pd3dDevice->GetFVF( &oldFVF )!=S_OK)
		oldFVF = 0;
    pd3dDevice->SetFVF( DXUT_SCREEN_VERTEX::FVF );

    // Preserve previous texture stage states.
    DWORD dwOldColorOp, dwOldAlphaOp;
    pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &dwOldColorOp );
    pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP, &dwOldAlphaOp );

    // Set new TSSs.
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

    pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 8, vertices, sizeof(DXUT_SCREEN_VERTEX) );

    // Restore previous TSSs.
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, dwOldColorOp );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, dwOldAlphaOp );

    // Restore the vertex decl
    pd3dDevice->SetVertexDeclaration( pDecl );
	if(oldFVF!=0)
		pd3dDevice->SetFVF(oldFVF);
    pDecl->Release();

    return S_OK;
}

HRESULT CDXUTDialog::DrawFineRect(const RECT* pRect, D3DCOLOR color )
{
    RECT rcScreen = *pRect;
    OffsetRect( &rcScreen, m_x, m_y );

    // If caption is enabled, offset the Y position by its height.
    if( m_bCaption )
        OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    DXUT_SCREEN_VERTEX vertices[4] =
    {
        (float)rcScreen.left,  (float)rcScreen.bottom, 0.0f, 1.0f, WHITE, 0.0f, 1.0f,
        (float)rcScreen.left,  (float)rcScreen.top,    0.0f, 1.0f, color, 0.0f, 0.0f,
        (float)rcScreen.right, (float)rcScreen.bottom, 0.0f, 1.0f, color, 1.0f, 1.0f,
        (float)rcScreen.right, (float)rcScreen.top,    0.0f, 1.0f, WHITE, 1.0f, 0.0f,
    };

    IDirect3DDevice9* pd3dDevice = s_pManager->GetD3DDevice();

    // Since we're doing our own drawing here we need to flush the sprites
    s_pManager->m_pSprite->Flush();
    IDirect3DVertexDeclaration9 *pDecl = NULL;
    pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
    pd3dDevice->SetFVF( DXUT_SCREEN_VERTEX::FVF );

    // Preserve previous texture stage states.
    DWORD dwOldColorOp, dwOldAlphaOp;
    pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &dwOldColorOp );
    pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP, &dwOldAlphaOp );

    // Set new TSSs.
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

    pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(DXUT_SCREEN_VERTEX) );

    // Restore previous TSSs.
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, dwOldColorOp );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, dwOldAlphaOp );

    // Restore the vertex decl
    pd3dDevice->SetVertexDeclaration( pDecl );
    pDecl->Release();

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::DrawPolyLine( POINT* apPoints, UINT nNumPoints, D3DCOLOR color )
{
    DXUT_SCREEN_VERTEX* vertices = new DXUT_SCREEN_VERTEX[ nNumPoints ];
    if( vertices == NULL )
        return E_OUTOFMEMORY;

    DXUT_SCREEN_VERTEX* pVertex = vertices;
    POINT* pt = apPoints;
    for( UINT i=0; i < nNumPoints; i++ )
    {
        pVertex->x = m_x + (float) pt->x;
        pVertex->y = m_y + (float) pt->y;
        pVertex->z = 0.5f;
        pVertex->h = 1.0f;
        pVertex->color = color;
        pVertex->tu = 0.0f;
        pVertex->tv = 0.0f;

        pVertex++;
        pt++;
    }

    IDirect3DDevice9* pd3dDevice = s_pManager->GetD3DDevice();

    // Since we're doing our own drawing here we need to flush the sprites
    s_pManager->m_pSprite->Flush();
    IDirect3DVertexDeclaration9 *pDecl = NULL;
    pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
    pd3dDevice->SetFVF( DXUT_SCREEN_VERTEX::FVF );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

    pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, nNumPoints - 1, vertices, sizeof(DXUT_SCREEN_VERTEX) );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

    // Restore the vertex decl
    pd3dDevice->SetVertexDeclaration( pDecl );
    pDecl->Release();

    SAFE_DELETE_ARRAY( vertices );
    return S_OK;
}

HRESULT CDXUTDialog::DrawSpriteScale(CDXUTElement* pElement , const RECT& rcDest)
{
	// No need to draw fully transparent layers
	if( pElement->TextureColor.Current.a == 0 )
		return S_OK;

	//pElement->rcTexture ; // 繪製區域
	RECT rcScreen = rcDest ;
	
	OffsetRect( &rcScreen, m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	if( m_bCaption )
		OffsetRect( &rcScreen, 0, m_nCaptionHeight );

	DXUTTextureNode* pTextureNode = GetTexture( pElement->iTexture );
	if( pTextureNode == NULL )
		return E_FAIL;

	RECT rcTxr;
	if(RectWidth(pElement->rcTexture) && RectHeight(pElement->rcTexture))
	{
		rcTxr = pElement->rcTexture;
	}
	else
	{
		
		SetRect(&rcTxr,0,0,pTextureNode->dwWidth,pTextureNode->dwHeight);
	}

	//rcScreen.right = rcScreen.left + (rcTxr.right - rcTxr.left) ; 
	//rcScreen.bottom = rcScreen.top + (rcTxr.bottom - rcTxr.top) ; 

		
	D3DXVECTOR3 vPos( (float)rcScreen.left, (float)rcScreen.top, 1.0f );
	

	float fScaleX = (float)RectWidth(rcScreen) / RectWidth(rcTxr) ;
	float fScaleY = (float)RectHeight(rcScreen) / RectHeight(rcTxr);

	D3DXMATRIXA16 matTransform;
	D3DXMatrixIdentity(&matTransform) ; 
	D3DXMatrixScaling( &matTransform, fScaleX , fScaleY , 1.0f );

	matTransform._41 = vPos.x ; 
	matTransform._42 = vPos.y ; 
	
	//if(m_pRender != NULL)
	//{
	//	m_pRender->CreateUiOp(pTextureNode->pTexture , rcScreen , &rcTxr , pTextureNode->dwWidth , 
	//		pTextureNode->dwHeight , pElement->TextureColor.Current) ; 
	//}
	s_pManager->m_pSprite->SetTransform( &matTransform );		
	s_pManager->m_pSprite->Draw( pTextureNode->pTexture, &rcTxr, NULL, 0, pElement->TextureColor.Current );
		
	return S_OK;
		
}
//--------------------------------------------------------------------------------------
// 畫材質
// 區分為
// 1. 使用元件預設的材質
// 2. 使用自訂的材質
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::DrawSprite( CDXUTElement* pElement, RECT* prcDest,
      LPDIRECT3DTEXTURE9 pTexture )
{
	 
    // No need to draw fully transparent layers
    if( pElement->TextureColor.Current.a == 0 )
        return S_OK;
    RECT rcTexture = pElement->rcTexture;



	//D3DXCOLOR CurrentColor = pElement->TextureColor.Current ;
	//float DialogAlpha = m_SidElement[ 0 ].TextureColor.Current.a ;
	//float InputAlpha = pElement->TextureColor.Current.a ;
	//float diffAlpha = fabs( DialogAlpha - InputAlpha ) ;
	//if( diffAlpha < 0.01 || diffAlpha > 0 )
	//{
	//	if( InputAlpha > DialogAlpha )
	//	{
	//		CurrentColor.a = DialogAlpha ;
	//	}
	//}

    
    RECT rcScreen = *prcDest;
    OffsetRect( &rcScreen, m_x, m_y );

    // If caption is enabled, offset the Y position by its height.
    if( m_bCaption )
        OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    DXUTTextureNode* pTextureNode = GetTexture( pElement->iTexture );
    if( pTextureNode == NULL )
        return E_FAIL;

    // ming edit >> RECT為0時，不做縮放
    float fScaleX = 1.0f;
    int RWidth = RectWidth( rcTexture );
    if( RWidth != 0 )
        fScaleX = (float) RectWidth( rcScreen ) / RWidth;

    float fScaleY = 1.0f;
    int RHeight = RectHeight( rcTexture );
    if( RHeight != 0 )
        fScaleY = (float) RectHeight( rcScreen ) / RHeight;
    // ming edit <<

    D3DXMATRIX matTransform;
	D3DXMatrixIdentity(&matTransform) ; 
    D3DXMatrixScaling( &matTransform, fScaleX, fScaleY, 1.0f );

    s_pManager->m_pSprite->SetTransform( &matTransform );

    D3DXVECTOR3 vPos( (float)rcScreen.left, (float)rcScreen.top, 0.0f );

    if( fScaleX*fScaleY != 0.0f )
    {
        vPos.x /= fScaleX;
        vPos.y /= fScaleY;
    }

 //	Draw2DTextureInfo inf;
 //	inf.pDevice = s_pManager->GetD3DDevice();	
 //	 
 //	inf.bDiffColor = true;
 //	inf.colors[0] = (DWORD)pElement->TextureColor.Current;
 //	inf.colors[1] = (DWORD)pElement->TextureColor.Current;
 //	inf.colors[2] = (DWORD)pElement->TextureColor.Current;
 //	inf.colors[3] = (DWORD)pElement->TextureColor.Current;
 //	inf.nHeight = pTextureNode->dwHeight;
 //	inf.nWidth = pTextureNode->dwWidth;
 //	inf.prcSrc = &rcTexture;	
 //	inf.pTxr = pTextureNode->pTexture;	
	//inf.rcScreen = rcScreen ; 
 	//SetRect(&inf.rcScreen,vPos.x,vPos.y,vPos.x + RectWidth(rcTexture),vPos.y + RectHeight(rcTexture));

	//if(m_pRender != NULL)
	//{
	//	RECT newRcScreen ; 

	//	//吃單一張圖的，如：listimage元件用的
	//	if( rcTexture.left == rcTexture.right || rcTexture.top == rcTexture.bottom )
	//	{
	//		SetRect(&rcTexture , 0 , 0 , pTextureNode->dwWidth , pTextureNode->dwHeight) ; 
	//		SetRect(&newRcScreen , rcScreen.left , rcScreen.top ,  rcScreen.left + (rcTexture.right - rcTexture.left) , 
	//			rcScreen.top + ( rcTexture.bottom - rcTexture.top)) ; 


	//		m_pRender->CreateUiOp(pTextureNode->pTexture , newRcScreen , &rcTexture , pTextureNode->dwWidth , 
	//				pTextureNode->dwHeight , (DWORD)pElement->TextureColor.Current) ; 
	//	}
	//	else
	//	{
	//		m_pRender->CreateUiOp(pTextureNode->pTexture , rcScreen , &rcTexture , pTextureNode->dwWidth , 
	//				pTextureNode->dwHeight , (DWORD)pElement->TextureColor.Current) ; 
	//	}

	//}


 	
    if( pTexture )
    {
        return s_pManager->m_pSprite->Draw( pTexture, NULL, NULL, &vPos, pElement->TextureColor.Current );
		//return s_pManager->m_pSprite->Draw( pTexture, NULL, NULL, &vPos, CurrentColor );
	}
    else
    {

        // ming add >> 當RECT為0時，RECT參數設為NULL
		if( rcTexture.left == rcTexture.right || rcTexture.top == rcTexture.bottom )
			return s_pManager->m_pSprite->Draw( pTextureNode->pTexture, NULL, NULL, &vPos, pElement->TextureColor.Current );
			//return s_pManager->m_pSprite->Draw( pTextureNode->pTexture, NULL, NULL, &vPos, CurrentColor );
            
        // ming add <<
		//s_pManager->m_pSprite->Begin(0);
		s_pManager->m_pSprite->Draw( pTextureNode->pTexture, &rcTexture, NULL, &vPos, pElement->TextureColor.Current );
		//s_pManager->m_pSprite->Draw( pTextureNode->pTexture, &rcTexture, NULL, &vPos, CurrentColor );
		//s_pManager->m_pSprite->End();
        return S_OK;
    }
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialog::CalcTextRect( LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, int nCount )
{
    HRESULT hr = S_OK;

    DXUTFontNode* pFontNode = GetFont( pElement->iFont );
    if( pFontNode == NULL )
        return E_FAIL;

    DWORD dwTextFormat = pElement->dwTextFormat | DT_CALCRECT;
    // Since we are only computing the rectangle, we don't need a sprite.
    hr = pFontNode->pFont->DrawText( NULL, strText, nCount, prcDest, dwTextFormat, pElement->FontColor.Current );
    if( FAILED(hr) )
        return hr;

    return S_OK;
}


INT CDXUTDialog::CalcTextWidth(LPCWSTR strText, CDXUTElement* pElement)
{
    DXUTFontNode* pFontNode = GetFont( pElement->iFont );
    if( pFontNode == NULL )
        return -1;

    DWORD dwTextFormat = pElement->dwTextFormat | DT_CALCRECT;
    // Since we are only computing the rectangle, we don't need a sprite.
	RECT rect={0, 0, 1000, 1000};
    HRESULT hr = pFontNode->pFont->DrawText( NULL, strText, (INT)_tcsclen(strText), &rect, dwTextFormat, 0xffffff);
    if( FAILED(hr) )
        return -1;

	return rect.right - rect.left +1;
}

//--------------------------------------------------------------------------------------
// 畫字
//--------------------------------------------------------------------------------------

void CDXUTDialog::ClientToScreen(RECT* prcDest)
{
    assert(prcDest);
    OffsetRect( prcDest, m_x, m_y );
    // If caption is enabled, offset the Y position by its height.
    if( m_bCaption )
        OffsetRect( prcDest, 0, m_nCaptionHeight );
}

HRESULT CDXUTDialog::DrawTextObject(CTextObject* pTextObject, CDXUTElement* pElement, RECT* prcDest, bool bShadow)
{  
    // No need to draw fully transparent layers
    if (pElement->FontColor.Current.a == 0)
        return S_OK;

    RECT rcScreen = *prcDest;
    ClientToScreen(&rcScreen);

    D3DXMATRIX matTransform;
    D3DXMatrixIdentity( &matTransform );
    s_pManager->m_pSprite->SetTransform( &matTransform );

    pTextObject->DrawText(pElement->dwTextFormat, &rcScreen, s_pManager->m_pSprite, bShadow);

    return S_OK;
}

HRESULT CDXUTDialog::DrawLineObject(CLineObject* pLineObject, CDXUTElement* pElement, RECT* prcDest, bool bShadow)
{ 
    // No need to draw fully transparent layers
    if (pElement->FontColor.Current.a == 0)
        return S_OK;

    RECT rcScreen = *prcDest;
    ClientToScreen(&rcScreen);

    D3DXMATRIX matTransform;
    D3DXMatrixIdentity( &matTransform );
    s_pManager->m_pSprite->SetTransform( &matTransform );

    pLineObject->DrawText(pElement->dwTextFormat, &rcScreen, s_pManager->m_pSprite, bShadow);
    return S_OK;
}

/*
HRESULT CDXUTDialog::DrawXmlText(LPCWSTR szText, CDXUTElement* pElement, RECT* prcDest, bool bShadow, int nCount)
{  
    // No need to draw fully transparent layers
    if( pElement->FontColor.Current.a == 0 )
        return S_OK;

    RECT rcScreen = *prcDest;
    ClientToScreen(&rcScreen);

    D3DXMATRIXA16 matTransform;
    D3DXMatrixIdentity( &matTransform );
    s_pManager->m_pSprite->SetTransform( &matTransform );

    DXUTFontNode* pFontNode = GetFont( pElement->iFont );
    g_FDOXML.SetFont(pFontNode->pFont);
    g_FDOXML.SetFormat(pElement->dwTextFormat);
    
    // Shadow support
    if (bShadow)
    {
        RECT rcShadow = rcScreen;
        DWORD dwColor = D3DCOLOR_ARGB(DWORD(pElement->FontColor.Current.a * 255), 0, 0, 0);
        g_FDOXML.SetColor(dwColor);
        for (int i = 0; i < 1; i++) {
            OffsetRect( &rcShadow, 1, 1 );
            g_FDOXML.SetDocumentRect(rcShadow);
            g_FDOXML.SetViewRect(rcShadow);
            g_FDOXML.DrawText(g_pD3DDevice, szText, nCount, s_pManager->m_pSprite);                
        } // end for
    } // end if

    g_FDOXML.SetColor(pElement->FontColor.Current);
    g_FDOXML.SetDocumentRect(rcScreen);
    g_FDOXML.SetViewRect(rcScreen);
    g_FDOXML.DrawText(g_pD3DDevice, szText, nCount, s_pManager->m_pSprite);    

    return S_OK;
}
*/
HRESULT CDXUTDialog::DrawPlainText( LPCWSTR strText, CDXUTElement* pElement, const RECT* prcDest, bool bShadow, int nCount )
{
	Rect32 rc = *prcDest;
	ClientToScreen(&rc);	

	uint dwFormat = pElement->dwTextFormat ;
	uint dwColor = D3DCOLOR_ARGB(DWORD(pElement->FontColor.Current.a * 255) 
		,DWORD(pElement->FontColor.Current.r * 255)
		,DWORD(pElement->FontColor.Current.g * 255)
		,DWORD(pElement->FontColor.Current.b * 255));

	CDXUTDialog::GetManager()->DrawFont(pElement->iFont ,strText,rc,dwFormat,dwColor,(bShadow)? 2 : 0);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CDXUTDialog::DrawPlainText( LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, bool bShadow, int nCount )
{
	
	Rect32 rc = *prcDest;
	ClientToScreen(&rc);	

	uint dwFormat = pElement->dwTextFormat ;
	uint dwColor = D3DCOLOR_ARGB(DWORD(pElement->FontColor.Current.a * 255) 
								,DWORD(pElement->FontColor.Current.r * 255)
								,DWORD(pElement->FontColor.Current.g * 255)
								,DWORD(pElement->FontColor.Current.b * 255));
	
	CDXUTDialog::GetManager()->DrawFont(pElement->iFont ,strText,rc,dwFormat,dwColor,(bShadow)? 2 : 0);

//     HRESULT hr = S_OK;
//     
//     // No need to draw fully transparent layers
//     if( pElement->FontColor.Current.a == 0 )
//         return S_OK;
// 
//     RECT rcScreen = *prcDest;
//     ClientToScreen(&rcScreen);
// 
//     D3DXMATRIXA16 matTransform;
//     D3DXMatrixIdentity( &matTransform );
//     s_pManager->m_pSprite->SetTransform( &matTransform );
// 
//     DXUTFontNode* pFontNode = GetFont( pElement->iFont );
// 
//     if( bShadow )
//     {
//         RECT rcShadow = rcScreen;
//         OffsetRect( &rcShadow, 1, 1 );
//         hr = pFontNode->pFont->DrawText(
//             s_pManager->m_pSprite, strText, nCount, &rcShadow, pElement->dwTextFormat,
//             D3DCOLOR_ARGB(DWORD(pElement->FontColor.Current.a * 255), 0, 0, 0) );
//         if( FAILED(hr) )
//             return hr;
//     }
// 
//     hr = pFontNode->pFont->DrawText(
//         s_pManager->m_pSprite, strText, nCount, &rcScreen,
//         pElement->dwTextFormat, pElement->FontColor.Current );
//     if( FAILED(hr) )
//         return hr;

    return S_OK;
}

//--------------------------------------------------------------------------------------
// ming add 設定Dialog的材質
//--------------------------------------------------------------------------------------
bool CDXUTDialog::SetBackgroundTexture( const string &FileName, RECT* prcTexture, RECT* prcDraw )
{
    int iTexture = s_pManager->AddTexture( FileName );
    if( iTexture < 0 )
        return false;

    m_DiaElement.SetTexture( iTexture, prcTexture );
    m_DiaElement.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    // Pre-blend as we don't need to transition the state
    m_DiaElement.TextureColor.Blend( DXUT_STATE_NORMAL, 10.0f );

    if( prcDraw )
        m_rcDraw = *prcDraw;
    else
        SetRectEmpty( &m_rcDraw );

    return true;
}

//-------------------------------------------------------------------------------------------
// 手動設定八個外筐材質*:iNum:0：介面底材質, 1：左上, 2：上邊, 3：右上, 4：右邊, 5：右下, 6：下邊, 7：左下, 8：左邊, 9:CAPTION下面那條橫槓
//-------------------------------------------------------------------------------------------
bool CDXUTDialog::SetBackgroundTexture2( int iNum, const string &FileName, RECT* prcTexture, RECT* prcDraw )
{
    int iTexture = s_pManager->AddTexture( FileName );
    if( iTexture < 0 )
        return false;

	if( iNum == 9 && m_bMinimized )
		return false;

    m_SidElement[iNum].SetTexture( iTexture, prcTexture );
    m_SidElement[iNum].TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    // Pre-blend as we don't need to transition the state
    m_SidElement[iNum].TextureColor.Blend( DXUT_STATE_NORMAL, 10.0f );

	if( iNum == 9)
		Set9Bar( 0, 0 );

    if( prcDraw )
        m_rcDraw = *prcDraw;
    else
        SetRectEmpty( &m_rcDraw );

    return true;
}

//--------------------------------------------------------------------------------------
//自動設定八個外筐材質 （範圍直接給整個大筐的RECT,但是底圖材質跟CAPTION下面那條要另外設定）
//---------------------------------------------------------------------------------------
bool CDXUTDialog::SetBackgroundTextureAuto9( const string &FileName, RECT* prcTexture, int iGauge, RECT* prcDraw )
{
    for(int i=1;i<9;i++)
    {
        int iTexture = s_pManager->AddTexture( FileName );
        if( iTexture < 0 )
            return false;

        RECT rcSide;
        if( i==1 )                                                                                                            //left  top
            SetRect(&rcSide, prcTexture->left, prcTexture->top, prcTexture->left+iGauge-1, prcTexture->top+iGauge-1 );
        else if( i==2 )                                                                                                       //	  top
            SetRect(&rcSide, prcTexture->left+iGauge, prcTexture->top, prcTexture->right-iGauge, prcTexture->top+iGauge-1 );
        else if( i==3 )                                                                                                       //right top
            SetRect(&rcSide, prcTexture->right-iGauge+1, prcTexture->top, prcTexture->right, prcTexture->top+iGauge-1 );
        else if( i==4 )                                                                                                       //right
            SetRect(&rcSide, prcTexture->right-iGauge+1, prcTexture->top+iGauge+15, prcTexture->right, prcTexture->bottom-iGauge );
        else if( i==5 )                                                                                                       //right bottom
            SetRect(&rcSide, prcTexture->right-iGauge+1, prcTexture->bottom-iGauge+1, prcTexture->right, prcTexture->bottom );
        else if( i==6 )                                                                                                       //	  bottom
            SetRect(&rcSide, prcTexture->left+iGauge, prcTexture->bottom-iGauge+1, prcTexture->right-iGauge, prcTexture->bottom );
        else if( i==7 )                                                                                                       //left  bottom
            SetRect(&rcSide, prcTexture->left, prcTexture->bottom-iGauge+1, prcTexture->left+iGauge-1, prcTexture->bottom );
        else if( i==8 )                                                                                                       //left
            SetRect(&rcSide, prcTexture->left, prcTexture->top+iGauge+15, prcTexture->left+iGauge-1, prcTexture->bottom-iGauge );
        //else if( i==9 )
        //    SetRect(&rcSide, prcTexture->left+5, prcTexture->top+26, prcTexture->right-4, prcTexture->top+31 );

        m_SidElement[i].SetTexture( iTexture, &rcSide );
        m_SidElement[i].TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
        // Pre-blend as we don't need to transition the state
        m_SidElement[i].TextureColor.Blend( DXUT_STATE_NORMAL, 10.0f );


        if( prcDraw )
            m_rcDraw = *prcDraw;
        else
            SetRectEmpty( &m_rcDraw );
    }

    return true;
}

//--------------------------------------------------------------------------------------
// ming add 設定Dialog的材質
//--------------------------------------------------------------------------------------
bool CDXUTDialog::SetBackgroundTexture( LPCWSTR FileName, RECT* prcTexture, RECT* prcDraw )
{
    return SetBackgroundTexture( UNICODEtoANSI(FileName), prcTexture );
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::SetBackgroundTexture2( int iNum, LPCWSTR FileName, RECT* prcTexture, RECT* prcDraw )
{
    return SetBackgroundTexture2( iNum, UNICODEtoANSI(FileName), prcTexture );
}
//--------------------------------------------------------------------------------------
bool CDXUTDialog::SetBackgroundTextureAuto9( LPCWSTR FileName, RECT* prcTexture, int iGauge, RECT* prcDraw )
{
    return SetBackgroundTextureAuto9( UNICODEtoANSI(FileName), prcTexture, iGauge );
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight )
{
    m_colorTopLeft = colorTopLeft;
    m_colorTopRight = colorTopRight;
    m_colorBottomLeft = colorBottomLeft;
    m_colorBottomRight = colorBottomRight;
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::SetNextDialog( CDXUTDialog* pNextDialog )
{ 
    if( pNextDialog == NULL )
        pNextDialog = this;
    
    m_pNextDialog = pNextDialog;
    m_pNextDialog->m_pPrevDialog = this;
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::ClearFocus()
{
    if( s_pControlFocus )
    {
        s_pControlFocus->OnFocusOut();
        s_pControlFocus = NULL;
    }

    ReleaseCapture();
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::OnDialogFocusOut()
{
    if (s_pFocusDialog == this) 
	{
        s_pFocusDialog = NULL;
		SendEvent(EVENT_DIALOG_FOCUSOUT, true , 0 , 0);
		//m_pCallbackEvent( EVENT_DIALOG_FOCUSOUT, -1, NULL, m_pCallbackEventUserContext );

        if (s_pDefFocusDialog)
            s_pDefFocusDialog->RequestDefaultFocus();

		
    } // end if
}

void CDXUTDialog::OnDialogFocusIn()
{
	if(s_pFocusDialog != 0 && s_pFocusDialog != this)
	{
		s_pFocusDialog->OnDialogFocusOut();
	}

	if(s_pFocusDialog != this)
	{
		SendEvent(EVENT_DIALOG_FOCUSIN, true , 0 , 0);
		//m_pCallbackEvent( EVENT_DIALOG_FOCUSIN, -1, NULL, m_pCallbackEventUserContext );
		s_pFocusDialog = this;
	}
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::FocusDefaultControl()
{
    // Check for default control in this dialog
    for( int i=0; i < m_Controls.GetSize(); i++ )
    {
        CDXUTControl* pControl = m_Controls.GetAt( i );
        if( pControl->m_bIsDefault )
        {
            // Remove focus from the current control
            ClearFocus();

            // Give focus to the default control
            s_pControlFocus = pControl;
            s_pControlFocus->OnFocusIn();
            return;
        }
    }
}



//--------------------------------------------------------------------------------------
bool CDXUTDialog::OnCycleFocus( bool bForward )
{
    // This should only be handled by the dialog which owns the focused control, and 
    // only if a control currently has focus
    if( s_pControlFocus == NULL || s_pControlFocus->m_pDialog != this )
        return false;

    CDXUTControl* pControl = s_pControlFocus;
    for( int i=0; i < 0xffff; i++ )
    {
        pControl = (bForward) ? GetNextControl( pControl ) : GetPrevControl( pControl );
        
        // If we've gone in a full circle then focus doesn't change
        if( pControl == s_pControlFocus )
            return true;

        // If the dialog accepts keybord input and the control can have focus then
        // move focus
        if( pControl->m_pDialog->m_bKeyboardInput && pControl->CanHaveFocus() )
        {
            s_pControlFocus->OnFocusOut();
            s_pControlFocus = pControl;
            s_pControlFocus->OnFocusIn();
            return true;
        }
    }

    // If we reached this point, the chain of dialogs didn't form a complete loop
    DXTRACE_ERR( L"CDXUTDialog: Multiple dialogs are improperly chained together", E_FAIL );
    return false;
}


bool CDXUTDialog::Raise(bool bSet)
{
	CDXUTDialog* pDlg = this;
	CDXUTDialog* pParent = 0;
	if(pDlg->GetParent())
	{
		pParent = pDlg->GetParent()->GetParent();
	}	
	while (pParent)
	{
		if(pParent->GetParent())
			pParent = pParent->GetParent()->GetParent();
		else
			break;
	}
	
	if(pParent)
	{
		if(bSet)
			pParent->push_front();
		else
			pParent->push_back();
	}
	else
	{
		if(bSet)
			push_front();
		else
			push_back();
	}
	return true;

}

void CDXUTDialog::SetAllControlEnable(bool bEnable)
{
	_FOREACH( ControlCache , mControlCache , it )
	{
		CDXUTControl* pControl = it->second ;
		if( fnIsSubDialogType(pControl->GetType()) )
		{
			CDXUTDialog* pSubDialog = ( ( CDXUTSubDialog* )pControl )->GetDialog() ;
			if( pSubDialog )
				pSubDialog->SetAllControlEnable( bEnable ) ;
		}
		else pControl->SetEnabled( bEnable ) ;
	}
	//for( int i=0; i < m_Controls.GetSize(); i++ )
	//{
	//	CDXUTControl* pControl = m_Controls.GetAt( i );
	//	pControl->SetEnabled(bEnable);
	//}
}

//--------------------------------------------------------------------------------------
// ming add 把這個Dialog推至串列的最前
//--------------------------------------------------------------------------------------
bool CDXUTDialog::push_front()
{
    if( !m_pDialogList )
        return true;

    if( m_pDialogList->front() != this )
    {
        m_pDialogList->remove( this );
        m_pDialogList->push_front( this );
    }
	
	for (int i = 0 ; i < m_Controls.GetSize() ; ++i)
	{
		CDXUTControl* pCtrl = m_Controls.GetAt(i);
		if( fnIsSubDialogType(pCtrl->GetType()) == false)
			continue;

		CDXUTSubDialog*	pSub = (CDXUTSubDialog*)pCtrl;
		pSub->Raise();
	}

	//if( s_pFocusDialog!=NULL && s_pFocusDialog!=this && s_pFocusDialog!=s_pMouseDialog )
        //s_pFocusDialog->OnMouseLeave(); // fade out Focus

	if( s_pFocusDialog && s_pFocusDialog != s_pMouseDialog && s_pMouseDialog == this ) 
	{
		OnDialogFocusIn();
	}
	else if( s_pFocusDialog == NULL && s_pMouseDialog == this ) 
	{
		OnDialogFocusIn();
	}
	//OnMouseEnter();
    return true;
}

//--------------------------------------------------------------------------------------
// ming add 把這個Dialog推至串列的最後
//--------------------------------------------------------------------------------------
bool CDXUTDialog::push_back()
{
    if( !m_pDialogList )
        return true;

	for (int i = 0 ; i < m_Controls.GetSize() ; ++i)
	{
		CDXUTControl* pCtrl = m_Controls.GetAt(i);
		if( fnIsSubDialogType(pCtrl->GetType()) == false)
			continue;

		CDXUTSubDialog*	pSub = (CDXUTSubDialog*)pCtrl;
		pSub->Raise(false);
	}

    if( m_pDialogList->back() != this )
    {
        m_pDialogList->remove( this );
        m_pDialogList->push_back( this );
    }

    if( s_pFocusDialog == this )
        OnDialogFocusOut();

	
    return true;
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::Show()
{
  //音效 *** modify
   g_SoundManager._Play2DSoundInMemory(GetSoundMap(UNICODEtoANSI(m_wszCaption)));

    SetVisible( true );
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::Hide()
{
    SetVisible( false );
}

//--------------------------------------------------------------------------------------
void CDXUTDialog::InitDefaultElements_HvoStyle()
{
    SetTexture( 0, L_OBJ_FILE_PATH );
    //SetFont( 0, L"Arial", 14, FW_NORMAL );
    //SetFont( 0, L"Comic Sans MS", 16, FW_NORMAL );
    //SetFont( 0, L"Lucida Sans Unicode", 16, FW_NORMAL );細明體
   // string str;
  //  str.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 51, CodeInlineText_Text ));//細明體
    SetFont( 0, sm_defaultFont.c_str(), 16, FW_NORMAL ); // 設定基本的字型

    CDXUTElement Element;
    RECT rcTexture;

    //-------------------------------------
    // Element for the dialog
    //-------------------------------------
    m_DiaElement.TextureColor.Init( 0, 0, 0, 0 );
    // Darkness add
    for(int i=0;i<10;i++)
        m_SidElement[i].TextureColor.Init( 0, 0, 0, 0 );
    // ------------

    //-------------------------------------
    // Element for the caption
    //-------------------------------------
    m_CapElement.SetFont( 0 );
    SetRect( &rcTexture, 8, 3, 10, 5 );//13, 334, 240, 349 );
    m_CapElement.SetTexture( 0, &rcTexture );
    m_CapElement.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    m_CapElement.FontColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    m_CapElement.SetFont( 0, D3DCOLOR_ARGB(255, 250, 250, 50), DT_LEFT | DT_VCENTER );
    // Pre-blend as we don't need to transition the state
    m_CapElement.TextureColor.Blend( DXUT_STATE_NORMAL, 10.0f );
    m_CapElement.FontColor.Blend( DXUT_STATE_NORMAL, 10.0f ,0.0f );
		
	//-------------------------------------
    // CDXUTStatic
    //-------------------------------------
    Element.SetFont( 0 );
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_STATIC, 0, &Element );

    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER );
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_IMAGEBOX, 0, &Element );
    
    //-------------------------------------
    // CDXUTButton - Button
    //-------------------------------------
    SetRect( &rcTexture, 90, 3, 155, 36 );//5, 3, 127, 51 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 0, 0, 0);

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_BUTTON, 0, &Element );

    //-------------------------------------
    // CDXUTButton - Fill layer
    //-------------------------------------
    SetRect( &rcTexture, 84, 38, 149, 71 );//141, 3, 263, 51 );
    Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(0, 255, 255, 255) );
    Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 0, 0, 0);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_BUTTON, 1, &Element );


    //-------------------------------------
    // CDXUTCheckBox - Box
    //-------------------------------------
    SetRect( &rcTexture, 21, 69, 31, 79 );//2, 58, 21, 77 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_CHECKBOX, 0, &Element );


    //-------------------------------------
    // CDXUTCheckBox - Check
    //-------------------------------------
    //SetRect( &rcTexture, 30, 58, 48, 77 );
    SetRect( &rcTexture, 47, 29, 57, 39 );//84, 58, 102, 77 );
    Element.SetTexture( 0, &rcTexture );
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_CHECKBOX, 1, &Element );


    //-------------------------------------
    // CDXUTRadioButton - Box
    //-------------------------------------
    SetRect( &rcTexture, 34, 69, 45, 80 );//55, 57, 77, 78 );
    //InflateRect( &rcTexture, 2, 2 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(200, 210, 210, 255);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 150, 150, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 245, 245, 255);

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_RADIOBUTTON, 0, &Element );


    //-------------------------------------
    // CDXUTRadioButton - Check
    //-------------------------------------
    SetRect( &rcTexture, 12, 71, 19, 78 );//82, 56, 104, 78 );
    InflateRect( &rcTexture, 2, 2 );
    Element.SetTexture( 0, &rcTexture );
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_RADIOBUTTON, 1, &Element );


    //-------------------------------------
    // CDXUTComboBox - Main
    //-------------------------------------
    SetRect( &rcTexture, 13, 3, 15, 5 );//5, 86, 243, 117 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 128, 128, 128), DT_LEFT | DT_VCENTER );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 200, 200, 200);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(170, 230, 230, 230);
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 200, 200, 200);
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 0, 0, 0);
    Element.FontColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 0, 0, 0);
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(200, 200, 200, 200);
    
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 0, &Element );


    //-------------------------------------
    // CDXUTComboBox - Button
    //-------------------------------------
    SetRect( &rcTexture, 26, 23, 45, 42 );//272, 4, 322, 49 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 150, 150, 150);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 1, &Element );


    //-------------------------------------
    // CDXUTComboBox - Dropdown
    //-------------------------------------
    SetRect( &rcTexture, 13, 3, 15, 5 );//10, 121, 241, 265 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 2, &Element );


    //-------------------------------------
    // CDXUTComboBox - Selection
    //-------------------------------------
    SetRect( &rcTexture, 8, 3, 10, 5 );//11, 266, 242, 288 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 3, &Element );


    //-------------------------------------
    // CDXUTSlider - Track
    //-------------------------------------
    //SetRect( &rcTexture, 349, 201, 362, 215 );//0, 297, 288, 325 );
	
	SetRect( &rcTexture, 62, 74, 77, 83 );//0, 297, 288, 325 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SLIDER, 0, &Element );

    //-------------------------------------
    // CDXUTSlider - Button
    //-------------------------------------
    SetRect( &rcTexture, 12, 71, 19, 78 );//253, 61, 284, 90 );
    Element.SetTexture( 0, &rcTexture );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SLIDER, 1, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Track
    //-------------------------------------
    SetRect( &rcTexture, 211, 275, 226, 286 );//244, 143, 261, 156 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 0, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Up Arrow
    //-------------------------------------
    SetRect( &rcTexture, 211, 257, 226, 272 );//244, 125, 261, 141 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 1, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Down Arrow
    //-------------------------------------
    SetRect( &rcTexture, 211, 290, 226, 305 );//244, 158, 261, 174 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 2, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Thumb
    //-------------------------------------
    SetRect( &rcTexture, 73, 23, 90, 32 );//268, 142, 282, 151 );
    Element.SetTexture( 0, &rcTexture );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 3, &Element );


    //-------------------------------------
    // CDXUTEditBox
    //-------------------------------------
    // Element assignment:
    //   0 - text area
    //   1 - top left border
    //   2 - top border
    //   3 - top right border
    //   4 - left border
    //   5 - right border
    //   6 - lower left border
    //   7 - lower border
    //   8 - lower right border

    Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );

    // Assign the style
    SetRect( &rcTexture, 12, 130, 201, 139 );//13, 93, 235, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 0, &Element );
    SetRect( &rcTexture, 3, 121, 11, 129 );//5, 86, 13, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 1, &Element );
    SetRect( &rcTexture, 12, 121, 201, 129 );//13, 86, 235, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 2, &Element );
    SetRect( &rcTexture, 202, 121, 210, 129 );//235, 86, 243, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 3, &Element );
    SetRect( &rcTexture, 3, 130, 11, 139 );//5, 93, 13, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 4, &Element );
    SetRect( &rcTexture, 202, 130, 210, 139 );//235, 93, 243, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 5, &Element );
    SetRect( &rcTexture, 3, 140, 11, 148 );//5, 110, 13, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 6, &Element );
    SetRect( &rcTexture, 12, 140, 201, 148 );//13, 110, 235, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 7, &Element );
    SetRect( &rcTexture, 202, 140, 210, 148 );//235, 110, 243, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 8, &Element );


    //-------------------------------------
    // CDXUTIMEEditBox
    //-------------------------------------

    Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );

    // Assign the style
    SetRect( &rcTexture, 12, 130, 201, 139 );//13, 93, 235, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 0, &Element );
    SetRect( &rcTexture, 3, 121, 11, 129 );//5, 86, 13, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 1, &Element );
    SetRect( &rcTexture, 12, 121, 201, 129 );//13, 86, 235, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 2, &Element );
    SetRect( &rcTexture, 202, 121, 210, 129 );//235, 86, 243, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 3, &Element );
    SetRect( &rcTexture, 3, 130, 11, 139 );//5, 93, 13, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 4, &Element );
    SetRect( &rcTexture, 202, 130, 210, 139 );//235, 93, 243, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 5, &Element );
    SetRect( &rcTexture, 3, 140, 11, 148 );//5, 110, 13, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 6, &Element );
    SetRect( &rcTexture, 12, 140, 201, 148 );//13, 110, 235, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 7, &Element );
    SetRect( &rcTexture, 202, 140, 210, 148 );//235, 110, 243, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 8, &Element );
    // Element 9 for IME text, and indicator button
    SetRect( &rcTexture, 90, 3, 154, 35 );//5, 3, 127, 51 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_CENTER | DT_VCENTER );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 9, &Element );

    //-------------------------------------
    // CDXUTListBox - Main
    //-------------------------------------

    SetRect( &rcTexture, 3, 3, 5, 5 );//11, 123, 240, 263 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_LISTBOX, 0, &Element );

    //-------------------------------------
    // CDXUTListBox - Selection
    //-------------------------------------

    SetRect( &rcTexture, 8, 8, 10, 10 );//12, 269, 239, 286 );
    Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(100, 255, 255, 255) );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_LISTBOX, 1, &Element );

	SetRect( &rcTexture , 198 , 78 , 231 , 111 ) ;
	Element.SetFont( 0 );
	Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(100, 255, 255, 255) );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR(0xffffffff);
	Element.TextureColor.States[ DXUT_STATE_DISABLED ]    = D3DXCOLOR(0xc8808080);
	Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR(0xffffffff);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR(0xffffffff);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR(0xffffffff);
	Element.TextureColor.States[ DXUT_STATE_UNFOCUS ]     = D3DXCOLOR(0xc8c8c8c8);
	SetDefaultElement( DXUT_CONTROL_LISTBOX, 2, &Element );

	//-------------------------------------
    // CDXUTTabPage - LeftButton - Normal
    //-------------------------------------
	SetRect( &rcTexture , 275 , 288 , 295 , 307 ) ;
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(60, 0, 0, 0);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 0 , &Element ) ;
	//-------------------------------------
    // CDXUTTabPage - LeftButton - Focus 
    //-------------------------------------
	SetRect( &rcTexture , 363 , 288 , 383 , 307 ) ;
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(0, 255, 255, 255);

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 1 , &Element ) ;

	//-------------------------------------
    // CDXUTTabPage - RightButton - Normal
    //-------------------------------------
	SetRect( &rcTexture , 297 , 288 , 317 , 307 ) ;
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
	Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(60, 0, 0, 0);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 2 , &Element ) ;

	//-------------------------------------
    // CDXUTTabPage - RighttButton - Focus
    //-------------------------------------
	SetRect( &rcTexture , 385 , 288 , 405 , 307 ) ;
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(0, 255, 255, 255);

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 3 , &Element ) ;
    
	//-------------------------------------
    // CDXUTTabPage - CheckButton - Normal
    //-------------------------------------
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	Element.FontColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8808080 ) ;
	Element.FontColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xFF461616 ) ; 
	Element.FontColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.FontColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;
   
	SetRect( &rcTexture, 299, 256, 364, 285 ) ;
	Element.SetTexture( 0 , &rcTexture ) ;
	Element.TextureColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8808080 ) ;
	Element.TextureColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xffffffff ) ; 
	Element.TextureColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.TextureColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 4 , &Element ) ;

	//-------------------------------------
    // CDXUTTabPage - CheckButton - Focus
    //-------------------------------------
	Element.SetFont( 1, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	Element.FontColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.FontColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xFF461616 ) ; 
	Element.FontColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.FontColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;

	SetRect( &rcTexture, 231, 256, 296, 285 ) ;
	Element.SetTexture( 0 , &rcTexture ) ;
	Element.TextureColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8808080 ) ;
	Element.TextureColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xffffffff ) ; 
	Element.TextureColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.TextureColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 5 , &Element ) ;
}

void CDXUTDialog::InitDefaultElements_FdoStyle()
{
    SetTexture( 0, L"UI\\Object.tga" );
    //SetFont( 0, L"Arial", 14, FW_NORMAL );
    //SetFont( 0, L"Comic Sans MS", 16, FW_NORMAL );
    //SetFont( 0, L"Lucida Sans Unicode", 16, FW_NORMAL );細明體
   // string str;
  //  str.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 51, CodeInlineText_Text ));//細明體
    SetFont( 0, sm_defaultFont.c_str(), 12, FW_NORMAL ); // 設定基本的字型

    CDXUTElement Element;
    RECT rcTexture;

    //-------------------------------------
    // Element for the dialog
    //-------------------------------------
    m_DiaElement.TextureColor.Init( 0, 0, 0, 0 );
    // Darkness add
    for(int i=0;i<10;i++)
        m_SidElement[i].TextureColor.Init( 0, 0, 0, 0 );
    // ------------

    //-------------------------------------
    // Element for the caption
    //-------------------------------------
    m_CapElement.SetFont( 0 );
    SetRect( &rcTexture, 8, 3, 10, 5 );//13, 334, 240, 349 );
    m_CapElement.SetTexture( 0, &rcTexture );
    m_CapElement.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    m_CapElement.FontColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    m_CapElement.SetFont( 0, D3DCOLOR_ARGB(255, 250, 250, 50), DT_LEFT | DT_VCENTER );
    // Pre-blend as we don't need to transition the state
    m_CapElement.TextureColor.Blend( DXUT_STATE_NORMAL, 10.0f );
    m_CapElement.FontColor.Blend( DXUT_STATE_NORMAL, 10.0f ,0.0f );

    //-------------------------------------
    // CDXUTStatic
    //-------------------------------------
    Element.SetFont( 0 );
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_STATIC, 0, &Element );

    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER );
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_IMAGEBOX, 0, &Element );
    
    //-------------------------------------
    // CDXUTButton - Button
    //-------------------------------------
    SetRect( &rcTexture, 90, 3, 155, 36 );//5, 3, 127, 51 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 0, 0, 0);

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_BUTTON, 0, &Element );
    

    //-------------------------------------
    // CDXUTButton - Fill layer
    //-------------------------------------
    SetRect( &rcTexture, 84, 38, 149, 71 );//141, 3, 263, 51 );
    Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(0, 255, 255, 255) );
    Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 0, 0, 0);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_BUTTON, 1, &Element );


    //-------------------------------------
    // CDXUTCheckBox - Box
    //-------------------------------------
    SetRect( &rcTexture, 21, 69, 31, 79 );//2, 58, 21, 77 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_CHECKBOX, 0, &Element );
    //-------------------------------------
    // CDXUTCheckBox - Check
    //-------------------------------------
    //SetRect( &rcTexture, 30, 58, 48, 77 );
    SetRect( &rcTexture, 47, 29, 57, 39 );//84, 58, 102, 77 );
    Element.SetTexture( 0, &rcTexture );
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_CHECKBOX, 1, &Element );


    //-------------------------------------
    // CDXUTRadioButton - Box
    //-------------------------------------
    SetRect( &rcTexture, 34, 69, 45, 80 );//55, 57, 77, 78 );
    //InflateRect( &rcTexture, 2, 2 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(200, 210, 210, 255);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 150, 150, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 245, 245, 255);

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_RADIOBUTTON, 0, &Element );


    //-------------------------------------
    // CDXUTRadioButton - Check
    //-------------------------------------
    SetRect( &rcTexture, 12, 71, 19, 78 );//82, 56, 104, 78 );
    InflateRect( &rcTexture, 2, 2 );
    Element.SetTexture( 0, &rcTexture );
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_RADIOBUTTON, 1, &Element );


    //-------------------------------------
    // CDXUTComboBox - Main
    //-------------------------------------
    SetRect( &rcTexture, 13, 3, 15, 5 );//5, 86, 243, 117 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 128, 128, 128), DT_LEFT | DT_VCENTER );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 200, 200, 200);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(170, 230, 230, 230);
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 200, 200, 200);
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 0, 0, 0);
    Element.FontColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 0, 0, 0);
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(200, 200, 200, 200);
    
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 0, &Element );


    //-------------------------------------
    // CDXUTComboBox - Button
    //-------------------------------------
    SetRect( &rcTexture, 26, 23, 45, 42 );//272, 4, 322, 49 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 150, 150, 150);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 1, &Element );


    //-------------------------------------
    // CDXUTComboBox - Dropdown
    //-------------------------------------
    SetRect( &rcTexture, 13, 3, 15, 5 );//10, 121, 241, 265 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 2, &Element );


    //-------------------------------------
    // CDXUTComboBox - Selection
    //-------------------------------------
    SetRect( &rcTexture, 8, 3, 10, 5 );//11, 266, 242, 288 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 3, &Element );


    //-------------------------------------
    // CDXUTSlider - Track
    //-------------------------------------
	//old
    SetRect( &rcTexture, 349, 201, 362, 215 );//0, 297, 288, 325 );
	//SetRect( &rcTexture, 62, 74, 77, 83 );//0, 297, 288, 325 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SLIDER, 0, &Element );

    //-------------------------------------
    // CDXUTSlider - Button
    //-------------------------------------
    SetRect( &rcTexture, 12, 71, 19, 78 );//253, 61, 284, 90 );
    Element.SetTexture( 0, &rcTexture );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SLIDER, 1, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Track
    //-------------------------------------
    SetRect( &rcTexture, 211, 275, 226, 286 );//244, 143, 261, 156 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 0, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Up Arrow
    //-------------------------------------
    SetRect( &rcTexture, 211, 257, 226, 272 );//244, 125, 261, 141 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 1, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Down Arrow
    //-------------------------------------
    SetRect( &rcTexture, 211, 290, 226, 305 );//244, 158, 261, 174 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 2, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Thumb
    //-------------------------------------
    SetRect( &rcTexture, 73, 23, 90, 32 );//268, 142, 282, 151 );
    Element.SetTexture( 0, &rcTexture );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 3, &Element );


    //-------------------------------------
    // CDXUTEditBox
    //-------------------------------------
    // Element assignment:
    //   0 - text area
    //   1 - top left border
    //   2 - top border
    //   3 - top right border
    //   4 - left border
    //   5 - right border
    //   6 - lower left border
    //   7 - lower border
    //   8 - lower right border

    Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );

    // Assign the style
    SetRect( &rcTexture, 12, 130, 201, 139 );//13, 93, 235, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 0, &Element );
    SetRect( &rcTexture, 3, 121, 11, 129 );//5, 86, 13, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 1, &Element );
    SetRect( &rcTexture, 12, 121, 201, 129 );//13, 86, 235, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 2, &Element );
    SetRect( &rcTexture, 202, 121, 210, 129 );//235, 86, 243, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 3, &Element );
    SetRect( &rcTexture, 3, 130, 11, 139 );//5, 93, 13, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 4, &Element );
    SetRect( &rcTexture, 202, 130, 210, 139 );//235, 93, 243, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 5, &Element );
    SetRect( &rcTexture, 3, 140, 11, 148 );//5, 110, 13, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 6, &Element );
    SetRect( &rcTexture, 12, 140, 201, 148 );//13, 110, 235, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 7, &Element );
    SetRect( &rcTexture, 202, 140, 210, 148 );//235, 110, 243, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 8, &Element );


    //-------------------------------------
    // CDXUTIMEEditBox
    //-------------------------------------

    Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );

    // Assign the style
    SetRect( &rcTexture, 12, 130, 201, 139 );//13, 93, 235, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 0, &Element );
    SetRect( &rcTexture, 3, 121, 11, 129 );//5, 86, 13, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 1, &Element );
    SetRect( &rcTexture, 12, 121, 201, 129 );//13, 86, 235, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 2, &Element );
    SetRect( &rcTexture, 202, 121, 210, 129 );//235, 86, 243, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 3, &Element );
    SetRect( &rcTexture, 3, 130, 11, 139 );//5, 93, 13, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 4, &Element );
    SetRect( &rcTexture, 202, 130, 210, 139 );//235, 93, 243, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 5, &Element );
    SetRect( &rcTexture, 3, 140, 11, 148 );//5, 110, 13, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 6, &Element );
    SetRect( &rcTexture, 12, 140, 201, 148 );//13, 110, 235, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 7, &Element );
    SetRect( &rcTexture, 202, 140, 210, 148 );//235, 110, 243, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 8, &Element );
    // Element 9 for IME text, and indicator button
    SetRect( &rcTexture, 90, 3, 154, 35 );//5, 3, 127, 51 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_CENTER | DT_VCENTER );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 9, &Element );

    //-------------------------------------
    // CDXUTListBox - Main
    //-------------------------------------

    SetRect( &rcTexture, 3, 3, 5, 5 );//11, 123, 240, 263 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_LISTBOX, 0, &Element );

    //-------------------------------------
    // CDXUTListBox - Selection
    //-------------------------------------

    SetRect( &rcTexture, 8, 8, 10, 10 );//12, 269, 239, 286 );
    Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(100, 255, 255, 255) );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_LISTBOX, 1, &Element );

	SetRect( &rcTexture , 198 , 78 , 231 , 111 ) ;
	Element.SetFont( 0 );
	Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(100, 255, 255, 255) );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR(0xffffffff);
	Element.TextureColor.States[ DXUT_STATE_DISABLED ]    = D3DXCOLOR(0xc8808080);
	Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR(0xffffffff);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR(0xffffffff);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR(0xffffffff);
	Element.TextureColor.States[ DXUT_STATE_UNFOCUS ]     = D3DXCOLOR(0xc8c8c8c8);
	SetDefaultElement( DXUT_CONTROL_LISTBOX, 2, &Element );
	/*SetTexture( 0, L"UI\\Common.tga" );
    //SetFont( 0, L"Arial", 14, FW_NORMAL );
    //SetFont( 0, L"Comic Sans MS", 16, FW_NORMAL );
    //SetFont( 0, L"Lucida Sans Unicode", 16, FW_NORMAL );細明體
    //string str;
    //str.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 51, CodeInlineText_Text ));//細明體
    SetFont( 0, sm_defaultFont.c_str(), 12, FW_NORMAL ); // 設定基本的字型

    CDXUTElement Element;
    RECT rcTexture;

    //-------------------------------------
    // Element for the dialog
    //-------------------------------------
    m_DiaElement.TextureColor.Init( 0, 0, 0, 0 );
    // Darkness add
    for(int i=0;i<10;i++)
        m_SidElement[i].TextureColor.Init( 0, 0, 0, 0 );
    // ------------

    //-------------------------------------
    // Element for the caption
    //-------------------------------------
    m_CapElement.SetFont( 0 );
    SetRect( &rcTexture, 13, 334, 240, 349 );
    m_CapElement.SetTexture( 0, &rcTexture );
    m_CapElement.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    m_CapElement.FontColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    m_CapElement.SetFont( 0, D3DCOLOR_ARGB(255, 66, 0, 0), DT_LEFT | DT_VCENTER );
    // Pre-blend as we don't need to transition the state
    m_CapElement.TextureColor.Blend( DXUT_STATE_NORMAL, 10.0f );
    m_CapElement.FontColor.Blend( DXUT_STATE_NORMAL, 10.0f ,0.0f );

    //-------------------------------------
    // CDXUTStatic
    //-------------------------------------
    Element.SetFont( 0 );
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_STATIC, 0, &Element );

    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER );
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_IMAGEBOX, 0, &Element );
    
    //-------------------------------------
    // CDXUTButton - Button
    //-------------------------------------
    SetRect( &rcTexture, 5, 3, 127, 51 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 0, 0, 0);

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_BUTTON, 0, &Element );
    

    //-------------------------------------
    // CDXUTButton - Fill layer
    //-------------------------------------
    SetRect( &rcTexture, 141, 3, 263, 51 );
    Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(0, 255, 255, 255) );
    Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 0, 0, 0);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_BUTTON, 1, &Element );


    //-------------------------------------
    // CDXUTCheckBox - Box
    //-------------------------------------
    SetRect( &rcTexture, 2, 58, 21, 77 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_VCENTER );
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_CHECKBOX, 0, &Element );


    //-------------------------------------
    // CDXUTCheckBox - Check
    //-------------------------------------
    //SetRect( &rcTexture, 30, 58, 48, 77 );
    SetRect( &rcTexture, 84, 58, 102, 77 );
    Element.SetTexture( 0, &rcTexture );
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_CHECKBOX, 1, &Element );


    //-------------------------------------
    // CDXUTRadioButton - Box
    //-------------------------------------
    SetRect( &rcTexture, 55, 57, 77, 78 );
    InflateRect( &rcTexture, 4, 4 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_VCENTER );
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_RADIOBUTTON, 0, &Element );


    //-------------------------------------
    // CDXUTRadioButton - Check
    //-------------------------------------
    SetRect( &rcTexture, 82, 56, 104, 78 );
    InflateRect( &rcTexture, 4, 4 );
    Element.SetTexture( 0, &rcTexture );
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_RADIOBUTTON, 1, &Element );


    //-------------------------------------
    // CDXUTComboBox - Main
    //-------------------------------------
    SetRect( &rcTexture, 5, 86, 243, 117 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 128, 128, 128), DT_LEFT | DT_VCENTER );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 200, 200, 200);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(170, 230, 230, 230);
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 200, 200, 200);
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 0, 0, 0);
    Element.FontColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 0, 0, 0);
    Element.FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(200, 200, 200, 200);
    
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 0, &Element );


    //-------------------------------------
    // CDXUTComboBox - Button
    //-------------------------------------
    SetRect( &rcTexture, 272, 4, 322, 49 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 150, 150, 150);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 1, &Element );


    //-------------------------------------
    // CDXUTComboBox - Dropdown
    //-------------------------------------
    SetRect( &rcTexture, 10, 121, 241, 265 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 2, &Element );


    //-------------------------------------
    // CDXUTComboBox - Selection
    //-------------------------------------
    SetRect( &rcTexture, 11, 266, 242, 288 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_COMBOBOX, 3, &Element );


    //-------------------------------------
    // CDXUTSlider - Track
    //-------------------------------------
    SetRect( &rcTexture, 0, 297, 288, 325 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);
    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SLIDER, 0, &Element );

    //-------------------------------------
    // CDXUTSlider - Button
    //-------------------------------------
    SetRect( &rcTexture, 253, 61, 284, 90 );
    Element.SetTexture( 0, &rcTexture );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SLIDER, 1, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Track
    //-------------------------------------
    SetRect( &rcTexture, 244, 143, 261, 156 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 0, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Up Arrow
    //-------------------------------------
    SetRect( &rcTexture, 244, 125, 261, 141 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 1, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Down Arrow
    //-------------------------------------
    SetRect( &rcTexture, 244, 158, 261, 174 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

    
    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 2, &Element );

    //-------------------------------------
    // CDXUTScrollBar - Thumb
    //-------------------------------------
    SetRect( &rcTexture, 268, 142, 282, 151 );
    Element.SetTexture( 0, &rcTexture );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_SCROLLBAR, 3, &Element );


    //-------------------------------------
    // CDXUTEditBox
    //-------------------------------------
    // Element assignment:
    //   0 - text area
    //   1 - top left border
    //   2 - top border
    //   3 - top right border
    //   4 - left border
    //   5 - right border
    //   6 - lower left border
    //   7 - lower border
    //   8 - lower right border

    Element.SetFont( 0, D3DCOLOR_ARGB( 255, 50, 50, 50 ), DT_LEFT | DT_VCENTER );

    // Assign the style
    SetRect( &rcTexture, 13, 93, 235, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 0, &Element );
    SetRect( &rcTexture, 5, 86, 13, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 1, &Element );
    SetRect( &rcTexture, 13, 86, 235, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 2, &Element );
    SetRect( &rcTexture, 235, 86, 243, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 3, &Element );
    SetRect( &rcTexture, 5, 93, 13, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 4, &Element );
    SetRect( &rcTexture, 235, 93, 243, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 5, &Element );
    SetRect( &rcTexture, 5, 110, 13, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 6, &Element );
    SetRect( &rcTexture, 13, 110, 235, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 7, &Element );
    SetRect( &rcTexture, 235, 110, 243, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_EDITBOX, 8, &Element );


    //-------------------------------------
    // CDXUTIMEEditBox
    //-------------------------------------

    Element.SetFont( 0, D3DCOLOR_ARGB( 255, 0, 0, 0 ), DT_LEFT | DT_VCENTER );

    // Assign the style
    SetRect( &rcTexture, 13, 93, 235, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 0, &Element );
    SetRect( &rcTexture, 5, 86, 13, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 1, &Element );
    SetRect( &rcTexture, 13, 86, 235, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 2, &Element );
    SetRect( &rcTexture, 235, 86, 243, 93 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 3, &Element );
    SetRect( &rcTexture, 5, 93, 13, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 4, &Element );
    SetRect( &rcTexture, 235, 93, 243, 110 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 5, &Element );
    SetRect( &rcTexture, 5, 110, 13, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 6, &Element );
    SetRect( &rcTexture, 13, 110, 235, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 7, &Element );
    SetRect( &rcTexture, 235, 110, 243, 117 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 8, &Element );
    // Element 9 for IME text, and indicator button
    SetRect( &rcTexture, 5, 3, 127, 51 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB( 255, 50, 50, 50 ), DT_CENTER | DT_VCENTER );
    SetDefaultElement( DXUT_CONTROL_IMEEDITBOX, 9, &Element );

    //-------------------------------------
    // CDXUTListBox - Main
    //-------------------------------------

    SetRect( &rcTexture, 11, 123, 240, 263 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 100, 100, 100), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_LISTBOX, 0, &Element );

    //-------------------------------------
    // CDXUTListBox - Selection
    //-------------------------------------

    SetRect( &rcTexture, 12, 269, 239, 286 );
    Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(100, 255, 255, 255) );
    Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( DXUT_CONTROL_LISTBOX, 1, &Element );*/

		//-------------------------------------
    // CDXUTTabPage - LeftButton - Normal
    //-------------------------------------
	SetRect( &rcTexture , 275 , 288 , 295 , 307 ) ;
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(60, 0, 0, 0);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 0 , &Element ) ;
	//-------------------------------------
    // CDXUTTabPage - LeftButton - Focus 
    //-------------------------------------
	SetRect( &rcTexture , 363 , 288 , 383 , 307 ) ;
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(0, 255, 255, 255);

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 1 , &Element ) ;

	//-------------------------------------
    // CDXUTTabPage - RightButton - Normal
    //-------------------------------------
	SetRect( &rcTexture , 297 , 288 , 317 , 307 ) ;
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
	Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(60, 0, 0, 0);
    Element.TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 2 , &Element ) ;

	//-------------------------------------
    // CDXUTTabPage - RighttButton - Focus
    //-------------------------------------
	SetRect( &rcTexture , 385 , 288 , 405 , 307 ) ;
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    Element.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(0, 255, 255, 255);

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 3 , &Element ) ;
    
	//-------------------------------------
    // CDXUTTabPage - CheckButton - Normal
    //-------------------------------------
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	Element.FontColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8808080 ) ;
	Element.FontColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xFF461616 ) ; 
	Element.FontColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.FontColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;
   
	SetRect( &rcTexture, 299, 256, 364, 285 ) ;
	Element.SetTexture( 0 , &rcTexture ) ;
	Element.TextureColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8808080 ) ;
	Element.TextureColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xffffffff ) ; 
	Element.TextureColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.TextureColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 4 , &Element ) ;

	//-------------------------------------
    // CDXUTTabPage - CheckButton - Focus
    //-------------------------------------
	Element.SetFont( 1, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	Element.FontColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.FontColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xFF461616 ) ; 
	Element.FontColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xFF461616 ) ;
	Element.FontColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.FontColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;

	SetRect( &rcTexture, 231, 256, 296, 285 ) ;
	Element.SetTexture( 0 , &rcTexture ) ;
	Element.TextureColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8808080 ) ;
	Element.TextureColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xffffffff ) ; 
	Element.TextureColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xffffffff ) ;
	Element.TextureColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
	Element.TextureColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;

	SetDefaultElement( DXUT_CONTROL_TABPAGE , 5 , &Element ) ;
}


//--------------------------------------------------------------------------------------
void CDXUTDialog::fnSetResourceManager(CDXUTDialogResourceManager* pManager )
{
	s_pManager = pManager;
	assert(s_pManager);
}
//--------------------------------------------------------------------------------------
void CDXUTDialog::SetControlOrder(bool bSet)
{
	m_bControlOrder = bSet;
}
//////////////////////////////////////////////////////////////////////////
CDXUTDialog* CDXUTDialog::fnGetOnMouseDialog()
{
	return s_pMouseDialog;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTDialog::fnAdjLayout(CDXUTDialog* pDlg,RECT& tmp )
{	
	typedef std::set<sint>	LocationKeySet;
	typedef std::multimap<sint , CDXUTControl*> ControlSet;
	ControlSet		ctrls;
	LocationKeySet	keys;
	for (int i = 0 ; i < pDlg->m_Controls.GetSize() ; ++i)
	{		
		CDXUTControl* pCtrl = pDlg->m_Controls.GetAt(i);
		int x,y;
		pCtrl->GetLocation(x,y);
		if(pCtrl->GetVisible())
		{
			ctrls.insert( make_pair(y,pCtrl) ) ;
			keys.insert(y);
		}
	}

	_FOREACH(LocationKeySet, keys , it)
	{
		sint pos = *it;
		ControlSet::iterator itcur	= ctrls.find(pos);
		ControlSet::iterator itend	= ctrls.upper_bound(pos);
		sint nBottom = tmp.bottom;
		for ( ; itcur != itend; ++itcur)
		{
			CDXUTControl* pCtrl = itcur->second;
			RECT rc = pCtrl->GetBoundingBox();
			RECT out ;		
			if( IntersectRect(&out,&tmp,&rc ) )
			{
				// 有交集
				pCtrl->SetLocation(rc.left , tmp.bottom) ; 			
				rc = pCtrl->GetBoundingBox();
			}
			if(rc.bottom > nBottom)
				nBottom = rc.bottom;
		}	
		tmp.bottom = nBottom;
	}
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CDXUTDialog::AdjLayout()
{
	
	for (int i = 0 ; i < m_Controls.GetSize() ; ++i)
	{		
		CDXUTPanel* pCtrl = (CDXUTPanel*)m_Controls.GetAt(i);
		
		if(pCtrl && pCtrl->GetType() == DXUT_CONTROL_PANEL && pCtrl->IsRoot())
		{	
			int w=0,h=0;		
			pCtrl->GetSize(w,h);

			CDXUTControl::Node* pNode = pCtrl->GetParentNode();
			CDXUTControl::Node::ChildIterator itcur = pNode->GetChildsIteratorBegin();
			CDXUTControl::Node::ChildIterator itend = pNode->GetChildsIteratorEnd();

			RECT rcPanel;	
			SetRectEmpty(&rcPanel);
// 			while(itcur != itend)
// 			{
// 				CDXUTPanel* pCtrl = (CDXUTPanel*) (*itcur)->GetObject();
// 				++itcur;
// 				if(pCtrl->GetType() == DXUT_CONTROL_PANEL)
// 				{
// 					rcPanel = pCtrl->GetStretchRect();			
// 					
// 					break;
// 				}				
// 			}

			typedef std::multimap<uint,CDXUTControl*,std::greater<uint>> PanelSet;
			PanelSet ctrls;
			for ( ;itcur != itend ; ++itcur)
			{
				CDXUTPanel* pCtrl = (CDXUTPanel*) (*itcur)->GetObject();
				if(pCtrl->GetType() == DXUT_CONTROL_PANEL)
				{										
					ctrls.insert( make_pair(pCtrl->GetAlign() , pCtrl) ) ; 
// 					const RECT& rcCtrl = pCtrl->GetStretchRect();			
// 					if(rcCtrl.left < rcPanel.left)
// 						rcPanel.left = rcCtrl.left;
// 					if(rcCtrl.top < rcPanel.top)
// 						rcPanel.top = rcCtrl.top;
// 					if(rcCtrl.right > rcPanel.right)
// 						rcPanel.right = rcCtrl.right;
// 					if(rcCtrl.bottom > rcPanel.bottom)
// 						rcPanel.bottom = rcCtrl.bottom;			
				}
			}

			PanelSet::iterator it = ctrls.begin();
			if(it != ctrls.end())
			{				
				CDXUTPanel* pCtrl = (CDXUTPanel*)it->second;
				rcPanel = pCtrl->GetStretchRect();			
				++it;
				for ( ; it != ctrls.end() ;++it )
				{
					CDXUTPanel* pCtrl = (CDXUTPanel*)it->second;
					if(pCtrl->GetAlign() == CDXUTControl::ALIGN_TOP 
						|| pCtrl->GetAlign() == CDXUTControl::ALIGN_BOTTOM
						|| pCtrl->GetAlign() == CDXUTControl::ALIGN_CLIENT)
					{
						// +高
						RECT rcCtrl = pCtrl->GetStretchRect();
						rcPanel.bottom += RectHeight(rcCtrl);
					}					

				}
			}

			
			int diffw=0,diffh=0;		
			pCtrl->SetSize(RectWidth(rcPanel),RectHeight(rcPanel));			
			pCtrl->UpdatePanel();
			diffw = w;
			diffh = h;
			pCtrl->GetSize(w,h);
			diffw = w - diffw;
			diffh = h - diffh;
			this->SetSize(GetWidth() + diffw , GetHeight() + diffh);
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	
	if(m_bCaption)
	{		
		this->SetSize(GetWidth() , GetHeight() + GetCaptionHeight());
	}
}
/*

class DragDropManager
{
DragDropable*	mpDD;
public:
void	Set(DragDropable* pDD)
{
if(mpDD)
{
mpDD->OnDrop();
}
mpDD = pDD;
}
};


class DragDropable
{
public:
typedef uint Type;
private:	
Type					mType;
Data					mData;
static DragDropManager*	mpMgr;

public:
DragDropable	(Type t);
virtual			~DragDropable	();

void			OnDrag			(const Data& d)
{
mData = d;
mpMgr->Set(this):
}
void			OnDrop			()
{

}
virtual void	Render			(float f) = 0;
};

class UIDragDropable : public DragDropable
{
CDXUTDialog*	mpParent;
public:	
UIDragDropable			(CDXUTDialog*);
~UIDragDropable			();
void			Enable					(bool bEnable):

virtual bool	MsgProc		( UINT uMsg, WPARAM wParam, LPARAM lParam ) ;

};

control : pblic DragDropable
*/


SHookHandleData::SHookHandleData()
{

}
SHookHandleData::~SHookHandleData()
{

}

SHookHandleData::SHookHandleData(DialogHandle ownHnd, DialogHandle hookHnd) 
: mOwnHnd(ownHnd), mHookHnd(hookHnd)
{
}

bool SHookHandleData::operator == (const SHookHandleData& HookData) const
{
	if( mOwnHnd == HookData.mOwnHnd && mHookHnd == HookData.mHookHnd )
	{
		return  true;
	}
	return false;
}





FindFuntor::FindFuntor(DialogHandle hnd) : mHnd(hnd) 
{

}

bool FindFuntor::operator()(SHookHandleData& test) const 
{
	if( mHnd == test.mOwnHnd )
	{
		CDXUTDialog* pDlg = CDXUTDialog::Query(test.mHookHnd);
		if( pDlg ) 
		{
			pDlg->SetAllControlEnable(true);
			pDlg->SendEvent(EVENT_DIALOG_UNHOOK,true , 0,0 );
		}
		
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//hook為要鎖定的對象
//////////////////////////////////////////////////////////////////////////
HandleHookManager::HandleHookManager()
{
}

HandleHookManager::~HandleHookManager()
{
}

CDXUTDialog* fnGetRootDialogHandle(CDXUTDialog* pDlg)
{
	if(!pDlg)
	{
		return 0;
	}

	CDXUTSubDialog* pCtrl = pDlg->GetParent();	
	CDXUTDialog* pParentDlg = 0;

	while(pCtrl)
	{		
		pParentDlg = pCtrl->GetParent();

		if( pParentDlg )
		{
			pCtrl = pParentDlg->GetParent();
		}
	}
	return pParentDlg;
}

void HandleHookManager::SetHookFromRoot(DialogHandle self, DialogHandle hnd)
{
	CDXUTDialog* pDlg = CDXUTDialog::Query(hnd);
	pDlg = fnGetRootDialogHandle(pDlg);
	SetHook( self, pDlg->GetHandle() );
}

void HandleHookManager::SetHook(DialogHandle self, DialogHandle hook)
{
	SHookHandleData dialoghook(self, hook);
	
	//鎖定所有ctrl
	CDXUTDialog* pDlg = CDXUTDialog::Query(hook);
	if(pDlg)
	{
		pDlg->SetAllControlEnable(false);
		pDlg->SendEvent(EVENT_DIALOG_BE_HOOK,true , 0,0 );
	}

	m_DialogHookList.push_back(dialoghook);
}

//////////////////////////////////////////////////////////////////////////
// wm_mouse_down & wm_mouse_up
// 找出監聽self的dlg hnd，點到時把dlg拉上來
//////////////////////////////////////////////////////////////////////////
DialogHandle HandleHookManager::CheckHook(DialogHandle self)
{
	//DialogHandle hnd;
	std::list<SHookHandleData>::reverse_iterator reIter;
	for( reIter = m_DialogHookList.rbegin(); reIter != m_DialogHookList.rend(); ++reIter )
	{
		SHookHandleData* pHandData = &*reIter;
		//如果self是被鎖定者
		uint64 ttt = pHandData->mHookHnd->GetKey() ;

		CDXUTDialog* pDlg = fnGetRootDialogHandle(CDXUTDialog::Query(self));
		if(pDlg)
		{
			if( pHandData->mHookHnd == pDlg->GetHandle() )
			{
				return pHandData->mOwnHnd;
			}
		}
		else
		{
			if( pHandData->mHookHnd == self )
			{
				return pHandData->mOwnHnd;
			}
		}
	}

// 	CDXUTDialog* pDlg = CDXUTDialog::Query(hnd);
// 	if(pDlg)
// 	{
// 		return hnd;
// 	}
	return DialogHandle();
}

void HandleHookManager::ClearOneHook(const SHookHandleData& hook)
{
	m_DialogHookList.remove(hook);
}

void HandleHookManager::ClearSpecifyHook(DialogHandle hnd)
{
	m_DialogHookList.remove_if(FindFuntor(hnd));
}


//取得自己的鎖定對象
// 	CDXUTDialog* GetOwnHook(DialogHandle hnd)
// 	{
// 		DialogHookList::iterator it;
// 
// 		it = find_if( m_DialogHookList.begin(), m_DialogHookList.end(), FindFuntor(hnd) );
// 	}

bool fnIsSubDialogType(uint nType)
{
	return nType >= DXUT_CONTROL_SUBDIALOG_BEGIN && nType <= DXUT_CONTROL_SUBDIALOG_END;
}
