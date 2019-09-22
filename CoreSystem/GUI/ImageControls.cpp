//
// ImageControls.cpp
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
#include <assert.h>
#include "Mousetrap.h"
#include "CDXUTDialog.h"
#include "CDXUTEditBox.h"
#include "CDXUTDialogResourceManager.h"
#pragma hdrstop
#include "ImageControls.h"

using namespace FDO;
//--------------------------------------------------------------------------------------
// CDXUTImageListBox class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CDXUTImageListBox::CDXUTImageListBox( CDXUTDialog *pDialog ) :
      CDXUTListBox( pDialog )
{
    m_bEnableEdit = true;
    m_bDrawSelect = true;
    m_bDragIcon = true;
    m_nBorder = 1;
    m_nMargin = 0;
    m_nSpace  = 1;
    m_iItemHeight  = 32;
    m_iStaticWidth = 30;
	m_iMouseOverIndex = -1 ;

    // per-item EditBox
    m_bEditEnabled = false;
    m_bEditVisible = false;
    m_iEditOffsetX = 86;
    m_iEditOffsetY = 21;
    m_iEditWidth   = 86;
    m_iEditHeight  = 26;
	m_iEditWords   = 16;
	m_iEditWordWidth = 12;
    m_bEditDrawFrame = false;
    m_EditFrameColor = 0xFFFFFFFF;

    m_EditBoxIconElement.iTexture = -1;
    SetRectEmpty(&m_EditBoxIconElement.rcTexture);

    m_bDrawText = true;
    m_pAddItemCallback = NULL;
    InitTextFormats();
}

//--------------------------------------------------------------------------------------
CDXUTImageListBox::~CDXUTImageListBox()
{
    RemoveAllItems();
}
//======================================================================================
// per-item EditBox
//======================================================================================
void CDXUTImageListBox::SetEditSize(int cxEdit, int cyEdit)
// Size of per-item EditBox.
{
    m_iEditWidth = cxEdit;
    m_iEditHeight = cyEdit;

	m_iEditWords = ( m_iEditWidth / m_iEditWordWidth );
}
//--------------------------------------------------------------------------------------
void CDXUTImageListBox::GetEditSize(int& cxEdit, int& cyEdit)
// Size of per-item EditBox.
{
    cxEdit = m_iEditWidth;
    cyEdit = m_iEditHeight;
}
//--------------------------------------------------------------------------------------
bool CDXUTImageListBox::GetEditDrawFrame()
{
    return m_bEditDrawFrame;
}
//--------------------------------------------------------------------------------------
void CDXUTImageListBox::SetEditDrawFrame(bool bDraw)
{
    m_bEditDrawFrame = bDraw;

    for( int i = 0; i < m_Items.GetSize(); ++i )
    {
        DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
        pItem->EditBox->SetDrawFrame(m_bEditDrawFrame);
    }
}
//--------------------------------------------------------------------------------------
void CDXUTImageListBox::SetEditFrameColor(D3DCOLOR Color)
{
    m_EditFrameColor = Color;

    for( int i = 0; i < m_Items.GetSize(); ++i )
    {
        DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
        pItem->EditBox->SetFrameColor(m_EditFrameColor);
    }
}
//--------------------------------------------------------------------------------------
// Insert Item   water add 2007/07/16
//--------------------------------------------------------------------------------------
HRESULT CDXUTImageListBox::InsertItem( int nIndex, const WCHAR *ImageFile,
      const WCHAR *wszText, D3DXCOLOR TextColor, void *pData, int iAmount, int iPrice )
{
    DXUTImageListBoxItem *pNewItem = new DXUTImageListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    StringCchCopy( pNewItem->strText, 256, wszText );
    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;

    // 產生 wszText 的 TextObject，並指定給該 Item。
    //LONG cxMaxExtent = m_iEditWords * m_iEditWordWidth; // chars * cxChar
	LONG cxMaxExtent = RectWidth(m_rcSelection) - 32 - 5 ;
    CTextObject* pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, NULL, TextColor, wszText, -1, cxMaxExtent);
    assert(pTextObject);
    pNewItem->TextObject = *pTextObject;
    delete pTextObject;

    //---------------------------------------------------------------------------------
    // 初始與設定 Image
    //---------------------------------------------------------------------------------
    pNewItem->Image.reset( new CDXUTImage(m_pDialog) );
    pNewItem->Image->SetSize( 32, 32 ); // 設定大小(固定)
    pNewItem->Image->SetImage( ImageFile );
    //---------------------------------------------------------------------------------
    // 初始 Static
    //---------------------------------------------------------------------------------
    pNewItem->Static.reset( new CDXUTStatic(m_pDialog) );
    m_pDialog->InitControl( pNewItem->Static.get() );
    pNewItem->Static->SetTextColor( TextColor ); // D3DCOLOR_ARGB( 255, 0, 40, 222 )
    pNewItem->Static->GetElement( 0 )->dwTextFormat = DT_RIGHT | DT_BOTTOM;
    pNewItem->Static->SetSize( 30, 32 ); // 設定大小(固定)
    //---------------------------------------------------------------------------------
    // 初始 EditBox
    //---------------------------------------------------------------------------------
    pNewItem->EditBox.reset( new CDXUTEditBox(m_pDialog) );
    m_pDialog->InitControl( pNewItem->EditBox.get() );
    pNewItem->EditBox->ClearTexture();
    pNewItem->EditBox->SetSize( m_iEditWidth, m_iEditHeight );
    pNewItem->EditBox->SetDrawFrame(m_bEditDrawFrame);
    pNewItem->EditBox->SetFrameColor(m_EditFrameColor);
    //---------------------------------------------------------------------------------

    HRESULT hr = m_Items.Insert( nIndex, pNewItem );
    if( SUCCEEDED( hr ) )
    {
        //---------------------------------------------------------------------------------
        // 設定 Static
        //---------------------------------------------------------------------------------
        pNewItem->Static->SetText( iAmount );
        if( iAmount > 0 )
            pNewItem->m_bStatic = true;
        else
            pNewItem->m_bStatic = false;
        //---------------------------------------------------------------------------------
        // 設定 EditBox
        //---------------------------------------------------------------------------------
        if( iPrice > 0 ) // 有設定價錢則填入價錢
            pNewItem->EditBox->SetText( iPrice );
        //---------------------------------------------------------------------------------
        m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
        if (-1 == iAmount)
            iAmount = pNewItem->Static->GetTextI();
        if (iAmount && m_pAddItemCallback)
            (*m_pAddItemCallback)(this, iAmount);

        UpdateRects();
    }
    else
        SAFE_DELETE( pNewItem );

    return hr;
}
//======================================================================================
// AddItem
//======================================================================================
HRESULT CDXUTImageListBox::AddItem( DXUTImageListBoxItem *pNewItem, int iAmount, int iPrice )
{
    HRESULT hr = m_Items.Add( pNewItem );
    if( FAILED(hr) )
    {
        SAFE_DELETE( pNewItem );
    }
    else
    {
        //---------------------------------------------------------------------------------
        // 設定 Static
        //---------------------------------------------------------------------------------
        pNewItem->Static->SetText( iAmount );
        if( iAmount > 0 )
            pNewItem->m_bStatic = true;
        else
            pNewItem->m_bStatic = false;
        //---------------------------------------------------------------------------------
        // 設定 EditBox
        //---------------------------------------------------------------------------------
        if( iPrice > 0 ) // 有設定價錢則填入價錢
            pNewItem->EditBox->SetText( iPrice );
        //---------------------------------------------------------------------------------
        m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
        if (-1 == iAmount)
            iAmount = pNewItem->Static->GetTextI();
        if (iAmount && m_pAddItemCallback)
            (*m_pAddItemCallback)(this, iAmount);
    }
    return hr;
}
//--------------------------------------------------------------------------------------
//	重新設定Item資料
//--------------------------------------------------------------------------------------
HRESULT CDXUTImageListBox::ResetItem( int pIndex , const wchar_t* pImageFile , const wchar_t* pWText , D3DXCOLOR pTextColor , void* pData , int pAmount , int pPrice )
{
	DXUTImageListBoxItem* Item = GetItem( pIndex ) ;
	if( !Item )
		return E_OUTOFMEMORY ;

	if( pWText )
	{
		StringCchCopy( Item->strText, 256, pWText ) ;
		//LONG cxMaxExtent = m_iEditWords * m_iEditWordWidth; // chars * cxChar
		LONG cxMaxExtent = RectWidth(m_rcSelection) - 32 - 5;
		CTextObject* pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, NULL, pTextColor, pWText, -1, cxMaxExtent);
		assert(pTextObject);
		Item->TextObject = *pTextObject;
		delete pTextObject;
	}

	if( pData )
	{
		Item->pData = pData ;
	}

	if( pImageFile )
	{
		Item->Image->SetImage( pImageFile );
	}
	
	if( pAmount )
	{
		Item->Static->SetTextColor( pTextColor ) ;
		Item->Static->SetText( pAmount );
        if(	pAmount > 0 )
            Item->m_bStatic = true;
        else
            Item->m_bStatic = false;
	}
    
	return S_OK ;
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTImageListBox::AddItem( const WCHAR *ImageFile,
      const WCHAR *wszText, D3DXCOLOR TextColor, void *pData, int iAmount, int iPrice )
{
    DXUTImageListBoxItem *pNewItem = new DXUTImageListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    StringCchCopy( pNewItem->strText, 256, wszText );
    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;

    // 產生 wszText 的 TextObject，並指定給該 Item。
	LONG cxMaxExtent = RectWidth(m_rcSelection) - 32 - 5;
    //LONG cxMaxExtent = m_iEditWords * m_iEditWordWidth; // chars * cxChar
    CTextObject* pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, NULL, TextColor, wszText, -1, cxMaxExtent);
    assert(pTextObject);
    pNewItem->TextObject = *pTextObject;
    delete pTextObject;

    //---------------------------------------------------------------------------------
    // 初始與設定 Image
    //---------------------------------------------------------------------------------
    pNewItem->Image.reset( new CDXUTImage(m_pDialog) );
    pNewItem->Image->SetSize( 32, 32 ); // 設定大小(固定)
    pNewItem->Image->SetImage( ImageFile );
    //---------------------------------------------------------------------------------
    // 初始 Static
    //---------------------------------------------------------------------------------
    pNewItem->Static.reset( new CDXUTStatic(m_pDialog) );
    m_pDialog->InitControl( pNewItem->Static.get() );
    pNewItem->Static->SetTextColor( TextColor ); // D3DCOLOR_ARGB( 255, 0, 40, 222 )
    pNewItem->Static->GetElement( 0 )->dwTextFormat = DT_RIGHT | DT_BOTTOM;
    pNewItem->Static->SetSize( 30, 32 ); // 設定大小(固定)
    //---------------------------------------------------------------------------------
    // 初始 EditBox
    //---------------------------------------------------------------------------------
    pNewItem->EditBox.reset( new CDXUTEditBox(m_pDialog) );
    m_pDialog->InitControl( pNewItem->EditBox.get() );
    pNewItem->EditBox->ClearTexture();
    pNewItem->EditBox->SetSize( m_iEditWidth, m_iEditHeight );
    pNewItem->EditBox->SetDrawFrame(m_bEditDrawFrame);
    pNewItem->EditBox->SetFrameColor(m_EditFrameColor);
	pNewItem->EditBox->SetTextColor( 0xFFFFFFFF );
    //---------------------------------------------------------------------------------
    return AddItem( pNewItem, iAmount, iPrice );
}
/*
HRESULT CDXUTImageListBox::AddItem( const WCHAR *ImageFile,
      const WCHAR *wszText, void *pData, int Amount, int Price )
{
    DXUTImageListBoxItem *pNewItem = new DXUTImageListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    StringCchCopy( pNewItem->strText, 256, wszText );
    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;

    // ming add >>
    // 設定Image
    pNewItem->Image.reset( new CDXUTImage(m_pDialog) );
    pNewItem->Image->SetSize( 32, 32 ); // 設定大小(固定)
    pNewItem->Image->SetImage( ImageFile );

    // 設定Static
    pNewItem->Static.reset( new CDXUTStatic(m_pDialog) );
    m_pDialog->InitControl( pNewItem->Static.get() );
    pNewItem->Static->SetTextColor( 0xFF000000 ); // D3DCOLOR_ARGB( 255, 0, 40, 222 )
    pNewItem->Static->GetElement( 0 )->dwTextFormat = DT_RIGHT | DT_BOTTOM;
    pNewItem->Static->SetSize( 30, 32 ); // 設定大小(固定)
    pNewItem->Static->SetText( Amount );
    if( Amount == 0 )
        pNewItem->m_bStatic = false;
    else
        pNewItem->m_bStatic = true;

    // 設定EditBox
    pNewItem->EditBox.reset( new CDXUTEditBox(m_pDialog) );
    m_pDialog->InitControl( pNewItem->EditBox.get() );
    pNewItem->EditBox->SetSize( 86, 26 ); // 設定大小(固定)
    pNewItem->EditBox->ClearTexture();
    if( Price ) // 有設定價錢則填入價錢
        pNewItem->EditBox->SetText( Price );
    // ming add <<

    return AddItem( pNewItem );
}
*/
//--------------------------------------------------------------------------------------
HRESULT CDXUTImageListBox::AddItem( const string &ImageFile,
      const string &szText, D3DXCOLOR TextColor, void *pData, int Amount, int Price )
{
    WCHAR wImageFile[256];
    ANSItoUNICODE( wImageFile, ImageFile );

    WCHAR wszText[256];
    ANSItoUNICODE( wszText, szText );

    return AddItem( wImageFile, wszText, TextColor, pData, Amount, Price );
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTImageListBox::AddItem( const string &ImageFile,
      const string &szText1st, const string &szText2nd, D3DXCOLOR TextColor,
      void *pData, int Amount, int Price )
{
    WCHAR wImageFile[256];
    ANSItoUNICODE( wImageFile, ImageFile );

    string szText = szText1st + "\n" + szText2nd;
    WCHAR wszText[256];
    ANSItoUNICODE( wszText, szText );
    
    return AddItem( wImageFile, wszText, TextColor, pData, Amount, Price );
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTImageListBox::AddItem( const string &ImageFile,
      const string &szText1st, const int &iText2nd, D3DXCOLOR TextColor,
      void *pData, int Amount, int Price )
{
    char temp[20];
    _itoa( iText2nd, temp, 10 );
    return AddItem( ImageFile, szText1st, temp, TextColor, pData, Amount, Price );
}

//--------------------------------------------------------------------------------------
DXUTImageListBoxItem *CDXUTImageListBox::GetItem( int nIndex )
{
    if( nIndex < 0 || nIndex >= (int)m_Items.GetSize() )
        return NULL;

    return m_Items[nIndex];
}
//--------------------------------------------------------------------------------------
int CDXUTImageListBox::GetItemIndexByData( void *pData )
{
    for( int i = 0; i < m_Items.GetSize(); ++i )
    {
        DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
        if( pItem->pData == pData )
            return i;
    }

    return -1;
}
//--------------------------------------------------------------------------------------
void CDXUTImageListBox::UpdateRects()
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
	if(GetElement( 0 )) // 
	{
		DXUTFontNode* pFontNode = m_pDialog->GetManager()->GetFontNode( GetElement( 0 )->iFont );
		if( pFontNode && pFontNode->nHeight )
		{
			m_ScrollBar.SetPageSize( (RectHeight(m_rcText)+m_nSpace) / (32+m_nSpace) ); // 32為圖示的高度(固定)

			// The selected item may have been scrolled off the page.
			// Ensure that it is in page again.
			m_ScrollBar.ShowItem( m_nSelected );
		}
	}
}
//--------------------------------------------------------------------------------------
// 文字列的 text-format
//--------------------------------------------------------------------------------------
DWORD CDXUTImageListBox::GetTextFormatLine0()
{
    return m_dwTextFormatLine0;
}

DWORD CDXUTImageListBox::GetTextFormatLine1()
{
    return m_dwTextFormatLine1;
}

void CDXUTImageListBox::SetTextFormatLine0(DWORD dwTextFormat)
{
    m_dwTextFormatLine0 = dwTextFormat;
}

void CDXUTImageListBox::SetTextFormatLine1(DWORD dwTextFormat)
{
    m_dwTextFormatLine1 = dwTextFormat;
}

void CDXUTImageListBox::InitTextFormats()
{
    // Default text formats.
    m_dwTextFormatLine0 = DT_LEFT|DT_TOP;
    m_dwTextFormatLine1 = DT_RIGHT|DT_BOTTOM;
}

//--------------------------------------------------------------------------------------
void CDXUTImageListBox::DrawItemTextBkgnd(DXUTImageListBoxItem* pItem, CDXUTElement* pElement, RECT* pDestRect)
{
}
//--------------------------------------------------------------------------------------
void CDXUTImageListBox::DrawItemText(DXUTImageListBoxItem* pItem, CDXUTElement* pElement, RECT* pDestRect)
{
    assert(pItem && pElement && pDestRect);
    
    DrawItemTextBkgnd(pItem, pElement, pDestRect);

    std::list<CLineObject>::iterator it = pItem->TextObject.m_Lines.begin();
    RECT rcDest = *pDestRect;

    DWORD dwOrgTextFormat = pElement->dwTextFormat;
    
    for (int i = 0; it != pItem->TextObject.m_Lines.end(); ++it, ++i) 
	{
        rcDest.top += 1; //間格
		if (0 == i) 
		{
			rcDest.top += 1 ; //上邊框
			rcDest.bottom = rcDest.top + 15 ;
			if( m_dwTextFormatLine0 )
				pElement->dwTextFormat = m_dwTextFormatLine0;
			else pElement->dwTextFormat = DT_VCENTER ;
        } 
		else if (1 == i) 
		{
			rcDest.top += 16 ; 
			rcDest.bottom -= 1 ; //下邊框
			if( m_dwTextFormatLine1 )
				pElement->dwTextFormat = m_dwTextFormatLine1;
			else pElement->dwTextFormat = DT_VCENTER ;
        }
		else break;
        m_pDialog->DrawLineObject(&(*it), pElement, &rcDest);
    } // end for
    
    pElement->dwTextFormat = dwOrgTextFormat;
}
//-----------------------------------------------------------------------------
void CDXUTImageListBox::RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    int nClicked;//, iTexture;

    // 畫出提示(Hint)
    if( m_bMouseOver )
    {
        if( m_Items.GetSize() > 0 && PtInRect( &m_rcSelection, m_NowPt ) )
        {
            if( m_nTextHeight )
            {
                nClicked = m_ScrollBar.GetTrackPos() + ( m_NowPt.y - m_rcText.top ) / m_nTextHeight;
            }
            else
            {
                nClicked = -1;
                return;
            }

            if( MultipleHint.empty() == false && (wcslen( MultipleHint[ nClicked ].m_strHint ) || wcslen( MultipleHint[ nClicked ].m_strHintImgPath )) )
            {
                if( nClicked < (int)m_Items.GetSize() )
                {
                    DXUTImageListBoxItem *pItem = m_Items.GetAt( nClicked );
                    CDXUTElement* pElement = pItem->Image->GetElement( 0 );
                    pElement->FontColor.Current = D3DCOLOR_ARGB(255, 255, 255, 255);
					pElement->dwTextFormat = DT_LEFT|DT_VCENTER;

					m_pDialog->RenderHintAll( m_cxWordMaxExtent, m_HintTextColor, m_FrameColor, m_BackgroundColor, pElement, m_NowPt, MultipleHint[ nClicked ].m_strHint , MultipleHint[ nClicked ].m_strHintImgPath, m_bHintTurnRight );

                    /*RECT rcText, rcTexture, rcBack, rcLine;

                    SetRectEmpty( &rcText );
                    SetRectEmpty( &rcTexture );
                    SetRectEmpty( &rcBack );

                    if( wcslen( MultipleHint[ nClicked ].m_strHintImgPath ) )
                    {
                        RECT *rcCatch = 0;
                        iTexture = m_pDialog->GetManager()->AddTexture( MultipleHint[ nClicked ].m_strHintImgPath );
                        pTextureElement.SetTexture( iTexture, rcCatch );
                        pTextureElement.TextureColor.Current = D3DCOLOR_ARGB( 200, 255, 255, 255 );//150 255 255 255

                        SetRect( &rcTexture, 36, 36, 100, 100 );
                        OffsetRect( &rcTexture, m_NowPt.x + 5, m_NowPt.y + 5 );

                        m_pDialog->CalcTextRect( MultipleHint[ nClicked ].m_strHint, pElement, &rcText );
                        wstring str = MultipleHint[ nClicked ].m_strHint;
                        int StrLong = str.length();
                        int W = (StrLong/10)*12;
                        SetRect( &rcText, 5, 143, 133, 138+W );
                        //W += 50;
                        InflateRect( &rcText, 5, 5 );
                        OffsetRect( &rcText, m_NowPt.x + 5, m_NowPt.y + 5 );

                        SetRect( &rcBack, 0, 0, RectWidth( rcTexture )*2 + 10, ( RectHeight( rcTexture )*2 + RectHeight( rcText ) + 25 ) );
                        OffsetRect( &rcBack, m_NowPt.x + 5, m_NowPt.y + 5 );

                        // 產生 wszText 的 TextObject，並指定給該 Item。
                        LONG cxMaxExtent = 25 * 6; // 12 chars * cxChar
                        CTextObject* pTextObject = g_FDOXML.MakeTextObject(
                            g_pD3DDevice, NULL, D3DCOLOR_ARGB( 255, 255, 255, 255 ), MultipleHint[ nClicked ].m_strHint, -1, cxMaxExtent);
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
                        delete pTextObject;
                    }
                    else
                    {
						// 過濾每行第一個字元為/n
						WCHAR temp[MAX_PATH] ;
						ZeroMemory( &temp, sizeof( temp ) );
						StringCchCopy( temp, MAX_PATH, MultipleHint[ nClicked ].m_strHint );
						RemoveFirst_SlashN( temp );

						// 產生 wszText 的 TextObject，並指定給該 Item。
                        LONG cxMaxExtent = 25 * 6; // 12 chars * cxChar
                        DWORD m_dwDefaultColor = 0xFFFFFFFF;
                        g_FDOXML.SetDefaultColor(m_dwDefaultColor);
                        CTextObject* pTextObject = g_FDOXML.MakeTextObject(
                            g_pD3DDevice, NULL, D3DCOLOR_ARGB(255, 255, 255, 255), temp, -1, cxMaxExtent);
                        assert(pTextObject);

						SIZE Extent;
                        pTextObject->GetExtent( Extent );
						
						//m_pDialog->CalcTextRect( MultipleHint[ nClicked ].m_strHint, pElement, &rcText );
                        //wstring str = MultipleHint[ nClicked ].m_strHint;
                        //int StrLong = str.length();
                        //int W = ((RectWidth( rcText )/120)+1)*12;
                        SetRect( &rcText, 5, 5, Extent.cx+5, Extent.cy+5 );
                        //InflateRect( &rcText, 5, 5 );
                        OffsetRect( &rcText, m_NowPt.x + 5, m_NowPt.y + 5 );

                        SetRect( &rcBack, 0, 0, (RectWidth( rcText ) + 10 ), ( RectHeight( rcText ) + 10 ) );
                        OffsetRect( &rcBack, m_NowPt.x + 5, m_NowPt.y + 5 );

                        POINT Pt;
                        m_pDialog->GetLocation( Pt );
                        if( rcBack.bottom > m_pDialog->GetHeight() )
                        {
                            OffsetRect( &rcText, 0, -RectHeight(rcText)-20 );
                            OffsetRect( &rcBack, 0, -RectHeight(rcText)-20 );
                        }
                        if( rcBack.right > m_pDialog->GetWidth() )
                        {
                            OffsetRect( &rcText, -RectWidth(rcText)-20, 0 );
                            OffsetRect( &rcBack, -RectWidth(rcText)-20, 0 );
                        }
                        
                        SetRect( &rcLine, rcBack.left+1, rcBack.top+1, rcBack.right-2, rcBack.bottom-2 );

                        m_pDialog->DrawRect( &rcBack, D3DCOLOR_ARGB(180, 0, 0, 0) );
                        m_pDialog->DrawFrameRect( &rcLine, D3DCOLOR_ARGB(255, 220, 220, 43) );
                        m_pDialog->DrawTextObject( pTextObject, pElement, &rcText, false );
						delete pTextObject;
                    }*/
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------
void CDXUTImageListBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if( m_bVisible == false )
        return;

    // 取出Element
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

            // 這裡處理Text
            RECT rcText = rc;  // 畫字的範圍
            rcText.left += 40; // 往右偏移
            //rcText.top  += 5;
            rcText.right -= 6;

            if( bSelectedStyle && m_bDrawSelect )
            {
                rcSel.top = rc.top;
                rcSel.bottom = rc.bottom;
                InflateRect( &rcSel, 0, -1 ); // 上下範圍-1
                m_pDialog->DrawSprite( pSelElement, &rcSel );
                DrawItemText(pItem, pSelElement, &rcText);
            }
            else
            {
                DrawItemText(pItem, pElement, &rcText);
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
                rcImage.right = rcImage.left + m_iStaticWidth;

                D3DXCOLOR clrOriginal = pElement->FontColor.Current;
                pElement->FontColor.Current = 0xFF000000;

                RECT rcTemp = rcImage;
                rcTemp.top -= 1;
                rcTemp.bottom -= 1;
                m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcTemp );

                rcTemp = rcImage;
                rcTemp.top += 1;
                rcTemp.bottom += 1;
                m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcTemp );

                rcTemp = rcImage;
                rcTemp.left -= 1;
                rcTemp.right -= 1;
                m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcTemp );

                rcTemp = rcImage;
                rcTemp.left += 1;
                rcTemp.right += 1;
                m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcTemp );

                pElement->FontColor.Current = clrOriginal;
                pElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
                m_pDialog->DrawPlainText( pItem->Static->GetText(), pElement, &rcImage ); // 畫
            }

            // 這裡畫EditBox
            if( m_bEditVisible )
            {
                pItem->EditBox->SetLocation( rc.right-m_iEditOffsetX, rc.bottom-m_iEditOffsetY );
                pItem->EditBox->SetDialog( m_pDialog );
                if (-1 != m_EditBoxIconElement.iTexture && !IsRectEmpty(&m_EditBoxIconElement.rcTexture)) {
                    RECT rcIcon;
                    const int CX_ICON = 12, CY_ICON = 12;
                    rcIcon.right = rc.right- m_iEditOffsetX + 5;
                    rcIcon.left = rcIcon.right - CX_ICON;
                    rcIcon.bottom = rc.bottom - 1;
                    rcIcon.top = rcIcon.bottom - CY_ICON;
                    m_pDialog->DrawSprite(&m_EditBoxIconElement, &rcIcon);
                } // end if
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
}

//--------------------------------------------------------------------------------------
void CDXUTImageListBox::RemoveAllItems()
{
    m_pDialog->Refresh();

    for( int i = 0; i < m_Items.GetSize(); ++i )
    {
        delete m_Items.GetAt( i );
    }

    m_Items.RemoveAll();
    m_ScrollBar.SetTrackRange( 0, 1 );
}

//--------------------------------------------------------------------------------------
void CDXUTImageListBox::RemoveItem( const int &nIndex )
{
    if( nIndex < 0 || nIndex >= (int)m_Items.GetSize() )
        return;

    delete m_Items.GetAt( nIndex );

    RemoveItemPointer( nIndex );
}

//--------------------------------------------------------------------------------------
// 只移除指標而不刪除實體
//--------------------------------------------------------------------------------------
void CDXUTImageListBox::RemoveItemPointer( const int &nIndex )
{
    m_Items.Remove( nIndex );
    m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
    if( m_nSelected >= (int)m_Items.GetSize() )
        m_nSelected = m_Items.GetSize() - 1;

    m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
}

//--------------------------------------------------------------------------------------
void CDXUTImageListBox::OnMouseLeave() 
{ 
	m_bDrag = false ;
	CDXUTControl::OnMouseLeave() ;
}
bool CDXUTImageListBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false ;
	
	m_NowPt = pt;

    if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
        return true;

	m_iMouseOverIndex = -1 ;
    if( m_nTextHeight )
		m_iMouseOverIndex = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight ;

    //if( !m_bEnableEdit )
    //    return false;

    //----------------------------------------------------------
    // First acquire focus
    // => Modified: ImageListBox 不再無條件取得輸入焦點。
    // if (WM_LBUTTONDOWN == uMsg)
    // if (!m_bHasFocus)
    //     m_pDialog->RequestFocus( this );
    //----------------------------------------------------------

    // 處理Item上的EditBox
    if( m_bEditEnabled && m_nSelected != -1 )
    {
		DXUTImageListBoxItem *pItem = m_Items.GetAt( m_nSelected ) ;
		if( pItem && pItem->EditBox->GetVisible() && pItem->EditBox->ContainsPoint( pt ) )
		{
			if( pItem->EditBox->HandleMouse( uMsg, pt , wParam , lParam ) )
				return true ;
		}
	}

	if( WM_MOUSEWHEEL == uMsg )
    {
        UINT uLines ;
        SystemParametersInfo( SPI_GETWHEELSCROLLLINES , 0, &uLines , 0 ) ;
        int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines ;
        m_ScrollBar.Scroll( -nScrollAmount );
        return true;
    }
	else if( m_bDragIcon && m_bDrag && WM_MOUSEMOVE == uMsg && ( MK_LBUTTON & wParam ) )
	{
		if( m_nSelected != -1 && m_nSelected == m_iMouseOverIndex && m_nSelected < m_Items.GetSize() )
		{
			m_bDrag = false ;
			DXUTImageListBoxItem *pItem = m_Items.GetAt( m_nSelected ) ;
			if( pItem )
			{
				m_pDialog->SendEvent( EVENT_CONTROL_DRAG , true , this ) ;
				OutputDebugStringA("==ImageListBox EVENT_CONTROL_DRAG==\n");
				return true ;
			}
		}
	}
	else if( WM_LBUTTONDOWN == uMsg )
	{
		//SetCapture( DXUTGetHWND() ) ;
		m_bDrag = true ;

		if( m_iMouseOverIndex != -1 && m_nSelected != m_iMouseOverIndex && m_iMouseOverIndex < m_Items.GetSize() )
		{
			if( m_nSelected != -1 && m_nSelected < m_Items.GetSize() )
			{
				DXUTImageListBoxItem *pItem = m_Items.GetAt( m_nSelected ) ;
				if( pItem )
					pItem->bSelected = false ;
			}
			DXUTImageListBoxItem *pItem = m_Items.GetAt( m_iMouseOverIndex ) ;
			if( pItem )
			{
				pItem->bSelected = true ;
				m_nSelected = m_iMouseOverIndex ;
				m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION , true , this ) ;	
				return true ;
			}
		}
		else if( m_iMouseOverIndex != -1 )//&& m_nSelected == m_iMouseOverIndex )
		{
			m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION , true , this ) ;	
			return true ;
		}
	}
	else if( WM_RBUTTONDOWN == uMsg )
	{
		m_pDialog->SendEvent( EVENT_LISTBOX_RSELECTION , true , this ) ;
	}
	else if( WM_LBUTTONUP == uMsg )
	{
		m_bDrag = false ;
		//ReleaseCapture() ;
		return true ;
	}
	return false ;
	

  //  switch( uMsg )
  //  {
  //      case WM_RBUTTONDOWN: // 右鍵
  //      case WM_RBUTTONDBLCLK:
  //          // Check for clicks in the text area
  //          // 有Item並且確認點擊範圍
  //          if( m_Items.GetSize() > 0 && PtInRect( &m_rcSelection, pt ) )
  //          {
  //              // Compute the index of the clicked item
  //              // 計算點擊的 index
  //              int nClicked;
  //              if( m_nTextHeight )
  //                  nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
  //              else
  //                  nClicked = -1;

  //              // Only proceed if the click falls on top of an item.

  //              if( nClicked >= m_ScrollBar.GetTrackPos() &&
  //                  nClicked < (int)m_Items.GetSize() &&
  //                  nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
  //              {
  //                  SetCapture( DXUTGetHWND() );
  //                  //m_bDrag = true;

  //                  // If this is a double click, fire off an event and exit
  //                  // since the first click would have taken care of the selection
  //                  // updating.
  //                  if( uMsg == WM_RBUTTONDBLCLK )
  //                  {
  //                      m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_RDBLCLK, true, this );
  //                      return true;
  //                  }

  //                  // 取消前次選取
  //                  if( m_nSelected != -1 )
  //                  {
  //                      if( m_nSelected < m_Items.GetSize() )
  //                      {
  //                          DXUTImageListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
  //                          pSelItem->bSelected = false;
  //                      }
  //                      else
  //                      {
  //                          m_nSelected = -1;
  //                      }
  //                  }
  //                  
  //                  m_nSelected = nClicked;
  //                  if( !( wParam & MK_SHIFT ) )
  //                      m_nSelStart = m_nSelected;

  //                  // If this is a multi-selection listbox, update per-item
  //                  // selection data.

  //                  // 複選
  //                  if( m_dwStyle & MULTISELECTION )
  //                  {
  //                      // Determine behavior based on the state of Shift and Ctrl

  //                      DXUTImageListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );

  //                      if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_CONTROL )
  //                      {
  //                          // Control click. Reverse the selection of this item.

  //                          pSelItem->bSelected = !pSelItem->bSelected;
  //                      } else
  //                      if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_SHIFT )
  //                      {
  //                          // Shift click. Set the selection for all items
  //                          // from last selected item to the current item.
  //                          // Clear everything else.

  //                          int nBegin = __min( m_nSelStart, m_nSelected );
  //                          int nEnd = __max( m_nSelStart, m_nSelected );

  //                          for( int i = 0; i < nBegin; ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = false;
  //                          }

  //                          for( int i = nEnd + 1; i < (int)m_Items.GetSize(); ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = false;
  //                          }

  //                          for( int i = nBegin; i <= nEnd; ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = true;
  //                          }
  //                      } else
  //                      if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == ( MK_SHIFT|MK_CONTROL ) )
  //                      {
  //                          // Control-Shift-click.

  //                          // The behavior is:
  //                          //   Set all items from m_nSelStart to m_nSelected to
  //                          //     the same state as m_nSelStart, not including m_nSelected.
  //                          //   Set m_nSelected to selected.

  //                          int nBegin = __min( m_nSelStart, m_nSelected );
  //                          int nEnd = __max( m_nSelStart, m_nSelected );

  //                          // The two ends do not need to be set here.

  //                          bool bLastSelected = m_Items.GetAt( m_nSelStart )->bSelected;
  //                          for( int i = nBegin + 1; i < nEnd; ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = bLastSelected;
  //                          }

  //                          pSelItem->bSelected = true;

  //                          // Restore m_nSelected to the previous value
  //                          // This matches the Windows behavior

  //                          m_nSelected = m_nSelStart;
  //                      } else
  //                      {
  //                          // Simple click.  Clear all items and select the clicked
  //                          // item.


  //                          for( int i = 0; i < (int)m_Items.GetSize(); ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = false;
  //                          }

  //                          pSelItem->bSelected = true;
  //                      }
  //                  }  // End of multi-selection case
  //                  else 
  //                  { // 單選
  //                      DXUTImageListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
  //                      pSelItem->bSelected = true;
		//				if ( m_bEditEnabled && m_bEditVisible )
		//					m_pDialog->RequestFocus( pSelItem->EditBox.get() ) ;
  //                  }

  //                  m_pDialog->SendEvent( EVENT_LISTBOX_RSELECTION, true, this );
  //              }
  //              
  //              return true;
  //          }
  //          break;
  //      
  //      case WM_LBUTTONDOWN:
  //      case WM_LBUTTONDBLCLK:
  //          // Check for clicks in the text area
  //          // 有Item並且確認點擊範圍
  //          if( m_Items.GetSize() > 0 && PtInRect( &m_rcSelection, pt ) )
  //          {
  //              //音效 *** modify
  //              PlaySoundWAV();  //under control class

  //              // Compute the index of the clicked item
  //              // 計算點擊的 index
  //              int nClicked;
  //              if( m_nTextHeight )
  //                  nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
  //              else
  //                  nClicked = -1;

  //              // Only proceed if the click falls on top of an item.

  //              if( nClicked >= m_ScrollBar.GetTrackPos() &&
  //                  nClicked < (int)m_Items.GetSize() &&
  //                  nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
  //              {
  //                  SetCapture( DXUTGetHWND() );
  //                  //m_bDrag = true;

  //                  // If this is a double click, fire off an event and exit
  //                  // since the first click would have taken care of the selection
  //                  // updating.
  //                  if( uMsg == WM_LBUTTONDBLCLK )
  //                  {
  //                      m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
  //                      return true;
  //                  }

  //                  // 取消前次選取
  //                  if( m_nSelected != -1 )
  //                  {
  //                      if( m_nSelected < m_Items.GetSize() )
  //                      {
  //                          DXUTImageListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
  //                          pSelItem->bSelected = false;
  //                      }
  //                      else
  //                      {
  //                          m_nSelected = -1;
  //                      }
  //                  }

  //                  m_nSelected = nClicked;
  //                  if( !( wParam & MK_SHIFT ) )
  //                      m_nSelStart = m_nSelected;

  //                  // If this is a multi-selection listbox, update per-item
  //                  // selection data.

  //                  // 複選
  //                  if( m_dwStyle & MULTISELECTION )
  //                  {
  //                      // Determine behavior based on the state of Shift and Ctrl

  //                      DXUTImageListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );

  //                      if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_CONTROL )
  //                      {
  //                          // Control click. Reverse the selection of this item.

  //                          pSelItem->bSelected = !pSelItem->bSelected;
  //                      } else
  //                      if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_SHIFT )
  //                      {
  //                          // Shift click. Set the selection for all items
  //                          // from last selected item to the current item.
  //                          // Clear everything else.

  //                          int nBegin = __min( m_nSelStart, m_nSelected );
  //                          int nEnd = __max( m_nSelStart, m_nSelected );

  //                          for( int i = 0; i < nBegin; ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = false;
  //                          }

  //                          for( int i = nEnd + 1; i < (int)m_Items.GetSize(); ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = false;
  //                          }

  //                          for( int i = nBegin; i <= nEnd; ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = true;
  //                          }
  //                      } else
  //                      if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == ( MK_SHIFT|MK_CONTROL ) )
  //                      {
  //                          // Control-Shift-click.

  //                          // The behavior is:
  //                          //   Set all items from m_nSelStart to m_nSelected to
  //                          //     the same state as m_nSelStart, not including m_nSelected.
  //                          //   Set m_nSelected to selected.

  //                          int nBegin = __min( m_nSelStart, m_nSelected );
  //                          int nEnd = __max( m_nSelStart, m_nSelected );

  //                          // The two ends do not need to be set here.

  //                          bool bLastSelected = m_Items.GetAt( m_nSelStart )->bSelected;
  //                          for( int i = nBegin + 1; i < nEnd; ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = bLastSelected;
  //                          }

  //                          pSelItem->bSelected = true;

  //                          // Restore m_nSelected to the previous value
  //                          // This matches the Windows behavior

  //                          m_nSelected = m_nSelStart;
  //                      } else
  //                      {
  //                          // Simple click.  Clear all items and select the clicked
  //                          // item.


  //                          for( int i = 0; i < (int)m_Items.GetSize(); ++i )
  //                          {
  //                              DXUTImageListBoxItem *pItem = m_Items.GetAt( i );
  //                              pItem->bSelected = false;
  //                          }

  //                          pSelItem->bSelected = true;
  //                      }
  //                  }  // End of multi-selection case
  //                  else
  //                  { // 單選
  //                      DXUTImageListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
  //                      
		//				
		//				pSelItem->bSelected = true;

		//				if ( m_bEditEnabled && m_bEditVisible )
		//				{
		//					m_pDialog->RequestFocus( pSelItem->EditBox.get() ) ;
		//				}
		//				

  //     //                 // ming add >> 設定拖曳!!
  //     //                 if( m_bDragIcon )
  //     //                 {
  //     //                     //CDXUTControl::s_pDragSource = this; // 設定拖曳來源
  //     //                     //CDXUTControl::s_SrcIndex = m_nSelected;
  //     //                     //CDXUTControl::s_pDragTexture = pSelItem->Image->GetTexture(0); // 設定拖曳材質
		//					//m_pDialog->SendEvent( EVENT_CONTROL_DRAG, true, this );
  //     //                 }
  //     //                 // ming add <<
  //                  }

  //                  m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
  //              }

  //              return true;
  //          }
  //          break;

  //      case WM_RBUTTONUP:
  //      {
  //          ReleaseCapture();
  //          m_bDrag = false;
  //          
  //          return false;
  //      }
  //      
  //      case WM_LBUTTONUP:
  //      {
  //          ReleaseCapture();
  //          m_bDrag = false;

  //          // Check for clicks in the text area
  //          // 有Item並且確認點擊範圍
  //          if( m_Items.GetSize() > 0 && PtInRect( &m_rcSelection, pt ) )
  //          {
  //              // Compute the index of the clicked item
  //              // 計算點擊的 index
  //              int nClicked;
  //              if( m_nTextHeight )
  //                  nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
  //              else
  //                  nClicked = -1;

  //              // Only proceed if the click falls on top of an item.

  //              if( nClicked >= m_ScrollBar.GetTrackPos() &&
  //                  nClicked < (int)m_Items.GetSize() &&
  //                  nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
  //              {
  //                  CDXUTControl::s_DestIndex = nClicked;
  //              }
  //          }

  //   //       if( CDXUTControl::s_pDragSource==this && CDXUTControl::s_SrcIndex==CDXUTControl::s_DestIndex );
  //   //       else
  //   //       {
  //   //           if( CDXUTControl::s_pDragSource != NULL &&
  //   //               CDXUTControl::s_SrcIndex >= 0 &&
  //   //               CDXUTControl::s_SrcIndex < ((CDXUTImageListBox*)CDXUTControl::s_pDragSource)->GetSize() )
  //   //           {
  //   //               m_pDialog->SendEvent( EVENT_ITEM_DROP, true, this );
		//			//m_pDialog->SendEvent( EVENT_CONTROL_DROP, true, this );
  //   //           }
  //   //       }    

  //          return false;
  //      }

  //      case WM_MOUSEMOVE:
		//{
		//	if( m_Items.GetSize() > 0 && m_nTextHeight )
		//		m_nMouseCell = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
  //          else
  //              m_nMouseCell = -1;

		//	if( m_LastMouseCell != m_nMouseCell )
		//	{
		//		m_LastMouseCell = m_nMouseCell ;
		//		m_pDialog->SendEvent( EVENT_ICONLISTBOX_CELLCROSS , true , this ) ;
		//		return true ;
		//	}
		//}
  //          if( m_bDrag )
  //          {
  //              /* ming mark...nothing to do
  //              // Compute the index of the item below cursor
  //              int nItem;
  //              if( m_nTextHeight )
  //                  nItem = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
  //              else
  //                  nItem = -1;

  //              // Only proceed if the cursor is on top of an item.

  //              if( nItem >= (int)m_ScrollBar.GetTrackPos() &&
  //                  nItem < (int)m_Items.GetSize() &&
  //                  nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
  //              {
  //                  m_nSelected = nItem;
  //                  m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
  //              } else
  //              if( nItem < (int)m_ScrollBar.GetTrackPos() )
  //              {
  //                  // User drags the mouse above window top
  //                  m_ScrollBar.Scroll( -1 );
  //                  m_nSelected = m_ScrollBar.GetTrackPos();
  //                  m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
  //              } else
  //              if( nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
  //              {
  //                  // User drags the mouse below window bottom
  //                  m_ScrollBar.Scroll( 1 );
  //                  m_nSelected = __min( (int)m_Items.GetSize(), m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() ) - 1;
  //                  m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
  //              }
  //              */
  //          }
  //          break;

  //      case WM_MOUSEWHEEL:
  //      {
  //          UINT uLines;
  //          SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
  //          int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
  //          m_ScrollBar.Scroll( -nScrollAmount );
  //          return true;
  //      }
  //  }

  //  return false;
}

//-----------------------------------------------------------------------------
// implementation of CDXUTImage
//-----------------------------------------------------------------------------
CDXUTImage::CDXUTImage( CDXUTDialog *pDialog ) : mbSetBlend(true) , mbStretch(false)
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
void CDXUTImage::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
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
		if(mbSetBlend)
		{
			pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
			pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );
		}
		else
		{
			pElement->TextureColor.Current	= pElement->TextureColor.States[DXUT_STATE_NORMAL];
			pElement->FontColor.Current		= pElement->FontColor.States[DXUT_STATE_NORMAL];
		}
        

        // here it is
		if(mbStretch == false)
			m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );
		else
			m_pDialog->DrawSpriteScale(pElement, m_rcBoundingBox);
        //m_pDialog->DrawText( m_strText, pElement, &m_rcBoundingBox );
    }
}

//-----------------------------------------------------------------------------
IDirect3DTexture9* CDXUTImage::GetTexture( int iTexture )
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
void CDXUTImage::SetColor( D3DCOLOR color )
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
bool CDXUTImage::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return false;
}
//-----------------------------------------------------------------------------
bool CDXUTImage::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    m_MousePoint = pt;
    return true;
}   

//-----------------------------------------------------------------------------
// implementation of CDXUTImageBox
//-----------------------------------------------------------------------------
CDXUTImageBox::CDXUTImageBox( CDXUTDialog *pDialog )
{
    m_Type = DXUT_CONTROL_IMAGEBOX;
    m_pDialog = pDialog;

    /*for( int i=0; i < m_Elements.GetSize(); i++ )
    {
        CDXUTElement* pElement = m_Elements.GetAt( i );
        SAFE_DELETE( pElement );
    }
    m_Elements.RemoveAll();*/

	m_bRenderBackground = false;
}

//-----------------------------------------------------------------------------
CDXUTImageBox::~CDXUTImageBox()
{
}

//--------------------------------------------------------------------------------------
void CDXUTImageBox::AddTextureItem( const string &ImageFile, RECT* prcTexture, void *pData, int iAmount, int iPrice )
{
    WCHAR wImageFile[256];
    ANSItoUNICODE( wImageFile, ImageFile );

   /* WCHAR wszText[256];
    ANSItoUNICODE( wszText, szText );*/

    return AddTextureItem( wImageFile, prcTexture, pData, iAmount, iPrice );
}
//-----------------------------------------------------------------------------
void CDXUTImageBox::AddTextureItem( const WCHAR *ImageFile, RECT* prcTexture, void *pData, int iAmount, int iPrice )
{
	RemoveAllItems();
    DXUTImageListBoxItem *pNewItem = new DXUTImageListBoxItem;
    if( !pNewItem )
        return ;

    StringCchCopy( pNewItem->strText, 256, L"" );
    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;

    // 產生 wszText 的 TextObject，並指定給該 Item。
	LONG cxMaxExtent = RectWidth(m_rcSelection) - 32 - 5;
    //LONG cxMaxExtent = m_iEditWords * m_iEditWordWidth; // chars * cxChar
    CTextObject* pTextObject = g_FDOXML.MakeTextObject(
        g_pD3DDevice, NULL, WHITE, L"", -1, cxMaxExtent);
    assert(pTextObject);
    pNewItem->TextObject = *pTextObject;
    delete pTextObject;

    //---------------------------------------------------------------------------------
    // 初始與設定 Image
    //---------------------------------------------------------------------------------
    pNewItem->Image.reset( new CDXUTImage(m_pDialog) );
    pNewItem->Image->SetSize( m_width, m_height ); // 設定大小
    pNewItem->Image->SetImage( ImageFile, prcTexture );
    //---------------------------------------------------------------------------------
    // 初始 Static
    //---------------------------------------------------------------------------------
    pNewItem->Static.reset( new CDXUTStatic(m_pDialog) );
    m_pDialog->InitControl( pNewItem->Static.get() );
    pNewItem->Static->SetTextColor( WHITE ); // D3DCOLOR_ARGB( 255, 0, 40, 222 )
    pNewItem->Static->GetElement( 0 )->dwTextFormat = DT_RIGHT | DT_BOTTOM;
    pNewItem->Static->SetSize( m_width-2, m_height ); // 設定大小
	//m_bDragIcon = false;	// 預設不畫
    //---------------------------------------------------------------------------------
    // 初始 EditBox
    //---------------------------------------------------------------------------------
    pNewItem->EditBox.reset( new CDXUTEditBox(m_pDialog) );
    m_pDialog->InitControl( pNewItem->EditBox.get() );
    pNewItem->EditBox->ClearTexture();
    pNewItem->EditBox->SetSize( m_iEditWidth, m_iEditHeight );
    pNewItem->EditBox->SetDrawFrame(m_bEditDrawFrame);
    pNewItem->EditBox->SetFrameColor(m_EditFrameColor);
	pNewItem->EditBox->SetTextColor( 0xFFFFFFFF );
	//m_bEnableEdit = false;	// 預設不畫
    //---------------------------------------------------------------------------------
    AddItem( pNewItem, iAmount, iPrice );
}
//------------------------------------------------------------------------------
void CDXUTImageBox::RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    // 畫出提示(Hint)
    if( m_bMouseOver && (wcslen( _GetHint() ) || wcslen( _GetHintImagePath() ) ) )
    {

        //int iTexture;
		CDXUTElement* pElement = GetElement( 0 ) ;
        pElement->FontColor.Current = D3DCOLOR_ARGB(255, 255, 255, 255);
		pElement->dwTextFormat = DT_LEFT | DT_VCENTER;

        //RECT rcText, rcTexture, rcBack, rcLine;

        //SetRectEmpty( &rcText );
        //SetRectEmpty( &rcTexture );
        //SetRectEmpty( &rcBack );

        m_pDialog->RenderHintAll( m_cxWordMaxExtent, m_HintTextColor, m_FrameColor, m_BackgroundColor, pElement, m_MousePoint, _GetHint(), _GetHintImagePath(), m_bHintTurnRight );
		/*if( wcslen( m_strHintImgPath ) )
        {
            RECT *rcCatch = 0;
            iTexture = m_pDialog->GetManager()->AddTexture( m_strHintImgPath );
            pTextureElement.SetTexture( iTexture, rcCatch );
            pTextureElement.TextureColor.Current = D3DCOLOR_ARGB( 255, 255, 255, 255 );

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
            
			WCHAR temp[MAX_PATH] ;
			ZeroMemory( &temp, sizeof( temp ) );
			StringCchCopy( temp, MAX_PATH, m_strHint );
			RemoveFirst_SlashN( temp );
			
			// 產生 wszText 的 TextObject，並指定給該 Item。
            LONG cxMaxExtent = 25 * 6; // 12 chars * cxChar
            DWORD m_dwDefaultColor = 0xFFFFFFFF;
            g_FDOXML.SetDefaultColor(m_dwDefaultColor);
            CTextObject* pTextObject = g_FDOXML.MakeTextObject(
                g_pD3DDevice, NULL, D3DCOLOR_ARGB(255, 255, 255, 255), temp, -1, cxMaxExtent);
            assert(pTextObject);

			SIZE Extent;
            pTextObject->GetExtent( Extent );
			
            SetRect( &rcText, 5, 5, Extent.cx+5, Extent.cy+5 );
            OffsetRect( &rcText, m_MousePoint.x - RectWidth( rcText ) - 10, m_MousePoint.y - RectHeight( rcText ) - 10 );

			SetRect( &rcBack, rcText.left - 5, rcText.top - 5, rcText.right + 5, rcText.bottom + 5 );
            //OffsetRect( &rcBack, m_MousePoint.x - RectWidth( rcText ) - 5, m_MousePoint.y - RectHeight( rcText )/2 - 5 );


            POINT Pt;
			m_pDialog->GetLocation( Pt );
			if( rcBack.top + Pt.y < 0 )//m_pDialog->GetHeight() )
            {
                OffsetRect( &rcText, 0, RectHeight( rcBack ) + 40 );
                OffsetRect( &rcBack, 0, RectHeight( rcBack ) + 40 );
            }
			if( rcBack.left + Pt.x < 0 )
            {
                OffsetRect( &rcText, RectWidth( rcBack ) + 40 , 0 );
                OffsetRect( &rcBack, RectWidth( rcBack ) + 40 , 0 );
            }
                        
            SetRect( &rcLine, rcBack.left+1, rcBack.top+1, rcBack.right-2, rcBack.bottom-2 );

            m_pDialog->DrawRect( &rcBack, D3DCOLOR_ARGB(180, 0, 0, 0) );
            m_pDialog->DrawFrameRect( &rcLine, D3DCOLOR_ARGB(255, 150, 150, 255) );
            m_pDialog->DrawTextObject( pTextObject, pElement, &rcText, false );
			delete pTextObject;
        }*/
    }
}

//-----------------------------------------------------------------------------
void CDXUTImageBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{

	if(m_bVisible == false)
		return ;
    CDXUTElement* pElement = 0;

    // 畫出ImageBox的底圖
    if( m_Elements.size() > 0 )
    {
        pElement = GetElement(0);
		if( !m_bRenderBackground )
			pElement->TextureColor.Current = D3DXCOLOR(0,0,0,0);
		else
			pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
        m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );
    }

    // 畫出ImageBox上的Item
    if( m_Items.GetSize() > 0 )
    {
        DXUTImageListBoxItem *pItem = m_Items.GetAt( 0 );

        // 這裡處理Image
        pElement = pItem->Image->GetElement(0); // 取得Image的Element
        // 下面這行修改 fRate 為 0.0f 以避免 HUIQBar 與 HUIInfo 閃爍。
        pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime, 0.0f ); // Blend處理
        //pElement->TextureColor.Current = D3DXCOLOR( 255, 255, 255, 255 );
        m_pDialog->DrawSprite( pElement, &m_rcBoundingBox ); // 畫

        // 畫出數量
        if( pItem->m_bStatic )
        {
            pElement = pItem->Static->GetElement(0);

            D3DXCOLOR clrOriginal = pElement->FontColor.Current;
            pElement->FontColor.Current = 0xFF000000;

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
bool CDXUTImageBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
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
                DXUTImageListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
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

                if (MK_LBUTTON == wParam) {
                    g_Mousetrap.CaptureMouse(this);
                } // end if

                m_nSelected = 0;  
                DXUTImageListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
                pSelItem->bSelected = true;

                // ming add >> 設定拖曳!!
                if( m_bDragIcon )
                {
                    CDXUTControl::s_pDragSource = this; // 設定拖曳來源
                    CDXUTControl::s_SrcIndex = m_nSelected;
                    CDXUTControl::s_pDragTexture = pSelItem->Image->GetTexture(0);
					m_pDialog->SendEvent( EVENT_CONTROL_DRAG, true, this );
                }
                // ming add <<                

                //音效 *** modify
                PlaySoundWAV();  //under control class

                m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );                

                return true;
            }
            break;

        case WM_LBUTTONUP:
        {
            g_Mousetrap.ReleaseMouse(this);
            ReleaseCapture();
            m_bDrag = false;

            // Check for clicks in the text area
            // 有Item並且確認點擊範圍
            if( m_Items.GetSize() > 0 )//&& PtInRect( &m_rcSelection, pt ) )
            {
                CDXUTControl::s_DestIndex = 0;
            }

            if (CDXUTControl::s_pDragSource == this && 
                CDXUTControl::s_SrcIndex == CDXUTControl::s_DestIndex) 
            {
                if (!ContainsPoint(pt)) {
                    m_pDialog->SendEvent(EVENT_ITEM_DRAG_AND_DROP_OUT, true, this);
                    //return true;
                } // end if
            }
            else 
            {
                if (CDXUTControl::s_pDragSource != NULL &&
                    CDXUTControl::s_SrcIndex >= 0 &&
                    CDXUTControl::s_SrcIndex < ((CDXUTImageListBox*)CDXUTControl::s_pDragSource)->GetSize() )
                {
                    m_pDialog->SendEvent( EVENT_ITEM_DROP, true, this );
					m_pDialog->SendEvent( EVENT_CONTROL_DROP, true, this );
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
void CDXUTImageBox::UpdateRects()
{
    SetRect( &m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height );
}         

//--------------------------------------------------------------------------------------
// CDXUTImagesList class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CDXUTImageBoxList::CDXUTImageBoxList( CDXUTDialog *pDialog ) :
      CDXUTListBox( pDialog )
{
}

//--------------------------------------------------------------------------------------
CDXUTImageBoxList::~CDXUTImageBoxList()
{
    RemoveAllItems();
}

//--------------------------------------------------------------------------------------
void CDXUTImageBoxList::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
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

    // Find out the height of a single line of text
    RECT rc = m_rcText;
    //RECT rcSel = m_rcSelection;
    // 改變bottom的位置
    rc.bottom = rc.top + 32; // 32為固定高度

    // Update the line height formation
    // 計算出一行的高度
    m_nTextHeight = RectHeight( rc );

    static bool bSBInit;
    if( !bSBInit )
    {
        // Update the page size of the scroll bar
        if( m_nTextHeight )
            m_ScrollBar.SetPageSize( RectHeight( m_rcText ) / m_nTextHeight );
        else
            m_ScrollBar.SetPageSize( RectHeight( m_rcText ) );
        bSBInit = true;
    }

    rc.right = m_rcText.right;
    for( int i = 0; i < (int)m_ImageBoxes.GetSize(); ++i ) // 處理所有的ImageBox
    {
        CDXUTImageBox *pImageBox = m_ImageBoxes.GetAt( i ); // 取得ImageBox

        // 設定不可視的圖案為全透明(目的是用來增加視覺效果)
        if( i/m_numCellsPerRow < m_ScrollBar.GetTrackPos() || rc.bottom > m_rcText.bottom )
        {
            if( pImageBox->HasItem() )
            {
                CDXUTElement* pElement = pImageBox->GetItem(0)->Image->GetElement(0);
                pElement->TextureColor.Current = D3DXCOLOR( 0, 0, 0, 0 );
            }
            continue;
        }

        int mod = i % m_numCellsPerRow;
            
        pImageBox->SetLocation( rc.left+32*mod, rc.top );
        pImageBox->Render( pd3dDevice, fElapsedTime );
            
        if( i && (mod==0) ) // 如果到了Row的最後才做位移
            OffsetRect( &rc, 0, m_nTextHeight );
    }

    // Render the scroll bar
    m_ScrollBar.Render( pd3dDevice, fElapsedTime );
}

//--------------------------------------------------------------------------------------
