//
// CFDOXML.cpp
//
// How to use CFDOXML:
// (1) CFDOXML m_FDOXML;
// (2) m_FDOXML.SetNormalFont(pFont);
// (3) m_FDOXML.SetMaxWidth(cxMax);
// (4) m_FDOXML.CalcTextExtent(szText, -1, sizeExtent);
// (5) m_FDOXML.DrawText(szText, -1, rectText);
//
#include "stdafx.h"
#pragma hdrstop
//-----------------------------------------------------------------------------------------
#include "CFDOXML.h"
#include <assert.h>
#include "TextObject.h"
//#include "Global.h"
#include "CDXUTDef.h"
#include "InlineImage.h"
#include "CDXUTDialogResourceManager.h"
#include "CDXUTDialog.h"

#ifndef RectWidth
#define RectWidth(refRect) (refRect.right - refRect.left)
#endif
#ifndef RectHeight
#define RectHeight(refRect) (refRect.bottom - refRect.top)
#endif

//=========================================================================================
// Utility Functions
//=========================================================================================
void fnCalcIconSrcRect(int index, RECT& rcSrc ,sint cxIcon,sint cyIcon)
{	
	int row = (index / 10);
	int col = (index % 10);
	rcSrc.left = col * cxIcon + 1; // 稍微修正截圖位置 (+1)
	rcSrc.top = row * cyIcon + 1; // 稍微修正截圖位置 (+1)
	rcSrc.right = rcSrc.left + cxIcon;
	rcSrc.bottom = rcSrc.top + cyIcon;
}

void CalcIconSrcRect(int index, RECT& rcSrc )
{   
	struct IconInfo
	{
		const char16*	path;
		SIZE			iconSize;
	};
	static const IconInfo iconInfos[] = 
	{
		{L"UI\\sysicon.tga",{12,12}		},
		{L"UI\\sysicon1.tga",{12,12}	},
		{L"UI\\sysicon2.tga",{12,12}	},
		{L"UI\\sysicon3.tga",{16,16}	},
		{L"UI\\sysicon4.tga",{16,16}	},
	};	
	TDGArray<const IconInfo> icons(iconInfos);
	sint idx = index / 100 ; 
	if(idx < (sint)icons.nCount)
	{
		fnCalcIconSrcRect(index % 100, rcSrc,icons.pObject[idx].iconSize.cx , icons.pObject[idx].iconSize.cy);
	}
	
}



//---------------------------------------------------------------------------
// 小圖示
//---------------------------------------------------------------------------

bool GetSysIconElement(int idxSysIcon, CDXUTElement& elemOut, DWORD dwTextureColor)
{
	// Get texture filename.
	struct IconInfo
	{
		const char16*	path;
		SIZE			iconSize;
	};
	static const IconInfo iconInfos[] = 
	{
		{L"UI\\sysicon.tga",{12,12}	},
		{L"UI\\sysicon1.tga",{12,12}	},
		{L"UI\\sysicon2.tga",{12,12}	},
		{L"UI\\sysicon3.tga",{24,24}	},
		{L"UI\\sysicon4.tga",{16,16}	},
	};	
	TDGArray<const IconInfo> icons(iconInfos);
	sint idx = idxSysIcon / 100 ; 
		
	if(idx < (sint)icons.nCount)
	{
		// Get texture source rectangle.
		RECT rcSrc;		
		fnCalcIconSrcRect(idxSysIcon % 100, rcSrc,icons.pObject[idx].iconSize.cx , icons.pObject[idx].iconSize.cy);

		LPCWSTR szImageFile = icons.pObject[idx].path;

		// Get texture-node index.
		CDXUTDialogResourceManager* pMgr = GetDialogResourceManager();
		assert(pMgr);
		int iTextureNode = pMgr->AddTexture(szImageFile);

		

		// Assign texture-node index and texture soruce rectangle to CDXUTElement.
		elemOut.SetTexture(iTextureNode, &rcSrc);
		elemOut.TextureColor.Current = dwTextureColor;
		return true;
	}
	return false;
}

BOOL _GetTextExtentExPoint(
  sint iFont,
  LPCWSTR lpszStr, // character string
  int cchString,   // number of characters
  int nMaxExtent,  // maximum width of formatted string
  LPINT lpnFit,    // maximum number of characters
  LPSIZE lpSize    // string dimensions
)
{
	
	CDXUTDialogResourceManager* pDRM = CDXUTDialog::GetManager();
	
	sint i = 0;
	lpSize->cx = lpSize->cy = 0;
	for (;	i < cchString ; ++i)
	{	
		SIZE szChar = pDRM->GetChar16Size(iFont ,lpszStr[i] );
		

		if((lpSize->cx + szChar.cx) >= nMaxExtent)
		{
			break;
		}			
		lpSize->cx += szChar.cx;		
		if(lpSize->cy < szChar.cy)
			lpSize->cy = szChar.cy;
		
	}
	*lpnFit = i ;
	
	

	
//     lpSize->cx = lpSize->cy = 0;
//     HDC hdc = pFont->GetDC();
// 
//     if (!GetTextExtentExPointW(hdc, lpszStr, cchString, nMaxExtent, lpnFit, NULL, lpSize))
//         return FALSE;
//     if (lpSize->cx > nMaxExtent)
//         lpSize->cx = nMaxExtent;
     return TRUE;
}

BOOL _GetTextExtentPoint(
  sint iFont,
  LPCWSTR lpszStr, // character string
  int cchString,   // number of characters
  LPSIZE lpSize    // string dimensions
)
{
	
	CDXUTDialogResourceManager* pDRM = CDXUTDialog::GetManager();

	sint i = 0;
	lpSize->cx = lpSize->cy = 0;
	for (;	i < cchString ; ++i)
	{	
		SIZE szChar = pDRM->GetChar16Size(iFont ,lpszStr[i] );
		
		lpSize->cx += szChar.cx;		
		if(lpSize->cy < szChar.cy)
			lpSize->cy = szChar.cy;

	}
	

//     lpSize->cx = lpSize->cy = 0;
//     HDC hdc = pFont->GetDC();
//     if (!GetTextExtentPointW(hdc, lpszStr, cchString, lpSize))
//         return FALSE;
     return TRUE;
}

//=========================================================================================
// Constants and Variables
//=========================================================================================

CFDOXML g_FDOXML;

//=========================================================================================
// Constructor/Destructor/Initializer
//=========================================================================================

CFDOXML::States::States()
{   
    dwColor = 0xFFFFFFFF; // default (text) color
    dwFormat = DT_SINGLELINE|DT_LEFT|DT_TOP|DT_NOCLIP; // default ID3DXFont::DrawText()'s dwFormat
    iFontNode = -1;
    bWordWrapping = false;
    // Channel fields.
    bUseTalkKind = false;
    talkKind = Talk_Total;
}

// Default Color
DWORD CFDOXML::GetDefaultColor()
{
    return m_dwDefaultColor;
}

void CFDOXML::SetDefaultColor(DWORD dwColor)
{
    m_dwDefaultColor = dwColor;
}

void CFDOXML::SetDefaultColor()
{   
    m_States.dwColor = m_dwDefaultColor;
}

void CFDOXML::SetDefaultFont()
{   
    SetFont(m_iNormalFontNode);
}

CFDOXML::CFDOXML()
{
    m_pD3DDevice = NULL;
    m_hParser = NULL;
    m_pSprite = NULL;
    m_pTextObject = NULL;
    m_dwDefaultColor = 0xFFFFFFFF;
    
    // Fonts
    m_iNormalFontNode = -1;
    m_iItalicFontNode = -1;
    m_iBoldFontNode = -1;
    
    // Text Extent
    m_lMaxWidth = (LONG)-1;

    // Document-view
    //SetRectEmpty(&m_rcDocument);
    //SetRectEmpty(&m_rcView);
}

CFDOXML::~CFDOXML()
{
    if (m_hParser) {
        FDOXML_DestroyParser(m_hParser);
        m_hParser = NULL;
    } // end if
}

bool CFDOXML::Initialize()
{
    if (m_hParser)
        return true;
      
    // Create parser.
    m_hParser = FDOXML_CreateParser();
    if (NULL == m_hParser)
        return false;
        
    m_pD3DDevice = g_pD3DDevice;
    //CreateFonts();
    return true;
}

//=========================================================================================
// Fonts
//=========================================================================================

bool CFDOXML::CreateFonts(const char16* fontName)
{
    if (!m_pD3DDevice)
        return false;
        
    if (-1 != m_iNormalFontNode)
        return true;

    //--------------------------------------------------------------------------
    // TalkRect fonts.
    //--------------------------------------------------------------------------
    const UINT cyFont = 16;
    LPCWSTR wszFontFace;
    int UserID = GetUserDefaultLangID(); // 1028 繁體 2052 簡體
	switch( UserID )
	{
		case 1028: {
			wszFontFace = L"細明體";
			break;
			}

		case 2052: {
			//wszFontFace = L"陔冼极";
			wszFontFace = L"宋体";
			break;
			}

		case 2336: {
			wszFontFace = L"Arial";
			break;
			}

		default:
			wszFontFace = L"Arial";
	}
wszFontFace = L"Arial";	
    // begintest
    //LPCWSTR wszFontFace = L"Meiryo";
    // endtest
    
    CDXUTDialogResourceManager* pRcMgr = GetDialogResourceManager();

    // Normal font.
    m_iNormalFontNode = pRcMgr->AddFont(wszFontFace, cyFont, FW_NORMAL);
    if (-1 == m_iNormalFontNode) return false;

    // Bold font.
    m_iBoldFontNode = pRcMgr->AddFont(wszFontFace, cyFont, FW_BOLD);
    if (-1 == m_iBoldFontNode) return false;

    // Italic font.
    m_iItalicFontNode = pRcMgr->AddFont(wszFontFace, cyFont, FW_NORMAL, TRUE);
    if (-1 == m_iItalicFontNode) return false;

    SetDefaultFont();

    return true;
}

int CFDOXML::GetNormalFont()
{
    return m_iNormalFontNode;
}

int CFDOXML::SetNormalFont(int iFontNode)
{
    int iOldFontNode = m_iNormalFontNode;
    m_iNormalFontNode = iFontNode;
    return iOldFontNode;
}

int CFDOXML::GetItalicFont()
{
    return m_iItalicFontNode;
}

int CFDOXML::SetItalicFont(int iFontNode)
{
    int iOldFontNode = m_iItalicFontNode;
    m_iItalicFontNode = iFontNode;
    return iOldFontNode;
}

int CFDOXML::GetBoldFont()
{
    return m_iBoldFontNode;
}

int CFDOXML::SetBoldFont(int iFontNode)
{
    int iOldFontNode = m_iBoldFontNode;
    m_iBoldFontNode = iFontNode;
    return iOldFontNode;
}

//=========================================================================================
// Text Extent
//=========================================================================================

void CFDOXML::SetMaxWidth(LONG lMaxExtent)
{
    m_lMaxWidth = lMaxExtent;
}

void CFDOXML::SetMaxHeight(LONG lMaxExtent)
{
	m_lMaxHeight = lMaxExtent;
}

//=========================================================================================
// States
//=========================================================================================

DWORD CFDOXML::GetColor()
{
    return m_States.dwColor;
}

DWORD CFDOXML::SetColor(DWORD dwColor)
{
    DWORD dwOldColor = m_States.dwColor;
    m_States.dwColor = dwColor;
    return dwOldColor;
}

DWORD CFDOXML::GetFormat()
{
    return m_States.dwFormat;
}

DWORD CFDOXML::SetFormat(DWORD dwFormat)
{
    DWORD dwOldFormat = m_States.dwFormat;
    m_States.dwFormat = dwFormat;
    return dwOldFormat;
}

int CFDOXML::GetFont()
{
    return m_States.iFontNode;
}

void CFDOXML::SetFont(int iFontNode)
{
    if (-1 == iFontNode) return;
    m_States.iFontNode = iFontNode;
}

bool CFDOXML::GetWordWrapping()
{
    return m_States.bWordWrapping;
}

bool CFDOXML::SetWordWrapping(bool bWordWrapping)
{
    bool bOldValue = m_States.bWordWrapping;
    m_States.bWordWrapping = bWordWrapping;
    return bOldValue;
}

eTalkKind CFDOXML::SetTalkKind(eTalkKind talkKind)
{
    eTalkKind old = m_States.talkKind;
    m_States.talkKind = talkKind;
    m_States.bUseTalkKind = true;
    return old;
}

//=========================================================================================
// Color Stack, Font Stack, And TalkKind Stack CustomId stack
//=========================================================================================

void CFDOXML::PushCustom(const CustomInfo& nid)
{
	m_listCustom.push_front(nid);
}

CFDOXML::CustomInfo CFDOXML::PopCustom()
{
	CustomInfo nId = m_listCustom.front();
	m_listCustom.pop_front();
	return nId;
}

CFDOXML::CustomInfo& CFDOXML::GetCurCustomInfo()
{
	return m_listCustom.front();
}

void CFDOXML::PushColor(DWORD dwColor)
{
    m_listColor.push_front(dwColor);
    SetColor(dwColor);
}

bool CFDOXML::PopColor()
{
    if (true == m_listColor.empty())
        return false;

    m_listColor.pop_front();

    if (true == m_listColor.empty())
        SetDefaultColor();
    else SetColor(m_listColor.front());

    return true;
}

void CFDOXML::PushFont(int iFontNode)
{
    m_listFont.push_front(iFontNode);
    SetFont(iFontNode);
}

bool CFDOXML::PopFont()
{
    if (true == m_listFont.empty())
        return false;

    m_listFont.pop_front();

    if (true == m_listFont.empty())
        SetDefaultFont();
    else SetFont(m_listFont.front());

    return true;
}

void CFDOXML::PushTalkKind(eTalkKind talkKind)
{
    m_listTalkKind.push_front(talkKind);
    SetTalkKind(talkKind);
}

bool CFDOXML::PopTalkKind()
{
    if (true == m_listTalkKind.empty())
        return false;

    m_listTalkKind.pop_front();

    if (true == m_listTalkKind.empty())
        m_States.bUseTalkKind = false;
    else SetTalkKind(m_listTalkKind.front());

    return true;
}

//=========================================================================================
// XML Parser Callbacks
//=========================================================================================

struct _Color {
    LPCWSTR name;
    DWORD value;
} _ColorArr[] = {
    { L"red", 0xFF0000 }, { L"yellow", 0xFFFF00 }, { L"lime", 0x00FF00 },
    { L"aqua", 0x00FFFF }, { L"blue", 0x0000FF }, { L"fuchsia", 0xFF00FF },
	{ L"white", 0xFFFFFF }, { L"silver", 0xC0C0C0 }, { L"teal" , 0x396200 } ,
    { L"c1", 0xFF0000 }, { L"c2", 0xFF8040 }, { L"c3", 0xF7F700 },
    { L"c4", 0x00F200 }, { L"c5", 0x00FFFF }, { L"c6", 0x0000FF },
    { L"c7", 0x9101AF }, { L"c8", 0xFF00FF }, { L"c9", 0x808080 },
	{ L"navyblue", 0x617BDB }, { L"deepgreen", 0x45852B },
};

struct _Command {
    LPCWSTR name;
} _CommandArr[] = {
    { L"id" }, { L"loc" }, { L"sit" }, { L"up" }, { L"ver" }, { L"w1" }, { L"w2" }
};

/*    
struct _Channel {
    LPCWSTR name;
    eTalkKind talkKind;
} _ChannelArr[] = {
    { L"A", Talk_Total }, { L"T", Talk_Team }, { L"G", Talk_Guild }, { L"S", Talk_Secret }
};
*/

void CALLBACK CFDOXML::XmlOnStartElement(void* pvUserData, LPCWSTR szElement)
{
    assert(pvUserData);
    CFDOXML* pFDOXML = (CFDOXML*)pvUserData;
    pFDOXML->m_strCurrentElement = szElement;
    //===============================================================
    // Color value tags: <C F="#FF0000">紅色</C>
    //===============================================================
    if (0 == lstrcmpiW(szElement, L"c")) {
        return;
    }
    //===============================================================
    // Font tags: <I></I>; <B></B>
    //===============================================================
    if (0 == lstrcmpiW(szElement, L"i")) {
        pFDOXML->PushFont(pFDOXML->GetItalicFont());
        return;
    }
    if (0 == lstrcmpiW(szElement, L"b")) {
        pFDOXML->PushFont(pFDOXML->GetBoldFont());
        return;
    }

	//===============================================================
	// custom tags: 
	//===============================================================
	if (0 == lstrcmpiW(szElement, L"custom")) 
	{
		CustomInfo ci;		
		pFDOXML->PushCustom(ci);
		return;
	} // end if


    //===============================================================
    // Line-break tag: <R/>; <R></R> 
    //===============================================================
/*
    if (0 == lstrcmpiW(szElement, L"r")) {
        if (pFDOXML->m_pTextObject) {
            pFDOXML->m_pTextObject->AddEmptyLine();
        } // end if
        return;
    }
*/
    //===============================================================
    // Color tags: See _ColorArr[].
    //===============================================================
    int length = sizeof(_ColorArr) / sizeof(_Color);
    for (int i = 0; i < length; i++) {
        if (0 == lstrcmpiW(szElement, _ColorArr[i].name)) {
            DWORD dwColor = _ColorArr[i].value;
            dwColor |= 0xFF000000;
            pFDOXML->PushColor(dwColor);
            return;
        } // end if
    } // end for
	
/*
    //===============================================================
    // Channel tags: A(ll), T(eam), G(uild), S(ecret)
    //===============================================================
    int nChannels = sizeof(_ChannelArr) / sizeof(_Channel);
    for (int i = 0; i < nChannels; i++) {
        if (0 == lstrcmpiW(szElement, _ChannelArr[i].name)) {
            pFDOXML->PushTalkKind(_ChannelArr[i].talkKind);
            return;
        } // end if
    } // end for
*/
}

void CALLBACK CFDOXML::XmlOnEndElement(void* pvUserData, LPCWSTR szElement)
{
    assert(pvUserData);
    CFDOXML* pFDOXML = (CFDOXML*)pvUserData;
    //===============================================================
    // Font tags
    //===============================================================
    if (0 == lstrcmpiW(szElement, L"i")) 
	{
        pFDOXML->PopFont();
        return;
    }
    if (0 == lstrcmpiW(szElement, L"b")) 
	{
        pFDOXML->PopFont();
        return;
    }
    //===============================================================
    // Line-break tag: <R/>; <R></R> 
    // 頂多兩行空白。兩行空白 = lines >= 2 | last 2 lines are empty.
    //===============================================================
    if (0 == lstrcmpiW(szElement, L"r")) 
	{
        if (pFDOXML->m_pTextObject) 
		{
            if (pFDOXML->m_pTextObject->TotalLines() >= 2) 
			{
                std::list<CLineObject>::iterator itBack = pFDOXML->m_pTextObject->m_Lines.end();
                CLineObject& lineLast1 = *(--itBack);
                CLineObject& lineLast2 = *(--itBack);
                if (lineLast1.IsEmpty() && lineLast2.IsEmpty())
                    return;
            } // end if
            pFDOXML->AddNewLine();
        } // end if
        return;
    } // end if
/*
    if (0 == lstrcmpiW(szElement, L"r")) {
        if (pFDOXML->m_pTextObject) {
            if (false == pFDOXML->m_pTextObject->m_Lines.empty()) {
                CLineObject& line = pFDOXML->m_pTextObject->m_Lines.back();
                if (false == line.IsEmpty())
                    pFDOXML->m_pTextObject->AddEmptyLine();
            } // end if
        } // end if
        return;
    }
*/
	if (0 == lstrcmpiW(szElement, L"c")) 
	{
		pFDOXML->PopColor();
		return ;
	}
        


    //===============================================================
    // Color tags
    //===============================================================
    int length = sizeof(_ColorArr) / sizeof(_Color);
    for (int i = 0; i < length; i++) {
        if (0 == lstrcmpiW(szElement, _ColorArr[i].name)) {
            pFDOXML->PopColor();
            return;
        } // end if
    } // end for
    //===============================================================
    // Icon tags.
    //===============================================================
    if (true == pFDOXML->ProcessIconTag(szElement)) {
        return;
    }
	//===============================================================
	// Icon tags.
	//===============================================================
	if (true == pFDOXML->ProcessCustomTag(szElement)) {
		return;
	}
    //===============================================================
    // Command tags
    //===============================================================
    int nCommands = sizeof(_CommandArr) / sizeof(_Command);
    for (int j = 0; j < nCommands; j++) {
        if (0 == lstrcmpiW(szElement, _CommandArr[j].name)) {
            if (pFDOXML->m_pTextObject) {
                pFDOXML->m_pTextObject->AddCommandElement(_CommandArr[j].name);
            } // end if
            return;
        } // end if
    } // end for
/*
    //===============================================================
    // Channel tags: A(ll), T(eam), G(uild), S(ecret)
    //===============================================================
    int nChannels = sizeof(_ChannelArr) / sizeof(_Channel);
    for (int i = 0; i < nChannels; i++) {
        if (0 == lstrcmpiW(szElement, _ChannelArr[i].name)) {
            pFDOXML->PopTalkKind();
            return;
        } // end if
    } // end for
*/
}
//=========================================================================================
// Custom Tags
//=========================================================================================
bool CFDOXML::ProcessCustomTag(LPCWSTR szElement)
{
	//自定文字字串
	assert(szElement);
	LPCWSTR tag = L"custom";
	const int nTagLen = lstrlen(tag);
	int nCount = 0;
	while(tag[nCount] == szElement[nCount] && ++nCount < nTagLen)
	{
		
	}
	if(nCount == nTagLen)
	{
		m_pTextObject->AddEmptyElement();
		CElement& ele = m_pTextObject->LastElement();
		// Set font.
		ele.SetFont(m_States.iFontNode);
		if (m_listColor.empty()) 
		{
			ele.UseDefaultColor(m_States.dwColor);
		} 
		else 
			ele.SetColor(m_States.dwColor);

		CDXUTDialogResourceManager* pRcMgr = CDXUTDialog::GetManager();
		if (!pRcMgr) 
			return NULL;
		DXUTFontNode* pFontNode = pRcMgr->GetFontNode(m_States.iFontNode);

		// Calc and update dimensions, to decide whether to add a new line.
		SIZE textExtent;
		textExtent.cx = 0;
		textExtent.cy = pFontNode->nHeight;

		CustomInfo ci = PopCustom();
		textExtent.cx = ci.width;
		ele.SetText(ci.defstr.c_str());

		CustomElement ce(ci.nId,&ele,textExtent);
		mcl.Send(ce);

		m_sizeTextObject.cx = m_lUsedWidth = m_lUsedWidth + textExtent.cx;
		
		m_pTextObject->LastLine().SetIfBiggerExtent(m_sizeTextObject.cx, textExtent.cy);
		m_pTextObject->SetIfBiggerExtent(m_sizeTextObject);

					
		ele.SetExtent(textExtent);		
		return true;
		
	}
		
	return false;
}
//=========================================================================================
// Image Tags
//=========================================================================================

bool CFDOXML::ProcessIconTag(LPCWSTR szElement)
// Returns true if an icon tag is recognized.
{
    assert(szElement);
    //-----------------------------------------------------------
    // "emo00" ~ "emo99" in "emoicon.tga".
    // "sys00" ~ "sys99" in "sysicon.tga".
    // "sysN00" ~ "sysN99" in "sysiconN.tga", where N = 1~9
    //-----------------------------------------------------------
    LPCWSTR szImageFileEm = L"UI\\emoicon.tga";

    if (lstrlen(szElement) < 3)
        return false;

    WCHAR chFirst = szElement[0];
    if (L'e' == chFirst || L'E' == chFirst) {
        //--------------------------------------------------
        // emotion icon tags (em[o]~)
        //--------------------------------------------------
        int i = 0;
        if (L'e' != szElement[i] && L'E' != szElement[i]) return false; i++;
        if (L'm' != szElement[i] && L'M' != szElement[i]) return false; i++;
        if (L'o' != szElement[i] && L'O' != szElement[i]) return false; i++;
        if (!iswdigit(szElement[i])) return false;
        int idxIcon = _wtoi(szElement + i);
        if (idxIcon > 99) return false;
        OnIconTag(szImageFileEm, idxIcon);
        return true;
        //--------------------------------------------------
    }
    else if (L's' == chFirst || L'S' == chFirst) {
        //--------------------------------------------------
        // system icon tags (sys~)
        //--------------------------------------------------
        int i = 0;
        if (L's' != szElement[i] && L'S' != szElement[i]) return false; i++;
        if (L'y' != szElement[i] && L'Y' != szElement[i]) return false; i++;
        if (L's' != szElement[i] && L'S' != szElement[i]) return false; i++;
        if (!iswdigit(szElement[i])) return false;
        int idxIcon = _wtoi(szElement + i);
        if (idxIcon > 999) return false;
        // Build image filename.
        std::wstring wstrFilename = L"UI\\sysicon";
        int idxImageFile = idxIcon / 100;
        if (idxImageFile >= 1) {
            WCHAR pwch[4];
            wsprintf(pwch, L"%d", idxImageFile);
            wstrFilename += pwch;
        }
        wstrFilename += L".tga";
        //idxIcon %= 100;
        OnIconTag(wstrFilename.c_str(), idxIcon);
        return true;
        //--------------------------------------------------
    }
    return false;
}

void CFDOXML::OnIconTag(LPCWSTR szFilePath, int index)
{
    assert(m_pTextObject);

    // Calculate source rectangle from index => rcSrc
    RECT rcSrc;
    CalcIconSrcRect(index, rcSrc);
        
    // Create a CInlineImage instance.
    CInlineImage* pInlineImage = new CInlineImage;
    pInlineImage->SetImage(szFilePath, &rcSrc);
    
    // Calc and update dimensions, to decide whether to add a new line.
    SIZE sizeImage = pInlineImage->m_ImageSize;
    m_sizeTextObject.cx = m_lUsedWidth = m_lUsedWidth + sizeImage.cx;
    if (0 == m_sizeTextObject.cy) {
        m_sizeTextObject.cy = sizeImage.cy;
    }
    if (m_lUsedWidth > m_lMaxWidth) {
        m_sizeTextObject.cx = m_lMaxWidth;
        m_lUsedWidth = 0;
        m_sizeTextObject.cy += sizeImage.cy;
        m_pTextObject->AddEmptyLine();
    }
    if (true == m_pTextObject->m_Lines.empty()) {
        m_pTextObject->AddEmptyLine();
    }
    m_pTextObject->LastLine().SetIfBiggerExtent(m_sizeTextObject.cx, sizeImage.cy);
    m_pTextObject->SetIfBiggerExtent(m_sizeTextObject);

    // Add the element.
    m_pTextObject->AddEmptyElement();
    m_pTextObject->LastElement().SetExtent(sizeImage);
    m_pTextObject->LastElement().CopyImage(pInlineImage);
    delete pInlineImage;
}

//=========================================================================================

void CALLBACK CFDOXML::XmlOnAttribute(void* pvUserData, LPCWSTR szName, LPCWSTR szValue)
{
    assert(pvUserData);
    CFDOXML* pFDOXML = (CFDOXML*)pvUserData;
    //===============================================================
    // Color value attributes
    //===============================================================
    if (0 == lstrcmpi(pFDOXML->m_strCurrentElement.c_str(), L"c")) {
        if (0 == lstrcmpiW(szName, L"f")) { // foreground color
            if (L'#' == szValue[0]) {
                szValue++;
                DWORD dwColor = HexToDWORD(szValue);
                dwColor |= 0xFF000000;
                pFDOXML->PushColor(dwColor);
            } // end if
            return;
        } // end if
    } // end if

	//===============================================================
	// custom attributes
	//===============================================================

	if (0 == lstrcmpi(pFDOXML->m_strCurrentElement.c_str(), L"custom")) 
	{
		CustomInfo& ci = pFDOXML->GetCurCustomInfo();
		if (0 == lstrcmpiW(szName, L"Id")) 
		{ 													
			ci.nId = _wtoi(szValue);			
			 // end if
			return;
		} // end if

		if (0 == lstrcmpiW(szName, L"def")) 
		{ 										
			ci.defstr = szValue;						
			// end if
			return;
		} // end if

		if (0 == lstrcmpiW(szName, L"Width")) 
		{ 	
			ci.width = _wtoi(szValue);			
			// end if
			return;
		} // end if
	} // end if

}

void CALLBACK CFDOXML::XmlOnError(void* pvUserData, LPCWSTR szError)
{
	assert(pvUserData);
	assert(szError);
	CFDOXML* pFDOXML = (CFDOXML*)pvUserData;

	pFDOXML->CalcTextElements(szError);
}

void CFDOXML::AddTextElement(LPCWSTR szText, UINT cchText, SIZE textExtent)
{
    m_pTextObject->AddEmptyElement();
    
    // Set font.
    m_pTextObject->LastElement().SetFont(m_States.iFontNode);

    // Set color.
    CElement& elem = m_pTextObject->LastElement();
    if (m_listColor.empty()) 
	{
        elem.UseDefaultColor(m_States.dwColor);
    } 
	else 
		elem.SetColor(m_States.dwColor);

/*
    // Set channel.
    if (true == m_States.bUseTalkKind) {
        elem.SetTalkKind(m_States.talkKind);
    } // end if
*/

    // Set text and text-extent. 
    if (szText && cchText) 
	{
        std::wstring wstr;
        if (-1 != cchText) 
		{
            wstr = std::wstring(szText, 0, cchText);
            szText = wstr.c_str();
        } // end if
        m_pTextObject->LastElement().SetText(szText);
        m_pTextObject->LastElement().SetExtent(textExtent);
    } // end if   
}

void CFDOXML::AddNewLine()
{
    if (NULL == m_pTextObject)
        return;
    SIZE sizeLastLine = { 0, 0 };
    if (false == m_pTextObject->m_Lines.empty()) {
        CLineObject& line = m_pTextObject->m_Lines.back();
        line.GetExtent(sizeLastLine);
    }
    m_pTextObject->AddEmptyLine();
    m_lUsedWidth = 0;
    m_sizeTextObject.cy += sizeLastLine.cy;
}

void CALLBACK CFDOXML::XmlCalcTextExtent(void* pvUserData, LPCWSTR szCharacters)
// The result will be put in m_sizeTextObject.
{
    assert(pvUserData);
    assert(szCharacters);
    CFDOXML* pFDOXML = (CFDOXML*)pvUserData;

	pFDOXML->CalcTextElements(szCharacters);
}

void CFDOXML::CalcTextElements(LPCWSTR szCharacters)
{
//     LPD3DXFONT pFont = GetCachedFont(m_States.iFontNode);
//     assert(pFont);
    bool bWordWrapping = m_States.bWordWrapping;
    //-------------------------------------------------------------------------
    // Variables to calculate TextRect.
    //-------------------------------------------------------------------------
    INT cchFit = 0;
    SIZE size;
    int nRemainedExtent = m_lMaxWidth - m_lUsedWidth;
    if (0 == nRemainedExtent)
        return;
    //-------------------------------------------------------------------------
    bool bWordBreak = false;
    INT cchLineFeed = 0;
    LPCWSTR wsz = szCharacters;
    int cchRemained = lstrlenW(wsz);    
    if (!cchRemained) return;

	bool bForceNewLine = false;
    for (; cchRemained; cchRemained-=(cchFit+cchLineFeed), wsz+=(cchFit+cchLineFeed)) 
    {
        // Initialize flags.
        bWordBreak = false;
        cchLineFeed = 0;

		
        // 估計可以繪製的字數 => cchFit
        if (!_GetTextExtentExPoint(m_States.iFontNode, wsz, cchRemained, nRemainedExtent, &cchFit, &size))
            break; // Failed.

		if(cchFit == 0 && m_lMaxWidth <= nRemainedExtent)
		{
			break;
		}
		else if(cchFit == 0)
		{
			bWordBreak = true;
			bForceNewLine = true;
		}
            
        // Word-break processing
        if (false == bWordWrapping) 
		{
            // 截取 LF 之前的字串
            int i = 0;
            for (; i < cchFit; i++) 
			{
                if (L'\n' == wsz[i]) 
				{
					//<gino>
					//while the first char is '\n' and previus char is the last char of the previous line 
					//(bForceNewLine == true), just skip this '\n'
					//</gino>
					if(i==0 && bForceNewLine)
					{
						continue;
					}
                    bWordBreak = true;
                    cchLineFeed = 1;
                    break;
                } // end if
            } // end for
            // 重新計算 size
            if (i != cchFit) 
			{
                cchFit = i;
                _GetTextExtentPoint(m_States.iFontNode, wsz, cchFit, &size);
            } // end if
        }
		//reset the flag
		bForceNewLine = false;

		// Calc new m_sizeTextObject.cy
        if (m_sizeTextObject.cy < size.cy) 
		{
            m_sizeTextObject.cy = size.cy;
        }
        // Calc new m_sizeTextObject.cx
        m_lUsedWidth += size.cx;
        if (m_sizeTextObject.cx < m_lUsedWidth) 
		{
            if (m_lUsedWidth > m_lMaxWidth)
                m_sizeTextObject.cx = m_lMaxWidth;
            else 
				m_sizeTextObject.cx = m_lUsedWidth;
        }
        // Add new element and save text extents.
        if (m_pTextObject ) 
		{
            AddTextElement(wsz, cchFit, size);
            // 更新 CLineObject 的 extent.
            m_pTextObject->LastLine().SetIfBiggerExtent(m_lUsedWidth, size.cy);
        }
        // Work-break processing - calc new y-dimension.
        nRemainedExtent -= size.cx;
		if (cchRemained > cchFit && nRemainedExtent <= 0)
		{
            bWordBreak = true;
			bForceNewLine = true;
		}
        if (true == bWordBreak) 
		{
            if (m_pTextObject)
                m_pTextObject->LastLine().SetIfBiggerExtent(m_lUsedWidth, size.cy);
            nRemainedExtent = m_lMaxWidth;
            m_lUsedWidth = 0;
            m_sizeTextObject.cy += size.cy;
            if (m_pTextObject) 
			{
                m_pTextObject->AddEmptyLine();
            } // end if
         } // end if
    } // end for

    if (m_pTextObject)
        m_pTextObject->SetIfBiggerExtent(m_sizeTextObject);
}

//=======================================================================================
// Interface of Calculating and Rendering Text
//=======================================================================================

void CFDOXML::ClearTagStacks()
{
    m_listColor.clear();
    m_listFont.clear();
}

CTextObject* CFDOXML::MakeTextObject(
    LPDIRECT3DDEVICE9 pd3dDevice, CDXUTElement* pElement,
    LPCWSTR szText, UINT cchText, LONG cxMaxExtent,LONG cyMaxExtent)
{
    assert(pElement);
    int iFontNode = pElement->iFont;
    D3DXCOLOR color = pElement->FontColor.Current;
    return MakeTextObject(pd3dDevice, iFontNode, color, szText, cchText, cxMaxExtent,cyMaxExtent);
}

CTextObject* CFDOXML::Test_MakeTextObject(
    LPDIRECT3DDEVICE9 pd3dDevice, CDXUTElement* pElement,
    LPCWSTR szText, UINT cchText, LONG cxMaxExtent,LONG cyMaxExtent)
{
    assert(pElement);
    int iFontNode = pElement->iFont;
    D3DXCOLOR color = pElement->FontColor.Current;
    return Test_MakeTextObject(pd3dDevice, iFontNode, color, szText, cchText, cxMaxExtent,cyMaxExtent);
}

CTextObject* CFDOXML::Test_MakeTextObject(
    LPDIRECT3DDEVICE9 pd3dDevice, int iFontNode, D3DXCOLOR color,
    LPCWSTR szText, UINT cchText, LONG cxMaxExtent,LONG cyMaxExtent)
{
    if (!szText || !cchText)
        return NULL;

    if (!m_pD3DDevice)	
        m_pD3DDevice = pd3dDevice;
	CreateFonts(0);
	
    
    
    if (-1 != iFontNode) SetFont(iFontNode);
    else SetDefaultFont();
    
    SetColor(color);

    // Create parser.
    static HXMLPARSER _hParser = NULL;
    if (NULL == _hParser) {
        _hParser = FDOXML_CreateParser();
        if (NULL == _hParser) return NULL;
    } // end if
    
    CTextObject* pTextObject = new CTextObject();
    m_pTextObject = pTextObject;
    //delete pTextObject;
    //pTextObject = NULL;

    // 設定計算 TextObject 的回呼函式。
    FDOXML_SetStartElementCallback(m_hParser, XmlOnStartElement);
    FDOXML_SetEndElementCallback(m_hParser, XmlOnEndElement);
    FDOXML_SetAttributeCallback(m_hParser, XmlOnAttribute);
    FDOXML_SetCharactersCallback(m_hParser, XmlCalcTextExtent);
    FDOXML_SetErrorCallback(m_hParser, XmlOnError);
  //	FDOXML_SetEndDocumentCallback(m_hParser, XmlOnEndDocument);

    // 初始 m_lUsedWidth 與 m_sizeTextObject。
    m_lUsedWidth = 0;
    m_sizeTextObject.cx = m_sizeTextObject.cy = 0;

    // 計算 TextObject。
    SetMaxWidth(cxMaxExtent);
	SetMaxHeight(cyMaxExtent);
    std::wstring wstr;
    if (-1 != cchText) {
        wstr = std::wstring(szText, 0, cchText);
        szText = wstr.c_str();
    }

    FDOXML_Parse(_hParser, szText, this);
    //m_pTextObject = NULL; => treat parsing as async callback
    
    ClearTagStacks();
    
    return pTextObject;
}

CTextObject* CFDOXML::MakeTextObject(
    LPDIRECT3DDEVICE9 pd3dDevice, int iFontNode, D3DXCOLOR color,
    LPCWSTR szText, UINT cchText, LONG cxMaxExtent,LONG cyMaxExtent)
{
    if (!szText || !cchText)
        return NULL;

    if (!m_pD3DDevice)
	    m_pD3DDevice = pd3dDevice;
	CreateFonts(0);
	
    
    if (-1 != iFontNode) SetFont(iFontNode);
    else SetDefaultFont();
    
    SetColor(color);

    // Create parser.
    if (NULL == m_hParser) {
        m_hParser = FDOXML_CreateParser();
        if (NULL == m_hParser) return NULL;
    } // end if
    
    CTextObject* pTextObject = new CTextObject();
    m_pTextObject = pTextObject;

    // 設定計算 TextObject 的回呼函式。
    FDOXML_SetStartElementCallback(m_hParser, XmlOnStartElement);
    FDOXML_SetEndElementCallback(m_hParser, XmlOnEndElement);
    FDOXML_SetAttributeCallback(m_hParser, XmlOnAttribute);
    FDOXML_SetCharactersCallback(m_hParser, XmlCalcTextExtent);
	FDOXML_SetErrorCallback(m_hParser, XmlOnError);
  //	FDOXML_SetEndDocumentCallback(m_hParser, XmlOnEndDocument);

	// 初始 m_lUsedWidth 與 m_sizeTextObject。
    m_lUsedWidth = 0;
    m_sizeTextObject.cx = m_sizeTextObject.cy = 0;

    // 計算 TextObject。
    SetMaxWidth(cxMaxExtent);
	SetMaxHeight(cyMaxExtent);
    std::wstring wstr;
    if (-1 != cchText) {
        wstr = std::wstring(szText, 0, cchText);
        szText = wstr.c_str();
    }
    FDOXML_Parse(m_hParser, szText, this);
    //m_pTextObject = NULL; => treat parsing as async callback
    
    ClearTagStacks();
    mcl.pfn = 0;
	mcl.nData = 0;
	return pTextObject;
}

void CALLBACK CFDOXML::XmlOnEndDocument(void* pvUserData)
{
    //assert(pvUserData);
    //CFDOXML* pFDOXML = (CFDOXML*)pvUserData;
}

void CFDOXML::CalcTextExtent(LPDIRECT3DDEVICE9 pd3dDevice, LPCWSTR szText, UINT cchText, SIZE& sizeOut)
// Program procedure, called by RoleManager::DrawGameRole().
{
    if (!m_pD3DDevice)
        m_pD3DDevice = pd3dDevice;
    CreateFonts(0);

    // Create parser.
    if (NULL == m_hParser) {
        m_hParser = FDOXML_CreateParser();
        if (NULL == m_hParser) return;
    } // end if

    // 設定計算方框大小的回呼函式。
    FDOXML_SetStartElementCallback(m_hParser, XmlOnStartElement);
    FDOXML_SetEndElementCallback(m_hParser, XmlOnEndElement);
    FDOXML_SetAttributeCallback(m_hParser, XmlOnAttribute);
    FDOXML_SetCharactersCallback(m_hParser, XmlCalcTextExtent);
    FDOXML_SetErrorCallback(m_hParser, XmlOnError);
	//FDOXML_SetEndDocumentCallback(m_hParser, XmlOnEndDocument);

	// 初始 m_lUsedWidth 與 m_sizeTextObject。
    m_lUsedWidth = 0;
	m_sizeTextObject.cx = m_sizeTextObject.cy = 0;

	// 計算方框的大小。
    std::wstring wstr;
    if (-1 != cchText) {
        wstr = std::wstring(szText, 0, cchText);
        szText = wstr.c_str();
    }
    FDOXML_Parse(m_hParser, szText, this);
    sizeOut = m_sizeTextObject;

    ClearTagStacks();
}
//////////////////////////////////////////////////////////////////////////
void CFDOXML::SetCustomElementCallback(PFN_ONCUSTOMELEMENT pfn,int nData) 
{
	mcl.pfn = pfn;
	mcl.nData = nData;
}
//////////////////////////////////////////////////////////////////////////
CFDOXML::CustomListener::CustomListener() :pfn(0),nData(0)
{

}
//////////////////////////////////////////////////////////////////////////
CFDOXML::CustomListener::~CustomListener() 
{

}
//////////////////////////////////////////////////////////////////////////
void CFDOXML::CustomListener::Send(CustomElement& ce)
{
	if(pfn)
		pfn(ce,nData);
}

bool CensoringTextObject(CTextObject* pTO,int iFont,const SIZE& szLimit,const char16* strCensoring /*= L"..."*/)
{	
	assert(pTO);
	CDXUTDialogResourceManager* pDRM = CDXUTDialog::GetManager();
	SIZE szCensoring = pDRM->GetStringSize(iFont , strCensoring);
	std::list<CLineObject>::iterator itCur = pTO->m_Lines.begin();
	std::list<CLineObject>::iterator itEnd = pTO->m_Lines.end();
	sint nHeight = szLimit.cy;
	CLineObject* pClo  = 0;
	while(itCur != itEnd)
	{
		CLineObject* pCurClo = &*itCur;
		
		
		SIZE szLine;
		pCurClo->GetExtent(szLine);
		nHeight -= szLine.cy;
		if( nHeight >= 0)
		{		
			pClo = pCurClo;
			++itCur;
			continue;
		}				
		if(pClo)
		{
			CLineObject& clo  = *pClo;
			sint nNeedWidth = szCensoring.cx;		
			while (nNeedWidth > 0 && clo.TotalElements())
			{
				CElement& cele = clo.LastElement();
				SIZE szLastEle ;
				cele.GetExtent(szLastEle);
				
				if(cele.IsImage())
				{
					nNeedWidth -= szLastEle.cx;
					clo.Remove(cele);
				}				
				else
				{
					const char16* text = cele.GetText();
					SIZE szText = pDRM->GetStringSize(iFont , text);
					if(szText.cx >= szCensoring.cx)
					{
						std::wstring out;
						CensoringString(out,iFont,text,szLastEle.cx - szCensoring.cx);
						cele.SetText(out.c_str());

					
						while(itCur != itEnd)
						{
							itCur = pTO->m_Lines.erase(itCur);		
						}
						pTO->SetExtent(szLimit);
						return true;
					}
					else
					{
						nNeedWidth -= szLastEle.cx;
						clo.Remove(cele);
					}					
				}
			}												
			while(itCur != itEnd)
			{
				itCur = pTO->m_Lines.erase(itCur);		
			}
			pTO->SetExtent(szLimit);


			pTO->AddEmptyElement();
			CElement& eleCensoring = pTO->LastElement();
			// 建立刪節號字串			
			eleCensoring.SetText(strCensoring);
			eleCensoring.SetExtent(szCensoring);
			eleCensoring.SetFont(iFont);
			eleCensoring.SetColor(0xffffffff);


			
			return true;
		}				
		return false;
	}
	
	return false;

}
