//
// CDXUTStatic.h
//
#ifndef __CDXUTStatic_h__
#define __CDXUTStatic_h__

#include "CDXUTDef.h"
#include "CDXUTControl.h"
#include "CFDOXML.h"
//-----------------------------------------------------------------------------
// Static control
//-----------------------------------------------------------------------------
class CDXUTStatic : public CDXUTControl
{
	bool				m_bOneLine;
public:
    CTextObject*		m_pTextObject;	
public:
    CDXUTStatic( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTStatic();

    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    //virtual BOOL ContainsPoint( POINT pt ) { return false; }
    virtual void RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime ); // 畫出元件提示
    virtual void SetTextColor( D3DCOLOR Color, UINT iElement = 0 );
	virtual void SetTextFocusColor( D3DCOLOR Color, UINT iElement = 1 );
	virtual void SetTextDisableColor( D3DCOLOR Color, UINT iElement = 0 );

    HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
    inline LPCWSTR GetText() const { return _GetText(); }
	inline	void	SetOneLine(bool bEnable) {m_bOneLine = bEnable ;};
    SIZE	SetText( LPCWSTR strText ,void (*pfn)(CustomElement& ce,int nData) = 0,int nData = 0);
	SIZE	SetTextResize(LPCWSTR strText);
	void	SetTextStretchHeight( LPCWSTR strText );

    // ming add >>
    HRESULT SetText( const string &strText );
    HRESULT SetText( int iText );
    string  GetTextA(); // ANSI
	wstring GetTextAW(); // ANSI
    int     GetTextI(); // integer

    CDXUTElement pTextureElement;


	static void fnSetText(const wchar_t* str ,CDXUTControl* pCtrl);
/*
    HRESULT SetHint( LPCWSTR strHint );
    HRESULT SetHint( const string &strHint );
    // ming add <<

protected:
    WCHAR m_strText[MAX_PATH];      // Window text
    WCHAR m_strHint[MAX_PATH];      // Window text (提示Hint)
*/
};

#endif // __CDXUTStatic_h__
