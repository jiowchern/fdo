//
// CDXUTButton.cpp
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
//#include "Global.h"
#include "TextObject.h"
//#include "BinaryTableManager.h"

using namespace FDO;

#include "CDXUTDialog.h"
#pragma hdrstop
#include "CDXUTButton.h"
//--------------------------------------------------------------------------------------
// CDXUTButton class
//--------------------------------------------------------------------------------------
std::string CDXUTButton::sm_actionMouseAnim; ///點選_動作
std::string CDXUTButton::sm_normalMouseAnim; ///點選_常態

void CDXUTButton::OnHotkey() 
{
    m_pDialog->RequestFocus( this ); 
    m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this ); 
}

BOOL CDXUTButton::ContainsPoint( POINT pt ) 
{ 
    return PtInRect( &m_rcBoundingBox, pt ); 
}

bool CDXUTButton::CanHaveFocus() 
{
    return (m_bVisible && m_bEnabled); 
}

//-----------------------------------------------------------------------------
// CheckBox control
//-----------------------------------------------------------------------------

void CDXUTCheckBox::OnHotkey()
{
    m_pDialog->RequestFocus( this ); 
    SetCheckedInternal( !m_bChecked, true ); 
}

bool CDXUTCheckBox::GetChecked() 
{
    return m_bChecked; 
}

void CDXUTCheckBox::SetChecked( bool bChecked ) 
{ 
    SetCheckedInternal( bChecked, false ); 
}

//-----------------------------------------------------------------------------
// RadioButton control
//-----------------------------------------------------------------------------

void CDXUTRadioButton::OnHotkey() 
{ 
    m_pDialog->RequestFocus( this ); 
    SetCheckedInternal( true, true, true ); 
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDXUTRadioButton::SelectRadioButton(bool bCheck, bool bClearGroup, bool bFromInput, bool bSendEvent)
{
	SetCheckedInternal( bCheck, bClearGroup, bFromInput, bSendEvent ); 
}

void CDXUTRadioButton::SetChecked( bool bChecked, bool bClearGroup, bool bSendEvent ) 
{ 
    SetCheckedInternal( bChecked, bClearGroup, false, bSendEvent ); 
}

void CDXUTRadioButton::SetButtonGroup( UINT nButtonGroup ) 
{ 
    m_nButtonGroup = nButtonGroup; 
}

UINT CDXUTRadioButton::GetButtonGroup() 
{ 
    return m_nButtonGroup; 
}
    
CDXUTButton::CDXUTButton( CDXUTDialog *pDialog )
{
	SetOneLine(false); // 
    m_Type = DXUT_CONTROL_BUTTON;
    m_pDialog = pDialog;

    m_bPressed = false;
    m_nHotkey = 0;
    m_bSwitchMouseButton = false;
    m_pTextObject = NULL;

	m_eventType = etClick;
}

bool CDXUTButton::SwitchMouseButton(bool bSwitch)
{
    bool bOldValue = m_bSwitchMouseButton;
    m_bSwitchMouseButton = bSwitch;
    return bOldValue;
}

//--------------------------------------------------------------------------------------
bool CDXUTButton::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    m_bPressed = true;
                    return true;
            }
        }

        case WM_KEYUP:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    if( m_bPressed == true )
                    {
                        m_bPressed = false;
                        m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this );
                    }
                    return true;
            }
        }
    }
    return false;
}

//--------------------------------------------------------------------------------------
bool CDXUTButton::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
// Returns bNoFurtherProcessing.
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    m_MousePoint = pt;
    
    // 滑鼠左右鍵的設定。
    // Left button messages
    UINT uLButtonDown = WM_LBUTTONDOWN;
    UINT uLButtonUp = WM_LBUTTONUP;
    UINT uLButtonDblClk = WM_LBUTTONDBLCLK;
    
    if (m_bSwitchMouseButton) {
        // Left button messages
        uLButtonDown = WM_RBUTTONDOWN;
        uLButtonUp = WM_RBUTTONUP;
        uLButtonDblClk = WM_RBUTTONDBLCLK;
    } // end if
    
    if (uLButtonDown == uMsg || uLButtonDblClk == uMsg) 
    {
        if( ContainsPoint( pt ) )
        {          
            // Pressed while inside the control
            m_bPressed = true;
            if (m_pDialog)
                m_pDialog->SetPressedControl(this);
            SetCapture( DXUTGetHWND() );

            //if( !m_bHasFocus )
            //    m_pDialog->RequestFocus( this );
           // string str;
           // str.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 887, CodeInlineText_Text ));//點選_動作
            g_Animation2DManager.GetMouseAnimation()->InsertPlay( sm_actionMouseAnim.c_str() );

            //音效 *** modify
            PlaySoundWAV();  //under control class

			if(m_eventType == etPressRelease && uLButtonDown == uMsg){
				 m_pDialog->SendEvent( EVENT_BUTTON_PRESSED, true, this );
			}
            return true;
        }
    }
    else if (uLButtonUp == uMsg)
    {
        if (m_bPressed)
        {              
            m_bPressed = false;
            ReleaseCapture();

            if( !m_pDialog->m_bKeyboardInput )
                m_pDialog->ClearFocus();

            // Button clicked
            if (ContainsPoint(pt)) {
				if(m_eventType == etClick)
					m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this );
				else {
					m_pDialog->SendEvent( EVENT_BUTTON_RELEASED, true, this);
				}
            } 
            return true;
        }
    }
    else
    {
        switch( uMsg )
        {
            case WM_MOUSEMOVE:
            {
                if (ContainsPoint(pt)) 
				{
                    if (!m_bPressed) 
					{
                       // string str;
                       // str.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 710, CodeInlineText_Text ));//點選
                       // str.append( "_" );
                        //str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 884, CodeInlineText_Text ));//常態
                        g_Animation2DManager.GetMouseAnimation()->Play( sm_normalMouseAnim.c_str() );
                    } // end if
                } 
                else 
				{ // cursor moved out
                    if (m_bPressed) 
					{
                        m_bPressed = false;
                        //ReleaseCapture();
                        if( !m_pDialog->m_bKeyboardInput )
                            m_pDialog->ClearFocus();
                    } // end if
                } // end else
                break;
            }
        } // end switch
    }
    return false;
}

//--------------------------------------------------------------------------------------
void CDXUTButton::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    int nOffsetX = 0;
    int nOffsetY = 0;

    DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

    if( m_bVisible == false )
    {
        iState = DXUT_STATE_HIDDEN;
    }
    else if( m_bEnabled == false )
    {
        iState = DXUT_STATE_DISABLED;
    }
    else if( m_bPressed )
    {
        iState = DXUT_STATE_PRESSED;

        // ming mark
        //nOffsetX = 1;
        //nOffsetY = 1;
    }
    else if( m_bMouseOver )
    {
        iState = DXUT_STATE_MOUSEOVER;

        // ming mark
        //nOffsetX = -1;
        //nOffsetY = -1;
    }
    else if( m_bHasFocus )
    {
        iState = DXUT_STATE_FOCUS;
    }

    // Background fill layer
    //TODO: remove magic numbers
	CDXUTElement* pElement0 = GetElement( 0 ) ;
    
    float fBlendRate = ( iState == DXUT_STATE_PRESSED ) ? 0.0f : 0.8f;

    RECT rcWindow = m_rcBoundingBox;
    OffsetRect( &rcWindow, nOffsetX, nOffsetY );

    // Blend current color
    pElement0->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    pElement0->FontColor.Blend( iState, fElapsedTime, fBlendRate );

    m_pDialog->DrawSprite( pElement0, &rcWindow );
    DrawText(_GetText(), pElement0, &rcWindow, false);

    // Main button
	CDXUTElement* pElement1 = GetElement( 1 ) ;

    // Blend current color
    pElement1->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    pElement1->FontColor.Blend( iState, fElapsedTime, fBlendRate );

    pElement1->dwTextFormat = pElement0->dwTextFormat;
    m_pDialog->DrawSprite( pElement1, &rcWindow );
    DrawText(_GetText(), pElement1, &rcWindow, false);
}
//--------------------------------------------------------------------------------------
void CDXUTButton::DrawText(LPCWSTR szText, CDXUTElement* pElement, RECT* pRect, bool bShadow)
{
    // Create a new TextObject for button text.
    if (!m_pTextObject) {
        m_pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, pElement, szText, -1, RectWidth(*pRect));
    }
    // pElement's FontColor varies by fElapsedTime. See CDXUTButton::Render().
    m_pTextObject->ChangeDefaultColor(pElement->FontColor.Current);
    m_pDialog->DrawTextObject(m_pTextObject, pElement, pRect, bShadow);
}
//--------------------------------------------------------------------------------------
void CDXUTButton::SetColor( D3DCOLOR color )
{
	CDXUTElement* pElement = GetElement( 0 ) ;
	if( pElement )
	{
		pElement->TextureColor.States[ DXUT_STATE_NORMAL ] = color ;
	}
}
//--------------------------------------------------------------------------------------
void CDXUTButton::SetNormalStateAlpha( int iAlpha )
{
    GetElement( 1 )->TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(iAlpha, 255, 255, 255);
}
//--------------------------------------------------------------------------------------
void CDXUTButton::SetFocusTexture( const string &FileName, RECT* prcTexture )
{
    CDXUTControl::SetTexture( FileName, prcTexture, 0 );

    GetElement( 0 )->FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    GetElement( 0 )->TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    GetElement( 0 )->TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(0, 255, 255, 255);
}
//--------------------------------------------------------------------------------------
void CDXUTButton::SetFocusTexture( int iTexture, RECT* prcTexture )
{
    CDXUTControl::SetTexture( iTexture, prcTexture, 0 );

    GetElement( 0 )->FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
    GetElement( 0 )->TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(200, 255, 255, 255);
    GetElement( 0 )->TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(0, 255, 255, 255);
}
//--------------------------------------------------------------------------------------
void CDXUTButton::SetNormalTexture( const string &FileName, RECT* prcTexture )
{
    CDXUTControl::SetTexture( FileName, prcTexture, 1 );

    GetElement( 1 )->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 255, 255, 255);
    GetElement( 1 )->TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(60, 0, 0, 0);
    GetElement( 1 )->TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);
}
//--------------------------------------------------------------------------------------
void CDXUTButton::SetNormalTexture( int iTexture, RECT* prcTexture )
{
    CDXUTControl::SetTexture( iTexture, prcTexture, 1 );

    GetElement( 1 )->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 255, 255, 255);
    GetElement( 1 )->TextureColor.States[ DXUT_STATE_PRESSED ] = D3DCOLOR_ARGB(60, 0, 0, 0);
    GetElement( 1 )->TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);
}

//--------------------------------------------------------------------------------------
// CDXUTCheckBox class
//--------------------------------------------------------------------------------------

CDXUTCheckBox::CDXUTCheckBox( CDXUTDialog *pDialog ) : m_nStyle(STYLE_NORMAL)
{
    m_Type = DXUT_CONTROL_CHECKBOX;
    m_pDialog = pDialog;

    m_bChecked = false;
}
   

//--------------------------------------------------------------------------------------
bool CDXUTCheckBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    m_bPressed = true;
                    return true;
            }
        }

        case WM_KEYUP:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    if( m_bPressed == true )
                    {
                        m_bPressed = false;
                        SetCheckedInternal( !m_bChecked, true );
                    }
                    return true;
            }
        }
    }
    return false;
}

//--------------------------------------------------------------------------------------
bool CDXUTCheckBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if( ContainsPoint( pt ) )
            {
                // Pressed while inside the control
                m_bPressed = true;
                SetCapture( DXUTGetHWND() );

                /*if( !m_bHasFocus )
                    m_pDialog->RequestFocus( this );*/

                 //音效 *** modify
                PlaySoundWAV();  //under control class

                return true;
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if( m_bPressed )
            {
                m_bPressed = false;
                ReleaseCapture();

                // Button click
                if( ContainsPoint( pt ) )
                    SetCheckedInternal( !m_bChecked, true );
                
                return true;
            }

            break;
        }
    };
    
    return false;
}

//--------------------------------------------------------------------------------------
void CDXUTCheckBox::SetCheckedInternal( bool bChecked, bool bFromInput ) 
{ 
    m_bChecked = bChecked; 
    m_pDialog->SendEvent( EVENT_CHECKBOX_CHANGED, bFromInput, this ); 
}

//--------------------------------------------------------------------------------------
BOOL CDXUTCheckBox::ContainsPoint( POINT pt ) 
{
    return ( PtInRect( &m_rcBoundingBox, pt ) ||
             PtInRect( &m_rcButton, pt ) );
}

//--------------------------------------------------------------------------------------
void CDXUTCheckBox::UpdateRects()
{
    CDXUTButton::UpdateRects();

	if(STYLE_NORMAL == m_nStyle)
	{
		m_rcButton = m_rcBoundingBox;
		m_rcButton.right = m_rcButton.left + RectHeight( m_rcButton );

		m_rcText = m_rcBoundingBox;
		m_rcText.left += (int) ( 1.25f * RectWidth( m_rcButton ) );
	}
	else if(STYLE_BUTTON == m_nStyle)
	{
		m_rcButton = m_rcBoundingBox;
		m_rcText = m_rcBoundingBox;
	}    
}

//--------------------------------------------------------------------------------------
void CDXUTCheckBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

    if( m_bVisible == false )
        iState = DXUT_STATE_HIDDEN;
    else if( m_bEnabled == false )
        iState = DXUT_STATE_DISABLED;
    else if( m_bPressed )
        iState = DXUT_STATE_PRESSED;
    else if( m_bMouseOver )
        iState = DXUT_STATE_MOUSEOVER;
    else if( m_bHasFocus )
        iState = DXUT_STATE_FOCUS;

	CDXUTElement* pElement = GetElement( 0 ) ;

    float fBlendRate = ( iState == DXUT_STATE_PRESSED ) ? 0.0f : 0.8f;

    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

    m_pDialog->DrawSprite( pElement, &m_rcButton );
    DrawText(_GetText(), pElement, &m_rcText, m_bShadow);

    if( !m_bChecked )
        iState = DXUT_STATE_HIDDEN;

	pElement = GetElement( 1 ) ;

    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    m_pDialog->DrawSprite( pElement, &m_rcButton );

	pElement = GetElement( 0 ) ;
	DrawText(_GetText(), pElement, &m_rcText, m_bShadow);

}
//--------------------------------------------------------------------------------------
void CDXUTCheckBox::SetStyle( int nStyle /*= STYLE_NORMAL*/)
{
	m_nStyle = nStyle;
	UpdateRects();
}
//--------------------------------------------------------------------------------------
// CDXUTRadioButton class
//--------------------------------------------------------------------------------------

CDXUTRadioButton::CDXUTRadioButton( CDXUTDialog *pDialog )
{
    m_Type = DXUT_CONTROL_RADIOBUTTON;
    m_pDialog = pDialog;
}

//--------------------------------------------------------------------------------------
bool CDXUTRadioButton::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    m_bPressed = true;
                    return true;
            }
        }

        case WM_KEYUP:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    if( m_bPressed == true )
                    {
                        m_bPressed = false;
                        
                        m_pDialog->ClearRadioButtonGroup( m_nButtonGroup );
                        m_bChecked = !m_bChecked;

                        m_pDialog->SendEvent( EVENT_RADIOBUTTON_CHANGED, true, this );
                    }
                    return true;
            }
        }
    }
    return false;
}


//--------------------------------------------------------------------------------------
bool CDXUTRadioButton::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if( ContainsPoint( pt ) )
            {
                // Pressed while inside the control
                m_bPressed = true;
                SetCapture( DXUTGetHWND() );

                /*if( !m_bHasFocus )
                    m_pDialog->RequestFocus( this );*/

                //音效 *** modify
                PlaySoundWAV();  //under control class

                return true;
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if( m_bPressed )
            {
                m_bPressed = false;
                ReleaseCapture();

                // Button click
                if( ContainsPoint( pt ) )
                {
                    m_pDialog->ClearRadioButtonGroup( m_nButtonGroup );
                    m_bChecked = !m_bChecked;

                    m_pDialog->SendEvent( EVENT_RADIOBUTTON_CHANGED, true, this );
                }

                return true;
            }

            break;
        }
    };
    
    return false;
}

//--------------------------------------------------------------------------------------
void CDXUTRadioButton::SetCheckedInternal(bool bChecked, bool bFromInput, bool bClearGroup, bool bSendEvent )
{
    if( bChecked && bClearGroup )
        m_pDialog->ClearRadioButtonGroup( m_nButtonGroup );

    m_bChecked = bChecked;

	if( bSendEvent )
		m_pDialog->SendEvent( EVENT_RADIOBUTTON_CHANGED, bFromInput, this );
}

//--------------------------------------------------------------------------------------
