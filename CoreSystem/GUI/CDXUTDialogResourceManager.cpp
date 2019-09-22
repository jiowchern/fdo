//
// CDXUTDialogResourceManager.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//--------------------------------------------------------------------------------------
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
#include "CDXUTIMEEditBox.h"
#include "dxerr9.h"
#include "CDXUTDialog.h"
using namespace FDO;

#pragma hdrstop
#include "CDXUTDialogResourceManager.h"
#include "win_fontsystem.h"

struct TDGFontNode : public DXUTFontNode
{
	FontHandle	hFont;
};
//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("管理模式啟動") 
#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------
// Static variables
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager* CDXUTDialog::s_pManager = NULL;
WinFontSystem*	gpWinFontSystem = 0;
//--------------------------------------------------------------------------------------

CDXUTDialogResourceManager::CDXUTDialogResourceManager()
{
	CDXUTDialog::SetManager(this);
    m_pd3dDevice = NULL;
    m_pStateBlock = NULL;
    m_pSprite = NULL;
}

//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager::~CDXUTDialogResourceManager()
{
	
    int i;
    for( i=0; i < m_FontCache.GetSize(); i++ )
    {
        DXUTFontNode* pFontNode = m_FontCache.GetAt( i );
		
        SAFE_DELETE( pFontNode );
    }
    m_FontCache.RemoveAll();   

    for( i=0; i < m_TextureCache.GetSize(); i++ )
    {
        DXUTTextureNode* pTextureNode = m_TextureCache.GetAt( i );
		
        SAFE_DELETE( pTextureNode );
    }
    m_TextureCache.RemoveAll();   

    CUniBuffer::Uninitialize();
    CDXUTIMEEditBox::Uninitialize();
	if(gpWinFontSystem != 0)
	{
		gpWinFontSystem->Release();
		delete gpWinFontSystem;
		gpWinFontSystem = 0;
	}
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialogResourceManager::OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr = S_OK;
    int i=0;

    m_pd3dDevice = pd3dDevice;
    
    for( i=0; i < m_FontCache.GetSize(); i++ )
    {
        hr = CreateFont( i );
        // ming mark
        //if( FAILED(hr) )
        //    return hr;
    }
    
    for( i=0; i < m_TextureCache.GetSize(); i++ )
    {
        hr = CreateTexture( i );
        // ming mark
        //if( FAILED(hr) )
        //    return hr;
    }

    hr = D3DXCreateSprite( pd3dDevice, &m_pSprite );
    if( FAILED(hr) )
        return DXUT_ERR( L"D3DXCreateSprite", hr );
	
	if(gpWinFontSystem == 0)
	{
		gpWinFontSystem = new WinFontSystem;
		gpWinFontSystem->Initial();
		gpWinFontSystem->RegisterFonts();
	}
	
    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTDialogResourceManager::OnResetDevice( LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr = S_OK;

    for( int i=0; i < m_FontCache.GetSize(); i++ )
    {
        DXUTFontNode* pFontNode = m_FontCache.GetAt( i );

        if( pFontNode->pFont )
            pFontNode->pFont->OnResetDevice();
    }

    if( m_pSprite )
        m_pSprite->OnResetDevice();

	if(m_pStateBlock==NULL){
		V_RETURN( pd3dDevice->CreateStateBlock( D3DSBT_ALL, &m_pStateBlock ) );
	}

    return S_OK;
}

//--------------------------------------------------------------------------------------
bool CDXUTDialogResourceManager::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    // Let the CDXUTIMEEditBox's static message proc handle the msg.
    // This is because some IME messages must be handled to ensure
    // proper functionalities and the static msg proc ensures that
    // this happens even if no control has the input focus.
    if( CDXUTIMEEditBox::StaticMsgProc( uMsg, wParam, lParam ) )
        return true;

    return false;
}


//--------------------------------------------------------------------------------------
void CDXUTDialogResourceManager::OnLostDevice()
{
    for( int i=0; i < m_FontCache.GetSize(); i++ )
    {
        DXUTFontNode* pFontNode = m_FontCache.GetAt( i );

        if( pFontNode->pFont )
            pFontNode->pFont->OnLostDevice();
    }

    if( m_pSprite )
        m_pSprite->OnLostDevice();

    SAFE_RELEASE( m_pStateBlock  );
}

    
//--------------------------------------------------------------------------------------
void CDXUTDialogResourceManager::OnDestroyDevice()
{
    int i=0; 

    m_pd3dDevice = NULL;

    // Release the resources but don't clear the cache, as these will need to be
    // recreated if the device is recreated
    for( i=0; i < m_FontCache.GetSize(); i++ )
    {
        DXUTFontNode* pFontNode = m_FontCache.GetAt( i );
        SAFE_RELEASE( pFontNode->pFont );
    }
    
    for( i=0; i < m_TextureCache.GetSize(); i++ )
    {
        DXUTTextureNode* pTextureNode = m_TextureCache.GetAt( i );
        SAFE_RELEASE( pTextureNode->pTexture );
    }

    SAFE_RELEASE( m_pSprite );
}
//--------------------------------------------------------------------------------------
TdgHandle CDXUTDialogResourceManager::AddFontW( LPCWSTR strFaceName, LONG height, LONG weight, BOOL bItalic/*=FALSE*/ ,bool bUnderline )
{
	FontSystem::Font::Style fs;
	fs.face = strFaceName;
	fs.size = height;
	fs.weight = weight;
	fs.italic = bItalic!=FALSE;
	fs.underline = bUnderline;
	fs.strikeout = false;
	return gpWinFontSystem->QueryFont(fs);
}
//--------------------------------------------------------------------------------------
TdgHandle CDXUTDialogResourceManager::AddFontA( LPCSTR strFaceName, LONG height, LONG weight, BOOL bItalic/*=FALSE*/ ,bool bUnderline )
{
	FontSystem::Font::Style fs;
	fs.face = AnsiToUnicode(strFaceName);
	fs.size = height;
	fs.weight = weight;
	fs.italic = bItalic!=FALSE;
	fs.underline = bUnderline;
	fs.strikeout = false;
	return gpWinFontSystem->QueryFont(fs);
}

//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::AddFont( LPCWSTR strFaceName, LONG height, LONG weight, BOOL bItalic ,bool bUnderline )
{
	FontStyle fs;
	fs.face		= strFaceName;
	fs.italic	= bItalic!=FALSE;
	fs.size		= height;
	fs.strikeout= false;
	fs.underline= bUnderline;
	fs.weight	= weight;
	FontHandle hFont = gpWinFontSystem->QueryFont(fs);
    // See if this font already exists
    for( int i=0; i < m_FontCache.GetSize(); i++ )
    {
        TDGFontNode* pFontNode = (TDGFontNode*)m_FontCache.GetAt(i);
        if( 0 == _wcsnicmp( pFontNode->strFace, strFaceName, MAX_PATH-1 ) &&
            pFontNode->nHeight == height &&
            pFontNode->nWeight == weight && pFontNode->hFont == hFont)
        {
            return i;
        }
    }

    // Add a new font and try to create it
    TDGFontNode* pNewFontNode = new TDGFontNode();
    if( pNewFontNode == NULL )
        return -1;

    ZeroMemory( pNewFontNode, sizeof(DXUTFontNode) );
    StringCchCopy( pNewFontNode->strFace, MAX_PATH, strFaceName );
    pNewFontNode->nHeight = height;
    pNewFontNode->nWeight = weight;
    pNewFontNode->bItalic = bItalic;
	
	pNewFontNode->hFont = hFont;
    m_FontCache.Add( pNewFontNode );
    
    int iFont = m_FontCache.GetSize()-1;

    // If a device is available, try to create immediately
    if( m_pd3dDevice )
        CreateFont( iFont );
	
    return iFont;
}

//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::AddFont( const string &strFaceName, LONG height, LONG weight )
{
    WCHAR wstrFaceName[256];
    ANSItoUNICODE( wstrFaceName, strFaceName );

    return AddFont( wstrFaceName, height, weight );
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::AddTexture( LPCWSTR strFilename )
{
	int iTexture = QueryTexture(strFilename);

	if(iTexture != -1)
		return iTexture;
	

    // Add a new texture and try to create it
    DXUTTextureNode* pNewTextureNode = new DXUTTextureNode();
    if( pNewTextureNode == NULL )
        return -1;

    ZeroMemory( pNewTextureNode, sizeof(DXUTTextureNode) );
    StringCchCopy( pNewTextureNode->strFilename, MAX_PATH, strFilename );
    m_TextureCache.Add( pNewTextureNode );
    
    iTexture = m_TextureCache.GetSize()-1;

    // If a device is available, try to create immediately
    if( m_pd3dDevice )
        CreateTexture( iTexture );

    return iTexture;
}
//--------------------------------------------------------------------------------------
bool CDXUTDialogResourceManager::ResetTexture(int iTxr,uint nWidth ,uint nHeight)
{
	if(iTxr < m_TextureCache.GetSize() )
	{
		DXUTTextureNode* pNode = m_TextureCache.GetAt(iTxr);		
		
		if(pNode)
		{
			if(pNode->pTexture)
				pNode->pTexture->Release();
			pNode->dwHeight = 0;
			pNode->dwWidth = 0;

			HRESULT hr = D3DXCreateTexture(m_pd3dDevice,nWidth,nHeight,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,&pNode->pTexture);
			if(hr == S_OK)
			{
				pNode->dwHeight				= nHeight;
				pNode->dwWidth				= nWidth;			
				return true;
			}		
		}
	}

	return false;
}
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// ming add 以 string 為檔名參數
//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::AddTexture( const string &strFilename )
{
    WCHAR wstrFaceName[256];
    ANSItoUNICODE( wstrFaceName, strFilename );

    return AddTexture( wstrFaceName );
}
//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::QueryTexture(LPCWSTR strDiscernName)
{
	wstring DiscernName = strDiscernName ;
	if( DiscernName.empty() )
		return -1 ;

	// See if this texture already exists
    for( int i=0; i < m_TextureCache.GetSize(); i++ )
    {
        DXUTTextureNode* pTextureNode = m_TextureCache.GetAt(i);
        if( 0 == _wcsnicmp( pTextureNode->strFilename, strDiscernName, MAX_PATH-1 ) )
        {
            return i;
        }
    }
	return -1;
}
//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::QueryTexture(LPCSTR strDiscernName)
{
	WCHAR wstrFaceName[256];
    ANSItoUNICODE( wstrFaceName, strDiscernName );

    return QueryTexture( wstrFaceName );
}
//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::AddTextureFromMemory( LPCWSTR strDiscernName ,void* pData,unsigned int DataSize,unsigned int nColorKey)
{
	int iTexture = QueryTexture(strDiscernName);

	if(iTexture != -1)
		return iTexture;
    // Add a new texture and try to create it
    DXUTTextureNode* pNewTextureNode = new DXUTTextureNode();
    if( pNewTextureNode == NULL )
        return -1;

    ZeroMemory( pNewTextureNode, sizeof(DXUTTextureNode) );
    StringCchCopy( pNewTextureNode->strFilename, MAX_PATH, strDiscernName );
    m_TextureCache.Add( pNewTextureNode );
    
    iTexture = m_TextureCache.GetSize()-1;

    // If a device is available, try to create immediately
    if( m_pd3dDevice )
        CreateTextureFromMemory( iTexture , pData,DataSize,nColorKey);

    return iTexture;
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialogResourceManager::CreateFont( UINT iFont )
{
    HRESULT hr = S_OK;

    DXUTFontNode* pFontNode = m_FontCache.GetAt( iFont );

    SAFE_RELEASE( pFontNode->pFont );
    
    V_RETURN( D3DXCreateFont( 
        m_pd3dDevice, pFontNode->nHeight, 0, pFontNode->nWeight, 1, 
        pFontNode->bItalic, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
        pFontNode->strFace, &pFontNode->pFont ) );

    return S_OK;
}
//--------------------------------------------------------------------------------------
void CDXUTDialogResourceManager::ReloadTexture(UINT index , void* pData,unsigned int DataSize ,unsigned int nColorKey )
{
	DXUTTextureNode* pNode = GetTextureNode(index);
	if(pNode && pNode->pTexture)
	{
		pNode->pTexture->Release();		
		pNode->pTexture = 0;
	}
	CreateTextureFromMemory(index,pData,DataSize,nColorKey);
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialogResourceManager::CreateTextureFromMemory( UINT iTexture , void* pData,unsigned int DataSize,unsigned int nColorKey)
{
	HRESULT hr = S_OK;

	DXUTTextureNode* pTextureNode = m_TextureCache.GetAt( iTexture );
    
    // Make sure there's a texture to create
    if( pTextureNode->strFilename[0] == 0 )
        return S_OK;

////////////////////////////////////////////////////////////////////////

     // Create texture
    D3DXIMAGE_INFO info;
	

	if (pData == NULL)
	{
		return S_OK;
	}

    if( pData )
    {
        hr = D3DXCreateTextureFromFileInMemoryEx(
            m_pd3dDevice,               // LPDIRECT3DDEVICE9 pDevice,
            pData,                    // LPCVOID pSrcData,
            DataSize,                // UINT SrcDataSize,
            D3DX_DEFAULT,               // UINT Width,
            D3DX_DEFAULT,               // UINT Height,
            D3DX_DEFAULT,               // UINT MipLevels,
            0,                          // DWORD Usage,
            D3DFMT_UNKNOWN,             // D3DFORMAT Format,
            D3DPOOL_MANAGED,            // D3DPOOL Pool,
            D3DX_DEFAULT,               // DWORD Filter,
            D3DX_DEFAULT,               // DWORD MipFilter,
            nColorKey,                         // D3DCOLOR ColorKey,
            &info,                      // D3DXIMAGE_INFO *pSrcInfo,
            NULL,                       // PALETTEENTRY *pPalette,
            &pTextureNode->pTexture     // LPDIRECT3DTEXTURE9 *ppTexture
        );


        if( FAILED(hr) )
        {
            // try to create sf2 
            return DXTRACE_ERR( L"D3DXCreateTextureFromFileInMemoryEx", hr );
        }
        else
        {
            // Store dimensions
            pTextureNode->dwWidth = info.Width;
            pTextureNode->dwHeight = info.Height;
        }
    }
		
	

	return hr;
}
//--------------------------------------------------------------------------------------
int CDXUTDialogResourceManager::CreateEmptyTexture( LPCWSTR strDiscernName , uint nWidth ,uint nHeight,uint nDxFmt /*= D3DFMT_UNKNOWN*/)
{
	int iTxr = QueryTexture(strDiscernName);
	if(iTxr != -1)
	{
		DXUTTextureNode* pNode = m_TextureCache.GetAt(iTxr);
		if(pNode->pTexture)
			pNode->pTexture->Release();
		pNode->dwHeight = 0;
		pNode->dwWidth = 0;
	}
	
	DXUTTextureNode& node = (iTxr != -1)? *m_TextureCache.GetAt(iTxr) : *new DXUTTextureNode();

	HRESULT hr = D3DXCreateTexture(m_pd3dDevice,nWidth,nHeight,D3DX_DEFAULT,0,(D3DFORMAT)nDxFmt,D3DPOOL_MANAGED,&node.pTexture);
	if(hr == S_OK)
	{
		node.dwHeight				= nHeight;
		node.dwWidth				= nWidth;
		wcsncpy(node.strFilename ,strDiscernName, MAX_PATH );

		if(iTxr != -1)
		{
			return iTxr;
		}
		else
		{
			m_TextureCache.Add(&node);
			return m_TextureCache.GetSize() - 1 ;
		}		
	}		

	return -1;
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTDialogResourceManager::CreateTexture( UINT iTexture )
{

    HRESULT hr = S_OK;

    DXUTTextureNode* pTextureNode = m_TextureCache.GetAt( iTexture );
    
    // Make sure there's a texture to create
    if( pTextureNode->strFilename[0] == 0 )
        return S_OK;

////////////////////////////////////////////////////////////////////////

     // Create texture
    D3DXIMAGE_INFO info;

#ifdef RESMGR


    // try to create in memory
    memPack *m = LoadFromPacket( UNICODEtoANSI(pTextureNode->strFilename) );	

    BYTE* pData = m->Data;

	if (pData == NULL)
	{
		return S_OK;
	}
	
    if( pData )
    {
        hr = D3DXCreateTextureFromFileInMemoryEx(
            m_pd3dDevice,               // LPDIRECT3DDEVICE9 pDevice,
            pData,                    // LPCVOID pSrcData,
            m->DataSize,                // UINT SrcDataSize,
            D3DX_DEFAULT,               // UINT Width,
            D3DX_DEFAULT,               // UINT Height,
            D3DX_DEFAULT,               // UINT MipLevels,
            0,                          // DWORD Usage,
            D3DFMT_UNKNOWN,             // D3DFORMAT Format,
            D3DPOOL_MANAGED,            // D3DPOOL Pool,
            D3DX_DEFAULT,               // DWORD Filter,
            D3DX_DEFAULT,               // DWORD MipFilter,
            0,                          // D3DCOLOR ColorKey,
            &info,                      // D3DXIMAGE_INFO *pSrcInfo,
            NULL,                       // PALETTEENTRY *pPalette,
            &pTextureNode->pTexture     // LPDIRECT3DTEXTURE9 *ppTexture
        );


        if( FAILED(hr) )
        {
            // try to create sf2 
            return DXTRACE_ERR( L"D3DXCreateTextureFromFileInMemoryEx", hr );
        }
        else
        {
            // Store dimensions
            pTextureNode->dwWidth = info.Width;
            pTextureNode->dwHeight = info.Height;
        }
    }
		
  //因為以經是唯一了，所以不需要再做管理了，直接將他移除。
	ReleaseResource(m) ;


#else
    hr =  D3DXCreateTextureFromFileEx(
        m_pd3dDevice,
        pTextureNode->strFilename,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        &info,
        NULL,
        &pTextureNode->pTexture );

    if( FAILED(hr) )
    {
        // try to create in memory
        memPack *m = LoadFromPacket( UNICODEtoANSI(pTextureNode->strFilename) );
        if( m )
        {
            hr = D3DXCreateTextureFromFileInMemoryEx(
                m_pd3dDevice,               // LPDIRECT3DDEVICE9 pDevice,
                m->Data,                    // LPCVOID pSrcData,
                m->DataSize,                // UINT SrcDataSize,
                D3DX_DEFAULT,               // UINT Width,
                D3DX_DEFAULT,               // UINT Height,
                D3DX_DEFAULT,               // UINT MipLevels,
                0,                          // DWORD Usage,
                D3DFMT_UNKNOWN,             // D3DFORMAT Format,
                D3DPOOL_MANAGED,            // D3DPOOL Pool,
                D3DX_DEFAULT,               // DWORD Filter,
                D3DX_DEFAULT,               // DWORD MipFilter,
                0,                          // D3DCOLOR ColorKey,
                &info,                      // D3DXIMAGE_INFO *pSrcInfo,
                NULL,                       // PALETTEENTRY *pPalette,
                &pTextureNode->pTexture     // LPDIRECT3DTEXTURE9 *ppTexture
            );


            if( FAILED(hr) )
            {
                // try to create sf2 
                return DXTRACE_ERR( L"D3DXCreateTextureFromFileInMemoryEx", hr );
            }
            else
            {
                // Store dimensions
                pTextureNode->dwWidth = info.Width;
                pTextureNode->dwHeight = info.Height;
            }
        }
	}
    else
    {
        // Store dimensions
        pTextureNode->dwWidth = info.Width;
        pTextureNode->dwHeight = info.Height;
    }

#endif

    


    return S_OK;
}
//--------------------------------------------------------------------------------------
void CDXUTDialogResourceManager::DrawFontHandle(TdgHandle hnd,const char16* text,const Rect32& rcDraw,uint nFormat,uint nColor,uint nStyle)
{
	WinTextDrawInfo di;
	di.nColor = nColor;
	di.nFormat= nFormat;
	di.nStyle = nStyle;
	gpWinFontSystem->Draw(hnd,text,rcDraw,di);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTDialogResourceManager::DrawFont( int iFont,const char16* text,const Rect32& rcDraw,uint nFormat,uint nColor,uint nStyle)
{

	 TDGFontNode* pNode = (TDGFontNode*)GetFontNode(iFont);
	 if (pNode)
	 {
		 
		 if(RS_HANDLEVALID(pNode->hFont))
		 {
			 DrawFontHandle(pNode->hFont,text,rcDraw,nFormat,nColor,nStyle);
		 }
	 }
}
//--------------------------------------------------------------------------------------
SIZE CDXUTDialogResourceManager::GetStringSize(FontHandle hFont,const char16* text)
{
	SIZE sz;
	sz.cx = 0;
	sz.cy = 0;
	sint nlen = wcslen(text);
	for ( sint idx = 0;idx < nlen ; ++idx)
	{
		SIZE szChar = gpWinFontSystem->GetCodeSize( hFont , text[idx]);	
		sz.cx += szChar.cx;
		if(sz.cy < szChar.cy)
			sz.cy = szChar.cy;
	}
	return sz;
}
//--------------------------------------------------------------------------------------
SIZE CDXUTDialogResourceManager::GetStringSize(int iIndex,const char16* text)
{
	SIZE sz;
	sz.cx = 0;
	sz.cy = 0;
	TDGFontNode* pNode = (TDGFontNode*)GetFontNode(iIndex);
	if(pNode)
	{
		sint nlen = wcslen(text);
		for ( sint idx = 0;idx < nlen ; ++idx)
		{
			SIZE szChar = gpWinFontSystem->GetCodeSize( pNode->hFont , text[idx]);	
			sz.cx += szChar.cx;
			if(sz.cy < szChar.cy)
				sz.cy = szChar.cy;
		}
		
	}
	
	return sz;
}
//--------------------------------------------------------------------------------------
SIZE CDXUTDialogResourceManager::GetChar16Size(int iIndex,char16 nchar)
{
	TDGFontNode* pNode = (TDGFontNode*)GetFontNode(iIndex);
	if(pNode)
		return gpWinFontSystem->GetCodeSize( pNode->hFont , nchar);
	return SIZE();
}
//


//--------------------------------------------------------------------------------------
bool CensoringString(std::wstring& out ,int iFont,const char16* text,sint nLimitWidth,const char16* strCensoring /*= L"..."*/)
{
	CDXUTDialogResourceManager* pDRM = CDXUTDialog::GetManager();
	if(pDRM->GetStringSize(iFont , text).cx < nLimitWidth)
	{
		out = text;
		return false;
	}
	else
	{

		SIZE szCensoring = pDRM->GetStringSize(iFont , strCensoring);
		sint nTextWidth = nLimitWidth;
		if(szCensoring.cx < nTextWidth)
		{
			nTextWidth -= szCensoring.cx;
			uint nTextLen(wcslen(text));
			char16* tmpText = new char16[nTextLen];			
			memset(tmpText,0,sizeof(char16) * nTextLen);
			sint iText = 0 ; 
			for ( ;iText < nTextLen ; ++iText )
			{
				SIZE szChar = pDRM->GetChar16Size(iFont , text[iText]);
				if( nTextWidth > szChar.cx  )
				{
					tmpText[iText] = text[iText];

					nTextWidth -= szChar.cx;
					continue;
				}
				else
				{					
					break;
				}
			}			
			
			StringFormat16(out , L"%s%s",tmpText,strCensoring);
			delete tmpText;
			return true;
		}
		else
		{
			//return L"";
		}
	}

	return false;
}
//--------------------------------------------------------------------------------------
DXUTFontNode* CDXUTDialogResourceManager::GetFontNode( int iIndex )     
{
	if(iIndex!=-1 && iIndex < m_FontCache.GetSize())
		return m_FontCache.GetAt( iIndex ); 
	return 0;
}
//--------------------------------------------------------------------------------------
DXUTTextureNode* CDXUTDialogResourceManager::GetTextureNode( int iIndex )  
{
	if(iIndex!=-1 && iIndex < m_TextureCache.GetSize()) 
		return m_TextureCache.GetAt( iIndex ); 
	return 0;
}
//--------------------------------------------------------------------------------------

