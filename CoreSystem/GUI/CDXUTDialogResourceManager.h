//
// CDXUTDialogResourceManager.h
//
#ifndef __CDXUTDialogResourceManager_h__
#define __CDXUTDialogResourceManager_h__

#include "CDXUTDef.h"
#include "GameDatabase.h"
//-----------------------------------------------------------------------------
// Manages shared resources of dialogs
//-----------------------------------------------------------------------------

class CDXUTDialogResourceManager
{
public:
    CDXUTDialogResourceManager();
    ~CDXUTDialogResourceManager();

    HRESULT     OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT     OnResetDevice( LPDIRECT3DDEVICE9 pd3dDevice ); // ming edit
    void        OnLostDevice();
    void        OnDestroyDevice();
    bool        MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    
    int			AddFont( LPCWSTR strFaceName, LONG height, LONG weight, BOOL bItalic=FALSE , bool bUnderline = false);
	ResoueceSystem::HANDLE	AddFontA( LPCSTR strFaceName, LONG height, LONG weight, BOOL bItalic=FALSE , bool bUnderline = false);
	ResoueceSystem::HANDLE	AddFontW( LPCWSTR strFaceName, LONG height, LONG weight, BOOL bItalic=FALSE , bool bUnderline = false);
    int			AddTexture( LPCWSTR strFilename );
    // ming add
    int			AddFont( const string &strFaceName, LONG height, LONG weight );
    int			AddTexture( const string &strFilename );
    // ming add

	int			AddTextureFromMemory( LPCWSTR strDiscernName ,void* pData,unsigned int DataSize,unsigned int nColorKey = 0);
	int			CreateEmptyTexture	( LPCWSTR strDiscernName , uint nWidth ,uint nHeight,uint nDxFmt = D3DFMT_UNKNOWN);

	int			QueryTexture		(LPCWSTR strDiscernName);
	int			QueryTexture		(LPCSTR strDiscernName);

	void		ReloadTexture		(UINT index , void* pData,unsigned int DataSize ,unsigned int nColorKey = 0);

	bool		ResetTexture		(int iTxr,uint nWidth ,uint nHeight);
	

    DXUTFontNode*     GetFontNode( int iIndex )     ;
	
    DXUTTextureNode*  GetTextureNode( int iIndex )  ;
    IDirect3DDevice9* GetD3DDevice()                { return m_pd3dDevice; }

	SIZE				GetStringSize	(ResoueceSystem::HANDLE hFont,const char16* text);
	SIZE				GetStringSize	(int iIndex,const char16* text);
	SIZE				GetChar16Size	(int iIndex,char16 nchar);
    // Shared between all dialogs
    IDirect3DStateBlock9* m_pStateBlock;
    ID3DXSprite*          m_pSprite;          // Sprite used for drawing

	void		DrawFontHandle		( ResoueceSystem::HANDLE hnd,const char16* text,const Rect32& rcDraw,uint nFormat,uint nColor,uint nStyle);
	void		DrawFont			( int iFont,const char16* text,const Rect32& rcDraw,uint nFormat,uint nColor,uint nStyle);

protected:
    CGrowableArray< DXUTTextureNode* > m_TextureCache;   // Shared textures
    CGrowableArray< DXUTFontNode* > m_FontCache;         // Shared fonts
    
    IDirect3DDevice9* m_pd3dDevice;

    // Resource creation helpers
    HRESULT CreateFont( UINT index );
    HRESULT CreateTexture( UINT index );
	HRESULT	CreateTextureFromMemory( UINT index , void* pData,unsigned int DataSize,unsigned int nColorKey = 0);	
};

//////////////////////////////////////////////////////////////////////////
bool CensoringString(std::wstring& out , int iFont,const char16* text,sint nLimitWidth,const char16* strCensoring = L"...");

#endif // __CDXUTDialogResourceManager_h__