//
// CDXUTControl.h
//
#ifndef __CDXUTControl_h__
#define __CDXUTControl_h__
//#include "Element.h"
#include "CDXUTDef.h"
#include "CDXUTDragDrop.h"
#include "Tree.h"

class RenderComponent ;

//-----------------------------------------------------------------------------
// Base class for controls
//-----------------------------------------------------------------------------
class CDXUTDropTarget : public DragDropTarget
{
	CDXUTControl*	mpOwner;
public:
					CDXUTDropTarget		(CDXUTControl* pOwner);
					~CDXUTDropTarget	();
	virtual void	OnDrop				(DragDropInfo* pInfo) ;
	virtual void	OnMouseOver			(DragDropInfo* pInfo) ;
	virtual void	OnDropNotTarget		(DragDropInfo* pInfo) ;
};

typedef map< uint , CDXUTElement* > ElementMap ;

class ElementsQueryer
{
public :
	ElementsQueryer( CDXUTControl* pControl ) ;
	~ElementsQueryer() ;

	CDXUTElement* QueryElement( uint uAdress ) ;
	void          ClearElement( uint uAdress ) ;
	RECT GetControlBoundingBox() ;
	RECT GetControlScreenBoundingBox() ;
	inline CDXUTControl* GetControl() { return m_pControl ; }

private :
	
	void InitElement( CDXUTElement* pElement ) ;
	typedef multimap< uint , uint > QueryTable ;
	QueryTable m_Table ;

	uint m_uQueryNo ;
	CDXUTControl* m_pControl ;
	ElementMap* m_pElementMap ;
} ;

class CDXUTControl
{

public:
	
    bool IsS_RenderHint;

    CDXUTControl( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTControl();

    virtual HRESULT OnInit() { return S_OK; }
    virtual void Refresh();
	void         RenderControl( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) ;
    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) {}
    virtual void RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) {} // �e�X���󴣥�

    // Windows message handler
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) { return false; }

    virtual bool CanHaveFocus() { return false; }
    virtual void OnFocusIn() ;
    virtual void OnFocusOut() ;
	inline	bool IsFocus() const {return m_bHasFocus;}
	virtual void OnDrag() ;
	virtual void OnDrop() ;
    virtual void OnMouseEnter() ;
    virtual void OnMouseLeave() ;
    virtual void OnHotkey() {}

    virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }

    virtual void SetEnabled( bool bEnabled ) ;
    virtual bool GetEnabled() { return m_bEnabled; }
    virtual void SetVisible( bool bVisible ) ; 
    virtual bool GetVisible() { return m_bVisible; }

    UINT GetType() const { return m_Type; }

    int  GetID() const ;
    void SetID( int ID );

    //���� *** modify
    void PlaySoundWAV();

    void SetLocation( int x, int y ) { m_x = x; m_y = y; UpdateRects(); UpdateComponentRects(); }
    void GetLocation( int& x, int& y ) { x = m_x;  y = m_y; }

	void	SetAlign(int nAlign);
	inline int		GetAlign()	const {	return mnAlign ; };
    void SetSize( int width, int height ) { m_width = width; m_height = height; UpdateRects(); UpdateComponentRects(); }
    void GetSize( int& width, int& height ) { width = m_width; height = m_height; }
	inline void	SetBody(int x,int y,int w,int h) {m_x = x; m_y = y; m_width = w; m_height = h; UpdateRects();}
	//inline RECT GetRect() {return m_rcBoundingBox ; };
	inline RECT GetBoundingBox() {return m_rcBoundingBox ; };

    void SetHotkey( UINT nHotkey ) { m_nHotkey = nHotkey; }
    UINT GetHotkey() { return m_nHotkey; }

    void SetUserData( void *pUserData ) { m_pUserData = pUserData; }
    void *GetUserData() const { return m_pUserData; }

    virtual void SetTextColor( D3DCOLOR Color, UINT iElement = 0 );
    CDXUTElement* GetElement( UINT iElement ) ;
    HRESULT SetElement( UINT iElement, CDXUTElement* pElement );

    bool m_bVisible;                // Shown/hidden flag
    bool m_bMouseOver;              // Mouse pointer is above control
    
    bool m_bIsDefault;              // Is the default control

    // These members are set by the container
    CDXUTDialog* m_pDialog;    // Parent container
	inline const CDXUTDialog*	GetParent() const { return m_pDialog;}
	inline CDXUTDialog*	GetParent() { return m_pDialog;}
    UINT m_Index;              // Index within the control list
    
    //CGrowableArray< CDXUTElement* > m_Elements;  // All display elements
	inline uint GetState() { return m_uCurState ; }
	ElementMap m_Elements ;

    // ming add >> for drag
    static CDXUTControl* s_pDragSource; // �O���즲�ӷ�
    static IDirect3DTexture9* s_pDragTexture; // �O���즲���󪺧������
    static int s_SrcIndex; // �O���즲�ӷ���index
    static int s_DestIndex; // �O���즲�ت���index

    void SetTextShadow( bool bShadow ) { m_bShadow = bShadow; }
    void SetTexture( const WCHAR *FileName, RECT* prcTexture=0, int iElement=0 );
    void SetTexture( const string &FileName, RECT* prcTexture=0, int iElement=0 );
	

    void SetTexture( int iTexture, RECT* prcTexture, int iElement=0 );
	
    void SetFont( const string &strFaceName, LONG height, LONG weight=FW_NORMAL, int iElement=0 );
    void SetFont( LPCWSTR strFaceName, LONG height, LONG weight=FW_NORMAL, int iElement=0 );
    void SetTextFormat( DWORD dwTextFormat, int iElement=0 ); // �]�w�r����m(�m���B���k�B����...)
    void SetBoundingBoxWidth( const int &width ) { m_rcBoundingBox.right = m_rcBoundingBox.left+width; }
    void SetRBoundingBoxWidth( const int &width ) { m_rcBoundingBox.left = m_rcBoundingBox.right-width; }
	
	void RemoveFirst_SlashN( WCHAR *temp );

    HRESULT SetHint( LPCWSTR strHint, LPCWSTR strHintImgPath=L"\0" );
    HRESULT SetHint( const string &strHint, const string &strHintImgPath="\0" );

    HRESULT SetMultipleHint( LPCWSTR strHint, LPCWSTR strHintImgPath, int Index );
    HRESULT SetMultipleHint( const string &strHint, const string &strHintImgPath, int Index );

	// HINT��r�����Ѽ�       Darknes Add
	virtual void SetHint_WordMaxWidth( int iWidth ){ m_cxWordMaxExtent = iWidth; }			// HINT����r�̤j�e�ס]�w�]��25*6 PIXOL�^
	virtual void SetHintFrameColor( D3DCOLOR color ){ m_FrameColor = color; }				// HINT�~�ؽu���C��]�w�]�ȬO�¦���z���^
	virtual void SetHintTextColor( D3DCOLOR color ){ m_HintTextColor = color; }				// HINT���e�媺���C��]�w�]�OWHITE�^
	virtual void SetHintWordFormat( DWORD dwTextFormat ){ m_dwTextFormat = dwTextFormat; }	// HINT����r�榡�]�w�]��DT��CENTER�UDT��VCENTER�^
	virtual void SetHintTurnRight( bool bTrue = true ){ m_bHintTurnRight = bTrue; }			// HINT����ܦ�m��ܥk�W�]�w�]�O���W�^
			void SetHintBackgroundColor( D3DCOLOR color ){ m_BackgroundColor = color; }		// HINT���I�����C��]�w�]�O�¦�^
			void SetHintBackgroundAlpha( int iAlpha );										// HINT���I�����z���ס]�w�]�O180�^
			void SetMultipleHintBufferSize( int iHintBufferSize );							// Set MultipeHint Buffer Size
	
	LONG m_cxWordMaxExtent;
	D3DCOLOR m_FrameColor;
	D3DCOLOR m_HintTextColor;
	D3DCOLOR m_BackgroundColor;
	int		 m_iHintBackAlpha;
	DWORD m_dwTextFormat;
	bool m_bHintTurnRight;

	typedef struct tagMultipleHint
    {
        WCHAR m_strHint[MAX_PATH];
        WCHAR m_strHintImgPath[MAX_PATH];
    }sMultipleHint;

	std::vector<sMultipleHint> MultipleHint;
    
	inline static CDXUTControl* fnGetOnMouseControl() { return s_pOnMouseCtrl;}
	inline CDXUTDropTarget*		GetDropTarget()			{	return &mDDTarget ; }
	static void fnGetScreenOffset	(POINT& ptOut,const POINT& ptScreen,CDXUTControl* pCtrl);
	static void	fnGetScreenRect		(RECT& outrc,CDXUTControl* pCtrl);
	virtual void GetScreenOffset(POINT& ptOut,const POINT& ptScreen) ;

	RenderComponent* AddRenderComponent( uint uComponentType ) ;
	RenderComponent* GetRenderComponent( uint uIndex ) ;	
	void			 RemoveRenderComponent( uint uIndex ) ;
	void			 RemoveRenderComponent( RenderComponent* pRenderComponent ) ;
	void             ClearRenderComponent() ;
	
	
	typedef Tree::TNode<CDXUTControl*>	Node;
	enum
	{
		ALIGN_NONE,		
		ALIGN_CLIENT,				
		ALIGN_RIGHT,
		ALIGN_BOTTOM,
		ALIGN_LEFT,
		ALIGN_TOP,
		
	};
// 	struct Node : public BaseNode 
// 	{
// 		
// 		void				SetRect	(const GUI::Rect32& rc)
// 		{
// 			mrcCtrl = rc;			
// 			NeedUpdate();
// 		}
// 		const GUI::Rect32&	GetRect	() const 
// 		{
// 			if(mbRectOutOfData)
// 			{
// 				//Update();
// 			}
// 			return mrcCtrl;
// 		}
// 		
// 		void NeedUpdate()
// 		{
// 			mbRectOutOfData = true;
// 			ChildIterator itCur =	GetChildsIteratorBegin();
// 			ChildIterator itE	=	GetChildsIteratorEnd();
// 			for ( ; itCur != itE ; ++itCur)
// 			{
// 				Node* pChild = (Node*)*itCur;				
// 				pChild->NeedUpdate();
// 			}
// 		}
// 	private:
// 		GUI::Rect32	mrcCtrl;
// 		bool		mbRectOutOfData;	
// 	};

	void	SetParentNode	(Node*);
	Node*	GetParentNode	();

	void SetRenderPipeline(IDXUTRender *pRender) {m_pRender =  pRender ; } 
protected:
	Node*	mpNode;

	ElementsQueryer m_ElementsQueryer ;
	typedef vector< RenderComponent* > RenderComponentVector ;
	RenderComponentVector m_RenderComponents ;
	void UpdateComponentRects() ;

	bool m_bHasFocus;               // Control has input focus	
	static CDXUTControl*	s_pOnMouseCtrl;
	CDXUTDropTarget	mDDTarget;	
    virtual void UpdateRects();

    int  m_ID;                 // ID number
    DXUT_CONTROL_TYPE m_Type;  // Control type, set once in constructor
    UINT m_nHotkey;            // Virtual key code for this control's hotkey
    void *m_pUserData;         // Data associated with this control that is set by user.

    bool m_bEnabled;           // Enabled/disabled flag
    bool m_bShadow;            // ming add �O�_�ݭn�e�X�r�����v

    // Size, scale, and positioning members
    int m_x, m_y;
    int m_width, m_height;

    RECT m_rcBoundingBox;      // Rectangle defining the active region of the control

    POINT m_MousePoint;        // �O���ƹ���m

	int	mnAlign;

	void			_SetText			(const char16* text);
	const char16*	_GetText			()					const;		
	void			_SetHint			(const char16* text);
	const char16*	_GetHint			()					const;		

	void			_SetHintImagePath	(const char16* text);
	const char16*	_GetHintImagePath	()					const;		
	IDXUTRender		*m_pRender ; 

	uint			m_uCurState ; //Control�ثe���A
	
private:
    WCHAR* m_strText;      // Window text
    WCHAR* m_strHint;      // Window text (����Hint)
    WCHAR* m_strHintImgPath;
	
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*                                   
²�檺dialog�H�X����
�u�B�zset location �Mset size���B�z
dialog�����T���B�z�Mø�s�����٬O�z�Luimanager�޲z
*/
/************************************************************************/


class CDXUTSubDialog : public CDXUTControl
{
	DialogHandle	mhHnd;
	bool			mbNeedDelete;

	
	void			_ResetLocation	();
public:
					CDXUTSubDialog	(CDXUTDialog* pDlg);
					~CDXUTSubDialog	();
	virtual bool	HandleMouse		( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

	virtual void	OnMouseEnter	() ;
	virtual void	Render			( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) ;
	void			SetDialog		(CDXUTDialog* pDlg , bool bAutoDelete = false);
	CDXUTDialog*	GetDialog		() const;
	virtual void	SetVisible		( bool bVisible ) ;
	void			Raise			(bool bSet = true);

	virtual void	UpdateRects		();
};
																							
	


#endif // __CDXUTControl_h__
