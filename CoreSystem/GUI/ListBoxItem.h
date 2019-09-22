//
// ListBoxItem.h
//
#ifndef __ListBoxItem_h__
#define __ListBoxItem_h__

#include "TextObject.h"

#ifndef _MaxItemTextSize_
#define _MaxItemTextSize_  512
#endif

struct DXUTListBoxItem
{
    WCHAR strText[_MaxItemTextSize_];
    void* pData;
    RECT  rcActive;
    bool  bSelected;
	bool  bCensoring;
    D3DXCOLOR FontColor; // ming add ¶r√C¶‚
    CLineObject LineObject; // used by VariableHeightListBox
    CTextObject TextObject; // used by CDXUTListBox
    
    DXUTListBoxItem() {
        ZeroMemory(strText, _MaxItemTextSize_ * sizeof(WCHAR));
        pData = NULL;
        ZeroMemory(&rcActive, sizeof(RECT));
        bSelected = false;
		bCensoring = false;
    }
};

struct DXUTComboBoxItem
{
    WCHAR strText[_MaxItemTextSize_];
    void*  pData;
    RECT  rcActive;
    bool  bVisible;
    D3DXCOLOR FontColor; // ming add ¶r√C¶‚
    CTextObject TextObject;
    
    DXUTComboBoxItem() {
        ZeroMemory(strText, _MaxItemTextSize_ * sizeof(WCHAR));
        pData = NULL;
        ZeroMemory(&rcActive, sizeof(RECT));
        bVisible = false;
    }
};


struct DXUTListItem
{

};

#endif // __ListBoxItem_h__

