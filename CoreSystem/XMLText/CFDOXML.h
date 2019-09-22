//
// CFDOXML.h
//
#ifndef __CFDOXML_h__
#define __CFDOXML_h__

#include "FDOXML.h"
//#include "Global.h"
class CElement;
class CTextObject;
class CDXUTElement;

struct CustomElement
{
	int			nId;
	CElement*	pEle;
	SIZE&		szCustom;
	CustomElement(int i,CElement* e,SIZE& s) 
		: nId(i) , pEle(e) , szCustom(s)
	{

	}
};
typedef void (*PFN_ONCUSTOMELEMENT)(CustomElement& ce,int nData);
class CFDOXML
{
private:
    LPDIRECT3DDEVICE9 m_pD3DDevice;
    HXMLPARSER m_hParser;
    ID3DXSprite* m_pSprite;
    CTextObject* m_pTextObject;
    std::wstring m_strCurrentElement;
    DWORD m_dwDefaultColor;
    
    // States 
    struct States {
        DWORD dwColor;
        DWORD dwFormat;
        int iFontNode;
        bool bWordWrapping;
        bool bUseTalkKind;
        eTalkKind talkKind;
        States();
    } m_States;
    
    void SetDefaultColor();
    void SetDefaultFont();

    // Color stack
    std::list<DWORD> m_listColor;
    void PushColor(DWORD dwColor);
    bool PopColor();
    
    // Font stack
    std::list<int> m_listFont;
    void PushFont(int iFontNode);
    bool PopFont();
    
    // eTalkKind stack
    std::list<eTalkKind> m_listTalkKind;
    void PushTalkKind(eTalkKind talkKind);
    bool PopTalkKind();

	struct CustomInfo
	{
		int				nId;
		std::wstring	defstr;
		int				width;
	};
	std::list<CustomInfo> m_listCustom;
	void PushCustom(const CustomInfo& ci);
	CustomInfo& GetCurCustomInfo();
	CustomInfo PopCustom();

    // Fonts
    int m_iNormalFontNode;
    int m_iItalicFontNode;
    int m_iBoldFontNode;

    // Text Extent
    long m_lMaxWidth;
	long m_lMaxHeight;
    long m_lUsedWidth;
    SIZE m_sizeTextObject;    
	struct CustomListener
	{
		PFN_ONCUSTOMELEMENT		pfn;
		int						nData;
		CustomListener() ;
		~CustomListener() ;
		void	Send(CustomElement& ce);
		
	};
	CustomListener	mcl;
	
private:    
    // Fonts.
    
    void SetFont(int iFontNode);

    // XML Parser Callbacks
    static void CALLBACK XmlOnError(void* pvUserData, LPCWSTR szError);
    static void CALLBACK XmlOnStartElement(void* pvUserData, LPCWSTR szElement);
    static void CALLBACK XmlOnEndElement(void* pvUserData, LPCWSTR szElement);
    static void CALLBACK XmlOnAttribute(void* pvUserData, LPCWSTR szName, LPCWSTR szValue);
    static void CALLBACK XmlOnEndDocument(void* pvUserData);
    static void CALLBACK XmlCalcTextExtent(void* pvUserData, LPCWSTR szCharacters);
    //static void CALLBACK XmlDrawText(void* pvUserData, LPCWSTR szCharacters);
    //static void CALLBACK XmlDrawSmoothScrollingText(void* pvUserData, LPCWSTR szCharacters);
    static void CALLBACK XmlMakeTextObject(void* pvUserData, LPCWSTR szCharacters);
    
    // Helper routines
    void ClearTagStacks();
    void AddTextElement(LPCWSTR szText, UINT cchText, SIZE textExtent);
    
    // Parsing subroutines
    void CalcTextElements(LPCWSTR szCharacters);
    bool ProcessIconTag(LPCWSTR szElement);
    void OnIconTag(LPCWSTR szFilePath, int index);
    bool ProcessCustomTag(LPCWSTR szElement);
public:
    CFDOXML();
    virtual ~CFDOXML();
    bool Initialize();
	bool CreateFonts(const char16* fontName);
    // TalkKind
    eTalkKind SetTalkKind(eTalkKind talkKind);
    // Fonts
    int GetNormalFont();
    int SetNormalFont(int iFontNode);
    int GetItalicFont();
    int SetItalicFont(int iFontNode);
    int GetBoldFont();
    int SetBoldFont(int iFontNode);
    // Default Color
    DWORD GetDefaultColor();
    void SetDefaultColor(DWORD dwColor);
    // States    
    DWORD GetColor();
    DWORD SetColor(DWORD dwColor);
    DWORD GetFormat();
    DWORD SetFormat(DWORD dwFormat);
    int GetFont();
    bool GetWordWrapping();
    bool SetWordWrapping(bool bWordWrapping);
    // Text Extent
    void SetMaxWidth(LONG lMaxWidth);
	void SetMaxHeight(LONG lMaxHeight);
    void CalcTextExtent(LPDIRECT3DDEVICE9 pd3dDevice, LPCWSTR szText, UINT cchText, SIZE& sizeOut);
    // MakeTextObject
    CTextObject* MakeTextObject(
        LPDIRECT3DDEVICE9 pd3dDevice, CDXUTElement* pElement,
        LPCWSTR szText, UINT cchText, LONG cxMaxExtent,LONG cyMaxExtent = -1);
    CTextObject* MakeTextObject(
        LPDIRECT3DDEVICE9 pd3dDevice, int iFontNode, D3DXCOLOR color,
        LPCWSTR szText, UINT cchText, LONG cxMaxExtent,LONG cyMaxExtent= -1);
    // Test_MakeTextObject
    CTextObject* Test_MakeTextObject(
        LPDIRECT3DDEVICE9 pd3dDevice, CDXUTElement* pElement,
        LPCWSTR szText, UINT cchText, LONG cxMaxExtent,LONG cyMaxExtent= -1);
    CTextObject* Test_MakeTextObject(
        LPDIRECT3DDEVICE9 pd3dDevice, int iFontNode, D3DXCOLOR color,
        LPCWSTR szText, UINT cchText, LONG cxMaxExtent,LONG cyMaxExtent= -1);

    void AddNewLine();

	/*
		!!使用注意 : 必須在MakeTextObject之前設定 MakeTextObject 執行完後callback將被清除
	*/
	void	SetCustomElementCallback	(PFN_ONCUSTOMELEMENT pfn,int nData) ;
};

extern CFDOXML g_FDOXML;


//===================================================================================
// 小圖示
//===================================================================================
void CalcIconSrcRect(int index, RECT& rcSrc);
bool GetSysIconElement(int idxSysIcon, CDXUTElement& elemOut, DWORD dwTextureColor=0xFFFFFFFF);

bool CensoringTextObject(CTextObject* pTO,int iFont,const SIZE& szLimit,const char16* strCensoring = L"...");
#endif // __CFDOXML_h__