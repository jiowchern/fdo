//
// CDXUTListBox.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
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
#include "ListBoxItem.h"
#include "CDXUTDialog.h"
#include "CDXUTDialogResourceManager.h"
//#include "Global.h"

using namespace FDO;

#pragma hdrstop
#include "CDXUTListBox.h"
#include "PhenixD3D9RenderSystem.h"
//--------------------------------------------------------------------------------------
// CDXUTListBox class
//--------------------------------------------------------------------------------------
HRESULT CDXUTListBox::OnInit() 
{ 	
	return m_pDialog->InitControl( &m_ScrollBar ); 
}
void CDXUTListBox::SetSpace( int nSpace ) { m_nSpace = nSpace; UpdateRects(); }// 設定Item間距
void CDXUTListBox::SetSelected( int iIndex){ m_nSelected = iIndex; }// Darkness Add
int  CDXUTListBox::GetSelected(){ return m_nSelected; }// Darkness Add
int  CDXUTListBox::GetItemSize(){ return m_Items.GetSize(); }// Darkness Add
DWORD CDXUTListBox::GetStyle() const { return m_dwStyle; }
int CDXUTListBox::GetSize() const { return m_Items.GetSize(); }
void CDXUTListBox::SetStyle( DWORD dwStyle ) { m_dwStyle = dwStyle; }
void CDXUTListBox::SetCustomItemDraw(CDXUTListBox::PFN_CUSTOMDRAW pfnItemDraw) {	mpfnDrawItem = pfnItemDraw ; }
void CDXUTListBox::SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }
void CDXUTListBox::SetBorder( int nBorder, int nMargin ) { m_nBorder = nBorder; m_nMargin = nMargin; }
void CDXUTListBox::RemoveItemByData( void *pData ) { RemoveItem( GetItemIndexByData(pData) ); }
DXUTListBoxItem* CDXUTListBox::GetItemByData( void *pData ) { return GetItem( GetItemIndexByData(pData) ); }
DXUTListBoxItem* CDXUTListBox::GetSelectedItem( int nPreviousSelected ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
int CDXUTListBox::GetTrackPos() { return m_ScrollBar.GetTrackPos(); }
void CDXUTListBox::SetTrackPos( int nPosition ) { m_ScrollBar.SetTrackPos(nPosition); }
void CDXUTListBox::TrackLast() { m_ScrollBar.SetTrackPos( m_Items.GetSize() ); } // 設定ScrollBar移至List最後
void CDXUTListBox::SetBackgroundTexture( const string &FileName, RECT* prcTexture )
    { CDXUTControl::SetTexture( FileName, prcTexture ); }
void CDXUTListBox::SetDrawSelect( bool bDraw ) { m_bDrawSelect = bDraw; }

//--------------------------------------------------------------------------------------
void CDXUTListBox::fnDrawItem(CDXUTListBox*pCtrl , DXUTListBoxItem* pItem, CDXUTElement* pElement, RECT* pRect)
{
	pCtrl->DrawItemText(pItem,pElement,pRect);
}
//--------------------------------------------------------------------------------------
CDXUTListBox::CDXUTListBox( CDXUTDialog *pDialog ) :
m_ScrollBar( pDialog ) ,mpfnDrawItem(CDXUTListBox::fnDrawItem)
{
    m_Type = DXUT_CONTROL_LISTBOX;
    m_pDialog = pDialog;

    m_dwStyle = 0;
    m_nSBWidth = 16;
    m_nSelected = -1;
    m_nSelStart = 0;
    m_bDrag = false;
    m_nBorder = 6;
    m_nMargin = 5;
    m_nTextHeight = 0;
    m_nSpace = 0;
	m_nMouseCell = -1;
	m_LastMouseCell = -1;
    m_bDrawSelect = true;
    m_iRenderingTrackPos = 0;
	m_bSBInit = false ;
}

//--------------------------------------------------------------------------------------
CDXUTListBox::~CDXUTListBox()
{
    RemoveAllItems();
}
//--------------------------------------------------------------------------------------


void CDXUTListBox::UpdateRects()
{
    CDXUTControl::UpdateRects();

    m_rcSelection = m_rcBoundingBox;
    m_rcSelection.right -= m_nSBWidth;
    InflateRect( &m_rcSelection, -m_nBorder, -m_nBorder );
    m_rcText = m_rcSelection;
    InflateRect( &m_rcText, -m_nMargin, 0 );

    // Update the scrollbar's rects
    m_ScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth, m_rcBoundingBox.top );
    m_ScrollBar.SetSize( m_nSBWidth, m_height );
    //DXUTFontNode* pFontNode = m_pDialog->GetManager()->GetFontNode( m_Elements.GetAt( 0 )->iFont );
	DXUTFontNode* pFontNode = m_pDialog->GetManager()->GetFontNode( GetElement( 0 )->iFont );
    if( pFontNode && pFontNode->nHeight )
    {
		m_nTextHeight = pFontNode->nHeight+m_nSpace ;
		m_ScrollBar.SetPageSize( (RectHeight(m_rcText)+m_nSpace) / m_nTextHeight ); 

        // The selected item may have been scrolled off the page.
        // Ensure that it is in page again.
        m_ScrollBar.ShowItem( m_nSelected );

		//RECT rc		= m_rcText;
		//RECT rcSel	= m_rcSelection;
		//rc.bottom	= rc.top + pFontNode->nHeight;
		// Update the line height formation
		//m_nTextHeight = RectHeight( rc ) + m_nSpace;
    }
}

//--------------------------------------------------------------------------------------
void CDXUTListBox::GetClientRect(RECT& rcOut) const
// client rectangle of listbox
{
    CopyRect(&rcOut, &m_rcText);
}

//--------------------------------------------------------------------------------------
void CDXUTListBox::UpdateTrackRange()
{
    m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
}
//////////////////////////////////////////////////////////////////////////

HRESULT CDXUTListBox::AddLine( const WCHAR *wszText, D3DXCOLOR FontColor/*=FDO::WHITE */,void* pData /*= 0*/ )
{
	uint	textLen = wcslen(wszText);	
	uint	nWidth	= RectWidth(m_rcText);	
	uint	nCurWidth = 0;

	char16	lineText[1024] = {0};
	uint	lineTextCount = 0;
	for (uint i = 0 ; i < textLen ; ++i)
	{					
		//SIZE szChar = CDXUTDialog::GetManager()->GetChar16Size(m_Elements.GetAt(0)->iFont ,wszText[i]) ; 		
		SIZE szChar = CDXUTDialog::GetManager()->GetChar16Size( GetElement( 0 )->iFont ,wszText[i]) ; 		
		nCurWidth += szChar.cx;
		if(nCurWidth >= nWidth)
		{
			lineText[lineTextCount] = 0;
			AddItem(lineText,pData,FontColor,false);
			lineTextCount = 0;
			nCurWidth = 0;			
		}		
		lineText[lineTextCount++] = wszText[i];
	}
	if(lineTextCount)
	{
		// 最後一行
		lineText[lineTextCount] = 0;
		AddItem(lineText,pData,FontColor,false);
		lineTextCount = 0;
		nCurWidth = 0;			
	}	
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
//把一整串的字變成一行一行的item
//////////////////////////////////////////////////////////////////////////
HRESULT CDXUTListBox::AddString( const WCHAR *wszText, D3DXCOLOR FontColor, const WCHAR* stoken )
{
	
	wstring Str = wszText ;
	wstring Stoken = stoken ;
	std::transform(Stoken.begin(), Stoken.end(), Stoken.begin(), std::tolower);
	std::transform(Str.begin(), Str.end(), Str.begin(), std::tolower);

	while( Str.find( stoken ) != wstring::npos )
	{		
		size_t TokenEndPos = Str.find( stoken ) + Stoken.size() ;
		wstring Token = Str.substr( 0 , TokenEndPos ) ;
		AddItem( Token.c_str() , 0 , FontColor , true) ;
		Str = Str.substr( TokenEndPos ) ;
	}
	if( !Str.empty() )
	{
		AddItem( Str.c_str() , 0 , FontColor , true ) ;
	}
	return S_OK;
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTListBox::AddItem( const WCHAR *wszText, void *pData, D3DXCOLOR FontColor , bool bCensoring /*= false*/ )
{
    DXUTListBoxItem *pNewItem = new DXUTListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    StringCchCopy( pNewItem->strText, _MaxItemTextSize_, wszText );
    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;
    pNewItem->FontColor = FontColor;
	CTextObject* pTextObject = 0;
	if(bCensoring == true)
	{
		
		pTextObject = g_FDOXML.MakeTextObject(
			g_pD3DDevice, GetElement( 0 )->iFont, FontColor, wszText, -1, RectWidth(m_rcText));

		SIZE szLimit = {RectWidth(m_rcText),m_nTextHeight};
		pNewItem->bCensoring = CensoringTextObject(pTextObject , GetElement( 0 )->iFont ,szLimit );
		pNewItem->TextObject = *pTextObject;
		delete pTextObject;
	}
	else
	{
		
		pTextObject = g_FDOXML.MakeTextObject(
			g_pD3DDevice, GetElement( 0 )->iFont, FontColor, wszText, -1, RectWidth(m_rcText));
		pNewItem->TextObject = *pTextObject;
		delete pTextObject;
	}

    

    HRESULT hr = m_Items.Add( pNewItem );
    if( FAILED(hr) )
    {
        SAFE_DELETE( pNewItem );
    }
    else
    {
        UpdateTrackRange();
    }
    return hr;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTListBox::AddItem( const string &szText, D3DXCOLOR FontColor, void *pData , bool bCensoring /*= false*/ )
{
    return AddItem( ANSItoUNICODE(szText), pData, FontColor , bCensoring );
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTListBox::InsertItem( int nIndex, const WCHAR *wszText , D3DXCOLOR FontColor, void *pData /*= 0*/ , bool bCensoring /*= false*/ )
{
    DXUTListBoxItem *pNewItem = new DXUTListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    StringCchCopy( pNewItem->strText, _MaxItemTextSize_, wszText );
    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;
	pNewItem->FontColor = FontColor;

	// Create a CTextObject for this item.
	CTextObject* pTextObject = 0 ;
	if(bCensoring == true)
	{
		//std::wstring out = CensoringString(m_Elements.GetAt(0)->iFont , wszText,RectWidth(m_rcText) );
		std::wstring out ; 
		CensoringString(out , GetElement( 0 )->iFont , wszText,RectWidth(m_rcText) );
		/*pTextObject = g_FDOXML.MakeTextObject(
			g_pD3DDevice, m_Elements.GetAt(0)->iFont, FontColor, out.c_str(), -1, RectWidth(m_rcText)) ;*/
		pTextObject = g_FDOXML.MakeTextObject(
			g_pD3DDevice, GetElement( 0 )->iFont, FontColor, out.c_str(), -1, RectWidth(m_rcText)) ;
	}
	else
	{
		/*pTextObject = g_FDOXML.MakeTextObject(
			g_pD3DDevice, m_Elements.GetAt(0)->iFont, FontColor, wszText, -1, RectWidth(m_rcText));*/
		pTextObject = g_FDOXML.MakeTextObject(
			g_pD3DDevice, GetElement( 0 )->iFont, FontColor, wszText, -1, RectWidth(m_rcText));
	}
	pNewItem->TextObject = *pTextObject;
		delete pTextObject;

    HRESULT hr = m_Items.Insert( nIndex, pNewItem );
    if( SUCCEEDED( hr ) )
        UpdateTrackRange();
    else
        SAFE_DELETE( pNewItem );

    return hr;
}

//--------------------------------------------------------------------------------------
void CDXUTListBox::RemoveItem( int nIndex )
{
    if( nIndex < 0 || nIndex >= (int)m_Items.GetSize() )
        return;

    DXUTListBoxItem *pItem = m_Items.GetAt( nIndex );

    delete pItem;
    m_Items.Remove( nIndex );
    UpdateTrackRange();

    if( m_nSelected >= (int)m_Items.GetSize() )
	{
        m_nSelected = -1;
		m_nSelStart = -1;
		m_pDialog->SendEvent( EVENT_LISTBOX_LOSTSELECTION , true, this );
	}

    
}

//--------------------------------------------------------------------------------------
void CDXUTListBox::RemoveItemByText( WCHAR *wszText )
{
}  

//--------------------------------------------------------------------------------------
int CDXUTListBox::GetItemIndexByData( void *pData )
{
    for( int i = 0; i < m_Items.GetSize(); ++i )
    {
        DXUTListBoxItem *pItem = m_Items.GetAt( i );
        if( pItem->pData == pData )
            return i;
    }

    return -1;
}

//--------------------------------------------------------------------------------------
void CDXUTListBox::RemoveAllItems()
{
    m_Items.RemoveAll();
    UpdateTrackRange();
    m_iRenderingTrackPos = 0;
    m_nSelected = -1;
}

//--------------------------------------------------------------------------------------
DXUTListBoxItem *CDXUTListBox::GetItem( int nIndex )
{
    if( nIndex < 0 || nIndex >= (int)m_Items.GetSize() )
        return NULL;

    return m_Items[nIndex];
}  

//--------------------------------------------------------------------------------------
// For single-selection listbox, returns the index of the selected item.
// For multi-selection, returns the first selected item after the nPreviousSelected position.
// To search for the first selected item, the app passes -1 for nPreviousSelected.  For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as.
// nPreviousSelected.
// Returns -1 on error or if no item is selected.
int CDXUTListBox::GetSelectedIndex( int nPreviousSelected )
{
    if( nPreviousSelected < -1 )
        return -1;

    if( m_dwStyle & MULTISELECTION )
    {
        // Multiple selection enabled. Search for the next item with the selected flag.
        for( int i = nPreviousSelected + 1; i < (int)m_Items.GetSize(); ++i )
        {
            DXUTListBoxItem *pItem = m_Items.GetAt( i );

            if( pItem->bSelected )
                return i;
        }
        return -1;
    }
    else
    {
        // Single selection
        return m_nSelected;
    }
}

//--------------------------------------------------------------------------------------
void CDXUTListBox::SelectItem( int nNewIndex )
{
    // If no item exists, do nothing.
    if( m_Items.GetSize() == 0 )
        return;

    int nOldSelected = m_nSelected;

    // Adjust m_nSelected
    m_nSelected = nNewIndex;

    // Perform capping
    if( m_nSelected < 0 )
        m_nSelected = 0;
    if( m_nSelected >= (int)m_Items.GetSize() )
        m_nSelected = m_Items.GetSize() - 1;

    if( nOldSelected != m_nSelected )
    {
        if( m_dwStyle & MULTISELECTION )
        {
            m_Items[m_nSelected]->bSelected = true;
        }

        // Update selection start
        m_nSelStart = m_nSelected;

        // Adjust scroll bar
        m_ScrollBar.ShowItem( m_nSelected );
    }

    m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
}

//--------------------------------------------------------------------------------------
bool CDXUTListBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    // Let the scroll bar have a chance to handle it first
    if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) )
        return true;

    switch( uMsg )
    {
        case WM_KEYDOWN:
            switch( wParam )
            {
                case VK_UP:
                case VK_DOWN:
                case VK_NEXT:
                case VK_PRIOR:
                case VK_HOME:
                case VK_END:

                    // If no item exists, do nothing.
                    if( m_Items.GetSize() == 0 )
                        return true;

                    int nOldSelected = m_nSelected;

                    // Adjust m_nSelected
                    switch( wParam )
                    {
                        case VK_UP: --m_nSelected; break;
                        case VK_DOWN: ++m_nSelected; break;
                        case VK_NEXT: m_nSelected += m_ScrollBar.GetPageSize() - 1; break;
                        case VK_PRIOR: m_nSelected -= m_ScrollBar.GetPageSize() - 1; break;
                        case VK_HOME: m_nSelected = 0; break;
                        case VK_END: m_nSelected = m_Items.GetSize() - 1; break;
                    }

                    // Perform capping
                    if( m_nSelected < 0 )
                        m_nSelected = 0;
                    if( m_nSelected >= (int)m_Items.GetSize() )
                        m_nSelected = m_Items.GetSize() - 1;

                    if( nOldSelected != m_nSelected )
                    {
                        if( m_dwStyle & MULTISELECTION )
                        {
                            // Multiple selection

                            // Clear all selection
                            for( int i = 0; i < (int)m_Items.GetSize(); ++i )
                            {
                                DXUTListBoxItem *pItem = m_Items[i];
                                pItem->bSelected = false;
                            }

                            if( GetKeyState( VK_SHIFT ) < 0 )
                            {
                                // Select all items from m_nSelStart to
                                // m_nSelected
                                int nEnd = __max( m_nSelStart, m_nSelected );

                                for( int n = __min( m_nSelStart, m_nSelected ); n <= nEnd; ++n )
                                    m_Items[n]->bSelected = true;
                            }
                            else
                            {
                                m_Items[m_nSelected]->bSelected = true;

                                // Update selection start
                                m_nSelStart = m_nSelected;
                            }
                        } else
                            m_nSelStart = m_nSelected;

                        // Adjust scroll bar

                        m_ScrollBar.ShowItem( m_nSelected );

                        // Send notification

                        m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                    }
                    return true;
            }
            break;
    }

    return false;
}


//--------------------------------------------------------------------------------------
bool CDXUTListBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    //----------------------------------------------------------
    // First acquire focus
    // => Modified: ListBox 不再無條件取得輸入焦點。
    // if (WM_LBUTTONDOWN == uMsg)
    //     if (!m_bHasFocus)
    //         m_pDialog->RequestFocus(this);
    //----------------------------------------------------------

    // Let the scroll bar handle it first.
    if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
        return true;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            m_bDrag = true;
            // Check for clicks in the text area
            if( m_Items.GetSize() > 0 && PtInRect( &m_rcSelection, pt ) )
            {
                // Compute the index of the clicked item

                //音效 *** modify
                PlaySoundWAV();  //under control class

                int nClicked;
                if( m_nTextHeight )
                    nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
                else
                    nClicked = -1;

                // Only proceed if the click falls on top of an item.

                if( nClicked >= m_ScrollBar.GetTrackPos() &&
                    nClicked < (int)m_Items.GetSize() &&
                    nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
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

                    m_nSelected = nClicked;
                    if( !( wParam & MK_SHIFT ) )
                        m_nSelStart = m_nSelected;

                    // If this is a multi-selection listbox, update per-item
                    // selection data.

                    if( m_dwStyle & MULTISELECTION )
                    {
                        // Determine behavior based on the state of Shift and Ctrl

                        DXUTListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
                        if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_CONTROL )
                        {
                            // Control click. Reverse the selection of this item.

                            pSelItem->bSelected = !pSelItem->bSelected;
                        } else
                        if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_SHIFT )
                        {
                            // Shift click. Set the selection for all items
                            // from last selected item to the current item.
                            // Clear everything else.

                            int nBegin = __min( m_nSelStart, m_nSelected );
                            int nEnd = __max( m_nSelStart, m_nSelected );

                            for( int i = 0; i < nBegin; ++i )
                            {
                                DXUTListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = false;
                            }

                            for( int i = nEnd + 1; i < (int)m_Items.GetSize(); ++i )
                            {
                                DXUTListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = false;
                            }

                            for( int i = nBegin; i <= nEnd; ++i )
                            {
                                DXUTListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = true;
                            }
                        } else
                        if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == ( MK_SHIFT|MK_CONTROL ) )
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
                                DXUTListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = bLastSelected;
                            }

                            pSelItem->bSelected = true;

                            // Restore m_nSelected to the previous value
                            // This matches the Windows behavior

                            m_nSelected = m_nSelStart;
                        } else
                        {
                            // Simple click.  Clear all items and select the clicked
                            // item.


                            for( int i = 0; i < (int)m_Items.GetSize(); ++i )
                            {
                                DXUTListBoxItem *pItem = m_Items.GetAt( i );
                                pItem->bSelected = false;
                            }

                            pSelItem->bSelected = true;
                        }
                    }  // End of multi-selection case

                    m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                }

                return true;
            }
            break;

        case WM_LBUTTONUP:
        {
            ReleaseCapture();

            if( m_bDrag )
            {
                m_bDrag = false;
                m_pDialog->SendEvent( EVENT_LISTBOX_CLICK, true, this );
            } 

            if( m_nSelected != -1 )
            {
                // Set all items between m_nSelStart and m_nSelected to
                // the same state as m_nSelStart
                

				if( m_nSelected < m_Items.GetSize())
				{
					int nEnd = __max( m_nSelStart, m_nSelected );
					for( int n = __min( m_nSelStart, m_nSelected ) + 1; n < nEnd; ++n )
						m_Items[n]->bSelected = m_Items[m_nSelStart]->bSelected;
					m_Items[m_nSelected]->bSelected = m_Items[m_nSelStart]->bSelected;
				}
                

                // If m_nSelStart and m_nSelected are not the same,
                // the user has dragged the mouse to make a selection.
                // Notify the application of this.
                if( m_nSelStart != m_nSelected )
                    m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
            }
            return false;
        }

        case WM_RBUTTONDOWN: // 右鍵
            if( ( m_Items.GetSize() > 0 ) && PtInRect( &m_rcSelection, pt ) )
            {
                // Compute the index of the clicked item

                int nClicked;
                if( m_nTextHeight )
                    nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
                else
                    nClicked = -1;

                // Only proceed if the click falls on top of an item.

                if( nClicked >= m_ScrollBar.GetTrackPos() &&
                    nClicked < (int)m_Items.GetSize() &&
                    nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
                {
                    DXUTListBoxItem* pSelItem = NULL;
                    // 取消前次選取
                    if( m_nSelected != -1 )
                    {
                        if( m_nSelected < m_Items.GetSize() )
                        {
                            pSelItem = m_Items.GetAt( m_nSelected );
                            pSelItem->bSelected = false;
                        }
                    }

                    m_nSelected = nClicked;
                    pSelItem = m_Items.GetAt( m_nSelected );
                    pSelItem->bSelected = true;

                    m_pDialog->SendEvent( EVENT_LISTBOX_RSELECTION, true, this );
                    return true;
                }
            }
            break;

		case WM_MOUSEMOVE:
			if( m_Items.GetSize() )
			{
				// Compute the index of the clicked item

                if( m_nTextHeight )
                    m_nMouseCell = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
                else
                    m_nMouseCell = -1;
			}
			if( m_LastMouseCell != m_nMouseCell )
			{
				m_LastMouseCell = m_nMouseCell ;
				m_pDialog->SendEvent( EVENT_ICONLISTBOX_CELLCROSS , true , this ) ;
				return true ;
			}
			else
			{
				if( m_nMouseCell < m_Items.GetSize() && m_nMouseCell >= 0)					
				{
					if(m_Items[m_nMouseCell]->bCensoring)
						SetHint(m_Items[m_nMouseCell]->strText);
					else
						SetHint(L"");
				}
			}
            break;
        /*
        case WM_MOUSEMOVE:
            if( m_bDrag )
            {
                // Compute the index of the item below cursor

                int nItem;
                if( m_nTextHeight )
                    nItem = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
                else
                    nItem = -1;

                // Only proceed if the cursor is on top of an item.

                if( nItem >= (int)m_ScrollBar.GetTrackPos() &&
                    nItem < (int)m_Items.GetSize() &&
                    nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
                {
                    m_nSelected = nItem;
                    m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                } else
                if( nItem < (int)m_ScrollBar.GetTrackPos() )
                {
                    // User drags the mouse above window top
                    m_ScrollBar.Scroll( -1 );
                    m_nSelected = m_ScrollBar.GetTrackPos();
                    m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                } else
                if( nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
                {
                    // User drags the mouse below window bottom
                    m_ScrollBar.Scroll( 1 );
                    m_nSelected = __min( (int)m_Items.GetSize(), m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() ) - 1;
                    m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                }
            }
            break;*/

        case WM_MOUSEWHEEL:
        {
            UINT uLines;
            SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
            int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
            m_ScrollBar.Scroll( -nScrollAmount );
			mDXUTScrollBarVisitor.AcceptScroll(-nScrollAmount);
            return true;
        }

        case WM_RBUTTONDBLCLK: // 右鍵雙擊
        {
            m_pDialog->SendEvent( EVENT_LISTBOX_RBUTTONDBLCLK, true, this );
                return true;
        }
    }

    return false;
}
//--------------------------------------------------------------------------------------
int CDXUTListBox::GetSelTop( int iOffset )
{
    RECT rc = m_rcText;
    //CDXUTElement* pElement = m_Elements.GetAt( 0 );
	CDXUTElement* pElement = GetElement( 0 ) ;
    int nTextHeight = m_pDialog->GetManager()->GetFontNode( pElement->iFont )->nHeight + iOffset;
    nTextHeight *= m_nSelected - m_ScrollBar.GetTrackPos();
    OffsetRect( &rc, 0, nTextHeight );
    return rc.top-m_pDialog->GetCaptionHeight();  
}

//--------------------------------------------------------------------------------------
void CDXUTListBox::DrawItemText(DXUTListBoxItem* pItem, CDXUTElement* pElement, RECT* pRect)
{	
	pItem->TextObject.ChangeDefaultColor(pElement->FontColor.Current);
	if (!pRect)
		pRect = &pItem->rcActive;
	m_pDialog->DrawTextObject(&pItem->TextObject, pElement, pRect);
}
void CDXUTListBox::RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	
	
	if( m_bMouseOver && m_bVisible && m_nMouseCell < m_Items.GetSize() && m_nMouseCell >= 0)
	{
		const char16* strHint = _GetHint();	
		if(strHint && strHint[0])
		{
			POINT ptMouse;
			::GetCursorPos(&ptMouse);
			::ScreenToClient(g_RenderSystem.GetHWND(),&ptMouse);

			POINT ptDlg;
			m_pDialog->GetLocation(ptDlg);
			ptMouse.x -= ptDlg.x;
			ptMouse.y -= ptDlg.y;
			ptMouse.y -= m_pDialog->GetCaptionHeight();
			m_pDialog->RenderHintAll(1920,0xffffffff,0xffffffff,0xff000000,GetElement(0),ptMouse,strHint,L"",false);
		}
		
	}

}
//--------------------------------------------------------------------------------------
void CDXUTListBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
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
        // Find out the height of a single line of text
        RECT rc = m_rcText;
        RECT rcSel = m_rcSelection;
        rc.bottom = rc.top + m_nTextHeight - m_nSpace;

		// mark by jiowchern - m_nTextHeight 改在 CDXUTListBox::UpdateRects 更新
        //m_nTextHeight = RectHeight( rc ) + m_nSpace;

        //設定scrollbar的pagesize改到updatarect設定
        //if( !m_bSBInit )
        //{
        //    // Update the page size of the scroll bar
        //    if( m_nTextHeight )
        //        //m_ScrollBar.SetPageSize( RectHeight( m_rcText ) / m_nTextHeight );
        //        m_ScrollBar.SetPageSize( ( RectHeight( m_rcText ) + m_nSpace ) / m_nTextHeight );
        //    else
        //        m_ScrollBar.SetPageSize( RectHeight( m_rcText ) );
        //    m_bSBInit = true;
        //}
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
        rc.right = m_rcText.right;
        for( int i = m_iRenderingTrackPos; i < (int)m_Items.GetSize() && i >= 0; ++i )
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
                mpfnDrawItem(this,pItem, pSelElement, &rc);
            }
            else
            {
                if( pItem->FontColor.a != 0 )
                {
                    D3DXCOLOR Color = pElement->FontColor.Current;
                    pElement->FontColor.Current = pItem->FontColor;
                    mpfnDrawItem(this,pItem, pElement, &rc);
                    pElement->FontColor.Current = Color;
                }
                else
                {
                    mpfnDrawItem(this,pItem, pElement, &rc);
                }
                
                //if( pItem->FontColor.a != 0 ) // ming add 換成該物件的顏色
                //    swap( pItem->FontColor, pElement->FontColor.Current );
                //m_pDialog->DrawText( pItem->strText, pElement, &rc );
                //if( pItem->FontColor.a != 0 ) // ming add 換回原色
                //    swap( pItem->FontColor, pElement->FontColor.Current );
            }

            OffsetRect( &rc, 0, m_nTextHeight );
        }
    }

    // Render the scroll bar

    m_ScrollBar.Render( pd3dDevice, fElapsedTime );
}
//--------------------------------------------------------------------------------------
void CDXUTListBox::Visit(ScrollBarEvent::CurPos& e) 
{

}
//--------------------------------------------------------------------------------------
void CDXUTListBox::AddVisitor(ScrollBarEvent::ControlVisitor* pVis)
{
	mDXUTScrollBarVisitor.Register(pVis);
}

void CDXUTListBox::SetTextFont(const char16* sName,uint nHeight,uint nW)
{
	//m_Elements.GetAt(0)->iFont = CDXUTDialog::GetManager()->AddFont(sName,nHeight,nW,0);
	GetElement(0)->iFont = CDXUTDialog::GetManager()->AddFont(sName,nHeight,nW,0);

	//const int cyFont = m_pDialog->GetManager()->GetFontNode( m_Elements.GetAt(0)->iFont )->nHeight;
	const int cyFont = m_pDialog->GetManager()->GetFontNode( GetElement(0)->iFont )->nHeight;
	m_nTextHeight = cyFont + m_nSpace;

	if( m_nTextHeight )
		m_ScrollBar.SetPageSize( ( RectHeight( m_rcText ) + m_nSpace ) / m_nTextHeight );
	else
		m_ScrollBar.SetPageSize( RectHeight( m_rcText ) );
}