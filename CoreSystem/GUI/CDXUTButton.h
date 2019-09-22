//
// CDXUTButton.h
//
#ifndef __CDXUTButton_h__
#define __CDXUTButton_h__

#include "CDXUTDef.h"
#include "CDXUTStatic.h"

class CTextObject;

//-----------------------------------------------------------------------------
// Button control
//-----------------------------------------------------------------------------
class CDXUTButton : public CDXUTStatic
{
public:
	//<gino>
	enum eEventType {
		etClick,
		etPressRelease,
	};
    //</gino>
public:
	static std::string sm_actionMouseAnim; ///�I��_�ʧ@
	static std::string sm_normalMouseAnim; ///�I��_�`�A
public:
    CDXUTButton( CDXUTDialog *pDialog = NULL );
    
    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey();

    virtual BOOL ContainsPoint( POINT pt );
    virtual bool CanHaveFocus();

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    void SetColor( D3DCOLOR color );
    void SetNormalStateAlpha( int iAlpha );
    void SetNormalTexture( const string &FileName, RECT* prcTexture=0 );
    void SetNormalTexture( int iTexture, RECT* prcTexture=0 );
    void SetFocusTexture( const string &FileName, RECT* prcTexture=0 );
    void SetFocusTexture( int iTexture, RECT* prcTexture=0 );
    
    // ��ӧO�����s�����k�䤬���C�Ҧp HUISMap �� IDC_BUTTON_MAP�CSee HUISMap::Create()�C
    bool SwitchMouseButton(bool bSwitch);

	void SetEventType(eEventType type){ m_eventType = type; }
	static void SetActionMouseAnimation(const char *name){ sm_actionMouseAnim = name; }
	static void SetNormalMouseAnimation(const char *name){ sm_normalMouseAnim = name; }

	inline bool GetPressed() const { return m_bPressed;}
protected:
    bool m_bPressed;
    bool m_bSwitchMouseButton;
    eEventType m_eventType;
protected:
    void DrawText(LPCWSTR szText, CDXUTElement* pElement, RECT* pRect, bool bShadow);
};


//-----------------------------------------------------------------------------
// CheckBox control
//-----------------------------------------------------------------------------
class CDXUTCheckBox : public CDXUTButton
{
public:
    CDXUTCheckBox( CDXUTDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey();

    virtual BOOL ContainsPoint( POINT pt ); 
    virtual void UpdateRects(); 

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    bool	GetChecked		();
    void	SetChecked		( bool bChecked );
	enum
	{
		STYLE_NORMAL,
		STYLE_BUTTON,
	};
    void	SetStyle		( int nStyle = STYLE_NORMAL);

	
protected:
    void SetCheckedInternal(bool bChecked, bool bFromInput);

    bool m_bChecked;
    RECT m_rcButton;
    RECT m_rcText;
	int m_nStyle;
};

//-----------------------------------------------------------------------------
// RadioButton control
//-----------------------------------------------------------------------------
class CDXUTRadioButton : public CDXUTCheckBox
{
public:
    CDXUTRadioButton( CDXUTDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey();

	void SelectRadioButton(bool bCheck, bool bClearGroup, bool bFromInput, bool bSendEvent = true);
    void SetChecked( bool bChecked, bool bClearGroup=true, bool bSendEvent = true );
    void SetButtonGroup( UINT nButtonGroup );
    UINT GetButtonGroup();
    
protected:
    void SetCheckedInternal(bool bChecked, bool bFromInput, bool bClearGroup, bool bSendEvent = true);
    UINT m_nButtonGroup;
};

#endif // __CDXUTButton_h__
