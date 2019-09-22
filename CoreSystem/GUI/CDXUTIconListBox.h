/*******************************************************************************

	Copyright 2006 by DYNASTY international Co., Ltd.
	All rights reserved.

	This software is the confidential and proprietary information of
	DYNASTY international Co., Ltd. ('Confidential Information').
	You shall not disclose such Confidential Information and shall
	use it only in accordance with the terms of the license agreement
	you entered into with DYNASTY international Co., Ltd.

*******************************************************************************/

#ifndef CDXUT_ICON_LIST_BOX_H
#define CDXUT_ICON_LIST_BOX_H

#include "CDXUTButton.h"
#include "CDXUTListBox.h"
#include "ListBoxItem.h"
#include <vector>
//-----------------------------------------------------------------------------
// Image control
//-----------------------------------------------------------------------------
class CDXUTIconImage : public CDXUTButton
{
public:
    CDXUTIconImage( CDXUTDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    // 設定此Image的材質，作用與CDXUTControl的SetTexture相同
    void SetImage( const string &FileName, RECT* prcTexture=0 )  //說明 ***
        { CDXUTControl::SetTexture( FileName, prcTexture ); }
    void SetImage( const WCHAR *FileName, RECT* prcTexture=0 )
        { CDXUTControl::SetTexture( FileName, prcTexture ); }

    void SetColor( D3DCOLOR color );

    IDirect3DTexture9* GetTexture( int iTexture ); // 取得此Image的材質
};

//-----------------------------------------------------------------------------
// ImageListBox control
//-----------------------------------------------------------------------------
struct DXUTIconListBoxItem : public DXUTListBoxItem
{
    auto_ptr<CDXUTIconImage> Image;
    auto_ptr<CDXUTStatic> Static;

    bool m_bStatic; // 是否顯示數量在Image右下方
    bool m_Visible;
};

class CDXUTIconListBox : public CDXUTListBox
{
public:
    CDXUTIconListBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTIconListBox();

    //virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }
    //virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    //virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void    RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
	virtual void	OnMouseEnter() ;
	virtual void	OnMouseLeave() ;
    virtual void    UpdateRects();

    

    HRESULT AddItem( DXUTIconListBoxItem *pItem );
    HRESULT AddItem( const WCHAR *ImageFile, void *pData, int Amount = 0, DWORD TextColor = 0xFF000000 );
    HRESULT AddItem( const string &ImageFile, void *pData, int Amount = 0, DWORD TextColor = 0xFF000000 );
	HRESULT InsertItem( int nIndex, const WCHAR *ImageFile, void *pData, int Amount = 0 );

    void RemoveItem( const int &nIndex );
    void RemoveItem( DXUTIconListBoxItem *pItem ) { RemoveItem( m_Items.IndexOf(pItem) ); }
    void RemoveAllItems();
    void RemoveItemPointer( const int &nIndex );
    void RemoveItemPointer( DXUTIconListBoxItem *pItem ) { RemoveItemPointer( m_Items.IndexOf(pItem) ); }

    int GetItemIndexByData( void *pData );
    DXUTIconListBoxItem *GetItem( int nIndex );
    DXUTIconListBoxItem *GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
    DXUTIconListBoxItem *GetItemByData( void *pData ) { return GetItem( GetItemIndexByData(pData) ); }

    // 設定可否拖曳
    void SetDragIcon( bool bDragIcon ) { m_bDragIcon = bDragIcon; }
    bool GetDragIcon() { return m_bDragIcon; }

    // 設定元件參數
	virtual void SetBorder( int nBorder , int nMargin ) { m_nBorder = nBorder ; m_nMargin = nMargin ; UpdateRects() ; }
	virtual void SetSpace( int nSpace ) { m_nSpace = nSpace; UpdateRects() ; }// 設定Cell之間的間距
	void SetRow( unsigned Row ) { m_iBoxRow = Row ; UpdateRects() ; }
	void SetCol( unsigned Col ) { m_iBoxCol = Col ; UpdateRects() ; }
	void SetItemWidth( int iWidth ) { m_iItemWidth = iWidth ; UpdateRects() ; }
    void SetItemHeight( int iHeight ) { m_iItemHeight = iHeight; UpdateRects() ; }
    void SetStaticWidth( int iWidth ) { m_iStaticWidth = iWidth; }
    void SetDrawText( bool bDraw ) { m_bDrawText = bDraw; }
	void SetDrawStaticZero( bool bStatue ) { m_bDrawStaticZero = bStatue ; }	// 設定是否畫static當文字為0的時候

	virtual int GetSize() const { return m_Items.GetSize() ; }
	void TrackLast() ; // 設定ScrollBar移至List最後
	inline int GetBoxRow() const {return m_iBoxRow ; }
	inline int GetBoxCol() const {return m_iBoxCol ; }
	inline int GetCellWidth() { return m_iItemWidth ; }
	inline int GetCellHeight() { return m_iItemHeight ; }
	inline int GetCheckCellIndex() { return m_Check - m_ScrollBar.GetTrackPos() * m_iBoxRow ; }	//回傳目前滑鼠點選的是第幾個空格
	int GetCheck() { return m_Check ; }										//回傳滑鼠目前所選Item的Index 
	int GetLastCheck() { return m_LastCheck ; }								//回傳滑鼠前一次所選Item的Index 
	void	   GetCellRect( int CellNum , RECT& Rect ) ;							//取得空格的RECT

	int GetMouseOnItemIndex( POINT& ptMouse ) ;	//取得目前滑鼠指向Item Index
	virtual void GetScreenOffset(POINT& ptOut,const POINT& ptScreen) ;
	
protected:

    CGrowableArray< DXUTIconListBoxItem* > m_Items;
	vector< RECT > m_CellsRect ;

    int  m_Check ;
	int  m_LastCheck ;
	int  m_ScrollBarTrackLastPos ;
	int  m_iItemWidth ;				// Cell的寬度
	int  m_iItemHeight;				// Cell的高度
	int  m_iBoxRow;					// IconListBox Cell一列的格子數      
    int  m_iBoxCol;					// IconListBox Cell一行的格子數
    int  m_iStaticWidth;			// Static的寬度
	bool m_bDrawText;				// 是否繪製文字
    bool m_bDragIcon;				// 可否拖曳
	bool m_bDrawStaticZero ;		// 是否畫static當文字為0的時候
};
//-----------------------------------------------------------------------------
// ImageBox control
//-----------------------------------------------------------------------------
class CDXUTIconImageBox : public CDXUTIconListBox
{
public:
    CDXUTIconImageBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTIconImageBox();

    virtual void    RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime ); // 畫出元件提示
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    bool HasItem() { return ( m_Items.GetSize() > 0 ) ? true : false; }

protected:
    virtual void UpdateRects();
};

#endif
