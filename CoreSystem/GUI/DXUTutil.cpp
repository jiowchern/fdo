#include "stdafx.h"
#include "DXUTutil.h"
#include "CDXUTDialog.h"
#include "CDXUTDialogResourceManager.h"

ID3DXFont* GetElementFont(CDXUTElement* pElement)
{
    if (!pElement)
        return NULL;
    DXUTFontNode* pFontNode = CDXUTDialog::GetFont(pElement->iFont);
    if (pFontNode)
        return pFontNode->pFont;
    return NULL;
}

CDXUTDialogResourceManager* GetDialogResourceManager()
{
    return CDXUTDialog::GetManager();
}

LPD3DXFONT GetCachedFont(int iFontNode)
{
    CDXUTDialogResourceManager* pRcMgr = CDXUTDialog::GetManager();
    if (!pRcMgr) return NULL;
    DXUTFontNode* pFontNode = pRcMgr->GetFontNode(iFontNode);
    if (!pFontNode) return NULL;
    return pFontNode->pFont;
}


//---------------------------------------------------------------------------
// HexToDWORD
//---------------------------------------------------------------------------

struct sHexCharValue {
    WCHAR wch;
    DWORD dwValue;
};
static sHexCharValue _HexCharValue[] = {
    { L'0', 0 }, { L'1', 1 }, { L'2', 2 }, { L'3', 3 }, { L'4', 4 }, 
    { L'5', 5 }, { L'6', 6 }, { L'7', 7 }, { L'8', 8 }, { L'9', 9 }, 
    { L'a', 10 }, { L'b', 11 }, { L'c', 12 }, { L'd', 13 }, { L'e', 14 }, { L'f', 15 }, 
    { L'A', 10 }, { L'B', 11 }, { L'C', 12 }, { L'D', 13 }, { L'E', 14 }, { L'F', 15 }
};

static DWORD HexCharToDWORD(WCHAR wch)
{
    int length = sizeof(_HexCharValue) / sizeof(sHexCharValue);
    for (int i = 0; i < length; i++) {
        if (wch == _HexCharValue[i].wch)
            return _HexCharValue[i].dwValue;
    } // end for
    return 0;
}

DWORD HexToDWORD(LPCWSTR wszValue)
{
    assert(wszValue);
    DWORD dwReturn = 0, dwMul = 1, dwTemp, dwDigitValue;
    int cch = lstrlen(wszValue);
    for (int i = cch - 1; i >= 0; i--) {
        dwDigitValue = HexCharToDWORD(wszValue[i]);
        dwTemp = dwMul * dwDigitValue;
        dwMul *= 16;
        dwReturn += dwTemp;
    } // end for
    return dwReturn;
}
