//
// TextObject.cpp
//
#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "TextObject.h"
#include <assert.h>
#include "InlineImage.h"
//#include "Global.h"
#include "Common.h"
//#include "HUITalkBar.h"
#include "DXUTutil.h"
#define RectWidth(refRECT) ((refRECT).right - (refRECT).left)
#define RectHeight(refRECT) ((refRECT).bottom - (refRECT).top)

#define _DEFAULT_FONT_HEIGHT  12 // for blank-line, see CTextObject::AddEmptyLine()




_ProcessCommandCallback CElement::sm_cbProcessCommand = NULL;
//==============================================================================================
// CElement
//==============================================================================================

CElement::CElement() 
{
    m_bUseDefaultColor = false;
    m_iFontNode = -1;
    m_dwColor = 0;
    m_Extent.cx = m_Extent.cy = 0;
    SetRectEmpty(&m_Rect);
/*
    // TalkKind
    m_bUseTalkKind = false;
    m_TalkKind = Talk_Total;
    m_bAddedToChannel = false;
*/
}

CElement::~CElement() 
{
    Clear();
}

void CElement::Clear()
{
}

void CElement::CopyImage(CInlineImage* pImage)
{
    m_Image = *pImage;
    m_Extent = m_Image.m_ImageSize;
}

void CElement::MoveTo(LONG left, LONG top)
{
    m_Rect.left = left;
    m_Rect.top = top;
    m_Rect.right = left + m_Extent.cx;
    m_Rect.bottom = top + m_Extent.cy;
}
inline bool CElement::IsImage() const 
{
	return m_Image.m_pD3DTexture != 0 ;
}



bool CElement::gbDebugRenderTextMode = true;

void CElement::DrawText(DWORD dwFormat, ID3DXSprite* pSprite, bool bShadow, bool bStretchImage /*= false*/)
{	
    //-----------------------------------------------------------------------------
    // If this is an inline-image CElement, render the m_Image.
    //-----------------------------------------------------------------------------
    if (CElement::IsImage()) 
	{
        m_Image.Render(m_Rect,bStretchImage);
        return;
    }
    //-----------------------------------------------------------------------------
    // If this is a command CElement, and it has not been executed, execute it.
    //-----------------------------------------------------------------------------
    if (false == m_strCommand.empty() && false == m_bCommandExecuted) {
		if(sm_cbProcessCommand)
			sm_cbProcessCommand(m_strCommand.c_str());
//        ProcessCommand(m_strCommand.c_str());
        m_bCommandExecuted = true;
        return;
    }
    //-----------------------------------------------------------------------------
    // This is a textual CElement, render the m_Text.
    //-----------------------------------------------------------------------------
	
	
	if (-1 == m_iFontNode)
    return;
// #ifndef _DEBUG
// 	Rect32 rc;
// 	rc.left = m_Rect.left;
// 	rc.right = m_Rect.right;
// 	rc.bottom = m_Rect.bottom;
// 	rc.top = m_Rect.top;
// 
// 	if (bShadow) 
// 	{
// 		Rect32 rect = rc;
// 		rect.top -= 1;		
// 		CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rect,0x80000000);
// 		rect.top += 2;
// 		CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rect,0x80000000);
// 		rect.top = rc.top;
// 		rect.left -= 1;
// 		CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rect,0x80000000);
// 		rect.left += 2;
// 		CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rect,0x80000000);
// 		
// 	}
// 	CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rc,m_dwColor);
// 	return;
// #else

	if(gbDebugRenderTextMode)
	{
		Rect32 rc;
		rc.left = m_Rect.left;
		rc.right = m_Rect.right;
		rc.bottom = m_Rect.bottom;
		rc.top = m_Rect.top;

		//if (bShadow) 
		//{
		//	Rect32 rect = rc;
		//	rect.top -= 1;		
		//	CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rect,dwFormat,0x80000000);
		//	rect.top += 2;
		//	CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rect,dwFormat,0x80000000);
		//	rect.top = rc.top;
		//	rect.left -= 1;
		//	CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rect,dwFormat,0x80000000);
		//	rect.left += 2;
		//	CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rect,dwFormat,0x80000000);

		//}
		CDXUTDialog::GetManager()->DrawFont(m_iFontNode ,m_Text.c_str(),rc,dwFormat,m_dwColor,(bShadow)?2 : 0);
				
		return;
	}
	else
	{
		LPD3DXFONT pFont = GetCachedFont(m_iFontNode);
		if (!pFont) return;
		INT hr ;

		// Text shadow support. Ref: JL\D3DFont.cpp\CD3DFont::DrawText().
		if(pSprite)
			pSprite->Flush();
		if (bShadow) 
		{
			RECT rect = m_Rect;
			rect.top -= 1;
			hr = pFont->DrawText(NULL, m_Text.c_str(), -1, &rect, dwFormat, 0x80000000);
			rect.top += 2;
			hr = pFont->DrawText(NULL, m_Text.c_str(), -1, &rect, dwFormat, 0x80000000);
			rect.top = m_Rect.top;
			rect.left -= 1;
			hr = pFont->DrawText(NULL, m_Text.c_str(), -1, &rect, dwFormat, 0x80000000);
			rect.left += 2;
			hr = pFont->DrawText(NULL, m_Text.c_str(), -1, &rect, dwFormat, 0x80000000);
		}
		// Draw main text.			

		hr = pFont->DrawText(pSprite, m_Text.c_str(), -1, &m_Rect, dwFormat, m_dwColor);
	}
	

	
//#endif
	
}

void CElement::ProcessTextEffects()
{
/*
    if (m_dwColor == 0xFF000000)
        m_dwColor = m_dwColorTemp;
    else {
        m_dwColorTemp = m_dwColor;
        m_dwColor = 0xFF000000;
    } 
*/
}

void CElement::SetColor(DWORD dwColor)
{
    m_bUseDefaultColor = false;
    m_dwColor = dwColor; 
}

void CElement::UseDefaultColor(DWORD dwColor)
{
    m_bUseDefaultColor = true;
    m_dwColor = dwColor;
}

bool CElement::UseDefaultColor()
{
    return m_bUseDefaultColor;
}

/*
// TalkKind
bool CElement::UseTalkKind()
{
    return m_bUseTalkKind;
}

void CElement::SetTalkKind(eTalkKind talkKind)
{
    m_TalkKind = talkKind;
    m_bUseTalkKind = true;
}

bool CElement::GetTalkKind(eTalkKind& rOut)
{
    if (false == m_bUseTalkKind)
        return false;
    rOut = m_TalkKind;
    return true;
}
*/

//==============================================================================================
// CElement Command methods
//==============================================================================================

void CElement::SetCommand(LPCWSTR szCommand)
{
    m_strCommand = szCommand;
}

LPCWSTR CElement::GetCommand()
{
    return m_strCommand.c_str();
}

void CElement::SetCommandExecuted(bool bExecuted)
{
    m_bCommandExecuted = bExecuted;
}

bool CElement::GetCommandExecuted()
{
    return m_bCommandExecuted;
}

//==============================================================================================
// CLineObject
//==============================================================================================

CLineObject::CLineObject()
{
    m_Extent.cx = m_Extent.cy = 0;
    SetRectEmpty(&m_Rect);
}

CLineObject::~CLineObject()
{
    Clear();
}

void CLineObject::Clear()
{
    if (false == m_Elements.empty()) {
        std::list<CElement>::iterator it = m_Elements.begin();
        for (; it != m_Elements.end(); ++it) {
            (*it).Clear();
        } // end for
        m_Elements.clear();
    } // end if
}

void CLineObject::AddEmptyElement()
{
    CElement elem;
    m_Elements.push_back(elem);
}

CElement& CLineObject::LastElement()
{
    return m_Elements.back();
}

void CLineObject::Remove(const CElement& ele)
{
	
	_FOREACH( std::list<CElement> , m_Elements , it)
	{
		if(&*it != &ele)
			continue;

		m_Elements.erase(it);
		return;
	}
}
bool CLineObject::IsEmpty()
{
    return m_Elements.empty();
}

void CLineObject::MoveTo(LONG left, LONG top)
{
    if (m_Elements.empty())
        return;

    SIZE extent;
    std::list<CElement>::iterator it = m_Elements.begin();
    for (; it != m_Elements.end(); it++) {
        (*it).MoveTo(left, top);
        (*it).GetExtent(extent);
        left += extent.cx;
    } // end for
}

void CLineObject::DrawText(DWORD dwFormat, const RECT* pRectView, ID3DXSprite* pSprite, bool bShadow)
// Assumed: pRectView is larger than m_Extent.
// ToDo: Add DT_CENTER support.
{
    assert(pRectView);
    MoveTo(pRectView->left, pRectView->top);
    //---------------------------------------------------------------------
    // Horizontal alignment: DT_CENTER, DT_RIGHT; default = DT_LEFT
    //---------------------------------------------------------------------
    LONG cxOffset = 0;

    if (dwFormat & DT_CENTER) {
        LONG cxView = RectWidth(*pRectView);
        LONG cxText = m_Extent.cx;
        cxOffset = (cxView - cxText) / 2;
    }
    else if (dwFormat & DT_RIGHT) {
        LONG cxView = RectWidth(*pRectView);
        LONG cxText = m_Extent.cx;
        cxOffset = cxView - cxText;
    }
    //---------------------------------------------------------------------
    // Vertical alignment: DT_VCENTER, DT_BOTTOM; default = DT_TOP
    //---------------------------------------------------------------------
    LONG cyOffset = 0;

    // ¥­·Æ±²°Ê (DT_BOTTOM)
    if (dwFormat & DT_BOTTOM) {
        LONG cyView = RectHeight(*pRectView);
        LONG cyText = m_Extent.cy;
        if (cyView - cyText < 0) {
            dwFormat &= ~DT_NOCLIP;
        } // end if
    } // end if
    //---------------------------------------------------------------------
    // Calc rendering rectangle.
    RECT rcElem;
    rcElem.left = pRectView->left + cxOffset;
    rcElem.top = pRectView->top + cyOffset;
    rcElem.right = rcElem.left + m_Extent.cx;
    rcElem.bottom = rcElem.top + m_Extent.cy;

    // Draw with left-align in rendering rectangle.
    dwFormat &= ~DT_RIGHT;
    dwFormat &= ~DT_CENTER;
    dwFormat |= DT_LEFT;
    
    std::list<CElement>::iterator it = m_Elements.begin();
    for (; it != m_Elements.end(); it++) {
        (*it).SetRect(rcElem);
        (*it).DrawText(dwFormat, pSprite, bShadow);
        SIZE extent;
        (*it).GetExtent(extent);
        rcElem.left += extent.cx;
    } // end for
}

void CLineObject::SetActualExtent(SIZE size)
{
    SetActualExtent(size.cx, size.cy);
}

void CLineObject::SetIfBiggerExtent(SIZE size)
{
    SetIfBiggerExtent(size.cx, size.cy);
}

void CLineObject::SetActualExtent(LONG cx, LONG cy)
{
    m_Extent.cx = cx; 
    m_Extent.cy = cy; 
}

void CLineObject::SetIfBiggerExtent(LONG cx, LONG cy)
{
    if (m_Extent.cx < cx)
        m_Extent.cx = cx;
    if (m_Extent.cy < cy)
        m_Extent.cy = cy;
}

void CLineObject::ProcessTextEffects()
{
    std::list<CElement>::iterator it = m_Elements.begin();
    for (; it != m_Elements.end(); it++) {
        (*it).ProcessTextEffects();
    } // end for
}

void CLineObject::ChangeDefaultColor(DWORD dwColor)
{
    std::list<CElement>::iterator it = m_Elements.begin();
    for (; it != m_Elements.end(); it++) {
        CElement& elem = *it;
        if (elem.UseDefaultColor()) {
            elem.UseDefaultColor(dwColor);
        } // end if
    } // end for
}

bool CLineObject::GetElementIter(std::list<CElement>::iterator& it)
{
	if(m_Elements.size())
	{
		it = m_Elements.begin();
		return true;
	}

	return false;
}

int CLineObject::TotalElements()
{
	return m_Elements.size();
}
//==============================================================================================
// CTextObject Constructor/Destructor
//==============================================================================================

CTextObject::CTextObject()
{
    m_Extent.cx = m_Extent.cy = 0;
    SetRectEmpty(&m_Rect);
}

CTextObject::~CTextObject()
{
    Clear();
}

void CTextObject::Clear()
{
    if (false == m_Lines.empty()) {
        std::list<CLineObject>::iterator it = m_Lines.begin();
        for (; it != m_Lines.end(); ++it) {
            (*it).Clear();
        } // end for
        m_Lines.clear();
    } // end if
}

//==============================================================================================
// CTextObject: Operations
//==============================================================================================

void CTextObject::AddEmptyLine()
{  
    // Adjust CTextObject.m_Extent.cy.
    int cyPrevLine = _DEFAULT_FONT_HEIGHT;
    if (false == m_Lines.empty()) {
        CLineObject& refLine = m_Lines.back();
        SIZE size;
        refLine.GetExtent(size);
        cyPrevLine = size.cy ? size.cy : _DEFAULT_FONT_HEIGHT;
    } // end if
    m_Extent.cy += cyPrevLine;

    // Add new line.
    CLineObject line;
    m_Lines.push_back(line);    
}

void CTextObject::AddEmptyElement()
// Add an CElement in current CLineObject object.
{
    if (true == m_Lines.empty())
        AddEmptyLine();
    LastLine().AddEmptyElement();
}

void CTextObject::AddCommandElement(LPCWSTR szCommand)
{
    assert(szCommand);
    if (0 == lstrlenW(szCommand))
        return;
    AddEmptyElement();
    CElement& rElement = LastElement();
    rElement.SetCommand(szCommand);
    rElement.SetCommandExecuted(false);
}

//==============================================================================================
// CTextObject: States
//==============================================================================================

void CTextObject::ChangeDefaultColor(DWORD dwColor)
{
    std::list<CLineObject>::iterator it = m_Lines.begin();
    for (; it != m_Lines.end(); it++) {
        (*it).ChangeDefaultColor(dwColor);
    } // end for
}

//==============================================================================================
// CTextObject: Text rendering
//==============================================================================================

void CTextObject::MoveTo(LONG left, LONG top)
{
    if (m_Lines.empty())
        return;

    SIZE extent;
    std::list<CLineObject>::iterator it = m_Lines.begin();
    for (; it != m_Lines.end(); it++) {
        (*it).MoveTo(left, top);
        (*it).GetExtent(extent);
        top += extent.cy;
    } // end for
}

void CTextObject::DrawText(DWORD dwFormat, const RECT* pRectView, ID3DXSprite* pSprite, bool bShadow)
// Assumed: pRectView is larger than m_Extent.
// ToDo: Add DT_VCENTER support.
{
	
    assert(pRectView);    
    
    // Support DT_VCENTER.
    LONG cyOffset = 0;
    if (dwFormat & DT_VCENTER) {
        LONG cyView = RectHeight(*pRectView);
        LONG cyText = m_Extent.cy;
        cyOffset = (cyView - cyText) / 2;
    } // end if
    
    RECT rcLine;
    CopyRect(&rcLine, pRectView);
    rcLine.top += cyOffset;
    
    SIZE sizeLine;
    int cyLine;
    std::list<CLineObject>::iterator it = m_Lines.begin();
    for (; it != m_Lines.end(); it++) 
	{
        CLineObject& line = *it;
        line.GetExtent(sizeLine);
        if (false == line.IsEmpty() && 0 != sizeLine.cx) 
		{
            line.DrawText(dwFormat, &rcLine, pSprite, bShadow);
        } // end if
        cyLine = sizeLine.cy ? sizeLine.cy : _DEFAULT_FONT_HEIGHT;
        rcLine.top += cyLine;
    } // end for
}

void CTextObject::ProcessTextEffects()
{
    std::list<CLineObject>::iterator it = m_Lines.begin();
    for (; it != m_Lines.end(); it++) {
        (*it).ProcessTextEffects();
    } // end for
}

//==============================================================================================
// CTextObject: Info
//==============================================================================================

UINT CTextObject::TotalLines()
{
    return (UINT)(m_Lines.size());
}

CLineObject& CTextObject::LastLine()
{
    assert(false == m_Lines.empty());
    return m_Lines.back();
}

CElement& CTextObject::LastElement()
{
    assert(!IsEmpty());
    return LastLine().LastElement();
}

bool CTextObject::IsEmpty()
{
    std::list<CLineObject>::iterator it = m_Lines.begin();
    for (; it != m_Lines.end(); it++) {
        if (false == (*it).IsEmpty())
            return false;
    } // end for
    return true;
}

//==============================================================================================
// TextObject Size
//==============================================================================================

void CTextObject::SetActualExtent(SIZE size)
{
    SetActualExtent(size.cx, size.cy);
}

void CTextObject::SetIfBiggerExtent(SIZE size)
{
    SetIfBiggerExtent(size.cx, size.cy);
}

void CTextObject::SetActualExtent(LONG cx, LONG cy)
{
    m_Extent.cx = cx; 
    m_Extent.cy = cy; 
}

void CTextObject::SetIfBiggerExtent(LONG cx, LONG cy)
{
    if (m_Extent.cx < cx)
        m_Extent.cx = cx;
    if (m_Extent.cy < cy)
        m_Extent.cy = cy;
}

