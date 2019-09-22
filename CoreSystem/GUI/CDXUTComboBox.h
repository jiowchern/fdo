//
// CDXUTComboBox.h
//
#ifndef __CDXUTComboBox_h__
#define __CDXUTComboBox_h__

#include "CDXUTDef.h"
#include "CDXUTButton.h"
#include "CDXUTScrollBar.h"
#include "Common.h"
struct DXUTComboBoxItem;

//-----------------------------------------------------------------------------
// ComboBox control
//-----------------------------------------------------------------------------

class CDXUTComboBox : public CDXUTButton
{
public:
    CDXUTComboBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTComboBox();
    
    virtual void SetTextColor( D3DCOLOR Color, UINT iElement = 0 );
    virtual HRESULT OnInit();

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey();

    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual void OnFocusOut();
    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime ); // 畫出元件提示

    virtual void UpdateRects(); 

	HRESULT AddItem( const WCHAR *wszText, void *pData=NULL, D3DXCOLOR FontColor=FDO::BLACK );
    void    RemoveAllItems();
    void    RemoveItem( UINT index );
    bool    ContainsItem( const WCHAR* strText, UINT iStart=0 );
    bool    ContainsItem( const string &strText, UINT iStart=0 ); // ming add
    int     FindItem( const WCHAR* strText, UINT iStart=0 );
    void*   GetItemData( const WCHAR* strText );
    void*   GetItemData( int nIndex );
    void    SetDropHeight( UINT nHeight );
    void    SetScrollBarWidth( int nWidth );

    void*   GetSelectedData();
    DXUTComboBoxItem* GetSelectedItem();

    UINT    GetNumItems() { return m_Items.GetSize(); }
    DXUTComboBoxItem* GetItem( UINT index ) { return m_Items.GetAt( index ); }

    HRESULT SetSelectedByIndex( UINT index, bool bTrigger = false);
    HRESULT SetSelectedByText( const WCHAR* strText );
    HRESULT SetSelectedByData( void* pData );

    // ming add >>
	HRESULT AddItem( const string &szText, D3DXCOLOR FontColor=FDO::BLACK, void *pData=0 );
    void SetButtonSize( const int &Width, const int &Height );
    void OffsetButton( const int &OffsetX, const int &OffsetY );
    void ShowMainTextBox( const bool &Show );
    // ming add <<

	inline CDXUTScrollBar*	GetScrollBar(){ return &m_ScrollBar;}
	inline int	GetSelected(){ return m_iSelected;}
	//取得item編號
	const int GetSelectedID(){return m_iSelected;}

protected:
    void DrawItemText(DXUTComboBoxItem* pItem, CDXUTElement* pElement, RECT* pRect=NULL);

protected:
    int     m_iSelected;
    int     m_iFocused;
    int     m_nDropHeight;
    CDXUTScrollBar m_ScrollBar;
    int     m_nSBWidth;

    bool    m_bOpened;

    RECT m_rcText;
    RECT m_rcButton;
    RECT m_rcDropdown;
    RECT m_rcDropdownText;

    // ming add >>
    int m_ButtonWidth;
    int m_ButtonHeight;
    int m_ButtonOffsetX;
    int m_ButtonOffsetY;

    bool m_ShowMain;
    // ming add <<

    CDXUTElement pTextureElement;
    
    CGrowableArray< DXUTComboBoxItem* > m_Items;
};

#endif // __CDXUTComboBox_h__