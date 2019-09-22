//
// CDXUTScrollBar.cpp
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

using namespace FDO;

#include "CDXUTDialog.h"
#pragma hdrstop
#include "CDXUTScrollBar.h"
//==========================================================================================
// Definitions
//==========================================================================================

// Minimum scroll bar thumb size
#define SCROLLBAR_MINTHUMBSIZE 8

// Delay and repeat period when clicking on the scroll bar arrows
#define SCROLLBAR_ARROWCLICK_DELAY  0.33
#define SCROLLBAR_ARROWCLICK_REPEAT 0.05

//--------------------------------------------------------------------------------------
// Static variables
//--------------------------------------------------------------------------------------

CDXUTScrollBar *CDXUTScrollBar::sm_DraggedScrollBar = NULL;

//--------------------------------------------------------------------------------------
CDXUTScrollBar::CDXUTScrollBar( CDXUTDialog *pDialog )
{
    m_Type = DXUT_CONTROL_SCROLLBAR;
    m_pDialog = pDialog;

    m_bShowThumb = true;
    m_bDragMove = false;

    SetRect( &m_rcUpButton, 0, 0, 0, 0 );
    SetRect( &m_rcDownButton, 0, 0, 0, 0 );
    SetRect( &m_rcTrack, 0, 0, 0, 0 );
    SetRect( &m_rcThumb, 0, 0, 0, 0 );
    m_nPosition = 0;
    m_nPageSize = 1;
    m_nStart = 0;
    m_nEnd = 1;
    m_Arrow = CLEAR;
    m_dArrowTS = 0.0;
	CDXUTElement empty;
	SetElement(0,&empty);
	SetElement(1,&empty);
	SetElement(2,&empty);
	SetElement(3,&empty);	
	
	m_ElemTrack = GetElement( 0 ) ;
	m_ElemUpArrow = GetElement( 1 ) ;
	m_ElemDownArrow = GetElement( 2 ) ;
	m_ElemThumb = GetElement( 3 ) ;

}


//--------------------------------------------------------------------------------------
CDXUTScrollBar::~CDXUTScrollBar()
{
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTScrollBar::OnInit()
{
    // ScrollBar 的渲染要素。
	m_ElemTrack = GetElement( 0 ) ;
    m_ElemUpArrow = GetElement( 1 ) ;
    m_ElemDownArrow = GetElement( 2 ) ;
    m_ElemThumb = GetElement( 3 ) ;
    return S_OK;
}
//--------------------------------------------------------------------------------------
// ScrollBar 的渲染要素。
//--------------------------------------------------------------------------------------
void CDXUTScrollBar::SetElementTrack(const CDXUTElement& elemSrc)
{
    *m_ElemTrack = elemSrc;
}

void CDXUTScrollBar::SetElementUpArrow(const CDXUTElement& elemSrc)
{
    *m_ElemUpArrow = elemSrc;
}

void CDXUTScrollBar::SetElementDownArrow(const CDXUTElement& elemSrc)
{
    *m_ElemDownArrow = elemSrc;
}

void CDXUTScrollBar::SetElementThumb(const CDXUTElement& elemSrc)
{
    *m_ElemThumb = elemSrc;
}

//--------------------------------------------------------------------------------------
void CDXUTScrollBar::UpdateRects()
{
    CDXUTControl::UpdateRects();

    // Make the buttons square
    const int cyButton = RectWidth(m_rcBoundingBox);

    SetRect( &m_rcUpButton, m_rcBoundingBox.left, m_rcBoundingBox.top,
                            m_rcBoundingBox.right, m_rcBoundingBox.top + cyButton );
    SetRect( &m_rcDownButton, m_rcBoundingBox.left, m_rcBoundingBox.bottom - cyButton,
                              m_rcBoundingBox.right, m_rcBoundingBox.bottom );
    SetRect( &m_rcTrack, m_rcUpButton.left, m_rcUpButton.bottom,
                         m_rcDownButton.right, m_rcDownButton.top );
    m_rcThumb.left = m_rcUpButton.left;
    m_rcThumb.right = m_rcUpButton.right;

    UpdateThumbRect();
}


//--------------------------------------------------------------------------------------
// Compute the dimension of the scroll thumb
void CDXUTScrollBar::UpdateThumbRect()
{
    if( m_nEnd - m_nStart > m_nPageSize && m_nEnd - m_nStart)
    {
		sint a = RectHeight( m_rcTrack ) * m_nPageSize / ( m_nEnd - m_nStart );
		sint b = SCROLLBAR_MINTHUMBSIZE ;
        int nThumbHeight = __max( a ,b );
        int nMaxPosition = m_nEnd - m_nStart - m_nPageSize;
        m_rcThumb.top = m_rcTrack.top + ( m_nPosition - m_nStart ) * ( RectHeight( m_rcTrack ) - nThumbHeight )
                        / nMaxPosition;
        m_rcThumb.bottom = m_rcThumb.top + nThumbHeight;
        m_bShowThumb = true;

    }
    else
    {
        // No content to scroll
        m_rcThumb.bottom = m_rcThumb.top;
        m_bShowThumb = false;
    }
}


//--------------------------------------------------------------------------------------
// Scroll() scrolls by nDelta items.  A positive value scrolls down, while a negative
// value scrolls up.
void CDXUTScrollBar::Scroll( int nDelta )
{
    // Perform scroll
    m_nPosition += nDelta;
	
    m_pDialog->SendEvent( EVENT_SCROLLBAR_VALUE_CHANGED, true, this );

    // Cap position
    Cap();

    // Update thumb position
    UpdateThumbRect();
//	DXUTScrollBar::AcceptCurPos(m_nPosition);
	mDXUTScrollBar.AcceptCurPos(m_nPosition);
	//Assert(m_nPosition>=0, L"nPosition<0");
}


//--------------------------------------------------------------------------------------
void CDXUTScrollBar::ShowItem( int nIndex )
{
    // Cap the index

    if( nIndex < 0 )
        nIndex = 0;

    if( nIndex >= m_nEnd )
        nIndex = m_nEnd - 1;

    // Adjust position

    if( m_nPosition > nIndex )
        m_nPosition = nIndex;
    else
    if( m_nPosition + m_nPageSize <= nIndex )
        m_nPosition = nIndex - m_nPageSize + 1;

    UpdateThumbRect();
}


//--------------------------------------------------------------------------------------
bool CDXUTScrollBar::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return false;
}


//--------------------------------------------------------------------------------------
bool CDXUTScrollBar::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(m_bVisible == false)
		return false;
    static int ThumbOffsetY;

    m_LastMouse = pt;
    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            // Check for click on up button

            if( PtInRect( &m_rcUpButton, pt ) )
            {
                //音效 *** modify
                PlaySoundWAV();  //under control class

                SetCapture( DXUTGetHWND() );
                if( m_nPosition > m_nStart )
                {
                    --m_nPosition;
					//DXUTScrollBar::AcceptCurPos(m_nPosition);
					mDXUTScrollBar.AcceptCurPos(m_nPosition);
                    m_pDialog->SendEvent( EVENT_SCROLLBAR_VALUE_CHANGED, true, this );
                }
                //Assert(m_nPosition>=0, L"nPosition<0");
                UpdateThumbRect();
                m_Arrow = CLICKED_UP;
                m_dArrowTS = DXUTGetTime();
                return true;
            }

            // Check for click on down button

            if( PtInRect( &m_rcDownButton, pt ) )
            {
                SetCapture( DXUTGetHWND() );
                if( m_nPosition + m_nPageSize < m_nEnd )
                {
                    ++m_nPosition;
					//DXUTScrollBar::AcceptCurPos(m_nPosition);
					mDXUTScrollBar.AcceptCurPos(m_nPosition);
                    m_pDialog->SendEvent( EVENT_SCROLLBAR_VALUE_CHANGED, true, this );
                }

                UpdateThumbRect();
                m_Arrow = CLICKED_DOWN;
                m_dArrowTS = DXUTGetTime();
                return true;
            }

            // Check for click on thumb

            if( PtInRect( &m_rcThumb, pt ) )
            {
                SetCapture( DXUTGetHWND() );
				SetDragged(this);
                ThumbOffsetY = pt.y - m_rcThumb.top;
                return true;
            }

            // Check for click on track

            if( m_rcThumb.left <= pt.x &&
                m_rcThumb.right > pt.x )
            {
                SetCapture( DXUTGetHWND() );
                if( m_rcThumb.top > pt.y &&
                    m_rcTrack.top <= pt.y )
                {
                    Scroll( -( m_nPageSize - 1 ) );
                    return true;
                } else
                if( m_rcThumb.bottom <= pt.y &&
                    m_rcTrack.bottom > pt.y )
                {
                    Scroll( m_nPageSize - 1 );
                    return true;
                }
            }

            break;
        }

        case WM_LBUTTONUP:
        {
			SetDragged(NULL);            
			ReleaseCapture();
            UpdateThumbRect();
            m_Arrow = CLEAR;
            break;
        }

        case WM_MOUSEMOVE:
        {
            if( IsDragged() )
            {
                m_bDragMove = true;
                m_rcThumb.bottom += pt.y - ThumbOffsetY - m_rcThumb.top;
                m_rcThumb.top = pt.y - ThumbOffsetY;
                if( m_rcThumb.top < m_rcTrack.top )
                    OffsetRect( &m_rcThumb, 0, m_rcTrack.top - m_rcThumb.top );
                else
                if( m_rcThumb.bottom > m_rcTrack.bottom )
                    OffsetRect( &m_rcThumb, 0, m_rcTrack.bottom - m_rcThumb.bottom );

                // Compute first item index based on thumb position

                int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize;  // Largest possible index for first item
                int nMaxThumb = RectHeight( m_rcTrack ) - RectHeight( m_rcThumb );  // Largest possible thumb position from the top
                int nOldPos = m_nPosition;
				if(nMaxFirstItem != 0)
				{
					m_nPosition = m_nStart +
								  ( m_rcThumb.top - m_rcTrack.top +
									nMaxThumb / ( nMaxFirstItem * 2 ) ) * // Shift by half a row to avoid last row covered by only one pixel
								  nMaxFirstItem  / nMaxThumb;              
				}
				else
				{
					m_nPosition = 0;
				}
				//Assert(m_nPosition>=0, L"nPosition<0");
                if (nOldPos != m_nPosition) 
				{
					mDXUTScrollBar.AcceptCurPos(m_nPosition);
                    m_pDialog->SendEvent( EVENT_SCROLLBAR_VALUE_CHANGED, true, this );
                }
                return true;
            }

            break;
        }
    }

    return false;
}


//--------------------------------------------------------------------------------------
bool CDXUTScrollBar::IsArrowUpHeld()
{
    if (HELD_UP == m_Arrow)
        return true;
    else return false;
}

bool CDXUTScrollBar::IsThumbHeld()
{
    if (IsDragged() && false == m_bDragMove)
        return true;
    else return false;
}


//--------------------------------------------------------------------------------------
void CDXUTScrollBar::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	if(m_bVisible == false)
		return ;
    // Check if the arrow button has been held for a while.
    // If so, update the thumb position to simulate repeated
    // scroll.
    if( m_Arrow != CLEAR )
    {
        double dCurrTime = DXUTGetTime();
        if( PtInRect( &m_rcUpButton, m_LastMouse ) )
        {
            switch( m_Arrow )
            {
                case CLICKED_UP:
                    if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
                    {
                        Scroll( -1 );
                        m_Arrow = HELD_UP;
                        m_dArrowTS = dCurrTime;
                    }
                    break;
                case HELD_UP:
                    if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
                    {
                        Scroll( -1 );
                        m_dArrowTS = dCurrTime;
                    }
                    break;
            }
        } else
        if( PtInRect( &m_rcDownButton, m_LastMouse ) )
        {
            switch( m_Arrow )
            {
                case CLICKED_DOWN:
                    if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
                    {
                        Scroll( 1 );
                        m_Arrow = HELD_DOWN;
                        m_dArrowTS = dCurrTime;
                    }
                    break;
                case HELD_DOWN:
                    if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
                    {
                        Scroll( 1 );
                        m_dArrowTS = dCurrTime;
                    }
                    break;
            } // end switch
        } // end if
    }
    
    // Clear the thumb dragged-and-moved flag.
    m_bDragMove = false;

    DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

    if( m_bVisible == false )
        iState = DXUT_STATE_HIDDEN;
    else if( m_bEnabled == false || m_bShowThumb == false )
        iState = DXUT_STATE_DISABLED;
    else if( m_bMouseOver )
        iState = DXUT_STATE_MOUSEOVER;
    else if( m_bHasFocus )
        iState = DXUT_STATE_FOCUS;

    float fBlendRate = ( iState == DXUT_STATE_PRESSED ) ? 0.0f : 0.8f;

    CDXUTElement* pElement;
    
    // Background track layer
    pElement = m_ElemTrack;
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    m_pDialog->DrawSprite( pElement, &m_rcTrack );

    // Up Arrow
    pElement = m_ElemUpArrow;
    m_ElemUpArrow->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    m_pDialog->DrawSprite( pElement, &m_rcUpButton );

    // Down Arrow
    pElement = m_ElemDownArrow;
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    m_pDialog->DrawSprite( pElement, &m_rcDownButton );

    // Thumb button
    pElement = m_ElemThumb;
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    m_pDialog->DrawSprite( pElement, &m_rcThumb );
}

//--------------------------------------------------------------------------------------
void CDXUTScrollBar::SetTrackRange( int nStart, int nEnd )
{
    m_nStart = nStart; 
    m_nEnd = nEnd;
	
		
    Cap();
    UpdateThumbRect();	
}
//--------------------------------------------------------------------------------------
uint CDXUTScrollBar::GetTrackSize() const
{
	return m_nEnd - m_nStart + 1; 
}
//--------------------------------------------------------------------------------------
void CDXUTScrollBar::GetTrackRange( int& nStart, int& nEnd )
{
    nStart = m_nStart;
    nEnd = m_nEnd;
}
//--------------------------------------------------------------------------------------
void CDXUTScrollBar::Cap()  // Clips position at boundaries. Ensures it stays within legal range.
{

	uint nPrePos = m_nPosition;
    if( m_nPosition < m_nStart ||
        m_nEnd - m_nStart <= m_nPageSize )
    {
        m_nPosition = m_nStart;		
    }
    else if (m_nPosition + m_nPageSize > m_nEnd) {
        m_nPosition = m_nEnd - m_nPageSize;		
    }
	//Assert(m_nPosition>=0, L"nPosition<0");
	//if(nPrePos != m_nPosition)
	//	mDXUTScrollBar.AcceptCurPos(m_nPosition);
}
//--------------------------------------------------------------------------------------
void CDXUTScrollBar::SetTrackPos( int nPosition )
{
	uint nPrePos = m_nPosition ;
	m_nPosition = nPosition ;
	Cap() ;
	if( nPrePos != m_nPosition )
		mDXUTScrollBar.AcceptCurPos(m_nPosition) ;
	UpdateThumbRect() ;
	//Assert(m_nPosition>=0, L"nPosition<0");
}
//--------------------------------------------------------------------------------------
void CDXUTScrollBar::Visit(ScrollBarEvent::MaxSize& e) 
{
	m_nEnd		= m_nStart + e.nMaxSize;	
	m_nPageSize = e.nPageSize;
	Cap();
	UpdateThumbRect();	
}

//--------------------------------------------------------------------------------------
void CDXUTScrollBar::Visit(ScrollBarEvent::ScrollAmount& e) 
{
	Scroll(e.nScrollAmount);
}
//--------------------------------------------------------------------------------------
void CDXUTScrollBar::AddVisitor(ScrollBarEvent::BarVisitor* pVis)
{
	mDXUTScrollBar.Register(pVis);
}