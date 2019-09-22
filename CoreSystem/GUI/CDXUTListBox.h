//
// CDXUTListBox.h
//
#ifndef __CDXUTListBox_h__
#define __CDXUTListBox_h__

#include "CDXUTDef.h"
#include "CDXUTScrollBar.h"
#include "Common.h"

struct DXUTListBoxItem;

//-----------------------------------------------------------------------------
// ListBox control
//-----------------------------------------------------------------------------


class CDXUTListBox : public CDXUTControl , public ScrollBarEvent::BarVisitor //自己是Bar拜訪者
{
protected:
	DXUTScrollBarVisitor	mDXUTScrollBarVisitor;
public:
	void	AddVisitor(ScrollBarEvent::ControlVisitor* pVis);
	typedef void	(*PFN_CUSTOMDRAW)(CDXUTListBox*pCtrl , DXUTListBoxItem* pItem, CDXUTElement* pElement, RECT* pRect);
    //--------------------------------------------------------------------------------
    // Constructor/destructor
    //--------------------------------------------------------------------------------
    CDXUTListBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTListBox();
    //--------------------------------------------------------------------------------
    // CDXUTControl interface overridings
    //--------------------------------------------------------------------------------
    virtual HRESULT OnInit();
    virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
	virtual void	RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    //--------------------------------------------------------------------------------
    // ListBox style (currrently, only support multi-selection style)
    //--------------------------------------------------------------------------------
    enum STYLE { MULTISELECTION = 1 };
    DWORD GetStyle() const;
    void SetStyle( DWORD dwStyle );
	void SetCustomItemDraw(PFN_CUSTOMDRAW pfnItemDraw);
    //--------------------------------------------------------------------------------
    // Visual effects
    //--------------------------------------------------------------------------------
    virtual void SetBackgroundTexture( const string &FileName, RECT* prcTexture=0 );
    //--------------------------------------------------------------------------------
    // Selection
    //--------------------------------------------------------------------------------
    int GetSelTop( int iOffset = 0 ); // 取得選擇物件範圍的top
    void SetDrawSelect( bool bDraw );
    //--------------------------------------------------------------------------------
    // ListBox visual elements layout
    //--------------------------------------------------------------------------------
    virtual void SetSpace( int nSpace );
	virtual void SetSelected( int iIndex);
	virtual int  GetSelected();
	virtual int  GetItemSize();
    virtual void SetBorder( int nBorder, int nMargin );
    void GetClientRect(RECT& rc) const; // client rectangle of listbox
    //--------------------------------------------------------------------------------
    // Scrollbar and track position.
    //--------------------------------------------------------------------------------
    CDXUTScrollBar m_ScrollBar;
	inline void SetScrollBarVisible(bool bSet) { m_ScrollBar.SetVisible(bSet) ;}
	inline int GetPageSize()  { return m_ScrollBar.GetPageSize() ;}
    void SetScrollBarWidth( int nWidth );
    int  GetTrackPos();
    void SetTrackPos( int nPosition );
    void TrackLast();
    virtual void UpdateTrackRange();
	void SetRenderingTrackPos( int iRenderingTrackPos ){ m_iRenderingTrackPos = iRenderingTrackPos; } // Darkness Add 
	int GetCheck() { return m_nMouseCell ; }										//回傳滑鼠目前所指Item的Index 
    //--------------------------------------------------------------------------------
    // Item Operations
    //--------------------------------------------------------------------------------
    // Number of items
    virtual int GetSize() const;
    // Adding items
	HRESULT AddItem( const string &szText, D3DXCOLOR FontColor=FDO::WHITE, void *pData=0 , bool bCensoring = true);
	HRESULT AddItem( const WCHAR *wszText, void *pData, D3DXCOLOR FontColor=FDO::WHITE , bool bCensoring = true);
	
	HRESULT AddString( const WCHAR *wszText, D3DXCOLOR FontColor=FDO::WHITE ,const WCHAR* stoken = L"<r/>" );
	HRESULT AddLine( const WCHAR *wszText, D3DXCOLOR FontColor=FDO::WHITE ,void* pData = 0 );
    HRESULT InsertItem( int nIndex, const WCHAR *wszText, D3DXCOLOR FontColor , void *pData = 0 , bool bCensoring = true) ;
	void	SetTextFont(const char16* sName,uint nHeight,uint nW);
    // Removing items
    void RemoveItem( int nIndex );
    void RemoveItemByText( WCHAR *wszText );
    void RemoveItemByData( void *pData );
    void RemoveAllItems();
    // Getting items
    int GetItemIndexByData( void *pData );
    DXUTListBoxItem *GetItemByData( void *pData );
    DXUTListBoxItem *GetItem( int nIndex );
    // Selecting items
    void SelectItem( int nNewIndex );
    int GetSelectedIndex( int nPreviousSelected = -1 );
    DXUTListBoxItem *GetSelectedItem( int nPreviousSelected = -1 );
	inline int GetTextHeight()const {return m_nTextHeight;}
	virtual void	Visit		(ScrollBarEvent::CurPos& e) ;	
protected:
    //--------------------------------------------------------------------------------
    // CDXUTControl internal methods overridings
    //--------------------------------------------------------------------------------
    virtual void UpdateRects();
	
    virtual void	DrawItemText(DXUTListBoxItem* pItem, CDXUTElement* pElement, RECT* pRect);	
	static	void	fnDrawItem(CDXUTListBox*pCtrl , DXUTListBoxItem* pItem, CDXUTElement* pElement, RECT* pRect);
	PFN_CUSTOMDRAW	mpfnDrawItem;
protected:
    //--------------------------------------------------------------------------------
    // Geometry calculation variables
    //--------------------------------------------------------------------------------
    RECT m_rcText;      // Text rendering bound
    RECT m_rcSelection; // Selection box bound
    int m_nTextHeight;  // Height of a single line of text
    //--------------------------------------------------------------------------------
    // Misc flags
    //--------------------------------------------------------------------------------
    bool	m_bDrag;       // Whether the user is dragging the mouse to select	
    DWORD m_dwStyle;    // List box style
    int m_iRenderingTrackPos;

    //--------------------------------------------------------------------------------
    // ListBox visual elements & layout
    //--------------------------------------------------------------------------------
	bool m_bSBInit;
    int m_nSBWidth;     // ScrollBar Width
    int m_nBorder;
    int m_nMargin;
    int m_nSpace;
    //--------------------------------------------------------------------------------
    // Selection
    //--------------------------------------------------------------------------------
	int m_nSelected;    // Index of the selected item for single selection list box
	int m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)
	int m_nMouseCell;
	int m_LastMouseCell;
    bool m_bDrawSelect;
    //--------------------------------------------------------------------------------
    // Item storage
    //--------------------------------------------------------------------------------
    CGrowableArray< DXUTListBoxItem* > m_Items; 

};

#endif //__CDXUTListBox_h__ 
