/*******************************************************************************

	Copyright 2006 by DYNASTY international Co., Ltd.
	All rights reserved.

	This software is the confidential and proprietary information of
	DYNASTY international Co., Ltd. ('Confidential Information').
	You shall not disclose such Confidential Information and shall
	use it only in accordance with the terms of the license agreement
	you entered into with DYNASTY international Co., Ltd.

*******************************************************************************/

#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
#include <strsafe.h>
#include <cmath>
#include <stdio.h>
#include "CDXUTTimer.h"
#include "FDOUT.h"
#include "PacketReader.h"
#include "JLAnimation2DManager.h"
//#include "HUISystemSet.h"
#include "J_SoundManager.h"
#include "iniReader.h"
#include "CFDOXML.h"
#include "DialogType.h"
#include "CDXUTDialog.h"
#pragma hdrstop
#include "CDXUTIconListBox.h"
#include "CDXUTDialogResourceManager.h"

using namespace FDO;

//--------------------------------------------------------------------------------------
// CDXUTIconListBox class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CDXUTIconListBox::CDXUTIconListBox( CDXUTDialog *pDialog ) :
      CDXUTListBox( pDialog )
{
    m_bDrag = false ;
	m_bDrawSelect = true;
    m_bDragIcon = true;
	m_bDrawStaticZero = false ;
    m_nBorder = 1;
    m_nMargin = 0;
    m_nSpace  = 1;
    m_iItemWidth = 32 ;
	m_iItemHeight  = 32 ;
    m_iStaticWidth = 30 ;
    m_bDrawText = true;
	m_Check = -1 ;
	m_LastCheck = -1 ;
	m_iBoxRow = 1 ;
	m_iBoxCol = 1 ;
	m_ScrollBarTrackLastPos = 0 ;
	m_bShadow = true ;
}

//--------------------------------------------------------------------------------------
CDXUTIconListBox::~CDXUTIconListBox()
{
    RemoveAllItems();
}

//--------------------------------------------------------------------------------------
// Insert Item
//--------------------------------------------------------------------------------------
HRESULT CDXUTIconListBox::InsertItem( int nIndex, const WCHAR *ImageFile, void *pData, int Amount )
{
    DXUTIconListBoxItem *pNewItem = new DXUTIconListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;

    // 設定Image
    pNewItem->Image.reset( new CDXUTIconImage(m_pDialog) );
    pNewItem->Image->SetSize( m_iItemWidth, m_iItemHeight ); // 設定大小(固定)
    pNewItem->Image->SetImage( ImageFile );

    // 設定Static
    pNewItem->Static.reset( new CDXUTStatic(m_pDialog) );
    m_pDialog->InitControl( pNewItem->Static.get() );
    pNewItem->Static->SetTextColor( 0xFF000000 ); // D3DCOLOR_ARGB( 255, 0, 40, 222 )
    pNewItem->Static->GetElement( 0 )->dwTextFormat = DT_RIGHT | DT_BOTTOM;
    pNewItem->Static->SetSize( m_iItemWidth - 2 , m_iItemWidth ); // 設定大小(固定)
    pNewItem->Static->SetText( Amount );
    if( !m_bDrawStaticZero )
	{
		if( Amount == 0 )
			pNewItem->m_bStatic = false;
		else
			pNewItem->m_bStatic = true;
	}
	else pNewItem->m_bStatic = true;

    HRESULT hr = m_Items.Insert( nIndex, pNewItem );
    if( SUCCEEDED( hr ) )
    {
        int i = ( m_Items.GetSize() % m_iBoxRow ? 1:0 );
        m_ScrollBar.SetTrackRange( 0, m_Items.GetSize()/m_iBoxRow + i );
    }
    else
        SAFE_DELETE( pNewItem );

    return hr;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTIconListBox::AddItem( DXUTIconListBoxItem *pItem )
{
    HRESULT hr = m_Items.Add( pItem );
    if( FAILED(hr) )
    {
        SAFE_DELETE( pItem );
    }
    else
    {
        int i = ( m_Items.GetSize() % m_iBoxRow ? 1:0 );
        m_ScrollBar.SetTrackRange( 0, m_Items.GetSize()/m_iBoxRow + i );
    }

    return hr;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTIconListBox::AddItem( const WCHAR *ImageFile, void *pData, int Amount, DWORD TextColor )
{
    DXUTIconListBoxItem *pNewItem = new DXUTIconListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;

    // 設定Image
    pNewItem->Image.reset( new CDXUTIconImage(m_pDialog) );
    pNewItem->Image->SetSize( m_iItemWidth , m_iItemHeight ); // 設定大小(固定)
    pNewItem->Image->SetImage( ImageFile );

    // 設定Static
    pNewItem->Static.reset( new CDXUTStatic(m_pDialog) );
    m_pDialog->InitControl( pNewItem->Static.get() );
    pNewItem->Static->SetTextColor( TextColor ); // D3DCOLOR_ARGB( 255, 0, 40, 222 )
    pNewItem->Static->GetElement( 0 )->dwTextFormat = DT_RIGHT | DT_BOTTOM;
    pNewItem->Static->SetSize( m_iItemWidth - 2 , m_iItemHeight ); // 設定大小(固定)
    pNewItem->Static->SetText( Amount );
    if( !m_bDrawStaticZero )
	{
		if( Amount == 0 )
			pNewItem->m_bStatic = false;
		else
			pNewItem->m_bStatic = true;
	}
	else pNewItem->m_bStatic = true ;

    return AddItem( pNewItem );
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTIconListBox::AddItem( const string &ImageFile, void *pData, int Amount, DWORD TextColor )
{
    WCHAR wImageFile[256];
    ANSItoUNICODE( wImageFile, ImageFile );

    return AddItem( wImageFile, pData, Amount, TextColor );
}
//--------------------------------------------------------------------------------------
DXUTIconListBoxItem *CDXUTIconListBox::GetItem( int nIndex )
{
    if( nIndex < 0 || nIndex >= m_Items.GetSize() )
        return NULL;

    return m_Items[nIndex];
}
//--------------------------------------------------------------------------------------
int CDXUTIconListBox::GetItemIndexByData( void *pData )
{
    for( int i = 0; i < m_Items.GetSize(); ++i )
    {
        DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
        if( pItem->pData == pData )
            return i;
    }
    return -1;
}
//--------------------------------------------------------------------------------------
//	設定ScrollBar移至List最後
//--------------------------------------------------------------------------------------
void CDXUTIconListBox::TrackLast()
{
	int i = ( m_Items.GetSize() % m_iBoxRow ? 1 : 0 ) ;
	int count = m_Items.GetSize() / m_iBoxRow ;
    if( count )
		m_ScrollBar.SetTrackPos( count + i ) ;
}
//--------------------------------------------------------------------------------------
//	取得空格的RECT
//--------------------------------------------------------------------------------------
void CDXUTIconListBox::GetCellRect( int CellNum , RECT& Rect )
{
	SetRectEmpty( &Rect ) ;
	
	if( CellNum < m_Items.GetSize() && CellNum > -1 )
	{
		POINT pt ;
		m_pDialog->GetLocation( pt ) ;
		Rect = m_CellsRect[ CellNum ] ;
		OffsetRect( &Rect , pt.x , pt.y + m_pDialog->GetCaptionHeight() ) ;	
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTIconListBox::GetScreenOffset(POINT& ptOut,const POINT& ptScreen) 
{
	RECT rcScreen ;
	GetCellRect( GetCheckCellIndex() , rcScreen ) ;
	ptOut.x = rcScreen.left - ptScreen.x ;
	ptOut.y = rcScreen.top - ptScreen.y ;
}
//--------------------------------------------------------------------------------------
int CDXUTIconListBox::GetMouseOnItemIndex( POINT& ptMouse )
{
	POINT ptDialog , ptCilentMouse ;
	m_pDialog->GetLocation( ptDialog ) ;
	ptCilentMouse.x = ptMouse.x - ptDialog.x ;
	ptCilentMouse.y = ptMouse.y - ptDialog.y ;
	int CaptionHeight = m_pDialog->GetCaptionHeight() ;
	ptCilentMouse.y -= CaptionHeight ;

	for( size_t i = 0 ; i < m_CellsRect.size() ; ++i )
	{
		if( PtInRect( &m_CellsRect[ i ] , ptCilentMouse ) )
		{
			int index = m_ScrollBar.GetTrackPos() * m_iBoxRow + ( int )i ;
			return index ;
		}
	}
	return -1 ;
}
//--------------------------------------------------------------------------------------
void CDXUTIconListBox::OnMouseEnter()
{
	m_LastCheck = -1 ;
	CDXUTControl::OnMouseEnter() ;
	//m_pDialog->SendEvent( EVENT_ICONLISTBOX_CELLCROSS , true , this ) ;
}
//--------------------------------------------------------------------------------------
void CDXUTIconListBox::OnMouseLeave()
{
	m_bDrag = false ;
	m_Check = -1 ;
	m_pDialog->SendEvent( EVENT_ICONLISTBOX_CELLCROSS , true , this ) ;	
	CDXUTControl::OnMouseLeave() ;
}
//--------------------------------------------------------------------------------------
void CDXUTIconListBox::UpdateRects()
{
	//每個Cell的寬度 * 行數 + Cell跟Cell間的間距( Space ) + 左右邊框( Border * 2 ) + 跟Scrollbar的間距( Margin ) + ScrollBar的寬度
	m_width = m_iBoxRow * m_iItemWidth + ( m_iBoxRow - 1 ) * m_nSpace + 2 * m_nBorder + m_nMargin + m_nSBWidth ;
	
	//每個Cell的高度 * 列數 + Cell跟Cell間的間距( Space ) + 上下編框( Bordar * 2 ) 
	m_height = m_iBoxCol * m_iItemHeight + ( m_iBoxCol - 1 ) * m_nSpace + 2 * m_nBorder ;

	CDXUTControl::UpdateRects();

	// Update the scrollbar's rects
	m_ScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth, m_rcBoundingBox.top ) ;
	m_ScrollBar.SetSize( m_nSBWidth , m_height ) ;

    m_rcSelection = m_rcBoundingBox ;
    m_rcSelection.right -= m_nSBWidth ;
    InflateRect( &m_rcSelection, -m_nBorder, -m_nBorder ) ;

	//設定Scrollbar的PageSize
	m_ScrollBar.SetPageSize( ( RectHeight( m_rcSelection ) ) / m_iItemHeight ) ;
	//m_ScrollBar.ShowItem( m_nSelected ) ;
    
	m_rcText = m_rcSelection ;
    InflateRect( &m_rcText, -m_nMargin, 0 ) ;

	m_CellsRect.clear() ;
	m_CellsRect.resize( m_iBoxRow * m_iBoxCol ) ;
	
	//設定 m_iBoxRow * m_iBoxCol 格的Rect
	RECT rect ;
	SetRectEmpty( &rect ) ;
	for( int i = 0 ; i < m_iBoxCol ; ++i )
	{
		rect.top = i * ( m_iItemHeight + m_nSpace ) + m_rcText.top ;
		rect.bottom = rect.top + m_iItemHeight ;
		for( int j = 0 ; j < m_iBoxRow ; ++j )
		{
			rect.left = j * ( m_iItemWidth + m_nSpace ) + m_rcText.left ; 
			rect.right = rect.left + m_iItemWidth ;
			m_CellsRect[ i * m_iBoxRow + j  ] = rect ;
		}
	}
}
//--------------------------------------------------------------------------------------
void CDXUTIconListBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if( m_bVisible == false )
        return;

    // 取出Element
	CDXUTElement* pElement = GetElement( 0 ) ;
    pElement->TextureColor.Blend( m_uCurState, fElapsedTime );
    // 畫出背景
    m_pDialog->DrawSprite( pElement, &m_rcText ) ;
	
	pElement = GetElement( 2 ) ;
	pElement->TextureColor.Blend( m_uCurState, fElapsedTime );
	for( size_t i = 0 ; i < m_CellsRect.size() ; ++i )
		m_pDialog->DrawSprite( pElement, &m_CellsRect[ i ] ) ;

    // 有Item才畫
    if( m_Items.GetSize() > 0 )
    {
        int i = m_ScrollBar.GetTrackPos() * m_iBoxRow ;
		int CellIndex = 0 ;
        for( ; i < m_Items.GetSize() && CellIndex < m_iBoxRow * m_iBoxCol ; ++i )
        {
            DXUTIconListBoxItem *pItem = m_Items.GetAt( i ) ;

			if( !pItem )
				continue ;

            pItem->m_Visible = true;

            // Determine if we need to render this item with the
            // selected element.
            bool bSelectedStyle = false;

            if( !( m_dwStyle & MULTISELECTION ) && i == m_nSelected )
                bSelectedStyle = true;
            else
            if( m_dwStyle & MULTISELECTION )
            {
                if( m_bDrag &&
                    ( ( i >= m_nSelected && i < m_nSelStart ) ||
                      ( i <= m_nSelected && i > m_nSelStart ) ) )
                    bSelectedStyle = m_Items[m_nSelStart]->bSelected;
                else
                if( pItem->bSelected )
                    bSelectedStyle = true;
            }

			RECT rcImage = m_CellsRect[ CellIndex ] ;
			CellIndex++ ;
			
            CDXUTElement* pElement = pItem->Image->GetElement(0); // 取得Image的Element
            pElement->TextureColor.Blend( m_uCurState, fElapsedTime ); // Blend處理
            m_pDialog->DrawSprite( pElement, &rcImage ); // 畫

            //畫出選擇的Item的遮罩
			if( bSelectedStyle && m_bDrawSelect )
            {
			    CDXUTElement* pSelElement = GetElement( 1 ) ;
				pSelElement->TextureColor.Blend( m_uCurState, fElapsedTime );
				pSelElement->FontColor.Blend( m_uCurState, fElapsedTime );
				m_pDialog->DrawSprite( pSelElement, &rcImage );
            }

            // 畫出數量
            if( pItem->m_bStatic )
            {
                pElement = pItem->Static->GetElement(0);
                rcImage.right = rcImage.left + m_iStaticWidth;
                pElement->FontColor.Blend( m_uCurState, fElapsedTime );
                m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcImage , m_bShadow ); 

				//srand((int)this);

            }
        }
    }
    // Render the scroll bar
    m_ScrollBar.Render( pd3dDevice, fElapsedTime );
}

//--------------------------------------------------------------------------------------
void CDXUTIconListBox::RemoveAllItems()
{
    m_pDialog->Refresh();
    m_Items.RemoveAll();
	m_nSelected = -1 ;
    m_ScrollBar.SetTrackRange( 0, 1 );
}

//--------------------------------------------------------------------------------------
void CDXUTIconListBox::RemoveItem( const int &nIndex )
{
    if( nIndex < 0 || nIndex >= m_Items.GetSize() )
        return;

	m_Items.Remove( nIndex ) ;
	if( nIndex == m_nSelected )
		m_nSelected = -1 ;
    int i = ( m_Items.GetSize() % m_iBoxRow ? 1 : 0 ) ;
    m_ScrollBar.SetTrackRange( 0 , m_Items.GetSize() / m_iBoxRow + i ) ;
}

//--------------------------------------------------------------------------------------
// 只移除指標而不刪除實體
//--------------------------------------------------------------------------------------
void CDXUTIconListBox::RemoveItemPointer( const int &nIndex )
{
    m_Items.Remove( nIndex );
    int i = ( m_Items.GetSize() % m_iBoxRow ? 1:0 );
    m_ScrollBar.SetTrackRange( 0, m_Items.GetSize()/m_iBoxRow + i );
	m_nSelected = -1 ;
	//if( m_nSelected >= m_Items.GetSize() )
        //m_nSelected = m_Items.GetSize() - 1;

    //m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
}
//--------------------------------------------------------------------------------------
void CDXUTIconListBox::RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    // 畫出提示(Hint)
    if( m_bMouseOver && m_Check != -1 && !MultipleHint.empty() )
    {
        if( m_Check < m_Items.GetSize() && (wcslen( MultipleHint[ m_Check ].m_strHint ) || wcslen( MultipleHint[ m_Check ].m_strHintImgPath ) ) )
        {
            DXUTIconListBoxItem *pItem = m_Items.GetAt( m_Check );
            CDXUTElement* pElement = pItem->Image->GetElement( 0 );
            pElement->FontColor.Current = D3DCOLOR_ARGB(255, 255, 255, 255);
			pElement->dwTextFormat = DT_LEFT;
			m_pDialog->RenderHintAll( m_cxWordMaxExtent, m_HintTextColor, m_FrameColor, m_BackgroundColor, pElement, m_MousePoint , MultipleHint[ m_Check ].m_strHint, MultipleHint[ m_Check ].m_strHintImgPath, m_bHintTurnRight );
        }     
    }
}
//--------------------------------------------------------------------------------------
bool CDXUTIconListBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if (!m_bEnabled || !m_bVisible)
        return false;

	if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
        return true ;

	m_MousePoint = pt ;

    switch( uMsg )
    {
        case WM_MOUSEMOVE :

			if( m_bDragIcon && m_bDrag && m_nSelected != -1 )
			{
				m_bDrag = false ;
				DXUTIconListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
                CDXUTControl::s_pDragSource = this; // 設定拖曳來源
                CDXUTControl::s_SrcIndex = m_nSelected;
                CDXUTControl::s_pDragTexture = pSelItem->Image->GetTexture(0); // 設定拖曳材質
				m_pDialog->SendEvent( EVENT_CONTROL_DRAG, true, this );
				return true ;
			}

			if( m_Items.GetSize() > 0 && PtInRect( &m_rcText, pt )  )
			{
				for( size_t i = 0 ; i < m_CellsRect.size() ; ++i )
				{
					if( PtInRect( &m_CellsRect[ i ] , pt ) )
					{
						m_Check = m_ScrollBar.GetTrackPos() * m_iBoxRow + i ;
						break ;
					}
					else m_Check = -1 ;
				}
			}
			else m_Check = -1 ;

			if( m_LastCheck != m_Check )
			{
				m_LastCheck = m_Check ;
				m_pDialog->SendEvent( EVENT_ICONLISTBOX_CELLCROSS , true , this ) ;
				return true ;
			}
            break;
	
		case WM_RBUTTONDOWN: // 右鍵
        case WM_RBUTTONDBLCLK:
            
			// 有Item並且確認點擊範圍
            if( m_Items.GetSize() > 0 && PtInRect( &m_rcSelection, pt ) )
            {
                // Only proceed if the click falls on top of an item.
                if( m_Check < m_Items.GetSize() )
                {
                    SetCapture( DXUTGetHWND() );

                    // If this is a double click, fire off an event and exit
                    // since the first click would have taken care of the selection
                    // updating.
                    if( uMsg == WM_RBUTTONDBLCLK )
                    {
                        m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_RDBLCLK, true, this );
                        return true;
                    }

                    // 取消前次選取
                    if( m_nSelected != -1 )
                    {
                        if( m_nSelected < m_Items.GetSize() )
                        {
                            DXUTIconListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
                            pSelItem->bSelected = false;
                        }
                        else
                        {
                            m_nSelected = -1;
                        }
                    }
                    
                    m_nSelected = m_Check ;
                    
					if( !( wParam & MK_SHIFT ) )
                        m_nSelStart = m_nSelected;

                    // If this is a multi-selection listbox, update per-item
                    // selection data.

                    // 複選
                    if( m_dwStyle & MULTISELECTION )
                    {
                        // Determine behavior based on the state of Shift and Ctrl

                        DXUTIconListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );

                        if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_CONTROL )
                        {
                            // Control click. Reverse the selection of this item.

                            pSelItem->bSelected = !pSelItem->bSelected;
                        } 
						else if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_SHIFT )
                        {
                            // Shift click. Set the selection for all items
                            // from last selected item to the current item.
                            // Clear everything else.

                            int nBegin = __min( m_nSelStart, m_nSelected );
                            int nEnd = __max( m_nSelStart, m_nSelected );

                            for( int i = 0; i < nBegin; ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = false;
                            }

                            for( int i = nEnd + 1; i < (int)m_Items.GetSize(); ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = false;
                            }

                            for( int i = nBegin; i <= nEnd; ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = true;
                            }
                        } 
						else if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == ( MK_SHIFT|MK_CONTROL ) )
                        {
                            // Control-Shift-click.

                            // The behavior is:
                            //   Set all items from m_nSelStart to m_nSelected to
                            //		the same state as m_nSelStart, not including m_nSelected.
                            //   Set m_nSelected to selected.

                            int nBegin = __min( m_nSelStart, m_nSelected );
                            int nEnd = __max( m_nSelStart, m_nSelected );

                            // The two ends do not need to be set here.

                            bool bLastSelected = m_Items.GetAt( m_nSelStart )->bSelected;
                            for( int i = nBegin + 1; i < nEnd; ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = bLastSelected;
                            }

                            pSelItem->bSelected = true;

                            // Restore m_nSelected to the previous value
                            // This matches the Windows behavior

                            m_nSelected = m_nSelStart;
                        } 
						else
                        {
                            // Simple click.  Clear all items and select the clicked
                            // item.


                            for( int i = 0; i < m_Items.GetSize(); ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = false;
                            }

                            pSelItem->bSelected = true;
                        }
                    }  // End of multi-selection case
                    else 
                    { // 單選
						if( m_nSelected != -1 )
						{
							DXUTIconListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
							pSelItem->bSelected = true;
							m_pDialog->SendEvent( EVENT_LISTBOX_RSELECTION, true, this );	
						}
                    }
                }
                return true;
            }
            break;
        
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            // Check for clicks in the text area
            // 有Item並且確認點擊範圍
            if( m_Items.GetSize() > 0 && PtInRect( &m_rcSelection, pt ) )
            {
                //音效 *** modify
                PlaySoundWAV();  //under control class

                // Only proceed if the click falls on top of an item.
                if( m_Check < m_Items.GetSize() )
                {
                    SetCapture( DXUTGetHWND() );
                    m_bDrag = true;

                    // If this is a double click, fire off an event and exit
                    // since the first click would have taken care of the selection
                    // updating.
                    if( uMsg == WM_LBUTTONDBLCLK )
                    {
                        m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
                        return true;
                    }

                    // 取消前次選取
                    if( m_nSelected != -1 )
                    {
                        if( m_nSelected < m_Items.GetSize() )
                        {
                            DXUTIconListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
                            pSelItem->bSelected = false;
                        }
                        else
                        {
                            m_nSelected = -1;
                        }
                    }

                    m_nSelected = m_Check ;
                    if( !( wParam & MK_SHIFT ) )
                        m_nSelStart = m_nSelected;

                    // If this is a multi-selection listbox, update per-item
                    // selection data.

                    // 複選
                    if( m_dwStyle & MULTISELECTION )
                    {
                        // Determine behavior based on the state of Shift and Ctrl

                        DXUTIconListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );

                        if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_CONTROL )
                        {
                            // Control click. Reverse the selection of this item.

                            pSelItem->bSelected = !pSelItem->bSelected;
                        } 
						else if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_SHIFT )
                        {
                            // Shift click. Set the selection for all items
                            // from last selected item to the current item.
                            // Clear everything else.

                            int nBegin = __min( m_nSelStart, m_nSelected );
                            int nEnd = __max( m_nSelStart, m_nSelected );

                            for( int i = 0; i < nBegin; ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = false;
                            }

                            for( int i = nEnd + 1; i < m_Items.GetSize(); ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = false;
                            }

                            for( int i = nBegin; i <= nEnd; ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = true;
                            }
                        } 
						else if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == ( MK_SHIFT|MK_CONTROL ) )
                        {
                            // Control-Shift-click.

                            // The behavior is:
                            //   Set all items from m_nSelStart to m_nSelected to
                            //     the same state as m_nSelStart, not including m_nSelected.
                            //   Set m_nSelected to selected.

                            int nBegin = __min( m_nSelStart, m_nSelected );
                            int nEnd = __max( m_nSelStart, m_nSelected );

                            // The two ends do not need to be set here.

                            bool bLastSelected = m_Items.GetAt( m_nSelStart )->bSelected;
                            for( int i = nBegin + 1; i < nEnd; ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = bLastSelected;
                            }

                            pSelItem->bSelected = true;

                            // Restore m_nSelected to the previous value
                            // This matches the Windows behavior

                            m_nSelected = m_nSelStart;
                        } 
						else
                        {
                            // Simple click.  Clear all items and select the clicked
                            // item.
                            for( int i = 0; i < m_Items.GetSize(); ++i )
                            {
                                DXUTIconListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = false;
                            }

                            pSelItem->bSelected = true;
                        }
                    }  // End of multi-selection case
                    else
                    { // 單選
						if( m_nSelected != -1 )
						{
							DXUTIconListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
							pSelItem->bSelected = true;
							m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );	
						}
					}
					
                }
                return true;
            }
            break;

        case WM_RBUTTONUP:
        {
            ReleaseCapture();
            m_bDrag = false;
            return true;
        }
		break ;

        
        case WM_LBUTTONUP:
        {  
			m_bDrag = false;
			ReleaseCapture();
            
            return true;
        }
		break ;

        case WM_MOUSEWHEEL :

            UINT uLines;
            SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
            int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
            m_ScrollBar.Scroll( -nScrollAmount );
			if( m_ScrollBarTrackLastPos != m_ScrollBar.GetTrackPos() )
			{
				if( m_Check != -1 )	
					m_Check += ( m_ScrollBar.GetTrackPos() - m_ScrollBarTrackLastPos ) * m_iBoxRow ;
				m_ScrollBarTrackLastPos = m_ScrollBar.GetTrackPos() ;
				m_pDialog->SendEvent( EVENT_ICONLISTBOX_CELLCROSS , true , this ) ;
				return true;
			}
			break ;
    }

    return false;
}
//-----------------------------------------------------------------------------
// implementation of CDXUTImage
//-----------------------------------------------------------------------------
CDXUTIconImage::CDXUTIconImage( CDXUTDialog *pDialog )
{
    m_Type = DXUT_CONTROL_IMAGE;
    m_pDialog = pDialog;

    /*for( int i=0; i < m_Elements.GetSize(); i++ )
    {
        CDXUTElement* pElement = m_Elements.GetAt( i );
        SAFE_DELETE( pElement );
    }
    m_Elements.RemoveAll();*/
}

//-----------------------------------------------------------------------------
void CDXUTIconImage::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

    if( m_bVisible == false )
        iState = DXUT_STATE_HIDDEN;
    //else if( m_bEnabled == false )
    //    iState = DXUT_STATE_DISABLED;
    else if( m_bPressed )
        iState = DXUT_STATE_PRESSED;
    else if( m_bMouseOver )
        iState = DXUT_STATE_MOUSEOVER;
    else if( m_bHasFocus )
        iState = DXUT_STATE_FOCUS;

    if( m_Elements.size() > 0 )
    {
		CDXUTElement* pElement = GetElement( 0 ) ;

        float fBlendRate = ( iState == DXUT_STATE_PRESSED ) ? 0.0f : 0.8f;

        // Blend current color
        pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
        pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

        // here it is
        m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );
        //m_pDialog->DrawText( m_strText, pElement, &m_rcBoundingBox );
    }
}

//-----------------------------------------------------------------------------
IDirect3DTexture9* CDXUTIconImage::GetTexture( int iTexture )
{
    CDXUTElement* pElement = GetElement( iTexture );
    if( !pElement )
        return NULL;

    CDXUTDialogResourceManager* pManager = m_pDialog->GetManager(); // 取得資源管理員
    if( !pManager )
        return NULL;

    DXUTTextureNode* pTextureNode = pManager->GetTextureNode( pElement->iTexture );
    if( !pTextureNode )
        return NULL;
        
    return pTextureNode->pTexture;
}
//-----------------------------------------------------------------------------
void CDXUTIconImage::SetColor( D3DCOLOR color )
{
    /*if( m_Elements.GetSize() <= 0 )
    {
        CDXUTElement Element;
        SetElement( 0, &Element );
    }

    CDXUTElement* pElement = m_Elements.GetAt( 0 );

    pElement->TextureColor.States[DXUT_STATE_NORMAL] = color;*/
	CDXUTButton::SetColor( color ) ;
}

//-----------------------------------------------------------------------------
bool CDXUTIconImage::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return false;
}
//-----------------------------------------------------------------------------
bool CDXUTIconImage::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    m_MousePoint = pt;
    return false;
}
//-----------------------------------------------------------------------------
// implementation of CDXUTImageBox
//-----------------------------------------------------------------------------
CDXUTIconImageBox::CDXUTIconImageBox( CDXUTDialog *pDialog )
{
    m_Type = DXUT_CONTROL_IMAGEBOX;
    m_pDialog = pDialog;


    /*for( int i=0; i < m_Elements.GetSize(); i++ )
    {
        CDXUTElement* pElement = m_Elements.GetAt( i );
        SAFE_DELETE( pElement );
    }
    m_Elements.RemoveAll();*/
}

//-----------------------------------------------------------------------------
CDXUTIconImageBox::~CDXUTIconImageBox()
{

}
//--------------------------------------------------------------------------------------
void CDXUTIconImageBox::RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    // 畫出提示(Hint)
    if( m_bMouseOver && (wcslen( _GetHint() ) || wcslen( _GetHintImagePath()) ) )
    {

        //int iTexture;
		CDXUTElement* pElement = GetElement( 0 ) ;
        pElement->FontColor.Current = D3DCOLOR_ARGB(255, 255, 255, 255);
		pElement->dwTextFormat = DT_LEFT;

        m_pDialog->RenderHintAll( m_cxWordMaxExtent, m_HintTextColor, m_FrameColor, m_BackgroundColor, pElement, m_MousePoint, _GetHint(), _GetHintImagePath(), m_bHintTurnRight );
		/*RECT rcText, rcTexture, rcBack;

        SetRectEmpty( &rcText );
        SetRectEmpty( &rcTexture );
        SetRectEmpty( &rcBack );

        if( wcslen( m_strHintImgPath ) )
        {
            RECT *rcCatch = 0;
            iTexture = m_pDialog->GetManager()->AddTexture( m_strHintImgPath );
            pTextureElement.SetTexture( iTexture, rcCatch );
            pTextureElement.TextureColor.Current = D3DCOLOR_ARGB( 200, 255, 255, 255 );

            SetRect( &rcTexture, 36, 36, 100, 100 );
            OffsetRect( &rcTexture, m_MousePoint.x + 5, m_MousePoint.y + 5 );

            m_pDialog->CalcTextRect( m_strHint, pElement, &rcText );
            wstring str = m_strHint;
            int StrLong = str.length();
            int W = (StrLong/10)*12;
            SetRect( &rcText, 5, 143, 133, 138+W );
            InflateRect( &rcText, 5, 5 );
            OffsetRect( &rcText, m_MousePoint.x + 5, m_MousePoint.y + 5 );

            SetRect( &rcBack, 0, 0, RectWidth( rcTexture )*2 + 10, ( RectHeight( rcTexture )*2 + RectHeight( rcText ) + 25 ) );
            OffsetRect( &rcBack, m_MousePoint.x + 5, m_MousePoint.y + 5 );

            // 產生 wszText 的 TextObject，並指定給該 Item。
            LONG cxMaxExtent = 25 * 6; // 12 chars * cxChar
            CTextObject* pTextObject = g_FDOXML.MakeTextObject(
                g_pD3DDevice, NULL, D3DCOLOR_ARGB( 255, 255, 255, 255 ), m_strHint, -1, cxMaxExtent);
            assert(pTextObject);

            POINT Pt;
            m_pDialog->GetLocation( Pt );
            if( rcBack.bottom > m_pDialog->GetHeight() )
            {
                OffsetRect( &rcText, 0, -RectHeight( rcText ) - 160 );
                OffsetRect( &rcTexture, 0, -RectHeight( rcTexture )*2 - W-35 );
                OffsetRect( &rcBack, 0, -RectHeight( rcBack ) - 10 );
            }
            if( rcBack.right > m_pDialog->GetWidth() )
            {
                OffsetRect( &rcText, -RectWidth(rcText)-10, 0 );
                OffsetRect( &rcTexture, -RectWidth( rcTexture )*2 - 20, 0 );
                OffsetRect( &rcBack, -RectWidth( rcBack )-10, 0 );
            }

            m_pDialog->DrawRect( &rcBack, D3DCOLOR_ARGB( 150, 0, 0, 0 ) );
            m_pDialog->DrawSprite( &pTextureElement, &rcTexture );
            m_pDialog->DrawTextObject( pTextObject, pElement, &rcText, false );
            delete pTextObject;
        }
        else
        {
            // 產生 wszText 的 TextObject，並指定給該 Item。
            LONG cxMaxExtent = 25 * 6; // 12 chars * cxChar
            CTextObject* pTextObject = g_FDOXML.MakeTextObject(
                g_pD3DDevice, NULL, D3DCOLOR_ARGB(255, 255, 255, 255), m_strHint, -1, cxMaxExtent);
            assert(pTextObject);
			
			SIZE Extent;
            pTextObject->GetExtent( Extent );
			
            SetRect( &rcText, 5, 5, Extent.cx+5, Extent.cy+5 );
			InflateRect( &rcText, 5, 5 );
            OffsetRect( &rcText, m_MousePoint.x+RectWidth(rcText)/2, m_MousePoint.y-RectHeight(rcText) );

            

            POINT Pt;
            m_pDialog->GetLocation( Pt );
            if( rcText.top + Pt.y < 0 )
                OffsetRect( &rcText, -RectWidth(rcText), RectHeight(rcText)*2 );
            else if( rcText.right > m_pDialog->GetWidth() )
                OffsetRect( &rcText, -RectWidth(rcText), 0 );

            m_pDialog->DrawRect( &rcText, D3DCOLOR_ARGB(150, 0, 0, 0) );
            m_pDialog->DrawTextObject( pTextObject, pElement, &rcText, false );
			delete pTextObject;
        }*/

    }
}

//-----------------------------------------------------------------------------
void CDXUTIconImageBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    CDXUTElement* pElement = 0;

    // 畫出ImageBox的底圖
    if( m_Elements.size() > 0 )
    {
        pElement = GetElement(0);
        pElement->TextureColor.Current = D3DXCOLOR(0,0,0,0);
        m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );
    }

    // 畫出ImageBox上的Item
    //if( m_Items.GetSize() > 0 )
    {
        DXUTIconListBoxItem *pItem = m_Items.GetAt( 0 );

        // 這裡處理Image
        pElement = pItem->Image->GetElement(0); // 取得Image的Element
        pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime ); // Blend處理
        //pElement->TextureColor.Current = D3DXCOLOR( 255, 255, 255, 255 );
        m_pDialog->DrawSprite( pElement, &m_rcBoundingBox ); // 畫

        // 畫出數量
        if( pItem->m_bStatic )
        {
            pElement = pItem->Static->GetElement(0);

            D3DXCOLOR clrOriginal = pElement->FontColor.Current;
            pElement->FontColor.Current = 0xFFFFFFFF;

            RECT rcTemp = m_rcBoundingBox;
            rcTemp.top -= 1;
            rcTemp.bottom -= 1;
            m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcTemp );

            rcTemp = m_rcBoundingBox;
            rcTemp.top += 1;
            rcTemp.bottom += 1;
            m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcTemp );

            rcTemp = m_rcBoundingBox;
            rcTemp.left -= 1;
            rcTemp.right -= 1;
            m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcTemp );

            rcTemp = m_rcBoundingBox;
            rcTemp.left += 1;
            rcTemp.right += 1;
            m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcTemp );

            pElement->FontColor.Current = clrOriginal;
            pElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
            m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &m_rcBoundingBox ); // 畫
        }
    }
}

//-----------------------------------------------------------------------------
bool CDXUTIconImageBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    m_MousePoint = pt;
    
    // First acquire focus
    if( WM_LBUTTONDOWN == uMsg )
    {
        CDXUTControl::s_pDragSource = NULL; // 設定拖曳來源
        CDXUTControl::s_SrcIndex = -1;
        CDXUTControl::s_DestIndex = -1;
        CDXUTControl::s_pDragTexture = NULL;

        //if( !m_bHasFocus )
        //    m_pDialog->RequestFocus( this );
    }
    
    switch( uMsg )
    {
        case WM_RBUTTONDOWN: // 右鍵
        case WM_RBUTTONDBLCLK: // 右鍵雙擊
            if( m_Items.GetSize() > 0 )// && PtInRect( &m_rcSelection, pt ) )
            {
                SetCapture( DXUTGetHWND() );
                m_bDrag = true;

                // If this is a double click, fire off an event and exit
                // since the first click would have taken care of the selection
                // updating.
                if( uMsg == WM_RBUTTONDBLCLK )
                {
                    m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_RDBLCLK, true, this );
                    return true;
                }

                m_nSelected = 0;
                DXUTIconListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
                pSelItem->bSelected = true;

                m_pDialog->SendEvent( EVENT_LISTBOX_RSELECTION, true, this );
                return true;
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:

            CDXUTControl::s_pDragSource = NULL; // 設定拖曳來源
            CDXUTControl::s_SrcIndex = -1;
            CDXUTControl::s_pDragTexture = NULL;

            // Check for clicks in the text area
            // 有Item並且確認點擊範圍
            if( m_Items.GetSize() > 0 )//&& PtInRect( &m_rcSelection, pt ) )
            {
                SetCapture( DXUTGetHWND() );
                m_bDrag = true;

                // If this is a double click, fire off an event and exit
                // since the first click would have taken care of the selection
                // updating.
                if( uMsg == WM_LBUTTONDBLCLK )
                {
                    m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
                    return true;
                }
                
                m_nSelected = 0;  
                DXUTIconListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
                pSelItem->bSelected = true;            

                //音效 *** modify
                PlaySoundWAV();  //under control class

                m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );                

                return true;
            }
            break;

        case WM_LBUTTONUP:
        {
            ReleaseCapture();
            m_bDrag = false;

            // Check for clicks in the text area
            // 有Item並且確認點擊範圍
            if( m_Items.GetSize() > 0 )//&& PtInRect( &m_rcSelection, pt ) )
            {
                CDXUTControl::s_DestIndex = 0;
            }

            if( CDXUTControl::s_pDragSource == this &&
                CDXUTControl::s_SrcIndex == CDXUTControl::s_DestIndex );
            else
            {
                if( CDXUTControl::s_pDragSource != NULL &&
                    CDXUTControl::s_SrcIndex >= 0 &&
                    CDXUTControl::s_SrcIndex < ((CDXUTIconImageBox*)CDXUTControl::s_pDragSource)->GetSize() )
                {
                    m_pDialog->SendEvent( EVENT_ITEM_DROP, true, this );
					//m_pDialog->SendEvent( EVENT_CONTROL_DROP, true, this );
                }
            }

            return false;
        }

        case WM_RBUTTONUP:
        {
            ReleaseCapture();
            m_bDrag = false;
            
            return false;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
void CDXUTIconImageBox::UpdateRects()
{
    SetRect( &m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height );
}   
