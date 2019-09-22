//
// CDXUTStatic.cpp
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
#include "DialogType.h"
#include "CFDOXML.h"
#include "TextObject.h"
//#include "Global.h"
#include "CDXUTDialog.h"
#include "CDXUTDialogResourceManager.h"
#pragma hdrstop
#include "CDXUTStatic.h"
#include "drawplane.h"
using namespace FDO;

//--------------------------------------------------------------------------------------
CDXUTStatic::CDXUTStatic( CDXUTDialog *pDialog ) : m_bOneLine(true)
{
    IsS_RenderHint=false;
    m_pTextObject = NULL;

    m_Type = DXUT_CONTROL_STATIC;
    m_pDialog = pDialog;

    //ZeroMemory( &m_strText, sizeof(m_strText) );
    //ZeroMemory( &m_strHint, sizeof(m_strHint) );

    /*for( int i=0; i < m_Elements.GetSize(); i++ )
    {
        CDXUTElement* pElement = m_Elements.GetAt( i );
        SAFE_DELETE( pElement );
    }

    m_Elements.RemoveAll();*/

}

CDXUTStatic::~CDXUTStatic()
{
    if(m_pTextObject)
    {
        delete m_pTextObject;
        m_pTextObject = NULL;
    }
}

//--------------------------------------------------------------------------------------
void CDXUTStatic::SetTextColor( D3DCOLOR Color, UINT iElement )
{
    //if( (int)iElement < m_Elements.GetSize() )
    {
		CDXUTElement* pElement = GetElement( iElement ) ;

        if( pElement )
        {
            pElement->FontColor.States[DXUT_STATE_NORMAL] = Color;
            pElement->FontColor.States[DXUT_STATE_DISABLED] = Color;
        }
    }
}
//--------------------------------------------------------------------------------------
void CDXUTStatic::SetTextDisableColor( D3DCOLOR Color, UINT iElement )
{
	//if( (int)iElement < m_Elements.GetSize() )
	{
		CDXUTElement* pElement = GetElement( iElement ) ;

		if( pElement )
		{
			//pElement->FontColor.States[DXUT_STATE_NORMAL] = Color;
			pElement->FontColor.States[DXUT_STATE_DISABLED] = Color;
		}
	}
}
//--------------------------------------------------------------------------------------

void CDXUTStatic::SetTextFocusColor( D3DCOLOR Color, UINT iElement  )
{
	//if( (int)iElement < m_Elements.GetSize() )
    {

		CDXUTElement* pElement = GetElement( iElement );

        if( pElement )
        {
            pElement->FontColor.States[DXUT_STATE_MOUSEOVER] = Color;
        }
    }
}
//--------------------------------------------------------------------------------------
void CDXUTStatic::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if( m_bVisible == false )
        return;

    DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

    if( m_bEnabled == false )
        iState = DXUT_STATE_DISABLED;
        
	CDXUTElement* pElement = GetElement( 0 ) ;

    pElement->FontColor.Blend( iState, fElapsedTime );

	DrawBackground( m_rcBoundingBox , pElement->TextureColor.Current);
    
    // Create a new TextObject.

	//assert(m_pTextObject);
//     if (NULL == m_pTextObject && m_width)
// 	{
// 		g_FDOXML.SetCustomElementCallback(,(int)this);
//         m_pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, pElement, m_strText, -1, m_width);
// 	}
	if(m_pTextObject)
	{
		m_pTextObject->ChangeDefaultColor(pElement->FontColor.Current);
		m_pDialog->DrawTextObject(m_pTextObject, pElement, &m_rcBoundingBox, m_bShadow);
	}
    
}

//--------------------------------------------------------------------------------------
void CDXUTStatic::RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    // 畫出提示(Hint)
    if( m_bMouseOver && (wcslen(_GetHint()) || wcslen( _GetHintImagePath()) ) && m_bVisible )
    {

        //int iTexture;
		CDXUTElement ele;
        CDXUTElement* pElement = &ele;//m_Elements.GetAt( 0 );
        pElement->FontColor.Current = D3DCOLOR_ARGB(255, 255, 255, 255);
		pElement->dwTextFormat = m_dwTextFormat;

        RECT rcText, rcTexture, rcBack;

        SetRectEmpty( &rcText );
        SetRectEmpty( &rcTexture );
        SetRectEmpty( &rcBack );

        if( wcslen( _GetHintImagePath() ) )
        {
            
			m_pDialog->RenderHintAll( m_cxWordMaxExtent, m_HintTextColor, m_FrameColor, m_BackgroundColor, pElement, m_MousePoint, _GetHint(), _GetHintImagePath(), m_bHintTurnRight );
			/*RECT *rcCatch = 0;
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
            if( rcBack.bottom > m_pDialog->GetHeight() )//rcBack.top + Pt.y < 0
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
            delete pTextObject;*/
        }
        else
        {

            if(IsS_RenderHint)
            {
                // 產生 wszText 的 TextObject，並指定給該 Item。
                LONG cxMaxExtent = 25 * 6; // 12 chars * cxChar
                CTextObject* pTextObject = g_FDOXML.MakeTextObject(
                    g_pD3DDevice, NULL, D3DCOLOR_ARGB(255, 255, 255, 255), _GetHint(), -1, cxMaxExtent);
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

                RECT rcText_r=rcText;

                rcText_r.left=m_x+80;
                rcText_r.right=rcText_r.left+(rcText.right-rcText.left);
                rcText_r.top=m_y;
                rcText_r.bottom=rcText_r.top+(rcText.bottom-rcText.top);
                m_pDialog->DrawRect( &rcText_r, D3DCOLOR_ARGB(150, 0, 0, 0) );

                pElement->FontColor.Current.r=255;
                pElement->FontColor.Current.g=255;
                pElement->FontColor.Current.b=255;

                m_pDialog->DrawTextObject( pTextObject, pElement, &rcText_r, false );
				delete pTextObject;

            }
            else
            {
                m_pDialog->RenderHintAll( m_cxWordMaxExtent, m_HintTextColor, m_FrameColor, m_BackgroundColor, pElement, m_MousePoint,_GetHint(), _GetHintImagePath(), m_bHintTurnRight );
				/*// 產生 wszText 的 TextObject，並指定給該 Item。
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
				delete pTextObject;*/
            }
        }
    }
}

//--------------------------------------------------------------------------------------
bool CDXUTStatic::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    m_MousePoint = pt;
	if(uMsg == WM_RBUTTONDOWN)
	{
		if(m_pDialog)
			m_pDialog->SendEvent(EVENT_CONTROL_RMOUSEBUTTONDOWN , true , this , 0);
		return true;
	}
    return false;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTStatic::GetTextCopy( LPWSTR strDest, UINT bufferCount )
{
    // Validate incoming parameters
    if( strDest == NULL || bufferCount == 0 )
    {
        return E_INVALIDARG;
    }

    // Copy the window text
    StringCchCopy( strDest, bufferCount, _GetText() );

    return S_OK;
}
/*
//--------------------------------------------------------------------------------------
HRESULT CDXUTStatic::SetHint( LPCWSTR strHint )
{
    if( strHint == NULL )
    {
        m_strHint[0] = 0;
        return S_OK;
    }
    
    StringCchCopy( m_strHint, MAX_PATH, strHint );
    return S_OK;
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTStatic::SetHint( const string &strHint )
{
    return SetHint( ANSItoUNICODE(strHint) );
}
*/
//--------------------------------------------------------------------------------------
void CDXUTStatic::SetTextStretchHeight( LPCWSTR strText )
{
	SIZE szText;
	szText.cx = 0;
	szText.cy = 0;
	if ( strText == NULL ) 
	{

		_SetText(L"");		
	}      
	_SetText(strText);

	// If setting new text, delete old TextObject.
	if (m_pTextObject) {
		delete m_pTextObject;
		m_pTextObject = NULL;
	}
	// Create new TextObject.
	CDXUTElement* pElement = GetElement( 0 ) ;
	if (g_pD3DDevice && pElement && m_width && wcslen(_GetText()))
	{		
		//m_pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, pElement, strText, -1, m_width);				
		if(m_bOneLine == false)
		{
			m_pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, pElement, strText, -1, 1920);
			SetHint(L"");
		}
		else
		{			
			m_pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, pElement, strText, -1, 1920);			

			SIZE szTO;
			m_pTextObject->GetExtent(szTO);

			int nlimitWidth;
			int nlimitHeight;
			CDXUTControl::GetSize(nlimitWidth,nlimitHeight);
			
			szText.cx = nlimitWidth;
			szText.cy = szTO.cy;

			if(CensoringTextObject(m_pTextObject , pElement->iFont ,szText ))
			{
				SetHint(strText);
			}
			else
			{
				SetHint(L"");
			}
		}        
		//m_pTextObject->GetExtent(szText);
		//szText.cy *= m_pTextObject->TotalLines();
		SetSize(szText.cx , szText.cy);
	}
	
}
//--------------------------------------------------------------------------------------
SIZE CDXUTStatic::SetText( LPCWSTR strText ,void (*pfn)(CustomElement& ce,int nData)/*= 0*/,int nData /*= 0*/)
{
	SIZE szText;
	szText.cx = 0;
	szText.cy = 0;
    if ( strText == NULL ) 
	{
		        
		_SetText(L"");
        return szText;
    }      
	_SetText(strText);

    // If setting new text, delete old TextObject.
    if (m_pTextObject) {
        delete m_pTextObject;
        m_pTextObject = NULL;
    }
    // Create new TextObject.
	CDXUTElement* pElement = GetElement( 0 ) ;
    if (g_pD3DDevice && pElement && m_width && wcslen(_GetText()))
	{
		g_FDOXML.SetCustomElementCallback(pfn,nData);
		//m_pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, pElement, strText, -1, m_width);				
		if(m_bOneLine == false)
		{
			m_pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, pElement, strText, -1, m_width);
			SetHint(L"");
		}
		else
		{			
			m_pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, pElement, strText, -1, m_width);
			int nlimitWidth;
			int nlimitHeight;
			CDXUTControl::GetSize(nlimitWidth,nlimitHeight);
			SIZE szLimit = {nlimitWidth,nlimitHeight};
			
			if(CensoringTextObject(m_pTextObject , pElement->iFont ,szLimit ))
			{
				SetHint(strText);
			}
			else
			{
				SetHint(L"");
			}
		}        
		m_pTextObject->GetExtent(szText);
		//szText.cy *= m_pTextObject->TotalLines();
	}
    
    return szText;
}
//--------------------------------------------------------------------------------------
SIZE CDXUTStatic::SetTextResize(LPCWSTR strText)
{
	m_width = 512 ;
	SIZE sz = SetText(strText);
	SetSize(sz.cx,sz.cy);
	return sz;
}
//--------------------------------------------------------------------------------------
// CDXUTStatic::TextInfo CDXUTStatic::SetText( LPCWSTR strText ,void (*pfn)(CustomElement& ce,int nData) /*= 0*/,int nData/* = 0*/)
// {
// 	TextInfo retInfo;
// 	memset(&retInfo,0,sizeof(retInfo));
// 	
// 	if ( strText == NULL ) {
// 		m_strText[0] = 0;
// 		return retInfo;
// 	}  
// 	StringCchCopy( m_strText, MAX_PATH, strText );
// 
// 	// If setting new text, delete old TextObject.
// 	if (m_pTextObject) {
// 		delete m_pTextObject;
// 		m_pTextObject = NULL;
// 	}
// 	// Create new TextObject.
// 	CDXUTElement* pElement = m_Elements.GetAt( 0 );
// 	if (g_pD3DDevice && pElement && m_width)
// 	{
// 		g_FDOXML.SetCustomElementCallback(pfn,nData);
// 		m_pTextObject = g_FDOXML.MakeTextObject(g_pD3DDevice, pElement, strText, -1, m_width);
// 		m_pTextObject->GetExtent(retInfo.szText);
// 		retInfo.nLine = m_pTextObject->TotalLines();
// 		retInfo.nFontHeight = retInfo.szText.cy / retInfo.nLine;
// 		//szText.cy *= m_pTextObject->TotalLines();
// 	}
// 
// 	return retInfo;
// }

//--------------------------------------------------------------------------------------
HRESULT CDXUTStatic::SetText( const string &strText )
{
	SetText( ANSItoUNICODE(strText) );
    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTStatic::SetText( int iText )
{
	std::wstring num;
	StringFormat16(num , L"%d" , iText);
	SetText( num.c_str() );
    return S_OK;
}

//--------------------------------------------------------------------------------------
string CDXUTStatic::GetTextA()
{
    return UNICODEtoANSI( _GetText() );
}

//--------------------------------------------------------------------------------------
wstring CDXUTStatic::GetTextAW()
{
    return _GetText();
}

//--------------------------------------------------------------------------------------
int CDXUTStatic::GetTextI()
{
    return atoi( GetTextA().c_str() );
}
//--------------------------------------------------------------------------------------
void CDXUTStatic::fnSetText(const wchar_t* str ,CDXUTControl* pCtrl)
{
	assert(pCtrl->GetType() == DXUT_CONTROL_STATIC);
	if(pCtrl->GetType() == DXUT_CONTROL_STATIC)
	{
		CDXUTStatic*pc = (CDXUTStatic*)pCtrl;				
		pc->SetText(str);
	}
}
//---------------------------------------------------------------------------

