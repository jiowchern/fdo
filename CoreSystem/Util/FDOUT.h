//--------------------------------------------------------------------------------------
#ifndef _FDOUT_H_
#define _FDOUT_H_
//--------------------------------------------------------------------------------------

#include <d3d9.h>
#include <d3dx9math.h>
//--------------------------------------------------------------------------------------

namespace FDO
{
//--------------------------------------------------------------------------------------
// Callback registration 
//--------------------------------------------------------------------------------------
typedef void    (CALLBACK *LPDXUTCALLBACKFRAMEMOVE)( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
typedef void    (CALLBACK *LPDXUTCALLBACKFRAMERENDER)( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
typedef void    (CALLBACK *LPDXUTCALLBACKKEYBOARD)( UINT nChar, bool bKeyDown, bool bAltDown, bool bCtrlDown, bool bShiftDown, bool bKeyDownRepeated);
typedef void    (CALLBACK *LPDXUTCALLBACKMOUSE)( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, bool bControl, bool bShift);
typedef LRESULT (CALLBACK *LPDXUTCALLBACKMSGPROC)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing );

// Frame callbacks
void DXUTSetCallbackFrameMove( LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove );
void DXUTSetCallbackFrameRender( LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender );

// Message callbacks
void DXUTSetCallbackKeyboard( LPDXUTCALLBACKKEYBOARD pCallbackKeyboard );
void DXUTSetCallbackMouse( LPDXUTCALLBACKMOUSE pCallbackMouse, bool bIncludeMouseMove = false );
void DXUTSetCallbackMsgProc( LPDXUTCALLBACKMSGPROC pCallbackMsgProc );

//--------------------------------------------------------------------------------------
// Initialization
//--------------------------------------------------------------------------------------
void    DXUTLaunch( );
bool    DXUTSetWindow( HWND hWnd );
void    DXUTShutdown(); ///<-- <gino>
bool    DXUTIsKeyDown( BYTE vKey ); // Pass a virtual-key code, ex. VK_F1, 'A', VK_RETURN, VK_LSHIFT, etc
bool    DXUTIsMouseButtonDown( BYTE vButton ); // Pass a virtual-key code: VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2
//<gino>
void    DXUTRegisterHotKey(int id, UINT fsModifiers, UINT vk);
//</gino>

//--------------------------------------------------------------------------------------
// State Retrieval  
//--------------------------------------------------------------------------------------
HWND    DXUTGetHWND();
float   DXUTGetTime();

//--------------------------------------------------------------------------------------
// Builds and sets a cursor for the D3D device based on hCursor.
//--------------------------------------------------------------------------------------
HRESULT DXUTSetDeviceCursor( IDirect3DDevice9* pd3dDevice, HCURSOR hCursor, bool bAddWatermark );


// 點擊偵測函式
bool    Pick				( const int& iX, const int& iY, const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2 );
//void	ScreenPosTo3DPos	( const int& iX, const int& iY, D3DXVECTOR3& vPickRayDir,D3DXVECTOR3& vPickRayOrig);
void	ScreenPosTo3DPos	(int iMouseX, int iMouseY, D3DXVECTOR3& vPickRayDir, D3DXVECTOR3& vPickRayOrig, const D3DXMATRIX* pMatWorld=NULL);
bool    Pick				( const D3DXVECTOR3& vPickRayDir, const D3DXVECTOR3& vPickRayOrig, const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2 );

void    CalcBoundingBox( const D3DXVECTOR3 &minB, const D3DXVECTOR3 &maxB, D3DXVECTOR3 *obb );
void    CalcAABBFromOBB( const D3DXVECTOR3 *obb,D3DXVECTOR3 *minB,D3DXVECTOR3 *maxB );

// 轉換16進位字串至整數
unsigned int HexToInt( char *pStr );

} // namespace FDO

//--------------------------------------------------------------------------------------
#endif




