//==============================================================================================
// TextObject.h
//
// Class Hierarchy:
//
// CTextObject { std::list<CLineObject> m_Lines; }
//   +-- CLineObject { std::list<CElement> m_Elements; }
//         +-- CElement
//==============================================================================================

#ifndef __TextObject_h__
#define __TextObject_h__

#include "InlineImage.h"
//#include "Global.h"
typedef void (*_ProcessCommandCallback)(const LPCWSTR cmd);



class CElement
{
private:
    // Command fields.
    SIZE m_Extent;
    RECT m_Rect;
    // Textual CElement fields.
    std::wstring m_Text;
    int m_iFontNode;
    DWORD m_dwColor;
    DWORD m_dwColorTemp;
    bool m_bUseDefaultColor;
    // Inline-image CElement fields
    CInlineImage m_Image;
    // Command CElement fields.
    std::wstring m_strCommand;
    bool m_bCommandExecuted; // Commands are executed only when first-time rendering.
/*
    // Channel
    bool m_bUseTalkKind;
    eTalkKind m_TalkKind;
    bool m_bAddedToChannel;
*/
	//<gino>
	static _ProcessCommandCallback sm_cbProcessCommand;
	//</gino>
public:
    CElement();
    virtual ~CElement();
    void Clear();
    void SetText(LPCWSTR szText) { m_Text = szText; }
    void SetColor(DWORD dwColor);
    void UseDefaultColor(DWORD dwColor);
    bool UseDefaultColor();
	inline bool IsImage() const ;
    void SetFont(int iFontNode) { m_iFontNode = iFontNode; }
    void SetExtent(SIZE size) { m_Extent = size; }
    void GetExtent(SIZE& sizeOut) { sizeOut = m_Extent; }
    void SetRect(RECT& rect) { m_Rect = rect; }
    void SetHeight(LONG cy) { m_Extent.cy = cy; }
    void SetWidth(LONG cx) { m_Extent.cx = cx; }
/*
    // TalkKind
    bool UseTalkKind();
    void SetTalkKind(eTalkKind talkKind);
    bool GetTalkKind(eTalkKind& rOut);
*/
    // Text rendering
    void MoveTo(LONG left, LONG top);
    void DrawText(DWORD dwFormat, ID3DXSprite* pSprite, bool bShadow, bool bStretchImage = false);
    void ProcessTextEffects();
    LPCWSTR GetText() { return m_Text.c_str(); }
    // Image
    void CopyImage(CInlineImage* pImage);
    // Command methods
    void SetCommand(LPCWSTR szCommand);
    void SetCommandExecuted(bool);
    LPCWSTR GetCommand();
    bool GetCommandExecuted();

	static void SetCallbackProcessCommand(_ProcessCommandCallback func){ sm_cbProcessCommand = func; }

	static bool gbDebugRenderTextMode;


};

class CLineObject
{
private:
    std::list<CElement> m_Elements;
    SIZE m_Extent;
    RECT m_Rect;
public:
    CLineObject();
    virtual ~CLineObject();
    void Clear();
    void AddEmptyElement();
    CElement& LastElement();
    bool IsEmpty();
    void SetActualExtent(LONG cx, LONG cy);
    void SetIfBiggerExtent(LONG cx, LONG cy);
    void SetActualExtent(SIZE size);
    void SetIfBiggerExtent(SIZE size);
    void GetExtent(SIZE& sizeOut) { sizeOut = m_Extent; }
    void ChangeDefaultColor(DWORD dwColor);
    // Text rendering
    void MoveTo(LONG left, LONG top);
    void DrawText(DWORD dwFormat, const RECT* pRectView, ID3DXSprite* pSprite, bool bShadow);
    void ProcessTextEffects();
	void	Remove(const CElement& ele);
	bool GetElementIter(std::list<CElement>::iterator& it);
	int	 TotalElements();
};



class CTextObject
{
private:
    SIZE m_Extent;
    RECT m_Rect;
public:
    std::list<CLineObject> m_Lines;
public:
    CTextObject();
    virtual ~CTextObject();
    void Clear();
    // Operations
    void AddEmptyLine();
    void AddEmptyElement();
    void AddCommandElement(LPCWSTR szCommand);
    void ChangeDefaultColor(DWORD dwColor);
    // Text extents
    void SetActualExtent(SIZE size);
    void SetIfBiggerExtent(SIZE size);
    void SetActualExtent(LONG cx, LONG cy);
    void SetIfBiggerExtent(LONG cx, LONG cy);
    inline void GetExtent(SIZE& sizeOut) const { sizeOut = m_Extent; }
	inline void SetExtent(const SIZE& newsize) { m_Extent = newsize; }
    // Text rendering
    void MoveTo(LONG left, LONG top);
    void DrawText(DWORD dwFormat, const RECT* pRectView, ID3DXSprite* pSprite, bool bShadow);
    void ProcessTextEffects();
    // Info
    CLineObject& LastLine();
    CElement& LastElement();
    bool IsEmpty();
    UINT TotalLines();
};
typedef ObjFactory<CTextObject> CTextObjectFactory;

#endif // __TextObject_h__

