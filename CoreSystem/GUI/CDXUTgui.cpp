//--------------------------------------------------------------------------------------
// File: DXUTgui.cpp
//
// Desc:
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "stdafx.h"
//#pragma hdrstop

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
#include "ImageControls.h"

#pragma hdrstop
#include "CDXUTgui.h"

using namespace FDO;

//==========================================================================================
// Variables
//==========================================================================================

//DXUTImageListBoxItem* CDXUTControl::s_pDragItem = NULL;

// DXUT_SCREEN_VERTEX
DWORD DXUT_SCREEN_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
const int size_of_DXUT_SCREEN_VERTEX = sizeof(DXUT_SCREEN_VERTEX);

//==========================================================================================
// Utility routines
//==========================================================================================

// 雙碼轉萬國碼
void ANSItoUNICODE( WCHAR *Dest, const std::string &Source )
{
    MultiByteToWideChar( CP_ACP, 0, Source.c_str(), -1, Dest, 510 );
}
// 雙碼轉萬國碼
WCHAR *ANSItoUNICODE( const std::string &Source )
{
    static WCHAR strText[512];
    MultiByteToWideChar( CP_ACP, 0, Source.c_str(), -1, strText, 510 );
    return strText;
}
// 萬國碼轉雙碼
string UNICODEtoANSI( const WCHAR *Source )
{
    char strText[512];
    WideCharToMultiByte ( CP_ACP, 0, Source, -1, strText, 510, NULL, NULL );
    return string( strText );
}
// 萬國碼轉越南(VISCII)
string UNICODEtoVISCII( const WCHAR *Source )
{
    char strText[512];
	WideCharToMultiByte ( 1258, 0, Source, -1, strText, 510, NULL, NULL );
    return string( strText );
}

//--------------------------------------------------------------------------------------
CDXUTSlider::CDXUTSlider( CDXUTDialog *pDialog )
{
    m_Type = DXUT_CONTROL_SLIDER;
    m_pDialog = pDialog;

    m_nMin = 0;
    m_nMax = 100;
    m_nValue = 50;

    m_bPressed = false;
}


//--------------------------------------------------------------------------------------
BOOL CDXUTSlider::ContainsPoint( POINT pt )    //拖曳 ***
{ 
    return ( PtInRect( &m_rcBoundingBox, pt ) || 
             PtInRect( &m_rcButton, pt ) ); 
}


//--------------------------------------------------------------------------------------
void CDXUTSlider::UpdateRects()
{
    CDXUTControl::UpdateRects();

    m_rcButton = m_rcBoundingBox;
    m_rcButton.right = m_rcButton.left + RectHeight( m_rcButton );
    OffsetRect( &m_rcButton, -RectWidth( m_rcButton )/2, 0 );

    m_nButtonX = (int) ( (m_nValue - m_nMin) * (float)RectWidth( m_rcBoundingBox ) / (m_nMax - m_nMin) );
    OffsetRect( &m_rcButton, m_nButtonX, 0 );
}

int CDXUTSlider::ValueFromPos( int x )
{ 
    float fValuePerPixel = (float)(m_nMax - m_nMin) / RectWidth( m_rcBoundingBox );
    return (int) (0.5f + m_nMin + fValuePerPixel * (x - m_rcBoundingBox.left)) ; 
}

//--------------------------------------------------------------------------------------
bool CDXUTSlider::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_HOME:
                    SetValueInternal( m_nMin, true );
                    return true;

                case VK_END:
                    SetValueInternal( m_nMax, true );
                    return true;

                case VK_PRIOR:
                case VK_LEFT:
                case VK_DOWN:
                    SetValueInternal( m_nValue - 1, true );
                    return true;

                case VK_NEXT:
                case VK_RIGHT:
                case VK_UP:
                    SetValueInternal( m_nValue + 1, true );
                    return true;
            }
            break;
        }
    }
    

    return false;
}


//--------------------------------------------------------------------------------------
bool CDXUTSlider::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if( PtInRect( &m_rcButton, pt ) )
            {
                //音效 *** modify
                PlaySoundWAV();  //under control class

                // Pressed while inside the control
                m_bPressed = true;
                SetCapture( DXUTGetHWND() );

                m_nDragX = pt.x;
                //m_nDragY = pt.y;
                m_nDragOffset = m_nButtonX - m_nDragX;

                //m_nDragValue = m_nValue;

                if( !m_bHasFocus )
                    m_pDialog->RequestFocus( this );

                return true;
            }

            if( PtInRect( &m_rcBoundingBox, pt ) )
            {
                m_nDragX = pt.x;
                m_nDragOffset = 0;
                m_bPressed = true;

                if( !m_bHasFocus )
                    m_pDialog->RequestFocus( this );

                if( pt.x > m_nButtonX + m_x )
                {
                    SetValueInternal( m_nValue + 1, true );
                    return true;
                }

                if( pt.x < m_nButtonX + m_x )
                {
                    SetValueInternal( m_nValue - 1, true );
                    return true;
                }
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if( m_bPressed )
            {
                m_bPressed = false;
                ReleaseCapture();
                m_pDialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, true, this );

                return true;
            }

            break;
        }

        case WM_MOUSEMOVE:
        {
            if( m_bPressed )
            {
                SetValueInternal( ValueFromPos( m_x + pt.x + m_nDragOffset ), true );
                return true;
            }

            break;
        }
    };
    
    return false;
}


//--------------------------------------------------------------------------------------
void CDXUTSlider::SetRange( int nMin, int nMax ) 
{
    m_nMin = nMin;
    m_nMax = nMax;

    SetValueInternal( m_nValue, false );
}


//--------------------------------------------------------------------------------------
void CDXUTSlider::SetValueInternal( int nValue, bool bFromInput )
{
    // Clamp to range
    nValue = __max( m_nMin, nValue );
    nValue = __min( m_nMax, nValue );
    
    if( nValue == m_nValue )
        return;

    m_nValue = nValue;
    UpdateRects();

    m_pDialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, bFromInput, this );
}


//--------------------------------------------------------------------------------------
void CDXUTSlider::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    // ming mark
    //int nOffsetX = 0;
    //int nOffsetY = 0;

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

        //nOffsetX = 1;
        //nOffsetY = 2;
    }
    else if( m_bMouseOver )
    {
        iState = DXUT_STATE_MOUSEOVER;
        
        //nOffsetX = -1;
        //nOffsetY = -2;
    }
    else if( m_bHasFocus )
    {
        iState = DXUT_STATE_FOCUS;
    }

    float fBlendRate = ( iState == DXUT_STATE_PRESSED ) ? 0.0f : 0.8f;

	CDXUTElement* pElement = GetElement( 0 ) ;
    
    // Blend current color
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate ); 
    m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );

    //TODO: remove magic numbers
	pElement = GetElement( 1 ) ;
       
    // Blend current color
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    m_pDialog->DrawSprite( pElement, &m_rcButton );
}

//--------------------------------------------------------------------------------------
void DXUTBlendColor::Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor,
      D3DCOLOR hiddenColor, D3DCOLOR unfocusedColor )
{
    for( int i=0; i < MAX_CONTROL_STATES; i++ )
    {
        States[ i ] = defaultColor;
    }

    States[ DXUT_STATE_DISABLED ] = disabledColor;
    States[ DXUT_STATE_HIDDEN ] = hiddenColor;
    States[ DXUT_STATE_UNFOCUS ] = unfocusedColor;
    Current = hiddenColor;
}

//--------------------------------------------------------------------------------------

void DXUTBlendColor::Blend( UINT iState, float fElapsedTime, float fRate )
{
    D3DXCOLOR destColor = States[ iState ];
    FLOAT s = 1.0f - fRate;
    D3DXColorLerp(&Current, &Current, &destColor, s);	
/*
    // ming edit >>
    if (int(fElapsedTime) == 0)
        fElapsedTime = 1.0f;
    if (int(fElapsedTime) != 0) {
        FLOAT s = 1.0f - powf(fRate, 30 * fElapsedTime);
        D3DXColorLerp(&Current, &Current, &destColor, s);
    } // end if
    // ming edit <<
*/
}

//--------------------------------------------------------------------------------------
void CDXUTElement::SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor )
{
    this->iTexture = iTexture;

    if( prcTexture )
        rcTexture = *prcTexture;
    else
        SetRectEmpty( &rcTexture );
    
    TextureColor.Init( defaultTextureColor );
}
    

//--------------------------------------------------------------------------------------
void CDXUTElement::SetFont( UINT iFont, D3DCOLOR defaultFontColor, DWORD dwTextFormat )
{
    this->iFont = iFont;
    this->dwTextFormat = dwTextFormat;

    FontColor.Init( defaultFontColor );
}


//--------------------------------------------------------------------------------------
void CDXUTElement::Refresh()
{
    TextureColor.Current = TextureColor.States[ DXUT_STATE_HIDDEN ];
    FontColor.Current = FontColor.States[ DXUT_STATE_HIDDEN ];
}

//--------------------------------------------------------------------------------------
// CDXUTProgressBar class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CDXUTProgressBar::CDXUTProgressBar( CDXUTDialog *pDialog )
{
    m_Type = DXUT_CONTROL_PROGRESSBAR;
    m_pDialog = pDialog;

    Value = 0;
    MAX = 0;
}
//--------------------------------------------------------------------------------------
CDXUTProgressBar::~CDXUTProgressBar()
{
    /*for( int i=0; i < m_Elements.GetSize(); i++ )
    {
        CDXUTElement* pElement = m_Elements.GetAt( i );
        SAFE_DELETE( pElement );
    }
    m_Elements.RemoveAll();*/
}
//--------------------------------------------------------------------------------------
void CDXUTProgressBar::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    int width = m_width;

    if( MAX )
        width = m_width * Value / MAX;

    if( width <= 0 )
    {
        if( Value )
            width = 1;
        else
            width = 0;
    }

    if( width > m_width )
        width = m_width;

    DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

	CDXUTElement* pElement = GetElement( 0 ) ;
    
    float fBlendRate = ( iState == DXUT_STATE_PRESSED ) ? 0.0f : 0.8f;

    RECT rcWindow = m_rcBoundingBox;

    // Blend current color
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

    rcWindow.right = rcWindow.left + m_width;
    m_pDialog->DrawSprite( pElement, &rcWindow );
    //m_pDialog->DrawText( m_strText, pElement, &rcWindow );

    // Main button
	pElement = GetElement( 1 ) ;

    // Blend current color
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

    rcWindow.right = rcWindow.left + width;
    m_pDialog->DrawSprite( pElement, &rcWindow );
    //m_pDialog->DrawText( m_strText, pElement, &rcWindow );
}
//--------------------------------------------------------------------------------------
moCDXUTBtnListBox::moCDXUTBtnListBox( CDXUTDialog *pDialog ) :
                                        CDXUTListBox(pDialog)
{
    for( int i=0; i<5; ++i )
        pBtn[i]=NULL;
}
//--------------------------------------------------------------------------------------
moCDXUTBtnListBox::~moCDXUTBtnListBox()
{
}
//--------------------------------------------------------------------------------------
bool moCDXUTBtnListBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    for( int i=0; i<5; ++i )
    {
        if( pBtn[i] && pBtn[i]->ContainsPoint(pt) )
        {
            if(pBtn[i]->HandleMouse( uMsg, pt, wParam, lParam ))
                return true;
        }
    }

    return CDXUTListBox::HandleMouse( uMsg, pt, wParam, lParam );
}
//--------------------------------------------------------------------------------------
void moCDXUTBtnListBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    CDXUTListBox::Render(pd3dDevice,fElapsedTime);
    for( int i=0; i<5; ++i )
    {
        if(pBtn[i])
            pBtn[i]->Render(pd3dDevice,fElapsedTime);
    }
}

//--------------------------------------------------------------------------------------
// CDXUTBtnImageListBox
//--------------------------------------------------------------------------------------
CDXUTBtnImageListBox::CDXUTBtnImageListBox( CDXUTDialog *pDialog ) : CDXUTImageListBox(pDialog)
{
    for( int i=0; i<2; ++i )
        pBtn[i] = NULL;
}
//--------------------------------------------------------------------------------------
CDXUTBtnImageListBox::~CDXUTBtnImageListBox()
{
}
//--------------------------------------------------------------------------------------
bool CDXUTBtnImageListBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    for( int i=0; i<2; ++i )
    {
        if( pBtn[i] && pBtn[i]->ContainsPoint(pt) )
        {
            if(pBtn[i]->HandleMouse( uMsg, pt, wParam, lParam ))
                return true;
        }
    }

    return CDXUTImageListBox::HandleMouse( uMsg, pt, wParam, lParam );
}
//--------------------------------------------------------------------------------------
void CDXUTBtnImageListBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if( m_bVisible == false )
        return;

    // 取出Element
	CDXUTElement* pElement = GetElement( 0 ) ;
    pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
    pElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

	CDXUTElement* pSelElement = GetElement( 1 ) ;
    pSelElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
    pSelElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

    // 畫出背景
    RECT rcBoundingBox = m_rcBoundingBox;
    rcBoundingBox.right -= m_nSBWidth;
    m_pDialog->DrawSprite( pElement, &rcBoundingBox );

    // Render the text
    // 有Item才畫
    if( m_Items.GetSize() > 0 )
    {
        // Find out the height of a single line of text
        RECT rc = m_rcText;
        RECT rcSel = m_rcSelection;
        // 改變bottom的位置
        rc.bottom = rc.top + m_iItemHeight; // 32為固定高度

        // Update the line height formation
        // 計算出一行的高度
        m_nTextHeight = RectHeight( rc ) + m_nSpace;

        static bool bSBInit;
        if( !bSBInit )
        {
            // Update the page size of the scroll bar
            if( m_nTextHeight )
                m_ScrollBar.SetPageSize( ( RectHeight( m_rcText ) + m_nSpace ) / m_nTextHeight );
            else
                m_ScrollBar.SetPageSize( RectHeight( m_rcText ) );
            bSBInit = true;
        }

        rc.right = m_rcText.right;
        for( int i = 0; i < (int)m_Items.GetSize(); ++i )
        {
            DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
            pItem->m_Visible = true;

            // 設定不可視的圖案為全透明(目的是用來增加視覺效果)
            if( i<m_ScrollBar.GetTrackPos() || rc.bottom > m_rcText.bottom )
            {
                CDXUTElement* pElement = pItem->Image->GetElement(0);
                pElement->TextureColor.Current = D3DXCOLOR(0,0,0,0);
                pItem->m_Visible = false;
                continue;
            }

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

            // 繪製選擇的反白區域
            if( bSelectedStyle )
            {       
                rcSel.top = rc.top;
                rcSel.bottom = rc.bottom;
                InflateRect( &rcSel, 0, -1 ); // 上下範圍-1
                m_pDialog->DrawSprite( pSelElement, &rcSel );
            }

            // 這裡處理Image
            RECT rcImage = rc;  // 畫圖的範圍  
            CDXUTElement* pElement = pItem->Image->GetElement(0); // 取得Image的Element
            pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime ); // Blend處理
            m_pDialog->DrawSprite( pElement, &rcImage ); // 畫

            // 畫出數量
            if( pItem->m_bStatic )
            {
                pElement = pItem->Static->GetElement(0);
                pElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
                rcImage.right = rcImage.left + m_iStaticWidth;
                m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcImage ); // 畫
            }

            // 這裡畫EditBox
            if( m_bEditVisible )
            {
                pItem->EditBox->SetLocation( rc.right-m_iEditOffsetX, rc.bottom-m_iEditOffsetY );
                pItem->EditBox->SetDialog( m_pDialog );
                pItem->EditBox->Render( pd3dDevice, fElapsedTime );
            }

            OffsetRect( &rc, 0, m_nTextHeight );
        }
    }
    else
    { // EditBox 失焦
    }

    // Render the scroll bar
    m_ScrollBar.Render( pd3dDevice, fElapsedTime );

    for( int i=0; i<2; ++i )
    {
        if(pBtn[i])
            pBtn[i]->Render( pd3dDevice, fElapsedTime );
    }
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTBtnImageListBox::AddItem( int iStaticNum, int iWidth1, int iHeight1, const string& strEditText,
                                       int iWidth2, int iHeight2, const string& strImageFile, void* pData )

{
    DXUTImageListBoxItem *pNewItem = new DXUTImageListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;
    
    // 設定Static
    pNewItem->Static.reset( new CDXUTStatic(m_pDialog) );
    m_pDialog->InitControl( pNewItem->Static.get() );
    pNewItem->Static->SetTextColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) ); // Change color to black
    pNewItem->Static->GetElement( 0 )->dwTextFormat = DT_CENTER | DT_VCENTER;
    pNewItem->Static->SetSize( iWidth1, iHeight1 );
    pNewItem->Static->SetText( iStaticNum );

    if( iStaticNum )
        pNewItem->m_bStatic = true;
    else
        pNewItem->m_bStatic = false;

    // 設定EditBox
    pNewItem->EditBox.reset( new CDXUTEditBox(m_pDialog) );
    m_pDialog->InitControl( pNewItem->EditBox.get() );
    pNewItem->EditBox->SetSize( iWidth2, iHeight2 );
    pNewItem->EditBox->SetText( strEditText );
    pNewItem->EditBox->ClearTexture();
	pNewItem->EditBox->SetFrameColor( m_EditFrameColor );

    // 設定Image
    WCHAR wszImageFile[256];
    ANSItoUNICODE( wszImageFile, strImageFile );

    pNewItem->Image.reset( new CDXUTImage(m_pDialog) );
    pNewItem->Image->SetSize( iWidth1, iHeight1 );
    pNewItem->Image->SetImage( wszImageFile );

    return CDXUTImageListBox::AddItem( pNewItem, iStaticNum );
}

//--------------------------------------------------------------------------------------
CDXUTSpecialButton::CDXUTSpecialButton( CDXUTDialog *pDialog )
{
    m_Type = DXUT_CONTROL_SPECIALBUTTON;
    m_pDialog = pDialog;
    m_nHotkey = 0;
}

//--------------------------------------------------------------------------------------
bool CDXUTSpecialButton::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bVisible || !m_bEnabled )
        return false;

    m_MousePoint = pt;

    switch( uMsg )
    {
        case WM_MOUSEMOVE:
        {
            m_pDialog->SendEvent( EVENT_BUTTON_MOUSEMOVE, true, this );
            return true;
        }

        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if( ContainsPoint( pt ) )
            {
                //音效 *** modify
                PlaySoundWAV();  //under control class

                m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this );
                return true;
            }
            break;
        }
    };

    return false;
}
//--------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CDXUTControl gEmpty;
CDXUTSlot::CDXUTSlot(CDXUTDialog *pDialog )
:CDXUTControl(pDialog) , mpControl(&gEmpty)
{


}
//////////////////////////////////////////////////////////////////////////
CDXUTSlot::~CDXUTSlot()
{

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
HRESULT CDXUTSlot::OnInit() 
{ 	
	return mpControl->OnInit();; 
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::Refresh()
{
	mpControl->Refresh();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) 
{
	mpControl->Render(pd3dDevice,fElapsedTime);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) 
{
	mpControl->RenderHint(pd3dDevice,fElapsedTime);
} // 畫出元件提示

// Windows message handler
//////////////////////////////////////////////////////////////////////////
bool CDXUTSlot::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{ 
	return mpControl->MsgProc(uMsg,wParam,lParam); 
}
//////////////////////////////////////////////////////////////////////////
bool CDXUTSlot::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{ 

	return mpControl->HandleKeyboard(uMsg,wParam,lParam);
}
//////////////////////////////////////////////////////////////////////////
bool CDXUTSlot::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) 
{ 

	return mpControl->HandleMouse(uMsg,pt,wParam,lParam);
}
//////////////////////////////////////////////////////////////////////////
bool CDXUTSlot::CanHaveFocus() 
{ 	
	return mpControl->CanHaveFocus(); 
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::OnFocusIn() 
{ 
	m_bHasFocus = true; 
	mpControl->OnFocusIn();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::OnFocusOut() 
{ 
	m_bHasFocus = false; 
	mpControl->OnFocusOut();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::OnMouseEnter() 
{ 
	m_bMouseOver = true; 
	mpControl->OnMouseEnter();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::OnMouseLeave() 
{ 
	m_bMouseOver = false; 
	mpControl->OnMouseLeave();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::OnHotkey() 
{
	mpControl->OnHotkey();
}
//////////////////////////////////////////////////////////////////////////
BOOL CDXUTSlot::ContainsPoint( POINT pt ) 
{ 	
	return mpControl->ContainsPoint(pt) && PtInRect( &m_rcBoundingBox, pt ); 
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::SetEnabled( bool bEnabled ) 
{ 
	m_bEnabled = bEnabled; 
	mpControl->SetEnabled(bEnabled);
}
//////////////////////////////////////////////////////////////////////////
bool CDXUTSlot::GetEnabled() 
{ 
	return mpControl->GetEnabled() && m_bEnabled; 
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::SetVisible( bool bVisible ) 
{ 
	m_bVisible = bVisible; 
	mpControl->SetVisible(bVisible);
}
//////////////////////////////////////////////////////////////////////////
bool CDXUTSlot::GetVisible() 
{ 
	return mpControl->GetVisible() && m_bVisible; 
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSlot::SetHint_WordMaxWidth( int iWidth )
{ 
	m_cxWordMaxExtent = iWidth; 
}			
//////////////////////////////////////////////////////////////////////////
// HINT的文字最大寬度（預設值25*6 PIXOL）
void CDXUTSlot::SetHintFrameColor( D3DCOLOR color )
{ 
	m_FrameColor = color; 
	mpControl->SetHintFrameColor(color);
}				
//////////////////////////////////////////////////////////////////////////
// HINT外框線的顏色（預設值是黑色全透明）
void CDXUTSlot::SetHintTextColor( D3DCOLOR color )
{ 
	m_HintTextColor = color; 
	mpControl->SetHintTextColor(color);
}				
//////////////////////////////////////////////////////////////////////////
// HINT內容文的自顏色（預設是WHITE）
void CDXUTSlot::SetHintWordFormat( DWORD dwTextFormat )
{ 
	m_dwTextFormat = dwTextFormat; 
	mpControl->SetHintWordFormat(dwTextFormat);
}	
//////////////////////////////////////////////////////////////////////////
// HINT的文字格式（預設值DT＿CENTER｜DT＿VCENTER）
void CDXUTSlot::SetHintTurnRight( bool bTrue /*= true */)
{ 
	m_bHintTurnRight = bTrue; 
	mpControl->SetHintTurnRight(bTrue);
}	
//////////////////////////////////////////////////////////////////////////
// HINT的顯示位置改至右上（預設是左上）
void CDXUTSlot::UpdateRects()
{
	//mpControl->UpdateRects();
	mpControl->SetLocation(m_x,m_y);
	mpControl->SetSize(m_width,m_height);
}
//////////////////////////////////////////////////////////////////////////