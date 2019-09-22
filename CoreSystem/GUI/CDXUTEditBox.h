//
// CDXUTEditBox.h
//
#ifndef __CDXUTEditBox_h__
#define __CDXUTEditBox_h__

#include "CDXUTDef.h"
#include "CDXUTControl.h"
#include "ElementDef.h"
#include "..\\CoreSystem\\Command\\GameCommand.h"
#include "CDXUTEditBoxInputPlugin.h"

//-----------------------------------------------------------------------------
// CUniBuffer class for the edit control
//-----------------------------------------------------------------------------
class CUniBuffer
{
public:
    CUniBuffer								(int nInitialSize = 1, EditBoxPlugin::CDXUTEditBoxInputPlugin* pInputPlugin = 0);
    ~CUniBuffer								();

    static void 			Initialize		();
    static void 			Uninitialize	();

    int  					GetBufferSize	()				{ return m_nBufferSize; }
    bool 					SetBufferSize	(int nSize);
    int  					GetTextSize		()				{ return lstrlenW( m_pwszBuffer ); }
    const WCHAR*			GetBuffer		()	const		{ return m_pwszBuffer; }
	WCHAR*					GetBuffer2		()				{ return m_pwszBuffer; }

	//void					SetBuffer		(const WCHAR* p)	{ m_pwszBuffer = p; }

    const WCHAR&	operator[]				(int n) const	{ return m_pwszBuffer[n]; }
    WCHAR&			operator[]				(int n);
    
	DXUTFontNode*			GetFontNode		()				{ return m_pFontNode; }
    void					SetFontNode(DXUTFontNode *pFontNode) { m_pFontNode = pFontNode; }
    
	struct InsertResult
	{
		friend CUniBuffer;
		enum	Result
		{
			RESULT_OK,
			RESULT_INVALIDINDEX,
			RESULT_MAXIUMLENGTH,
			RESULT_OUTOFMEMORY,			
		};
		
				InsertResult	(const InsertResult& nCode);
				~InsertResult	();
		bool	operator==		(const InsertResult& nCode) const;
		bool	operator==		(const Result& nCode) const;
		
		void	operator=		(const InsertResult& nCode) ;
	private:
				InsertResult	(Result nCode);
		
		Result mnCode;
	};

	void 					Clear			();
    InsertResult			InsertChar		(int nIndex, const WCHAR& wChar, EditBoxPlugin::CDXUTEditBoxInputPlugin& filterPlugin); // Inserts the char at specified index. If nIndex == -1, insert to the end.
    bool 					RemoveChar		(int nIndex);  // Removes the char at specified index. If nIndex == -1, remove the last char.
    InsertResult			InsertString	(int nIndex, const WCHAR *pStr, int nCount = -1);  // Inserts the first nCount characters of the string pStr at specified index.  If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.
		

    bool 					SetText			(LPCWSTR wszText, WORD wLangID);

	bool					ChkInputLimit	(int nCount);	// 檢查是否還可以輸入文字
	void					SetInputLimit	(int nCount);
	inline int				GetInputLimit	() const	 { return m_nInputLimitCount; }

    // Uniscribe
    HRESULT 				CPtoX			(int nCP, BOOL bTrail, int *pX);
    HRESULT 				XtoCP			(int nX, int *pCP, int *pnTrail);
    void 					GetPriorItemPos	(int nCP, int *pPrior);
    void 					GetNextItemPos	(int nCP, int *pPrior);

private:

	EditBoxPlugin::CDXUTEditBoxInputPlugin* m_pInputFilterPlugin;
    HRESULT					Analyse			();      // Uniscribe -- Analyse() analyses the string in the buffer

    WCHAR*					m_pwszBuffer;    // Buffer to hold text
    int    					m_nBufferSize;   // Size of the buffer allocated, in characters
	int	   					m_nInputLimitCount;	// 設定限制字數
    // Uniscribe-specific
    DXUTFontNode*			m_pFontNode;          // Font node for the font that this buffer uses
    bool					m_bAnalyseRequired;            // True if the string has changed since last analysis.
    SCRIPT_STRING_ANALYSIS	m_Analysis;  // Analysis for the current string

private:
    // Empty implementation of the Uniscribe API
    static HRESULT WINAPI Dummy_ScriptApplyDigitSubstitution( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringAnalyse( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringCPtoX( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringXtoCP( SCRIPT_STRING_ANALYSIS, int, int*, int* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringFree( SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
    static const SCRIPT_LOGATTR* WINAPI Dummy_ScriptString_pLogAttr( SCRIPT_STRING_ANALYSIS ) { return NULL; }
    static const int* WINAPI Dummy_ScriptString_pcOutChars( SCRIPT_STRING_ANALYSIS ) { return NULL; }

    // Function pointers
    static HRESULT (WINAPI *_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* );
    static HRESULT (WINAPI *_ScriptStringAnalyse)( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* );
    static HRESULT (WINAPI *_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* );
    static HRESULT (WINAPI *_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* );
    static HRESULT (WINAPI *_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* );
    static const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS );
    static const int* (WINAPI *_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS );

    static HINSTANCE s_hDll;  // Uniscribe DLL handle

};
//-----------------------------------------------------------------------------
// EditBox control
//-----------------------------------------------------------------------------

class DosKey
{	
	std::list<wstring>				mStrs;
	int								mnStackCount;
	std::list<wstring>::iterator	mCurString;
public:
					DosKey	(int nStackCount);
					~DosKey	();

	void			Push	(const WCHAR* str);
	const WCHAR*	GetPrev	();
	const WCHAR*	GetNext	();
};
//-----------------------------------------------------------------------------
// EditBox control
//-----------------------------------------------------------------------------
class CDXUTEditBox : public CDXUTControl , public DosKey
{
	::GameCommand::CommandHandle mhChecker;
	::GameCommand::CommandHandle mhLengthResponser;
	::GameCommand::Executer* mpExr;
public:
	struct EditBoxCheckParam 
	{
		const char16*	text;
		uint			nSize;
		char16			newChar;
		bool			bSuccess;
	};

	struct EditBoxLimitWarn
	{
		const char16*	text;
		uint			nSize;
		uint			nLimitSize;
		CDXUTEditBox*	pEditBox;
	};

    CDXUTEditBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTEditBox();

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void UpdateRects();
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void OnFocusIn();
	virtual void OnFocusOut();

	inline	void	SetChecker			(::GameCommand::Executer* pExr, ::GameCommand::CommandHandle hnd) { mpExr = pExr ; mhChecker = hnd ;}
	inline	void	SetLengthResponse	(::GameCommand::Executer* pExr, ::GameCommand::CommandHandle hnd) { mpExr = pExr ; mhChecker = hnd ;}

    void SetText( const string &szText, bool bSelected = false ); // ming add
    void SetText( LPCWSTR wszText, bool bSelected = false );
    void SetText( const int &iText, bool bSelected = false ); // ming add
    LPCWSTR GetText() const { return m_Buffer.GetBuffer(); }
    int GetTextLength() { return m_Buffer.GetTextSize(); }  // Returns text length in chars excluding NULL.
    HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
    void ClearText();
    virtual void SetTextColor( D3DCOLOR Color, UINT iElement = 0 ) { m_TextColor = Color; }  // Text color
    void SetSelectedTextColor( D3DCOLOR Color ) { m_SelTextColor = Color; }  // Selected text color
    void SetSelectedBackColor( D3DCOLOR Color ) { m_SelBkColor = Color; }  // Selected background color
    void SetCaretColor( D3DCOLOR Color ) { m_CaretColor = Color; }  // Caret color
    void SetBorderWidth( int nBorder ) { m_nBorder = nBorder; UpdateRects(); }  // Border of the window
    void SetSpacing( int nSpacing ) { m_nSpacing = nSpacing; UpdateRects(); }
    void ParseFloatArray( float *pNumbers, int nCount );
    void SetTextFloatArray( const float *pNumbers, int nCount );
    bool GetDrawFrame() { return m_bDrawFrame; }
    void SetDrawFrame(bool bDraw) { m_bDrawFrame = bDraw; }
    D3DCOLOR GetFrameColor() { return m_FrameColor; }
    void SetFrameColor(D3DCOLOR Color) { m_FrameColor = Color; }
	void SetInputMode(EditBoxPlugin::InputMode mode, sint nMaxNum = EditBoxPlugin::NO_MILIT_NUMBER_RANGE, sint nMinNum = EditBoxPlugin::NO_MILIT_NUMBER_RANGE);

    // ming add >>
    string GetTextA();
	wstring GetTextAW();
    int    GetTextI();
    void SetStarText( bool  bPassword ); // 設定輸入的字串使用星號顯示
    void ClearTexture(); // 不使用任何材質
    void SetDialog( CDXUTDialog *pDialog );
    // ming add <<

	bool	IsRecordable() const;
	void	EnableRecord(bool bEnable);
	
	void	SetInputCount(int nCount);
	static bool	HasEditFocus()	;
	static void	SetLimitWarn(::GameCommand::Executer* pExr,TdgHandle hCallBack);
	void StartFilter();
	
	
protected:
    void PlaceCaret( int nCP );
    void DeleteSelectionText();
    void ResetCaretBlink();
    void CopyToClipboard();
    void PasteFromClipboard();
	


	EditBoxPlugin::CDXUTEditBoxInputPlugin m_InputPlugin;
    CUniBuffer m_Buffer;     // Buffer to hold text
    int      m_nBorder;      // Border of the window
    int      m_nSpacing;     // Spacing between the text and the edge of border
    RECT     m_rcText;       // Bounding rectangle for the text
    RECT     m_rcRender[9];  // Convenient rectangles for rendering elements
    double   m_dfBlink;      // Caret blink time in milliseconds
    double   m_dfLastBlink;  // Last timestamp of caret blink
    bool     m_bCaretOn;     // Flag to indicate whether caret is currently visible
    int      m_nCaret;       // Caret position, in characters
    bool     m_bInsertMode;  // If true, control is in insert mode. Else, overwrite mode.
    int      m_nSelStart;    // Starting position of the selection. The caret marks the end.
    int      m_nFirstVisible;// First visible character in the edit control

	int		m_nInputCount;

    D3DCOLOR m_TextColor;    // Text color
    D3DCOLOR m_SelTextColor; // Selected text color
    D3DCOLOR m_SelBkColor;   // Selected background color
    D3DCOLOR m_CaretColor;   // Caret color
    D3DCOLOR m_FrameColor;
    bool     m_bPassword;    // ming add 顯示字元為*
    bool     m_bDrawFrame;
	bool	m_bRecord;
    // Mouse-specific
    bool m_bMouseDrag;       // True to indicate drag in progress

    // Static
    static bool s_bHideCaret;   // If true, we don't render the caret.
	
	void	_OnLimitString	(CUniBuffer& uniBuffer);
	

	
};

#endif // __CDXUTEditBox_h__
