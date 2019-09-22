//
// VariableHeightListBox.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//-----------------------------------------------------------------------------------------
#include "CDXUTDialog.h"
#include "CDXUTDialogResourceManager.h"
#include "CDXUTListBox.h"
#include "ListBoxItem.h"
#include "CFDOXML.h"
#include "TextObject.h"
#include <assert.h>
//#include "DrawPlane.h"
#include "ImageControls.h"
//#include "Global.h"
#include "Timer.h"
#include "Common.h"
#pragma hdrstop
#include "VariableHeightListBox.h"
using namespace FDO;

//=========================================================================================
// Constants
//=========================================================================================

static const int _SMOOTH_SCROLL_INTERVAL = 20;
const bool CVariableHeightListBox::sm_bSmoothScroll = true;
//=========================================================================================
// Constructor/Destructor
//=========================================================================================

CVariableHeightListBox::CVariableHeightListBox(CDXUTDialog *pDialog)
: CDXUTListBox(pDialog)
{
    m_iOldRenderingTrackPos = 0;
    m_cyScrollUp = 0;
    m_bSmoothScroll = false;
    m_bAddItem = false;
    m_nNewItems = 0;
    m_bAlignBottom = false;
    m_dwDefaultColor = WHITE;
}

CVariableHeightListBox::~CVariableHeightListBox()
{
}

//=========================================================================================
// Default Color
//=========================================================================================

void CVariableHeightListBox::SetDefaultColor(DWORD dwColor)
{
    m_dwDefaultColor = dwColor;
}

//=========================================================================================
// Alignment (text and inline-image)
//=========================================================================================

void CVariableHeightListBox::SetAlignBottom(bool bAlign)
{
    m_bAlignBottom = bAlign;
}

//=========================================================================================
// ListBox item operations (polymorphism)
//=========================================================================================

HRESULT CVariableHeightListBox::AddItem( const wstring &strText, D3DXCOLOR FontColor, void *pData )
{
    return AddItem( strText.c_str(), pData, FontColor );
}

HRESULT CVariableHeightListBox::AddItem( const WCHAR *szText, void *pData, D3DXCOLOR FontColor )
{
    g_FDOXML.SetDefaultColor(m_dwDefaultColor);


    // 計算 szText 的 TextObject。
    /*CTextObject* pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, m_Elements.GetAt( 0 )->iFont , FontColor, szText, -1, RectWidth(m_rcText));*/
	CTextObject* pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, GetElement( 0 )->iFont , FontColor, szText, -1, RectWidth(m_rcText));
    assert(pTextObject);

    // 對每個 LineObject 產生一個相對應的 Item，並指定該 LineObject 給該 Item。
    std::wstring strTest;
    std::list<CLineObject>::iterator it = pTextObject->m_Lines.begin();
    m_nNewItems = 0;
    for (; it != pTextObject->m_Lines.end(); it++) {
        //CDXUTListBox::AddItem(szText, pData, FontColor);
        //DXUTListBoxItem *pNewItem = m_Items.GetAt(m_Items.GetSize()-1);

		
		DXUTListBoxItem* pNewItem = new DXUTListBoxItem ;
		m_Items.Add( pNewItem );
		pNewItem->pData = pData;
		pNewItem->FontColor = FontColor;
		
        assert(pNewItem);
        pNewItem->LineObject = *it;
        m_nNewItems++;
    } // end for    
    
    delete pTextObject;
    if (0 == m_nNewItems)
        return S_OK;
    
    UpdateTrackRange();

    // 平滑捲動判定。
    //if (true == sm_bSmoothScroll)
        InAddItemDecideIfSmoothScrolling(m_nNewItems);
    //else m_bSmoothScroll = false;

    return S_OK;
}

void CVariableHeightListBox::InAddItemDecideIfSmoothScrolling(int nNewItems)
// 平滑捲動判定 => m_bSmoothScroll
{
    // 如果平滑捲動還沒捲完，重設。
    m_bAddItem = true;
    if (m_cyScrollUp) {
        m_cyScrollUp = 0;
        m_bSmoothScroll = false;
        return;
    }    
    
    // 平滑捲動只發生在新增訊息的時候。
    m_bSmoothScroll = true;

    // 平滑捲動只發生在 TrackRange.End-(nNewItems+1) 位於 m_rcText 裡面，End-nNewItems 在外面的時候。
    int nStart, nEnd;
    m_ScrollBar.GetTrackRange(nStart, nEnd);
    int iItemBottom = m_rcText.top;
    int i = m_iRenderingTrackPos;
    for (; i <= nEnd-(nNewItems+1); i++) {
        DXUTListBoxItem *pItem = m_Items.GetAt(i);
        SIZE size;
        pItem->LineObject.GetExtent(size);
        iItemBottom += size.cy;
        if (iItemBottom > m_rcText.bottom) {
            m_bSmoothScroll = false;
            break;
        } // end if
    } // end for
    
    if (m_bSmoothScroll) {
        DXUTListBoxItem *pItem = m_Items.GetAt(i);
        SIZE size;
        pItem->LineObject.GetExtent(size);
        iItemBottom += size.cy;
        if (iItemBottom < m_rcText.bottom) {
            m_bSmoothScroll = false;
        } // end if
    } // end if
}

/*
HRESULT CVariableHeightListBox::AddItem( const WCHAR *szText, void *pData, D3DXCOLOR FontColor )
{   
    // 計算 szText 的 TextObject。
    RECT rcClient;
    CDXUTListBox::GetClientRect(rcClient);
    CTextObject* pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, szText, -1, RectWidth(rcClient));
    assert(pTextObject);

    // 對每個 LineObject 產生一個相對應的 Item，並指定給它。
    std::wstring strTest;
    std::list<CLineObject>::iterator it = pTextObject->m_Lines.begin();
    for (; it != pTextObject->m_Lines.end(); it++) {
        CElement elem = (*it).CurrentElement();
        strTest = elem.GetText();
        // Create new item and add to m_Items.
        DXUTListBoxItem *pNewItem = new DXUTListBoxItem;
        assert(pNewItem);
        pNewItem->pData = pData;
        pNewItem->FontColor = FontColor;
        pNewItem->LineObject = *it;
        elem = pNewItem->LineObject.CurrentElement();
        strTest = elem.GetText();
        HRESULT hr = m_Items.Add(pNewItem);
        assert(!FAILED(hr));
    } // end for
    
    delete pTextObject;
    UpdateTrackRange();

    return S_OK;
}
*/

CLineObject& CVariableHeightListBox::GetLastLineObject()
{
    int nItems = GetSize();
    DXUTListBoxItem* pItem = GetItem(nItems - 1);
    assert(pItem);
    return (pItem->LineObject);
}

void CVariableHeightListBox::DrawLineObject(CLineObject* pLineObject, CDXUTElement* pElement, RECT* pDestRect)
{
    assert(pLineObject);
    assert(pElement);
    assert(pDestRect);
    
    if (m_bAlignBottom) {
        pElement->dwTextFormat &= ~DT_TOP;
        pElement->dwTextFormat &= ~DT_VCENTER;
        pElement->dwTextFormat |= DT_BOTTOM;
    } else { // Not align-bottom. Assuming there is a valid alignment flag in dwTextFormat.
        pElement->dwTextFormat &= ~DT_BOTTOM;
    }
    m_pDialog->DrawLineObject(pLineObject, pElement, pDestRect);
}

//=========================================================================================
// CDXUTControl overridings
//=========================================================================================

/*
void CVariableHeightListBox::UpdateRenderingTrackPos()
// 非平滑捲動版
{
    //========================================================================
    // 如果 ScrollBar 上的滑鼠被按住，不要被新訊息拉回。
    //========================================================================
    if (true == m_ScrollBar.IsArrowUpHeld()) {
        // ArrowUp button held, only scroll-up allowed.
        int iPos = m_ScrollBar.GetTrackPos();
        if (m_iRenderingTrackPos > iPos)
            m_iRenderingTrackPos = iPos;
        else m_ScrollBar.SetTrackPos(m_iRenderingTrackPos);
    } 
    else if (m_ScrollBar.IsThumbHeld()) {
        // The thumb-button was held without moving, use old track pos.
        m_ScrollBar.SetTrackPos(m_iRenderingTrackPos);
    } 
    else {
        // Arrow is not held or held on DownButton, use new track pos.
        m_iRenderingTrackPos = m_ScrollBar.GetTrackPos();
    }
}
*/

void CVariableHeightListBox::UpdateRenderingTrackPos()
// 平滑捲動版
{
    // 如果平滑捲動還沒捲完，而且未新增 Item，不更新捲動位置。
    if (m_cyScrollUp > 0 && false == m_bAddItem) {
        return;
    }
    m_bAddItem = false;
    // Save RenderingTrackPos for smooth scrolling (how much to scroll up).
    m_iOldRenderingTrackPos = m_iRenderingTrackPos;
    //========================================================================
    // 如果 ScrollBar 上的滑鼠被按住，不要被新訊息拉回。
    //========================================================================
    if (true == m_ScrollBar.IsArrowUpHeld()) {
        m_bSmoothScroll = false;
        // ArrowUp button held, only scroll-up allowed.
        int iPos = m_ScrollBar.GetTrackPos();
        if (m_iRenderingTrackPos > iPos)
            m_iRenderingTrackPos = iPos;
        else m_ScrollBar.SetTrackPos(m_iRenderingTrackPos);
    } 
    else if (m_ScrollBar.IsThumbHeld()) {
        m_bSmoothScroll = false;
        // The thumb-button was held without moving, use old track pos.
        m_ScrollBar.SetTrackPos(m_iRenderingTrackPos);
    } 
    else {
        // Arrow is not held or held on DownButton, use new track pos.
        m_iRenderingTrackPos = m_ScrollBar.GetTrackPos();
    }
    //========================================================================
    // Calc ScrollUp Height 更新平滑捲動量
    //========================================================================
    int iPos = m_iRenderingTrackPos - m_nNewItems;
    if (m_bSmoothScroll && iPos >= 0) {
        m_cyScrollUp = 0;
        m_cyScrolled = 0;
        for (int i = iPos; i < m_iRenderingTrackPos; i++) {
            DXUTListBoxItem *pItem = m_Items.GetAt(iPos);
            SIZE size;
            pItem->LineObject.GetExtent(size);
            m_cyScrollUp += size.cy; // or: m_cyScrollUp += m_nTextHeight;
        } // end for
    } // end if
}

void CVariableHeightListBox::RenderListBox(IDirect3DDevice9* pd3dDevice, float fElapsedTime)
// 平滑捲動版
{
    //========================================================================
    // 從 m_iRenderingTrackPos 開始更新 ListBox 內容。
    //========================================================================
    //CDXUTElement* pElement = m_Elements.GetAt( 0 );
	CDXUTElement* pElement = GetElement( 0 ) ;
    //CDXUTElement* pSelElement = m_Elements.GetAt( 1 );
	CDXUTElement* pSelElement = GetElement( 1 ) ;

    RECT rcSel = m_rcSelection;
    RECT rcItem = m_rcText;
    rcItem.bottom = rcItem.top + m_pDialog->GetManager()->GetFontNode( pElement->iFont )->nHeight;
    rcItem.right = m_rcText.right;

    // 平滑捲動
    // Ref: ActiveRole::FrameMoveTalk()
    int iStartPos = m_iRenderingTrackPos;
    int iScrollPos = m_iRenderingTrackPos - m_nNewItems;
    if (m_bSmoothScroll && iScrollPos >= 0) {
        if (0 == m_cyScrollUp) {
            m_bSmoothScroll = false;
        }
//        else if (0 == fElapsedTime) {
//            m_bSmoothScroll = false;
//        }
        else {
            iStartPos = iScrollPos;
            //unsigned int uTimeDelta = g_Timer.GetTimeDelta();
            //if (uTimeDelta >= _SMOOTH_SCROLL_INTERVAL) {
            //if (0 == (DWORD)fElapsedTime % _SMOOTH_SCROLL_INTERVAL) {
                m_cyScrolled++;
                rcItem.top -= m_cyScrolled;
                m_cyScrollUp--;
                if (0 == m_cyScrollUp)
                    m_bSmoothScroll = false;
             //} // end if
        } // end else
    } // end if   
	
	for (int i = iStartPos ; i < (int)m_Items.GetSize(); ++i)
    {
		
        if( rcItem.bottom > m_rcText.bottom )
            break;
		
        DXUTListBoxItem *pItem = m_Items.GetAt( i );
        
        // Update LineObject's rect.
        //SIZE extent;
        //pItem->LineObject.GetExtent(extent);
        //rcItem.bottom = rcItem.top + extent.cy;
        rcItem.bottom = rcItem.top + m_nTextHeight;
        RECT rcInter;
		
        IntersectRect(&rcInter, &rcItem, &m_rcText);

		// 如果在範圍內
		if(rcInter.bottom - rcInter.top >= m_nTextHeight)
		{
			rcInter.left = rcItem.left;

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

			if( bSelectedStyle && m_bDrawSelect )
			{
				rcSel.top = rcItem.top; 
				rcSel.bottom = rcItem.bottom;
				m_pDialog->DrawSprite( pSelElement, &rcSel );
				DrawLineObject(&pItem->LineObject, pSelElement, &rcInter);
			}
			else
			{
				if( pItem->FontColor.a != 0 )
				{
					D3DXCOLOR Color = pElement->FontColor.Current;
					pElement->FontColor.Current = pItem->FontColor;
					
					DrawLineObject(&pItem->LineObject, pElement, &rcInter);
					pElement->FontColor.Current = Color;
				}
				else
				{
					DrawLineObject(&pItem->LineObject, pElement, &rcInter);
				}
			}

		}
			
        
        //SIZE size;
        //pItem->LineObject.GetExtent(size);
        //OffsetRect(&rc, 0, size.cy);

        OffsetRect(&rcItem, 0, m_nTextHeight);
        
    } // end for
}
/*
void CVariableHeightListBox::RenderListBox(IDirect3DDevice9* pd3dDevice, float fElapsedTime)
// 非平滑捲動版
{
    CDXUTElement* pElement = m_Elements.GetAt( 0 );
    CDXUTElement* pSelElement = m_Elements.GetAt( 1 );

    RECT rcSel = m_rcSelection;
    RECT rc = m_rcText;
    rc.bottom = rc.top + m_pDialog->GetManager()->GetFontNode( pElement->iFont )->nHeight;
    rc.right = m_rcText.right;

    //========================================================================
    // 從 m_iRenderingTrackPos 開始更新 ListBox 內容。
    //========================================================================
    for( int i = m_iRenderingTrackPos; i < (int)m_Items.GetSize(); ++i )
    {
        if( rc.bottom > m_rcText.bottom )
            break;

        DXUTListBoxItem *pItem = m_Items.GetAt( i );

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

        if( bSelectedStyle && m_bDrawSelect )
        {
            rcSel.top = rc.top; rcSel.bottom = rc.bottom;
            m_pDialog->DrawSprite( pSelElement, &rcSel );
            m_pDialog->DrawLineObject(&pItem->LineObject, pSelElement, &rc);
        }
        else
        {
            if( pItem->FontColor.a != 0 )
            {
                D3DXCOLOR Color = pElement->FontColor.Current;
                pElement->FontColor.Current = pItem->FontColor;
                m_pDialog->DrawLineObject(&pItem->LineObject, pElement, &rc);
                pElement->FontColor.Current = Color;
            }
            else
            {
                m_pDialog->DrawLineObject(&pItem->LineObject, pElement, &rc);
            }
        }

        //SIZE size;
        //pItem->LineObject.GetExtent(size);
        //OffsetRect(&rc, 0, size.cy);

        OffsetRect(&rc, 0, m_nTextHeight);
    } // end for
}
*/
void CVariableHeightListBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if( m_bVisible == false )
        return;

    //CDXUTElement* pElement = m_Elements.GetAt( 0 );
	CDXUTElement* pElement = GetElement( 0 ) ;
    pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
    pElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

    //CDXUTElement* pSelElement = m_Elements.GetAt( 1 );
	CDXUTElement* pSelElement = GetElement( 1 ) ;
    pSelElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
    pSelElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
     
    // 畫出背景
    RECT rcBoundingBox = m_rcBoundingBox;
    rcBoundingBox.right -= m_nSBWidth;
    m_pDialog->DrawSprite( pElement, &rcBoundingBox );

    // Render the text
    if( m_Items.GetSize() > 0 )
    {
        // Find out the height of a single line of text and update the line height formation.
        const int cyFont = m_pDialog->GetManager()->GetFontNode( pElement->iFont )->nHeight;
        m_nTextHeight = cyFont + m_nSpace;

        static bool _bSBInit;
        if( !_bSBInit )
        {
            // Update the page size of the scroll bar
            if( m_nTextHeight )
                m_ScrollBar.SetPageSize( ( RectHeight( m_rcText ) + m_nSpace ) / m_nTextHeight );
            else
                m_ScrollBar.SetPageSize( RectHeight( m_rcText ) );
            _bSBInit = true;
        }
        //========================================================================
        // 如果 ScrollBar 上的滑鼠被按住，不要被新訊息拉回。
        //========================================================================
        UpdateRenderingTrackPos();
        //========================================================================
        // 從 m_iRenderingTrackPos 開始更新 ListBox 內容。
        //========================================================================
        RenderListBox(pd3dDevice, fElapsedTime);
    }
    // Render the scroll bar
    m_ScrollBar.Render( pd3dDevice, fElapsedTime );
}

DXUTListBoxItem *CVariableHeightListBox::GetItem( int nIndex )
{
    if( nIndex < 0 || nIndex >= (int)m_Items.GetSize() )
        return NULL;

    return m_Items[nIndex];
}

/*
void CVariableHeightListBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if (m_bVisible == false)
        return;

    CDXUTElement* pElement = m_Elements.GetAt( 0 );
    pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
    pElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

    CDXUTElement* pSelElement = m_Elements.GetAt( 1 );
    pSelElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
    pSelElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

    // 畫出背景
    RECT rcBoundingBox = m_rcBoundingBox;
    rcBoundingBox.right -= m_nSBWidth;
    m_pDialog->DrawSprite(pElement, &m_rcBoundingBox);

    // Render the text
    if (m_Items.GetSize() > 0)
    {
        //========================================================================
        // 如果 ScrollBar 上的滑鼠被按住，不要被新訊息拉回。
        //========================================================================
        if (true == m_ScrollBar.IsArrowUpHeld()) {
            // ArrowUp button held, only scroll-up allowed.
            int iPos = m_ScrollBar.GetTrackPos();
            if (m_iRenderingTrackPos > iPos)
                m_iRenderingTrackPos = iPos;
            else m_ScrollBar.SetTrackPos(m_iRenderingTrackPos);
        } 
        else if (m_ScrollBar.IsThumbHeld()) {
            // The thumb-button was held without moving, use old track pos.
            m_ScrollBar.SetTrackPos(m_iRenderingTrackPos);
        } 
        else {
            // Arrow is not held or held on DownButton, use new track pos.
            m_iRenderingTrackPos = m_ScrollBar.GetTrackPos();
        }
        //========================================================================
        // 從 m_iRenderingTrackPos 開始更新 ListBox 內容。
        //========================================================================
        RECT rcClient = m_rcText;
        int i;
        for (i = m_iRenderingTrackPos; i < (int)m_Items.GetSize(); ++i) {
            if (rcClient.bottom > m_rcText.bottom) break;
            DXUTListBoxItem *pItem = m_Items.GetAt(i);
            //pItem->LineObject.DrawText(DT_CENTER, &rcClient, NULL);
            m_pDialog->DrawText(&pItem->LineObject, pElement, &rcClient);
            SIZE size;
            pItem->LineObject.GetExtent(size);
            OffsetRect(&rcClient, 0, size.cy);
        } // end for
        // UpdateScrollBarPageSize = number of items (rows) rendered
        m_ScrollBar.SetPageSize(i);
    } // end if

    m_ScrollBar.Render( pd3dDevice, fElapsedTime );
}
*/
