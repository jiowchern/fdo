//-----------------------------------------------------------------------------
#ifndef _D3DFONT_H_
#define _D3DFONT_H_

#include <d3dx9core.h>   

namespace FDO
{

//-----------------------------------------------------------------------------
class CD3DFont
{
private:
    LPDIRECT3DDEVICE9       m_D3DDevice;
    LPD3DXFONT              m_D3DFont;
    TCHAR                   m_FontName[32];                         //�r�ΦW��
    int                     m_FontSize;                             //�r���j�p

public:
    CD3DFont( LPDIRECT3DDEVICE9 pd3dDevice, int iFontSize, const WCHAR* wszFontName );
    ~CD3DFont();

    //�禡
    HRESULT         CreateFont();
    void            RemoveFont();
    void            DrawText( LPCTSTR pString, LPRECT pRect, DWORD Color, bool IsShade = true );
    void            DrawText(LPCTSTR pString, INT cch, LPRECT pRect, DWORD Color, bool IsShade = true);
    INT             CalcRect(LPCTSTR pString, INT cch, LPRECT pRect);
    void            SetFontName( const TCHAR *FontName );
    const TCHAR*    GetFontName();
    void            SetFontSize( int Size );
    int             GetFontSize();
};

} //namespace FDO

#endif
//-----------------------------------------------------------------------------


