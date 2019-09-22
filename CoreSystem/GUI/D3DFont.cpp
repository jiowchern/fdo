//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "D3DFont.h"


namespace FDO
{

//-----------------------------------------------------------------------------
CD3DFont::CD3DFont( LPDIRECT3DDEVICE9 pd3dDevice, int iFontSize, const WCHAR* wszFontName )
{
    m_D3DDevice = pd3dDevice;
    m_D3DFont   = NULL;
    m_FontSize  = iFontSize;
    wcscpy( m_FontName, wszFontName );   // System   Arial
    CreateFont();
}
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont()
{
    RemoveFont();
}
//-----------------------------------------------------------------------------
void CD3DFont::RemoveFont()
{
    if(m_D3DFont)
    {
        m_D3DFont->Release();
        m_D3DFont = NULL;
    }
}
//-----------------------------------------------------------------------------
HRESULT CD3DFont::CreateFont()
{
    // To create a Windows friendly font using only a point size, an
    // application must calculate the logical height of the font.
    // 
    // This is because functions like CreateFont() and CreateFontIndirect() 
    // only use logical units to specify height.
    //
    // Here's the formula to find the height in logical pixels:
    //
    //             -( point_size * LOGPIXELSY )
    //    height = ----------------------------
    //                          72
    //

    if( m_D3DFont )
        return S_OK;

	HDC hDC = GetDC( NULL );

	int iHeight = -( MulDiv( m_FontSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );

	ReleaseDC( NULL, hDC );

	HRESULT hr = D3DXCreateFont( m_D3DDevice, iHeight, 0, FW_BOLD, 0, FALSE,
		                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
		                    DEFAULT_PITCH | FF_DONTCARE, m_FontName,
		                    &m_D3DFont );

	if( FAILED( hr ) )
		MessageBox(NULL, L"Call to D3DXCreateFont failed!", L"CD3DFont::CreateFont", NULL);

    return hr;
}
//-----------------------------------------------------------------------------
INT CD3DFont::CalcRect(LPCTSTR pString, INT cch, LPRECT pRect)
{
    const DWORD dwFormat = DT_SINGLELINE|DT_LEFT|DT_TOP|DT_NOCLIP|DT_CALCRECT;
    
    if (!m_D3DFont)
        return 0;
    return m_D3DFont->DrawText(NULL, pString, cch, pRect, dwFormat, 0);
}

//-----------------------------------------------------------------------------
void CD3DFont::DrawText(LPCTSTR pString, INT cch, LPRECT pRect, DWORD Color, bool IsShade )
{
    const DWORD dwFormat = DT_SINGLELINE|DT_LEFT|DT_TOP|DT_NOCLIP;
    
    if( m_D3DFont )
    {
        if( IsShade )
        {
            RECT rect = *pRect;
            //rect.left += 2;
            //rect.top += 1;
            //m_D3DFont->DrawText(NULL, pString, cch, &rect, dwFormat, 0x80000000);

            rect.top -= 1;
            m_D3DFont->DrawText(NULL, pString, cch, &rect, dwFormat, 0x80000000);

            rect.top += 2;
            m_D3DFont->DrawText(NULL, pString, cch, &rect, dwFormat, 0x80000000);

            rect.top = pRect->top;

            rect.left -= 1;
            m_D3DFont->DrawText(NULL, pString, cch, &rect, dwFormat, 0x80000000);

            rect.left += 2;
            m_D3DFont->DrawText(NULL, pString, cch, &rect, dwFormat, 0x80000000);
        }

        m_D3DFont->DrawText(NULL, pString, cch, pRect, dwFormat, Color);
    }
}
//-----------------------------------------------------------------------------
void CD3DFont::DrawText(LPCTSTR pString, LPRECT pRect, DWORD Color, bool IsShade )
{
    const DWORD dwFormat = DT_SINGLELINE|DT_LEFT|DT_TOP|DT_NOCLIP;

    if( m_D3DFont )
    {
        if( IsShade )
        {
            RECT rect = *pRect;
            //rect.left += 2;
            //rect.top += 1;
            //m_D3DFont->DrawText(NULL, pString, -1, &rect, DT_LEFT|DT_NOCLIP, 0x80000000);

            rect.top -= 1;
            m_D3DFont->DrawText(NULL, pString, -1, &rect, dwFormat, 0x80000000);

            rect.top += 2;
            m_D3DFont->DrawText(NULL, pString, -1, &rect, dwFormat, 0x80000000);

            rect.top = pRect->top;

            rect.left -= 1;
            m_D3DFont->DrawText(NULL, pString, -1, &rect, dwFormat, 0x80000000);

            rect.left += 2;
            m_D3DFont->DrawText(NULL, pString, -1, &rect, dwFormat, 0x80000000);
        }

        m_D3DFont->DrawText(NULL, pString, -1, pRect, dwFormat, Color);
    }
}
//----------------------------------------------------------------------------
//設定顯示字形
//----------------------------------------------------------------------------
void CD3DFont::SetFontName(const TCHAR *FontName)
{
    wcscpy(m_FontName, FontName);
    RemoveFont();
    CreateFont();
}
//----------------------------------------------------------------------------
//取得顯示字形
//----------------------------------------------------------------------------
const TCHAR *CD3DFont::GetFontName()
{
    return m_FontName;
}
//----------------------------------------------------------------------------
//設定字形大小
//----------------------------------------------------------------------------
void CD3DFont::SetFontSize(int Size)
{
    m_FontSize = Size;
    RemoveFont();
    CreateFont();
}
//----------------------------------------------------------------------------
//取得字形大小
//----------------------------------------------------------------------------
int CD3DFont::GetFontSize()
{
    return m_FontSize;
}
//----------------------------------------------------------------------------

} //namespace FDO





