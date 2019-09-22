#include "stdafx.h"
#pragma hdrstop
//--------------------------------------------------------------------------------------
#include "FDOUT.h"
#include "PhenixD3D9RenderSystem.h" 
#include "Timer.h"
//#include "FDO_RoleManager.h"
//#include "UIManager.h"
//#include "HotKey.h"
#include "Hookers.h"
//#include "Global.h"
//#include "MyGame.h"
#include "Debug.h"

//--------------------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define MIN_WINDOW_SIZE_X 200
#define MIN_WINDOW_SIZE_Y 200
//--------------------------------------------------------------------------------------

namespace FDO
{
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
LRESULT CALLBACK DXUTStaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

//--------------------------------------------------------------------------------------
// Thread safety 
//--------------------------------------------------------------------------------------
CRITICAL_SECTION g_cs;  
bool g_bThreadSafe = true;

//--------------------------------------------------------------------------------------
// Automatically enters & leaves the CS upon object creation/deletion
//--------------------------------------------------------------------------------------
class DXUTLock
{
public:
    inline DXUTLock()  { if( g_bThreadSafe ) EnterCriticalSection( &g_cs ); }
    inline ~DXUTLock() { if( g_bThreadSafe ) LeaveCriticalSection( &g_cs ); }
};

//--------------------------------------------------------------------------------------
// Stores DXUT state and data access is done with thread safety (if g_bThreadSafe==true)
//--------------------------------------------------------------------------------------
class DXUTState
{
protected:
    struct STATE
    {
        HWND                                m_HWNDFocus;                // the main app focus window
        bool                                m_WindowCreated;            // if true, then DXUTCreateWindow() or DXUTSetWindow() has succeeded
        bool                                m_NotifyOnMouseMove;        // if true, include WM_MOUSEMOVE in mousecallback

        LPDXUTCALLBACKFRAMEMOVE             m_FrameMoveFunc;            // frame move callback
        LPDXUTCALLBACKFRAMERENDER           m_FrameRenderFunc;          // frame render callback
        LPDXUTCALLBACKKEYBOARD              m_KeyboardFunc;             // keyboard callback
        LPDXUTCALLBACKMOUSE                 m_MouseFunc;                // mouse callback
        LPDXUTCALLBACKMSGPROC               m_WindowMsgFunc;            // window messages callback

        bool                                m_Keys[256];                // array of key state
        bool                                m_MouseButtons[5];          // array of mouse states
    };
    
    STATE m_state;

public:
    DXUTState()  { Create(); }
    ~DXUTState() { Destroy(); }

    void Create()
    {
        ZeroMemory( &m_state, sizeof(STATE) );
        g_bThreadSafe = true;
        InitializeCriticalSection( &g_cs );
    }

    void Destroy()
    {
        DeleteCriticalSection( &g_cs );

        if( m_state.m_HWNDFocus )
            UnregisterHotKey( m_state.m_HWNDFocus, 5 );
    }

    void SetHWNDFocus( HWND t )  { DXUTLock l; m_state.m_HWNDFocus = t; }
    HWND GetHWNDFocus() { DXUTLock l; return m_state.m_HWNDFocus; }

    void SetNotifyOnMouseMove( bool t )  { DXUTLock l; m_state.m_NotifyOnMouseMove = t; }
    bool GetNotifyOnMouseMove() { DXUTLock l; return m_state.m_NotifyOnMouseMove; }

    void SetFrameMoveFunc( LPDXUTCALLBACKFRAMEMOVE t )  { DXUTLock l; m_state.m_FrameMoveFunc = t; }
	LPDXUTCALLBACKFRAMEMOVE GetFrameMoveFunc() { DXUTLock l; return m_state.m_FrameMoveFunc; }

    void SetFrameRenderFunc( LPDXUTCALLBACKFRAMERENDER t )  { DXUTLock l; m_state.m_FrameRenderFunc = t; }
	LPDXUTCALLBACKFRAMERENDER GetFrameRenderFunc() { DXUTLock l; return m_state.m_FrameRenderFunc; }

    void SetKeyboardFunc( LPDXUTCALLBACKKEYBOARD t )  { DXUTLock l; m_state.m_KeyboardFunc = t; }
	LPDXUTCALLBACKKEYBOARD GetKeyboardFunc() { DXUTLock l; return m_state.m_KeyboardFunc; }

    void SetMouseFunc( LPDXUTCALLBACKMOUSE t )  { DXUTLock l; m_state.m_MouseFunc = t; }
	LPDXUTCALLBACKMOUSE GetMouseFunc() { DXUTLock l; return m_state.m_MouseFunc; }

    void SetWindowMsgFunc( LPDXUTCALLBACKMSGPROC t )  { DXUTLock l; m_state.m_WindowMsgFunc = t; }
	LPDXUTCALLBACKMSGPROC GetWindowMsgFunc() { DXUTLock l; return m_state.m_WindowMsgFunc; }

//    void SetKeys( bool* t )  { DXUTLock l; m_state.m_Keys = t; }
	bool* GetKeys() { DXUTLock l; return m_state.m_Keys; }

//    void SetMouseButtons( bool* t )  { DXUTLock l; m_state.m_MouseButtons = t; }
	bool* GetMouseButtons() { DXUTLock l; return m_state.m_MouseButtons; }

};
//--------------------------------------------------------------------------------------
// Global state class
//--------------------------------------------------------------------------------------
DXUTState& GetDXUTState()
{
    // Using an accessor function gives control of the construction order
    static DXUTState state;
    return state;
}
//--------------------------------------------------------------------------------------
// Internal functions forward declarations
//--------------------------------------------------------------------------------------
int  DXUTMapButtonToArrayIndex( BYTE vButton );

//--------------------------------------------------------------------------------------
// External callback setup functions
//--------------------------------------------------------------------------------------
void DXUTSetCallbackFrameMove( LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove )             { GetDXUTState().SetFrameMoveFunc( pCallbackFrameMove ); }
void DXUTSetCallbackFrameRender( LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender )       { GetDXUTState().SetFrameRenderFunc( pCallbackFrameRender ); }

void DXUTSetCallbackKeyboard( LPDXUTCALLBACKKEYBOARD pCallbackKeyboard )                { GetDXUTState().SetKeyboardFunc( pCallbackKeyboard ); }
void DXUTSetCallbackMouse( LPDXUTCALLBACKMOUSE pCallbackMouse, bool bIncludeMouseMove ) { GetDXUTState().SetMouseFunc( pCallbackMouse ); GetDXUTState().SetNotifyOnMouseMove( bIncludeMouseMove ); }
void DXUTSetCallbackMsgProc( LPDXUTCALLBACKMSGPROC pCallbackMsgProc )                   { GetDXUTState().SetWindowMsgFunc( pCallbackMsgProc ); }

//---------------------------------------------------------------------------
// Sets a previously created window for the framework to use.  If DXUTInit()
// has not already been called, it will call it with the default parameters.
// Instead of calling this, you can call DXUTCreateWindow() to create a new window.
//---------------------------------------------------------------------------
static HWND g_hwndGame = NULL;
HWND GetHwndGame()
{
    return g_hwndGame;
}

void SetHwndGame(HWND hwnd)
{
    g_hwndGame = hwnd;
}

static WNDPROC g_TAppOldWndProc = NULL;
static HWND g_hWndForm = NULL;

static void ShowGameForm()
{
    if (!IsWindow(g_hWndForm)) return;
    if (IsWindowVisible(g_hWndForm)) return;
    int iWidth = g_RenderSystem.GetBackBufferWidth();
    int iHeight = g_RenderSystem.GetBackBufferHeight();
    SetWindowPos(g_hWndForm, HWND_TOPMOST, 0, 0, iWidth, iHeight, SWP_SHOWWINDOW);
    ShowWindow(g_hWndForm, SW_RESTORE);
}

static void HideGameForm()
{
    if (!IsWindow(g_hWndForm)) return;
    if (!IsWindowVisible(g_hWndForm)) return;
    SetWindowPos(g_hWndForm, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDEWINDOW);
}
/*
static void OnTAppActivateApp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (g_RenderSystemPtr) {
        if (!g_RenderSystemPtr->GetIsFullScreen())
            return;
    } // end if

    BOOL bActivate =(BOOL)wParam;
    if (hWnd == Application->Handle) {
        if (!bActivate) {
            HideGameForm();
        } // end if
    } // end if
}

static void OnTAppActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (!g_RenderSystemPtr)
        return;
    if (false == g_RenderSystem.GetIsFullScreen())
        return;
    if (NULL == g_RenderSystem.GetD3DDevice())
        return;

    WORD fActivate = LOWORD(wParam);
    
    switch (fActivate) {
        case WA_ACTIVE:
        {
            HWND hWndDeactivated = (HWND)lParam;
            if (hWndDeactivated == g_hWndForm) {
                HideGameForm();
            } else {
                ShowGameForm();
            } // end else
            break;
        }
        case WA_INACTIVE:
        {
            HWND hWndActivated = (HWND)lParam;
            if (hWndActivated == g_hWndForm) {
                HideGameForm();
            } else {
                HideGameForm();
            } // end else
            break;
        }
    } // end switch
}

LRESULT CALLBACK TAppWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    assert(g_TAppOldWndProc);
    LRESULT lResult = 0;
    switch (uMsg) {
        case WM_ACTIVATE:
            OnTAppActivate(hWnd, wParam, lParam);
            break;
        case WM_ACTIVATEAPP:
		   // CallWindowProc((FARPROC)g_TAppOldWndProc, hWnd, uMsg, wParam, lParam);
            OnTAppActivateApp(hWnd, wParam, lParam);
            return 0;
        default:
			return  CallWindowProc((FARPROC)g_TAppOldWndProc, hWnd, uMsg, wParam, lParam);
    } // end switch
    return lResult;
}
*/

void DXUTLaunch() // coresystem Launch
{
	FDOXML_SystemInitial();
	if(GetConfig("DebugGUI") == "Yes")
	{
		CDXUTDialog::EnableDebugInfo(true);
	}
}
LONG g_oldWndProc = NULL;
//////////////////////////////////////////////////////////////////////////
bool DXUTSetWindow( HWND hWndForm )
{
    if( hWndForm == NULL )
        return false;
        
    // Switch window procedures
	g_oldWndProc = ::GetWindowLongPtr( hWndForm, GWLP_WNDPROC);
    LONG_PTR nResult = SetWindowLongPtr( hWndForm, GWLP_WNDPROC, (LONG)(LONG_PTR)DXUTStaticWndProc );
    if( nResult == 0 )
        return false;
        
    GetDXUTState().SetHWNDFocus( hWndForm );
    SetHwndGame(hWndForm);

    // Hook TApplication's WndProc.
   // g_hWndForm = hWndForm;
   // g_TAppOldWndProc = (WNDPROC)GetWindowLongPtr(Application->Handle, GWLP_WNDPROC);
  //  SetWindowLongPtr(Application->Handle, GWLP_WNDPROC, (LONG)TAppWndProc);

    return true;
}

void DXUTShutdown()
{
	FDOXML_SystemFinalize();
	::SetWindowLongPtr(GetHwndGame(), GWLP_WNDPROC, g_oldWndProc);
}

//<gino>
void DXUTRegisterHotKey(int id, UINT fsModifiers, UINT vk)
{
	RegisterHotKey(GetHwndGame(), id, fsModifiers, vk);
}
//</gino>

//---------------------------------------------------------------------------
bool DXUTIsKeyDown( BYTE vKey )
{
    bool* bKeys = GetDXUTState().GetKeys(); 
    if( vKey >= 0xA0 && vKey <= 0xA5 )  // VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_LMENU, VK_RMENU
        return GetAsyncKeyState( vKey ) != 0; // these keys only are tracked via GetAsyncKeyState()
    else if( vKey >= 0x01 && vKey <= 0x06 && vKey != 0x03 ) // mouse buttons (VK_*BUTTON)
        return DXUTIsMouseButtonDown(vKey);
    else
        return bKeys[vKey];
}
//---------------------------------------------------------------------------
bool DXUTIsMouseButtonDown( BYTE vButton )          
{ 
    bool* bMouseButtons = GetDXUTState().GetMouseButtons(); 
    int nIndex = DXUTMapButtonToArrayIndex(vButton); 
    return bMouseButtons[nIndex]; 
}
//---------------------------------------------------------------------------
// Internal function to map MK_* to an array index
//---------------------------------------------------------------------------
int DXUTMapButtonToArrayIndex( BYTE vButton )
{
    switch( vButton )
    {
        case MK_LBUTTON: return 0;
        case VK_MBUTTON:
        case MK_MBUTTON: return 1;
        case MK_RBUTTON: return 2;
        case VK_XBUTTON1:
        case MK_XBUTTON1: return 3;
        case VK_XBUTTON2:
        case MK_XBUTTON2: return 4;
    }

    return 0;
}
//--------------------------------------------------------------------------------------
// Gives the D3D device a cursor with image and hotspot from hCursor.
//--------------------------------------------------------------------------------------
HRESULT DXUTSetDeviceCursor( IDirect3DDevice9* pd3dDevice, HCURSOR hCursor, bool bAddWatermark )
{
    HRESULT hr = E_FAIL;
    ICONINFO iconinfo;
    bool bBWCursor;
    LPDIRECT3DSURFACE9 pCursorSurface = NULL;
    HDC hdcColor = NULL;
    HDC hdcMask = NULL;
    HDC hdcScreen = NULL;
    BITMAP bm;
    DWORD dwWidth;
    DWORD dwHeightSrc;
    DWORD dwHeightDest;
    COLORREF crColor;
    COLORREF crMask;
    UINT x;
    UINT y;
    BITMAPINFO bmi;
    COLORREF* pcrArrayColor = NULL;
    COLORREF* pcrArrayMask = NULL;
    DWORD* pBitmap;
    HGDIOBJ hgdiobjOld;

    ZeroMemory( &iconinfo, sizeof(iconinfo) );
    if( !GetIconInfo( hCursor, &iconinfo ) )
        goto End;

    if (0 == GetObject((HGDIOBJ)iconinfo.hbmMask, sizeof(BITMAP), (LPVOID)&bm))
        goto End;
    dwWidth = bm.bmWidth;
    dwHeightSrc = bm.bmHeight;

    if( iconinfo.hbmColor == NULL )
    {
        bBWCursor = TRUE;
        dwHeightDest = dwHeightSrc / 2;
    }
    else 
    {
        bBWCursor = FALSE;
        dwHeightDest = dwHeightSrc;
    }

    // Create a surface for the fullscreen cursor
    if( FAILED( hr = pd3dDevice->CreateOffscreenPlainSurface( dwWidth, dwHeightDest, 
        D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pCursorSurface, NULL ) ) )
    {
        goto End;
    }

    pcrArrayMask = new DWORD[dwWidth * dwHeightSrc];

    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = dwWidth;
    bmi.bmiHeader.biHeight = dwHeightSrc;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hdcScreen = GetDC( NULL );
    hdcMask = CreateCompatibleDC( hdcScreen );
    if( hdcMask == NULL )
    {
        hr = E_FAIL;
        goto End;
    }
    hgdiobjOld = SelectObject(hdcMask, iconinfo.hbmMask);
    GetDIBits(hdcMask, iconinfo.hbmMask, 0, dwHeightSrc, 
        pcrArrayMask, &bmi, DIB_RGB_COLORS);
    SelectObject(hdcMask, hgdiobjOld);

    if (!bBWCursor)
    {
        pcrArrayColor = new DWORD[dwWidth * dwHeightDest];
        hdcColor = CreateCompatibleDC( hdcScreen );
        if( hdcColor == NULL )
        {
            hr = E_FAIL;
            goto End;
        }
        SelectObject(hdcColor, iconinfo.hbmColor);
        GetDIBits(hdcColor, iconinfo.hbmColor, 0, dwHeightDest, 
            pcrArrayColor, &bmi, DIB_RGB_COLORS);
    }

    // Transfer cursor image into the surface
    D3DLOCKED_RECT lr;
    pCursorSurface->LockRect( &lr, NULL, 0 );
    pBitmap = (DWORD*)lr.pBits;
    for( y = 0; y < dwHeightDest; y++ )
    {
        for( x = 0; x < dwWidth; x++ )
        {
            if (bBWCursor)
            {
                crColor = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
                crMask = pcrArrayMask[dwWidth*(dwHeightSrc-1-y) + x];
            }
            else
            {
                crColor = pcrArrayColor[dwWidth*(dwHeightDest-1-y) + x];
                crMask = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
            }
            if (crMask == 0)
                pBitmap[dwWidth*y + x] = 0xff000000 | crColor;
            else
                pBitmap[dwWidth*y + x] = 0x00000000;

            // It may be helpful to make the D3D cursor look slightly 
            // different from the Windows cursor so you can distinguish 
            // between the two when developing/testing code.  When
            // bAddWatermark is TRUE, the following code adds some
            // small grey "D3D" characters to the upper-left corner of
            // the D3D cursor image.
            if( bAddWatermark && x < 12 && y < 5 )
            {
                // 11.. 11.. 11.. .... CCC0
                // 1.1. ..1. 1.1. .... A2A0
                // 1.1. .1.. 1.1. .... A4A0
                // 1.1. ..1. 1.1. .... A2A0
                // 11.. 11.. 11.. .... CCC0

                const WORD wMask[5] = { 0xccc0, 0xa2a0, 0xa4a0, 0xa2a0, 0xccc0 };
                if( wMask[y] & (1 << (15 - x)) )
                {
                    pBitmap[dwWidth*y + x] |= 0xff808080;
                }
            }
        }
    }
    pCursorSurface->UnlockRect();

    // Set the device cursor
    if( FAILED( hr = pd3dDevice->SetCursorProperties( iconinfo.xHotspot, 
        iconinfo.yHotspot, pCursorSurface ) ) )
    {
        goto End;
    }

    hr = S_OK;

End:
    if( iconinfo.hbmMask != NULL )
        DeleteObject( iconinfo.hbmMask );
    if( iconinfo.hbmColor != NULL )
        DeleteObject( iconinfo.hbmColor );
    if( hdcScreen != NULL )
        ReleaseDC( NULL, hdcScreen );
    if( hdcColor != NULL )
        DeleteDC( hdcColor );
    if( hdcMask != NULL )
        DeleteDC( hdcMask );
    SAFE_DELETE_ARRAY( pcrArrayColor );
    SAFE_DELETE_ARRAY( pcrArrayMask );
    SAFE_RELEASE( pCursorSurface );
    return hr;
}
//--------------------------------------------------------------------------------------
// Handles window messages
//--------------------------------------------------------------------------------------

static BOOL DXUTKeyboardProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bAltDown)
// Returns bContinueProcessing.
// 鍵盤訊息處理的第一層：
// (1) 攔截系統鍵，使不要發出警示音。
// (2) 將一般鍵盤訊息與 IME 鍵盤訊息轉譯成 DXUTState::KeyboardFunc() 的參數列格式，並呼叫該函式。
{
    bool bKeyDown = false, bCtrlDown = false, bShiftDown = false;
    bAltDown = false;
    bool bKeyDownRepeated = false;
    
    switch (uMsg) 
    {
        //=========================================================
        // 系統字元鍵。注意不要發出警示音。
        //=========================================================
        case WM_SYSCHAR:
        {
            // Test if Alt is down (29th bit of lParam).
            if (0x20000000 & lParam) {
                bKeyDown = true;
                bAltDown = true;
                // 依據 Caps Lock 的狀態更改字母鍵為大寫﹝virtual-key codes 只支援大寫﹞。
                if (0 == (GetKeyState(VK_CAPITAL) & 0x0001)) {
                    // Caps Lock is untoggled (turned off).
                    if (wParam >= 'a' && wParam <= 'z')
                        wParam = wParam - 'a' + 'A';
                } // end if
                break;
            } else return FALSE;
        }
//        case WM_SYSDEADCHAR:
//            return FALSE;

        //=========================================================
        // 系統鍵。
        //=========================================================
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            // Alt key
            if (VK_MENU == wParam)
                return FALSE;
			if (VK_F4 == wParam)
				return FALSE;
            break;
        }
        //=========================================================
        // Enter 鍵：切換至聊天視窗。
        //=========================================================
        case WM_CHAR:
        {
//            if (VK_RETURN == wParam) {
//                if (g_RoleManager.GetIsGameStart()) {
//                    if (!g_HUITalkBar.HasFocus())
//                        g_HUITalkBar.RequestFocus();
//                }
//            }
			 bKeyDown = true;
            break;
        }
        //=========================================================
        // 一般按鍵。
        //=========================================================
        case WM_KEYDOWN:
        {
            // 偵測是否該鍵被按下不放。(bit 30 of lParam = 0x40000000)
            if (lParam & 0x40000000)
                bKeyDownRepeated = true;
            // ScrollLock 鍵用來切換 UI 的顯示與否。
           /* if (VK_SCROLL == wParam) {
                if (RoleManager::GetIsGameStart())
                    g_UIManager.ToggleVisibility();
                return FALSE; // no further processing
            }*/
            bKeyDown = true;
			break;
        }
        case WM_KEYUP:
        {
            // 處理 IME 鍵盤訊息：將功能鍵與 Ctrl、Shift 組合鍵取出。
            if (VK_PROCESSKEY == wParam) {
                UINT nScanCode = (0x00FF0000 & lParam) >> 16;
                wParam = MapVirtualKey(nScanCode, 1);
            }
		    
            break;
        }
    } // end switch

	// 判斷 Ctrl 是否被按下
	if (GetKeyState(VK_CONTROL) & 0x8000) 
		bCtrlDown = true;
	// 判斷 Shift 是否被按下
	if (GetKeyState(VK_SHIFT) & 0x8000)
		bShiftDown = true;	

    bool* bKeys = GetDXUTState().GetKeys();
    bKeys[ (BYTE) (wParam & 0xFF) ] = bKeyDown;

    // Keyboard hooking.
    if (g_KeyboardHooker.Hook(wParam, bKeyDown, bAltDown, bCtrlDown, bShiftDown, bKeyDownRepeated))
        return FALSE;

    // 呼叫 DXUTState 的 KeyboardProc() => [MyGame.cpp\GamingKeyboardProc()]
    LPDXUTCALLBACKKEYBOARD pCallbackKeyboard = GetDXUTState().GetKeyboardFunc();
    if (pCallbackKeyboard)
	    pCallbackKeyboard((UINT)wParam, bKeyDown, bAltDown, bCtrlDown, bShiftDown, bKeyDownRepeated);
	    
	return TRUE;
}

static BOOL DXUTMouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// Returns bContinueProcessing.
// 滑鼠訊息處理的第一層：
// (1) 將滑鼠訊息轉譯成 DXUTState::MouseFunc() 的參數列格式，並呼叫該函式。
{
//	GetHWNDFocus
	{
		POINT ptScreen;
		::GetCursorPos(&ptScreen);
		RECT rcClient;
		::GetWindowRect(GetDXUTState().GetHWNDFocus(),&rcClient);

		if( !( (rcClient.left <= ptScreen.x &&  rcClient.right >= ptScreen.x)
			&& (rcClient.top <= ptScreen.y &&  rcClient.bottom >= ptScreen.y)))
			return TRUE;
	}

	int xPos = (short)LOWORD(lParam);
	int yPos = (short)HIWORD(lParam);

    if (WM_MOUSEMOVE == uMsg) 
	{

        if (false == GetDXUTState().GetNotifyOnMouseMove())
            return TRUE;
		
		

		
    } // end if

	int nMouseWheelDelta = 0;
	if( uMsg == WM_MOUSEWHEEL )
	{
		// WM_MOUSEWHEEL passes screen mouse coords
		// so convert them to client coords
		POINT pt;
		pt.x = xPos; pt.y = yPos;
		ScreenToClient( hWnd, &pt );
		xPos = pt.x; yPos = pt.y;
		nMouseWheelDelta = (short)HIWORD(wParam);
	}

	int nMouseButtonState = LOWORD(wParam);
	bool bLeftButton   = ((nMouseButtonState & MK_LBUTTON ) != 0);
	bool bRightButton  = ((nMouseButtonState & MK_RBUTTON ) != 0);
	bool bMiddleButton = ((nMouseButtonState & MK_MBUTTON ) != 0);
	bool bSideButton1  = ((nMouseButtonState & MK_XBUTTON1) != 0);
	bool bSideButton2  = ((nMouseButtonState & MK_XBUTTON2) != 0);
	bool bControl      = ((nMouseButtonState & MK_CONTROL ) != 0);
	bool bShift        = ((nMouseButtonState & MK_SHIFT   ) != 0);

	bool* bMouseButtons = GetDXUTState().GetMouseButtons();
	bMouseButtons[0] = bLeftButton;
	bMouseButtons[1] = bMiddleButton;
	bMouseButtons[2] = bRightButton;
	bMouseButtons[3] = bSideButton1;
	bMouseButtons[4] = bSideButton2;

    // 呼叫 DXUTState 的 MouseProc().
	LPDXUTCALLBACKMOUSE pCallbackMouse = GetDXUTState().GetMouseFunc();
	if( pCallbackMouse )
	{
		pCallbackMouse(
			bLeftButton,
			bRightButton,
			bMiddleButton,
			bSideButton1,
			bSideButton2,
			nMouseWheelDelta,
			xPos,
			yPos,
			bControl,
			bShift );
	}
	
	return TRUE;
}

struct RecordMessage
{
	bool			mbEnable;
	std::wstring	mstr;
	uint64			mnCount;
	RecordMessage() : mnCount(0){}
	void Rec(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(mbEnable)
		{			
			StringFormat16(mstr , L"%I64d , hwnd %d,msg %d\n" , ++mnCount ,hWnd , uMsg);
			OutputDebugStringW(mstr.c_str());
		}
	}
};

RecordMessage gRecordMessage;

LRESULT CALLBACK DXUTStaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// 視窗訊息處理的第一層。
{
// 	if(WM_LBUTTONDOWN == uMsg)
// 		gRecordMessage.mbEnable = true;
// 
// 	if(WM_LBUTTONUP == uMsg)
// 		gRecordMessage.mbEnable = false;
// 
// 	if(WM_MOUSEMOVE != uMsg)
// 		gRecordMessage.Rec(hWnd , uMsg , wParam , lParam);
	if(uMsg == WM_WINDOWPOSCHANGING || uMsg  == WM_WINDOWPOSCHANGED)
	{
		WINDOWPOS* pWP = (WINDOWPOS*)lParam;
		char16 winText[256];
		::GetWindowTextW( pWP->hwndInsertAfter , winText , 256);
				
		return 0;
		
	}else if(uMsg == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hdc;				
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);		
	}else if(uMsg == WM_NCHITTEST)
	{
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
		LRESULT Result = DefWindowProc(hWnd,uMsg,wParam,lParam);
		if (Result != HTCAPTION && Result != HTCLOSE) 
		{
			Result = HTCLIENT;
		}
		return Result;		
	}else if( uMsg == WM_SETCURSOR )
	{
		// Turn off window cursor. 
		SetCursor( NULL ) ;
		g_RenderSystem.GetD3DDevice()->ShowCursor( TRUE ) ;
		LPDXUTCALLBACKMSGPROC pCallbackMsgProc = GetDXUTState().GetWindowMsgFunc();
		if( pCallbackMsgProc )
		{
			bool bNoFurtherProcessing = false;
			LRESULT nResult = pCallbackMsgProc( hWnd, uMsg, wParam, lParam, &bNoFurtherProcessing );
			if( bNoFurtherProcessing )
				return nResult;
				//return TRUE ; // Prevent Windows from setting cursor to window class cursor.
		}

		return TRUE ; // Prevent Windows from setting cursor to window class cursor.
	}

    // 鍵盤事件
    bool bAltDown = false;
    if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) { 
        if (FALSE == DXUTKeyboardProc(hWnd, uMsg, wParam, lParam, bAltDown))
            return 0;
    }    
    // 滑鼠事件
    else if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) {
        if (FALSE == DXUTMouseProc(hWnd, uMsg, wParam, lParam))
            return 0;
    }
    // Pass all messages to the app's MsgProc callback, and don't
    // process further messages if the apps says not to.
    LPDXUTCALLBACKMSGPROC pCallbackMsgProc = GetDXUTState().GetWindowMsgFunc();
    if( pCallbackMsgProc )
    {
        bool bNoFurtherProcessing = false;
        LRESULT nResult = pCallbackMsgProc( hWnd, uMsg, wParam, lParam, &bNoFurtherProcessing );
        if( bNoFurtherProcessing )
            return nResult;
    }

    // Post message-handling.
    switch( uMsg )
    {
        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = MIN_WINDOW_SIZE_X;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = MIN_WINDOW_SIZE_Y;
            break;

        // GM用
#if defined _FDO_GM
        case WM_SETCURSOR:
                SetCursor( NULL );
                return true; // prevent Windows from setting cursor to window class cursor
#endif
        /*
        case WM_SETCURSOR:
            //Turn off Windows cursor in full screen mode
            if( g_RenderSystem.GetIsFullScreen() )
            {
                SetCursor( NULL );
                g_RenderSystem.GetD3DDevice()->ShowCursor( true );
                return true; // prevent Windows from setting cursor to window class cursor
            }
            break;

        case WM_MOUSEMOVE:
            if( g_RenderSystem.GetIsFullScreen() )
            {
                POINT ptCursor;
                GetCursorPos( &ptCursor );
                g_RenderSystem.GetD3DDevice()->SetCursorPosition( ptCursor.x, ptCursor.y, 0 );
            }
            break;

        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_ESCAPE:
                {
                    //PostQuitMessage(0);
                    ExitGame(0);
                    return 0;

                    break;
                }
            } 
            break;
        }
        */
    }

    // 攔截系統鍵，使不要發出警示音。
	if (bAltDown)
	    return 0;
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
//--------------------------------------------------------------------------------------
bool Pick( const int& iX, const int& iY, const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2 )
{
    D3DXVECTOR3 vPickRayDir;
    D3DXVECTOR3 vPickRayOrig;
	ScreenPosTo3DPos(iX,iY,vPickRayDir,vPickRayOrig);
    return Pick(vPickRayDir,vPickRayOrig,v0,v1,v2);
}
//--------------------------------------------------------------------------------------
//void ScreenPosTo3DPos( const int& iX, const int& iY, D3DXVECTOR3& vPickRayDir,D3DXVECTOR3& vPickRayOrig)
void ScreenPosTo3DPos(int iMouseX, int iMouseY, D3DXVECTOR3& vPickRayDir,
                      D3DXVECTOR3& vPickRayOrig, const D3DXMATRIX* pMatWorld)
{
	// 取得投射矩陣
    D3DXMATRIX matTemp;
    g_RenderSystem.GetD3DDevice()->GetTransform( D3DTS_PROJECTION, &matTemp );

    //計算螢幕座標
    D3DXVECTOR3 vPos;
    vPos.x =  ( ( ( 2.0f * iMouseX ) / g_RenderSystem.GetClientWidth()  ) - 1 ) / matTemp._11;
    vPos.y = -( ( ( 2.0f * iMouseY ) / g_RenderSystem.GetClientHeight() ) - 1 ) / matTemp._22;
    vPos.z =  1.0f;

    // 取得反相視矩陣
    g_RenderSystem.GetD3DDevice()->GetTransform( D3DTS_VIEW, &matTemp );

    // <yuming add>
    if (pMatWorld != NULL)
    {
        matTemp = (*pMatWorld) * matTemp;
    }
    // </yuming add>

    D3DXMatrixInverse( &matTemp, NULL, &matTemp );    

	D3DXVECTOR3 orig(0, 0, 0) ; 
	D3DXVECTOR3 dir(vPos.x, vPos.y , 1.0f) ; 

    // 轉換螢幕座標至3D座標
	D3DXVec3TransformCoord(&vPickRayOrig , &orig , &matTemp) ; 
	D3DXVec3TransformNormal(&vPickRayDir , &dir , &matTemp) ; 
	D3DXVec3Normalize(&vPickRayDir , &vPickRayDir) ; 

    //vPickRayDir.x  = vPos.x*matTemp._11 + vPos.y*matTemp._21 + vPos.z*matTemp._31;
    //vPickRayDir.y  = vPos.x*matTemp._12 + vPos.y*matTemp._22 + vPos.z*matTemp._32;
    //vPickRayDir.z  = vPos.x*matTemp._13 + vPos.y*matTemp._23 + vPos.z*matTemp._33;
    //vPickRayOrig.x = matTemp._41;
    //vPickRayOrig.y = matTemp._42;
    //vPickRayOrig.z = matTemp._43;
}
//--------------------------------------------------------------------------------------
bool Pick( const D3DXVECTOR3& vPickRayDir, const D3DXVECTOR3& vPickRayOrig, const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2 )
{
	// Find vectors for two edges sharing vert0
    D3DXVECTOR3 vEdge1 = v1 - v0;
    D3DXVECTOR3 vEdge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 vPvec;
    D3DXVec3Cross( &vPvec, &vPickRayDir, &vEdge2 );

    // If determinant is near zero, ray lies in plane of triangle
    float fDet = D3DXVec3Dot( &vEdge1, &vPvec );
    
    // Calculate distance from vert0 to ray origin
    D3DXVECTOR3 vTvec;
    if( fDet > 0.0f )
    {
        vTvec = vPickRayOrig - v0; // 射線從正面穿越三角形
    }
    else
    {
        vTvec = v0 - vPickRayOrig; // 射線從反面穿越三角形
        fDet = -fDet;
    }

    if( fDet < 0.0001f )
        return false;

    // Calculate U parameter and test bounds
    float fPickU = D3DXVec3Dot( &vTvec, &vPvec );
    if( (fPickU < 0.0f) || (fPickU > fDet) )
        return false;

    // Prepare to test V parameter
    D3DXVECTOR3 vQvec;
    D3DXVec3Cross( &vQvec, &vTvec, &vEdge1 );

    // Calculate V parameter and test bounds
    float fPickV = D3DXVec3Dot( &vPickRayDir, &vQvec );
    if( (fPickV < 0.0f) || (fPickU + fPickV > fDet) )
        return false;
	
    return true;
}
//---------------------------------------------------------------------------
// Desc: Given the 8 corners of a OBB bounding box in world space,
// create a AABB bounding box in world space
// AABB為垂直xyz軸的BoundingBox
// OBB為模型在世界座標的BoundingBox
// 若OBB不作旋轉，則 AABB == OBB
//---------------------------------------------------------------------------
void CalcAABBFromOBB(const D3DXVECTOR3 *obb,D3DXVECTOR3 *minB,D3DXVECTOR3 *maxB)
{
    //assert(minB);
    //assert(maxB);
    //assert(obb);

    minB->x = maxB->x = obb[0].x;
    minB->y = maxB->y = obb[0].y;
    minB->z = maxB->z = obb[0].z;

    for( int i=1; i<8; ++i )
    {
        if (obb[i].x < minB->x) minB->x=obb[i].x;
        if (obb[i].x > maxB->x) maxB->x=obb[i].x;
        if (obb[i].y < minB->y) minB->y=obb[i].y;
        if (obb[i].y > maxB->y) maxB->y=obb[i].y;
        if (obb[i].z < minB->z) minB->z=obb[i].z;
        if (obb[i].z > maxB->z) maxB->z=obb[i].z;
    }
}

//--------------------------------------------------------------------------------------
// We have min and max values, use these to get the 8 corners of the bounding box
// 給最小及最大點，計算出BoundingBox的八個點
//
//  6 _____7
//   /____/
//  2     3
//
//  4 _____5
//   /____/
//  0     1
//--------------------------------------------------------------------------------------
void CalcBoundingBox(const D3DXVECTOR3 &minB, const D3DXVECTOR3 &maxB, D3DXVECTOR3 *obb )
{
    //assert(minB);
    //assert(maxB);
    //assert(obb);

    obb[0] = D3DXVECTOR3( minB.x, minB.y, minB.z ); // xyz
    obb[1] = D3DXVECTOR3( maxB.x, minB.y, minB.z ); // Xyz
    obb[2] = D3DXVECTOR3( minB.x, maxB.y, minB.z ); // xYz
    obb[3] = D3DXVECTOR3( maxB.x, maxB.y, minB.z ); // XYz
    obb[4] = D3DXVECTOR3( minB.x, minB.y, maxB.z ); // xyZ
    obb[5] = D3DXVECTOR3( maxB.x, minB.y, maxB.z ); // XyZ
    obb[6] = D3DXVECTOR3( minB.x, maxB.y, maxB.z ); // xYZ
    obb[7] = D3DXVECTOR3( maxB.x, maxB.y, maxB.z ); // XYZ
}

//--------------------------------------------------------------------------------------
HWND DXUTGetHWND()
{
    return GetDXUTState().GetHWNDFocus();
}

//--------------------------------------------------------------------------------------
float DXUTGetTime()
{
    return g_Timer.GetfLastTime();
}

//--------------------------------------------------------------------------------------
unsigned int HexToInt( char *pStr )
{
    unsigned int iValue = 0;

    while( *pStr )
    {
        if( ( *pStr >= '0' ) && ( *pStr <= '9' ) )
        {
            iValue <<= 4;
            iValue += *pStr - '0';
        }
        else if ( ( *pStr >= 'A' ) && ( *pStr <= 'F' ) )
        {
            iValue <<= 4;
            iValue += *pStr - 'A' + 10;
        }
        else
            break;

        ++pStr;
    }

    return iValue;
}   

//--------------------------------------------------------------------------------------
}   //namespace FDO

//--------------------------------------------------------------------------------------


