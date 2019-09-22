//
// CDXUTComboBox.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
#include <strsafe.h>
#include "CDXUTTimer.h"
#include "FDOUT.h"
#include "PacketReader.h"
#include "JLAnimation2DManager.h"
#include "CDXUTDialogResourceManager.h"
//#include "HUISystemSet.h"
#include <cmath>
#include "J_SoundManager.h"
#include <stdio.h>
#include "iniReader.h"
#include "CFDOXML.h"
#include "DialogType.h"
#include "dxerr9.h"
//#include "Global.h"
#include "TextObject.h"
#include "ListBoxItem.h"

using namespace FDO;

#include "CDXUTDialog.h"
#pragma hdrstop
#include "CDXUTComboBox.h"
//--------------------------------------------------------------------------------------
// CDXUTComboBox class
//--------------------------------------------------------------------------------------
HRESULT CDXUTComboBox::OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }
void CDXUTComboBox::SetDropHeight( UINT nHeight ) { m_nDropHeight = nHeight; UpdateRects(); }
void CDXUTComboBox::SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }
void CDXUTComboBox::SetButtonSize( const int &Width, const int &Height ) { m_ButtonWidth=Width; m_ButtonHeight=Height; }
void CDXUTComboBox::OffsetButton( const int &OffsetX, const int &OffsetY ) { m_ButtonOffsetX=OffsetX; m_ButtonOffsetY=OffsetY; }
void CDXUTComboBox::ShowMainTextBox( const bool &Show ) { m_ShowMain = Show; }

CDXUTComboBox::CDXUTComboBox( CDXUTDialog *pDialog ) :
    m_ScrollBar( pDialog )
{
    m_Type = DXUT_CONTROL_COMBOBOX;
    m_pDialog = pDialog;

    m_nDropHeight = 100;

    m_nSBWidth = 16;
    m_bOpened = false;
    m_iSelected = -1;
    m_iFocused = -1;

    // ming add >> Initial...
    m_ButtonWidth = 0;
    m_ButtonHeight = 0;
    m_ButtonOffsetX = 0;
    m_ButtonOffsetY = 0;

    m_ShowMain = true;
    // ming add <<
}

//--------------------------------------------------------------------------------------
CDXUTComboBox::~CDXUTComboBox()
{
    RemoveAllItems();
}

//--------------------------------------------------------------------------------------
void CDXUTComboBox::SetTextColor( D3DCOLOR Color, UINT iElement )
{
	CDXUTElement* pElement = GetElement( 0 ) ;

    if( pElement )
        pElement->FontColor.States[DXUT_STATE_NORMAL] = Color;
    
	pElement = GetElement( 2 );

    if( pElement )
        pElement->FontColor.States[DXUT_STATE_NORMAL] = Color;
}

//--------------------------------------------------------------------------------------
void CDXUTComboBox::UpdateRects()
{
    CDXUTButton::UpdateRects();

    m_rcButton = m_rcBoundingBox;
    m_rcButton.left = m_rcButton.right - RectHeight( m_rcButton );

    m_rcText = m_rcBoundingBox;
    m_rcText.right = m_rcButton.left;

    m_rcDropdown = m_rcText;
    OffsetRect( &m_rcDropdown, 0, (int) (0.90f * RectHeight( m_rcText )) );

    m_rcDropdown.bottom += m_nDropHeight;
    m_rcDropdown.right -= m_nSBWidth;

    // ming add >> 如果下拉式選單超過底邊，則往上展開
    if( m_rcDropdown.bottom > m_pDialog->GetHeight() )
    {
        OffsetRect( &m_rcDropdown, 0, -RectHeight( m_rcDropdown ) );
        OffsetRect( &m_rcDropdown, 0, (int) (-0.90f * RectHeight( m_rcText )) );
    }
    // ming add <<

    m_rcDropdownText = m_rcDropdown;
    m_rcDropdownText.left   += (int) (0.1f * RectWidth ( m_rcDropdown ));
    m_rcDropdownText.right  -= (int) (0.1f * RectWidth ( m_rcDropdown ));
    m_rcDropdownText.top    += (int) (0.1f * RectHeight( m_rcDropdown ));
    m_rcDropdownText.bottom -= (int) (0.1f * RectHeight( m_rcDropdown ));

    // Update the scrollbar's rects
    m_ScrollBar.SetLocation( m_rcDropdown.right, m_rcDropdown.top+2 );
    m_ScrollBar.SetSize( m_nSBWidth, RectHeight( m_rcDropdown )-2 );
	DXUTFontNode* pFontNode = m_pDialog->GetManager()->GetFontNode( GetElement( 2 )->iFont );
    if( pFontNode && pFontNode->nHeight )
    {
        m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / pFontNode->nHeight );

        // The selected item may have been scrolled off the page.
        // Ensure that it is in page again.
        m_ScrollBar.ShowItem( m_iSelected );
    }
}

//--------------------------------------------------------------------------------------
void CDXUTComboBox::OnFocusOut()
{
    CDXUTButton::OnFocusOut();

	if( m_iSelected != m_iFocused )
	{
		m_iSelected = m_iFocused;
		m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
	}
    m_bOpened = false;
}

//--------------------------------------------------------------------------------------
bool CDXUTComboBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    const DWORD REPEAT_MASK = (0x40000000);

    if( !m_bEnabled || !m_bVisible )
        return false;

    // Let the scroll bar have a chance to handle it first
    if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) )
        return true;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_RETURN:
                    if( m_bOpened )
                    {
                        if( m_iSelected != m_iFocused )
                        {
                            m_iSelected = m_iFocused;
                            m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                        }
                        m_bOpened = false;
                        
                        if( !m_pDialog->m_bKeyboardInput )
                            m_pDialog->ClearFocus();

                        return true;
                    }
                    break;

                case VK_F4:
                    // Filter out auto-repeats
                    if( lParam & REPEAT_MASK )
                        return true;

                    m_bOpened = !m_bOpened;

                    if( !m_bOpened )
                    {
                        m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );

                        if( !m_pDialog->m_bKeyboardInput )
                            m_pDialog->ClearFocus();
                    }

                    return true;

                case VK_LEFT:
                case VK_UP:
                    if( m_iFocused > 0 )
                    {
                        m_iFocused--;
                        m_iSelected = m_iFocused;

                        if( !m_bOpened )
                            m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                    }
                    
                    return true;

                case VK_RIGHT:
                case VK_DOWN:
                    if( m_iFocused+1 < (int)GetNumItems() )
                    {
                        m_iFocused++;
                        m_iSelected = m_iFocused;

                        if( !m_bOpened )
                            m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                    }

                    return true;
            }
            break;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------
bool CDXUTComboBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    m_MousePoint = pt;
    
    // Let the scroll bar handle it first.
    if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
        return true;

    switch( uMsg )
    {
        case WM_MOUSEMOVE:
        {
            if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
            {
                // Determine which item has been selected
                for( int i=0; i < m_Items.GetSize(); i++ )
                {
                    DXUTComboBoxItem* pItem = m_Items.GetAt( i );
                    if( pItem -> bVisible &&
                        PtInRect( &pItem->rcActive, pt ) )
                    {
                        m_iFocused = i;
                    }
                }
                return true;
            }
            break;
        }

        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if( ContainsPoint( pt ) )
            {
                //音效 *** modify
                PlaySoundWAV();  //under control class

                // Pressed while inside the control
                m_bPressed = true;
                SetCapture( DXUTGetHWND() );

                if( !m_bHasFocus )
                    m_pDialog->RequestFocus( this );

                // Toggle dropdown
                if( m_bHasFocus )
                {
                    m_bOpened = !m_bOpened;
                
                    if( !m_bOpened )
                    {
                        if( !m_pDialog->m_bKeyboardInput )
                            m_pDialog->ClearFocus();
                    }
					else
					{
						
					}
                }

                return true;
            }

            // Perhaps this click is within the dropdown
            if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
            {
                // Determine which item has been selected
                for( int i=m_ScrollBar.GetTrackPos(); i < m_Items.GetSize(); i++ )
                {
                    DXUTComboBoxItem* pItem = m_Items.GetAt( i );
                    if( pItem -> bVisible &&
                        PtInRect( &pItem->rcActive, pt ) )
                    {
                        m_iFocused = m_iSelected = i;
                        m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                        m_bOpened = false;
						m_pDialog->RequestFocus(0);
                        
                        if( !m_pDialog->m_bKeyboardInput )
                            m_pDialog->ClearFocus();

                        break;
                    }
                }

                return true;
            }

            // Mouse click not on main control or in dropdown, fire an event if needed
            if( m_bOpened )
            {
                m_iFocused = m_iSelected;

                m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                m_bOpened = false;
            }

            // Make sure the control is no longer in a pressed state
            m_bPressed = false;

            // Release focus if appropriate
            if( !m_pDialog->m_bKeyboardInput )
            {
                m_pDialog->ClearFocus();
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if( m_bPressed && ContainsPoint( pt ) )
            {
                // Button click
                m_bPressed = false;
                ReleaseCapture();
                return true;
            }

            break;
        }

        case WM_MOUSEWHEEL:
        {
            int zDelta = (short) HIWORD(wParam) / WHEEL_DELTA;
            if( m_bOpened )
            {
                UINT uLines;
                SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
                m_ScrollBar.Scroll( -zDelta * uLines );
            } else
            {
                if( zDelta > 0 )
                {
                    if( m_iFocused > 0 )
                    {
                        m_iFocused--;
                        m_iSelected = m_iFocused;     
                        
                        if( !m_bOpened )
                            m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                    }          
                }
                else
                {
                    if( m_iFocused+1 < (int)GetNumItems() )
                    {
                        m_iFocused++;
                        m_iSelected = m_iFocused;   

                        if( !m_bOpened )
                            m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                    }
                }
            }
            return true;
        }
    };
    
    return false;
}

//--------------------------------------------------------------------------------------
void CDXUTComboBox::OnHotkey()
{
    if( m_bOpened )
        return;

    if( m_iSelected == -1 )
        return;

    m_pDialog->RequestFocus( this ); 

    m_iSelected++;
    
    if( m_iSelected >= (int) m_Items.GetSize() )
        m_iSelected = 0;

    m_iFocused = m_iSelected;
    m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
}
//--------------------------------------------------------------------------------------
void CDXUTComboBox::DrawItemText(DXUTComboBoxItem* pItem, CDXUTElement* pElement, RECT* pRect)
{
    pItem->TextObject.ChangeDefaultColor(pElement->FontColor.Current);
    if (!pRect)
        pRect = &pItem->rcActive;
    m_pDialog->DrawTextObject(&pItem->TextObject, pElement, pRect);
}
//--------------------------------------------------------------------------------------
void CDXUTComboBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;
    
    if( !m_bOpened )
        iState = DXUT_STATE_HIDDEN;

    // Dropdown box
	CDXUTElement* pElement = GetElement( 2 ) ;

    // If we have not initialized the scroll bar page size,
    // do that now.
    static bool bSBInit;
    if( !bSBInit )
    {
        // Update the page size of the scroll bar
        if( m_pDialog->GetManager()->GetFontNode( pElement->iFont )->nHeight )
            m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / m_pDialog->GetManager()->GetFontNode( pElement->iFont )->nHeight );
        else
            m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) );
        bSBInit = true;
    }

    // Scroll bar
    if( m_bOpened )
        m_ScrollBar.Render( pd3dDevice, fElapsedTime ); // 畫出下拉式選單的ScrollBar

    // Blend current color
    pElement->TextureColor.Blend( iState, fElapsedTime );
    pElement->FontColor.Blend( iState, fElapsedTime );

    m_pDialog->DrawSprite( pElement, &m_rcDropdown ); // 畫出下拉式選單

    // Selection outline
	CDXUTElement* pSelectionElement = GetElement( 3 ) ;
    pSelectionElement->TextureColor.Current = pElement->TextureColor.Current;
    pSelectionElement->FontColor.Current = pSelectionElement->FontColor.States[ DXUT_STATE_NORMAL ];

    DXUTFontNode* pFont = m_pDialog->GetFont( pElement->iFont );
    if( pFont )
    {
        int curY = m_rcDropdownText.top;
        int nRemainingHeight = RectHeight( m_rcDropdownText );
        //WCHAR strDropdown[4096] = {0};

        for( int i = m_ScrollBar.GetTrackPos(); i < m_Items.GetSize(); i++ )
        {
            DXUTComboBoxItem* pItem = m_Items.GetAt( i );

            // Make sure there's room left in the dropdown
            nRemainingHeight -= pFont->nHeight;
            if( nRemainingHeight < 0 )
            {
                pItem->bVisible = false;
                continue;
            }

            SetRect( &pItem->rcActive, m_rcDropdownText.left, curY, m_rcDropdownText.right, curY + pFont->nHeight );
            curY += pFont->nHeight;
            
            //debug
            //int blue = 50 * i;
            //m_pDialog->DrawRect( &pItem->rcActive, 0xFFFF0000 | blue );

            pItem->bVisible = true;

            if( m_bOpened )
            {
                if( (int)i == m_iFocused )
                {
                    RECT rc;
                    SetRect( &rc, m_rcDropdown.left, pItem->rcActive.top-2, m_rcDropdown.right, pItem->rcActive.bottom+2 );
                    m_pDialog->DrawSprite( pSelectionElement, &rc );
                    DrawItemText(pItem, pSelectionElement);
                }
                else
                    {
                    // ming add >> 換成該物件的顏色
                    if( pItem->FontColor.a != 0 )
                        swap( pItem->FontColor, pElement->FontColor.Current );
                    // ming add <<

                    DrawItemText(pItem, pElement);

                    // ming add >> 換回原來的顏色
                    if( pItem->FontColor.a != 0 )
                        swap( pItem->FontColor, pElement->FontColor.Current );
                    // ming add <<
                }
            }
        }
    }

    int nOffsetX = 0;
    int nOffsetY = 0;

    iState = DXUT_STATE_NORMAL;
    
    if( m_bVisible == false )
        iState = DXUT_STATE_HIDDEN;
    else if( m_bEnabled == false )
        iState = DXUT_STATE_DISABLED;
    else if( m_bPressed )
    {
        iState = DXUT_STATE_PRESSED;

        nOffsetX = 1;
        nOffsetY = 1;
    }
    else if( m_bMouseOver )
    {
        iState = DXUT_STATE_MOUSEOVER;

        nOffsetX = -1;
        nOffsetY = -1;
    }
    else if( m_bHasFocus )
        iState = DXUT_STATE_FOCUS;

    float fBlendRate = ( iState == DXUT_STATE_PRESSED ) ? 0.0f : 0.8f;
    
    // Button
	pElement = GetElement( 1 ) ;
    
    // Blend current color
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );

    // ming edit >>
    RECT rcWindow = m_rcButton;
    if( m_ButtonWidth != 0 && m_ButtonHeight != 0 )
        InflateRect( &rcWindow, (m_ButtonWidth-RectWidth(rcWindow))/2, (m_ButtonHeight-RectHeight(rcWindow))/2 );

    OffsetRect( &rcWindow, nOffsetX+m_ButtonOffsetX, nOffsetY+m_ButtonOffsetY );
    m_pDialog->DrawSprite( pElement, &rcWindow );
    // ming edit <<

    if( m_bOpened )
        iState = DXUT_STATE_PRESSED;

    if( m_ShowMain ) // ming add
    {
        // Main text box
        //TODO: remove magic numbers
		pElement = GetElement( 0 ) ;
    
        // Blend current color
        pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
        pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

        m_pDialog->DrawSprite( pElement, &m_rcText);

        if( m_iSelected >= 0 && m_iSelected < (int) m_Items.GetSize() )
        {
            DXUTComboBoxItem* pItem = m_Items.GetAt( m_iSelected );
            if( pItem != NULL )
            {
                // ming add >> 修改畫字範圍( BoundingBox - 按鈕寬 )
                RECT rcText = m_rcText;
                rcText.right = m_rcBoundingBox.right - RectWidth(rcWindow) - 5;
                DrawItemText(pItem, pElement, &rcText);
                // ming add <<
            }
        }
    }
}

//--------------------------------------------------------------------------------------
void CDXUTComboBox::RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    // 畫出提示(Hint)
    if( m_bMouseOver && (wcslen( _GetHint() ) || wcslen( _GetHintImagePath() ) ) && m_bVisible )
    {

        //int iTexture;
		CDXUTElement* pElement = GetElement( 0 ) ;
        pElement->FontColor.Current = D3DCOLOR_ARGB(255, 255, 255, 255);
		pElement->dwTextFormat = DT_LEFT;


		//WCHAR* pstrHint = m_strHint; 
		//WCHAR* pstrHintImPath = m_strHintImgPath; 
		m_pDialog->RenderHintAll( m_cxWordMaxExtent, m_HintTextColor, m_FrameColor, m_BackgroundColor, pElement, m_MousePoint, _GetHint(),_GetHintImagePath(), m_bHintTurnRight );

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

//--------------------------------------------------------------------------------------
HRESULT CDXUTComboBox::AddItem( const WCHAR* strText, void* pData, D3DXCOLOR FontColor )
{
    // Validate parameters
    if( strText== NULL )
    {
        return E_INVALIDARG;
    }

    // Create a new item and set the data
    DXUTComboBoxItem* pItem = new DXUTComboBoxItem;
    if( pItem == NULL )
    {
        return DXTRACE_ERR_MSGBOX( L"new", E_OUTOFMEMORY );
    }
    
    StringCchCopy( pItem->strText, _MaxItemTextSize_ - 1, strText );
    pItem->pData = pData;
    pItem->FontColor = FontColor; // ming add 設定字的顏色
    
    // Create a CTextObject for this item.
    CTextObject* pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, NULL, FontColor, strText, -1, RectWidth(m_rcText));
    pItem->TextObject = *pTextObject;
    delete pTextObject;

    m_Items.Add( pItem );

    // Update the scroll bar with new range
    m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );

    // If this is the only item in the list, it's selected
    if( GetNumItems() == 1 )
    {
        m_iSelected = 0;
        m_iFocused = 0;
        m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTComboBox::AddItem( const string &szText, D3DXCOLOR FontColor, void *pData )
{
    return AddItem( ANSItoUNICODE(szText), pData, FontColor );
}

//--------------------------------------------------------------------------------------
void CDXUTComboBox::RemoveItem( UINT index )
{
    DXUTComboBoxItem* pItem = m_Items.GetAt( index );
    SAFE_DELETE( pItem );
    m_Items.Remove( index );
    m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
    if( m_iSelected >= m_Items.GetSize() )
        m_iSelected = m_Items.GetSize() - 1;
}

//--------------------------------------------------------------------------------------
void CDXUTComboBox::RemoveAllItems()
{
    for( int i=0; i < m_Items.GetSize(); i++ )
    {
        DXUTComboBoxItem* pItem = m_Items.GetAt( i );
        SAFE_DELETE( pItem );
    }

    m_Items.RemoveAll();
    m_ScrollBar.SetTrackRange( 0, 1 );
    m_iFocused = m_iSelected = -1;
}

//--------------------------------------------------------------------------------------
bool CDXUTComboBox::ContainsItem( const WCHAR* strText, UINT iStart )
{
    return ( -1 != FindItem( strText, iStart ) );
}

//--------------------------------------------------------------------------------------
bool CDXUTComboBox::ContainsItem( const string &strText, UINT iStart )
{
    return ContainsItem( ANSItoUNICODE(strText),iStart );
}

//--------------------------------------------------------------------------------------
int CDXUTComboBox::FindItem( const WCHAR* strText, UINT iStart )
{
    if( strText == NULL )
        return -1;

    for( int i = iStart; i < m_Items.GetSize(); i++ )
    {
        DXUTComboBoxItem* pItem = m_Items.GetAt(i);

        if( 0 == wcscmp( pItem->strText, strText ) )
        {
            return i;
        }
    }

    return -1;
}

//--------------------------------------------------------------------------------------
void* CDXUTComboBox::GetSelectedData()
{
    if( m_iSelected < 0 )
        return NULL;

    DXUTComboBoxItem* pItem = m_Items.GetAt( m_iSelected );
    return pItem->pData;
}

//--------------------------------------------------------------------------------------
DXUTComboBoxItem* CDXUTComboBox::GetSelectedItem()
{
    if( m_iSelected < 0 )
        return NULL;

    return m_Items.GetAt( m_iSelected );
}

//--------------------------------------------------------------------------------------
void* CDXUTComboBox::GetItemData( const WCHAR* strText )
{
    int index = FindItem( strText );
    if( index == -1 )
    {
        return NULL;
    }

    DXUTComboBoxItem* pItem = m_Items.GetAt(index);
    if( pItem == NULL )
    {
        DXTRACE_ERR( L"CGrowableArray::GetAt", E_FAIL );
        return NULL;
    }

    return pItem->pData;
}

//--------------------------------------------------------------------------------------
void* CDXUTComboBox::GetItemData( int nIndex )
{
    if( nIndex < 0 || nIndex >= m_Items.GetSize() )
        return NULL;

    return m_Items.GetAt( nIndex )->pData;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTComboBox::SetSelectedByIndex( UINT index, bool bTrigger )
{
    if( index >= GetNumItems() )
        return E_INVALIDARG;

    m_iFocused = m_iSelected = index;
    m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, bTrigger, this );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTComboBox::SetSelectedByText( const WCHAR* strText )
{
    if( strText == NULL )
        return E_INVALIDARG;

    int index = FindItem( strText );
    if( index == -1 )
        return E_FAIL;

    m_iFocused = m_iSelected = index;
    m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTComboBox::SetSelectedByData( void* pData )
{
    for( int i=0; i < m_Items.GetSize(); i++ )
    {
        DXUTComboBoxItem* pItem = m_Items.GetAt(i);

        if( pItem->pData == pData )
        {
            m_iFocused = m_iSelected = i;
            m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );
            return S_OK;
        }
    }

    return E_FAIL;
}

//--------------------------------------------------------------------------------------
