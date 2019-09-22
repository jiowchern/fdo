#ifndef __CDXUTDialog_h_
#define __CDXUTDialog_h_

#include "CDXUTDef.h"
#include "Common.h"
#include "GameDatabase.h"
#include "CDXUTDragDrop.h"
class CSmoothScrollingListBox;
class CVariableHeightListBox;
class CTextObject;
class CLineObject;
class CTalkBarImageControl;

/************************************************************************/
/* 
	�s��callback 
*/
/************************************************************************/
class CDXUTDialog;
class CDXUTSubDialog;
namespace UIEvent
{
	

	typedef CDXUTDialog*		UIControlPtr;
	enum
	{
		CONTINUED,
		DONE,
		
	};
	//////////////////////////////////////////////////////////////////////////
	struct EventData
	{
		uint			nCtrlId;		// �QĲ�o��Control
		uint			nEventId;		// �ƥ�ID
		UIControlPtr	pUI;			// Control��Dialog������ -> CDXUTDialog
		uint			nParam;			// �ϥΪ̦۩w�q�Ѽ�
		uint			nEventData;		// Control �w�q�Ѽ�
	};
	
	//////////////////////////////////////////////////////////////////////////

	class FunctionSlot
	{
	public:
		virtual int operator() (const EventData& data) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	class MemeberFunctionSlot : public FunctionSlot
	{		
		typedef int(T::*PFN_)(const EventData& data);

		PFN_	mpFn;
		T*		mpObj;
	public:
		MemeberFunctionSlot(T* obj,PFN_ func ) : mpFn(func), mpObj(obj)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		~MemeberFunctionSlot()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		int operator() (const EventData& data) 
		{
			return (mpObj->*mpFn)(data);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	class StaticFunctionSlot : public FunctionSlot
	{
	private:
		T* mpFn;
	public:
		StaticFunctionSlot(T* functor) :
		  mpFn(functor)
		  {}
		  //////////////////////////////////////////////////////////////////////////
		  ~StaticFunctionSlot()
		  {

		  }
		  //////////////////////////////////////////////////////////////////////////
		  int operator() (const EventData& data) 
		  {
			  return (*mpFn)(data);
		  }
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	struct EventKey
	{		
		union
		{
			uint64	nKey;
			struct
			{
				uint	nId;
				uint	nEvent;
			};			
		};
		bool operator == (const EventKey& k) const 
		{
			return k.nKey == nKey;
		}

		bool operator != (const EventKey& k) const 
		{
			return k.nKey != nKey;
		}

		bool operator < (const EventKey& k) const 
		{
			return  nKey < k.nKey;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct EventInfo
	{
		uint			nData;
		FunctionSlot*	pFncSlot;
	};

	//////////////////////////////////////////////////////////////////////////
	typedef std::multimap<EventKey,EventInfo> EventMap;

	class EventManager : public EventMap
	{
		UIControlPtr	mpUI;
	public:
					EventManager		(UIControlPtr pUI);
			virtual	~EventManager		();
		
		template<typename T>
		bool	SetListener		(uint nId,uint nEvent,T* pObj,int (T::*function)(const EventData& data),uint nData)
		{
			bool bNoChange = true;
			EventKey key;
			key.nId = nId;
			key.nEvent = nEvent;
// 			EventMap::iterator it = EventMap::find(key);
// 			if(it != EventMap::end())
// 			{
// 				delete it->second.pFncSlot;
// 				EventMap::erase(it);
// 				bNoChange = false;
// 			}

			EventInfo inf;
			inf.nData = nData;
			inf.pFncSlot = new MemeberFunctionSlot<T>(pObj,function);
			EventMap::insert( make_pair(key,inf) );
			return bNoChange;
		}
		//////////////////////////////////////////////////////////////////////////
		bool	SetListener		(uint nId,uint nEvent,int (*function)(const EventData& data),uint nData)
		{
			bool bNoChange = true;
			EventKey key;
			key.nId = nId;
			key.nEvent = nEvent;
// 			EventMap::iterator it = EventMap::find(key);
// 			if(it != EventMap::end())
// 			{
// 				delete it->second.pFncSlot;
// 				EventMap::erase(it);
// 				bNoChange = false;
// 			}

			EventInfo inf;
			inf.nData = nData;
			inf.pFncSlot = new StaticFunctionSlot<int (const EventData& data)>(function);
			EventMap::insert( make_pair(key,inf) );
			return bNoChange;
		}

		void	RiseListener		(uint nId,uint nEvent);
		void	ReleaseAllListener	();
		uint	SendToListener		(uint nId,uint nEvent,uint nData);
	};
};
typedef UIEvent::EventManager	UIEventManager;
typedef UIEvent::EventKey		UIEventKey;
typedef UIEvent::EventInfo		UIEventInfo;
typedef UIEvent::EventData		UIEventData;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CDXUTDialogDropTarget : public DragDropTarget
{
	CDXUTDialog*	mpOwner;
public:
	CDXUTDialogDropTarget	(CDXUTDialog* pOwner);
	~CDXUTDialogDropTarget	();
	virtual void	OnDrop				(DragDropInfo* pInfo) ;
	virtual void	OnMouseOver			(DragDropInfo* pInfo) ;
	virtual void	OnDropNotTarget		(DragDropInfo* pInfo) ;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Contains all the display information for a given control type
//-----------------------------------------------------------------------------
struct DXUTElementHolder
{
    UINT nControlType;
    UINT iElement;
    CDXUTElement Element;
};
//////////////////////////////////////////////////////////////////////////
struct DialogEventUserContext
{
	CDXUTDialog*	pDlg;
	UINT			nEventData;
	uint			nRet;
};

//////////////////////////////////////////////////////////////////////////
//��@dialog hook����
//////////////////////////////////////////////////////////////////////////
//template<typename TObject,typename TType,int noffset>
struct SHookHandleData
{
public:
	SHookHandleData			();
	SHookHandleData			(DialogHandle ownHnd, DialogHandle hookHnd);
	~SHookHandleData		();
	bool	operator ==		(const SHookHandleData& HookData) const;

	DialogHandle	mOwnHnd;	//a
	DialogHandle	mHookHnd;	//�Q��w������
};

struct FindFuntor
{
public:
	FindFuntor(DialogHandle hnd);
	bool operator()(SHookHandleData& test) const; 
	
private:
	DialogHandle mHnd;
};


typedef std::list<SHookHandleData> DialogHookList;
typedef std::list<SHookHandleData>::iterator HookIter;

class HandleHookManager
{
private:
	DialogHookList m_DialogHookList;
	
public:	
	HandleHookManager					();
	~HandleHookManager					();
	void			SetHook				(DialogHandle self, DialogHandle hook);
	void			SetHookFromRoot		(DialogHandle self, DialogHandle hnd);
	DialogHandle	CheckHook			(DialogHandle self);
			
	void 			ClearOneHook		(const SHookHandleData& hook);
	void 			ClearSpecifyHook	(DialogHandle hnd);
	// 	CDXUTDialog* GetOwnHook(DialogHandle hnd);//���o�ۤv����w��H
};

//-----------------------------------------------------------------------------
// All controls must be assigned to a dialog, which handles
// input and rendering for the controls.
//-----------------------------------------------------------------------------
namespace FDO
{
	class CRenderer ; 
};
bool fnIsSubDialogType(uint nType);

class CDXUTDialog : public UIEventManager
{
private:
	std::wstring	m_DebugLayoutPath;
	static FDO::CRenderer			*m_pRender ; 
public:
	static void SetRenderer(FDO::CRenderer *pR) { m_pRender = pR ; }
	static FDO::CRenderer* GetRenderer() { return m_pRender ; } 
	inline void CDXUTDialog::SetBackgroundColor(D3DXCOLOR color) { m_BackgroundColor = color; }
public:

    CDXUTDialog();
    CDXUTDialog(list<CDXUTDialog*> *pDialogList);
    virtual ~CDXUTDialog();
    
    int m_iDialogType;

    // Need to call this now
    void Init( CDXUTDialogResourceManager* pManager );

	inline	void	SetDebugLayoutPath(const char16* path) { m_DebugLayoutPath = path ;}
	void JoinDialogList(list<CDXUTDialog*> *pDialogList);
    // Windows message handler
    virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void AddRenderPipeline(IDXUTRender *pPipe) { m_pPipeVec.push_back(pPipe) ; } 
	void ClearRenderPipeline() { m_pPipeVec.clear() ; }
    // Control creation
    HRESULT AddStatic( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, CDXUTStatic** ppCreated=NULL );
    HRESULT AddButton( int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey=0, bool bIsDefault=false, CDXUTButton** ppCreated=NULL );
    HRESULT AddButton( int ID, string strText, int x, int y, int width, int height, UINT nHotkey=0, bool bIsDefault=false, CDXUTButton** ppCreated=NULL ); // ming add
    HRESULT AddCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CDXUTCheckBox** ppCreated=NULL );
    HRESULT AddRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CDXUTRadioButton** ppCreated=NULL );
    HRESULT AddRadioButton( int ID, UINT nButtonGroup, string strText, int x, int y, int width, int height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CDXUTRadioButton** ppCreated=NULL ); // ming add
    HRESULT AddComboBox( int ID, int x, int y, int width, int height, UINT nHotKey=0, bool bIsDefault=false, CDXUTComboBox** ppCreated=NULL );
    HRESULT AddSlider( int ID, int x, int y, int width, int height, int min=0, int max=100, int value=50, bool bIsDefault=false, CDXUTSlider** ppCreated=NULL );
    HRESULT AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, CDXUTEditBox** ppCreated=NULL );
    HRESULT AddEditBox( int ID, string strText, int x, int y, int width, int height, bool bIsDefault=false, CDXUTEditBox** ppCreated=NULL ); // ming add
    HRESULT AddIMEEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, CDXUTIMEEditBox** ppCreated=NULL );
    HRESULT AddListBox( int ID, int x, int y, int width, int height, DWORD dwStyle=0, CDXUTListBox** ppCreated=NULL );
    HRESULT AddVariableHeightListBox( int ID, int x, int y, int width, int height, DWORD dwStyle=0, CVariableHeightListBox** ppCreated=NULL );
    HRESULT AddSmoothScrollingListBox( int ID, int x, int y, int width, int height, DWORD dwStyle=0, CSmoothScrollingListBox** ppCreated=NULL );
    // ming add >> �W�[�ۭq����
    HRESULT AddBtnListBox( int ID, int x, int y, int width, int height, DWORD dwStyle=0, moCDXUTBtnListBox** ppCreated=NULL );
    HRESULT AddImageListBox( int ID, int x, int y, int width, int height, DWORD dwStyle=0, CDXUTImageListBox** ppCreated=NULL ); // �]�t�ϧ�ListBox
    HRESULT AddBtnImageListBox( int ID, int x, int y, int width, int height, DWORD dwStyle=0, CDXUTBtnImageListBox** ppCreated=NULL );
    HRESULT AddImage( int ID, int x, int y, int width, int height, bool bIsDefault=false, CDXUTImage** ppCreated=NULL ); //
    HRESULT AddIconImage( int ID, int x, int y, int width, int height, bool bIsDefault=false, CDXUTIconImage** ppCreated=NULL );
    HRESULT AddIconImageBox( int ID, int x, int y, int width, int height, bool bIsDefault=false, CDXUTIconImageBox** ppCreated=NULL );
    HRESULT AddTalkBarImageControl( int ID, int x, int y, int width, int height, bool bIsDefault=false, CTalkBarImageControl** ppCreated=NULL ); //
    HRESULT AddImageBox( int ID, int x, int y, int width, int height, bool bIsDefault=false, CDXUTImageBox** ppCreated=NULL ); // �i�ΨӸ�Image��Box
    HRESULT AddImageBoxList( int ID, int x, int y, int numCellsPerRow, int numCellsPerCol, CDXUTImageBoxList** ppCreated=NULL ); // �u��ImageBox��ListBox
    HRESULT AddIconListBox( int ID, int x, int y, int Row, int Col, CDXUTIconListBox** ppCreated=NULL );
	HRESULT AddIconListBoxVN( int ID, int x, int y, int Row, int Col, CDXUTIconListBox** ppCreated=NULL );		// �o�ӬO�V�n�o��Ϊ�
    HRESULT AddProgressBar( int ID, int x, int y, int width, int height, bool bIsDefault=false, CDXUTProgressBar** ppCreated=NULL ); //
    HRESULT AddSpecialButton( int ID, int x, int y, int width, int height, UINT nHotkey=0, bool bIsDefault=false, CDXUTSpecialButton** ppCreated=NULL );
    HRESULT AddMapControl( int ID, int x, int y, int width, int height, UINT nHotkey=0, bool bIsDefault=false, CMapControl** ppCreated=NULL );//�W�[�p�a�ϱ���(cosiann's edit)
    HRESULT AddPuzzle( int ID, int x, int y, int cellwidth, int cellheight, UINT nHotkey=0, bool bIsDefault=false, CDXUTPuzzle** ppCreated=NULL );
	HRESULT AddTabPage( int ID, int x, int y, int tabwidth, int tabheight, UINT nHotkey=0, bool bIsDefault=false, CDXUTTabPage** ppCreated=NULL );
	// ming add <<
    HRESULT AddControl( CDXUTControl* pControl , int ID , bool bInitControl /*= true*/);
    HRESULT InitControl( CDXUTControl* pControl );
    HRESULT AddControl(CDXUTControl* pControl, int ID, int x, int y, int width, int height);
    HRESULT AddControl(CDXUTControl* pControl, int ID, const RECT& rcControl);
    // Levian Add
    HRESULT SetButtonText( int ID, LPCWSTR strText );

    // Control retrieval
    CDXUTStatic*        GetStatic( int ID ) { return (CDXUTStatic*) GetControl( ID, DXUT_CONTROL_STATIC ); }
    CDXUTButton*        GetButton( int ID ) { return (CDXUTButton*) GetControl( ID, DXUT_CONTROL_BUTTON ); }
    CDXUTCheckBox*      GetCheckBox( int ID ) { return (CDXUTCheckBox*) GetControl( ID, DXUT_CONTROL_CHECKBOX ); }
    CDXUTRadioButton*   GetRadioButton( int ID ) { return (CDXUTRadioButton*) GetControl( ID, DXUT_CONTROL_RADIOBUTTON ); }
    CDXUTComboBox*      GetComboBox( int ID ) { return (CDXUTComboBox*) GetControl( ID, DXUT_CONTROL_COMBOBOX ); }
    CDXUTSlider*        GetSlider( int ID ) { return (CDXUTSlider*) GetControl( ID, DXUT_CONTROL_SLIDER ); }
    CDXUTEditBox*       GetEditBox( int ID ) { return (CDXUTEditBox*) GetControl( ID, DXUT_CONTROL_EDITBOX ); }
    CDXUTIMEEditBox*    GetIMEEditBox( int ID ) { return (CDXUTIMEEditBox*) GetControl( ID, DXUT_CONTROL_IMEEDITBOX ); }
    CDXUTListBox*       GetListBox( int ID ) { return (CDXUTListBox*) GetControl( ID, DXUT_CONTROL_LISTBOX ); }
	CVariableHeightListBox*	GetVHListBox( int ID ) { return (CVariableHeightListBox*) GetControl( ID, DXUT_CONTROL_LISTBOX ); }
    // ming add >> ���o�ۭq����
    CDXUTImageListBox*  GetImageListBox( int ID ) { return (CDXUTImageListBox*)GetControl( ID, DXUT_CONTROL_LISTBOX ); }
    CDXUTIconListBox*   GetIconListBox( int ID ) { return (CDXUTIconListBox*)GetControl( ID, DXUT_CONTROL_LISTBOX ); }
    CDXUTImage*         GetImage( int ID ) { return (CDXUTImage*)GetControl( ID, DXUT_CONTROL_IMAGE ); }
    CDXUTIconImage*     GetIconImage( int ID ) { return (CDXUTIconImage*)GetControl( ID, DXUT_CONTROL_IMAGE ); }
    CDXUTIconImageBox*  GetIconImageBox( int ID ) { return (CDXUTIconImageBox*)GetControl( ID, DXUT_CONTROL_IMAGEBOX ); }
    CDXUTImageBox*      GetImageBox( int ID ) { return (CDXUTImageBox*)GetControl( ID, DXUT_CONTROL_IMAGEBOX ); }
    CDXUTProgressBar*   GetProgressBar( int ID ) { return (CDXUTProgressBar*)GetControl( ID, DXUT_CONTROL_PROGRESSBAR ); }
    CDXUTSpecialButton* GetSpecialButton( int ID ) { return (CDXUTSpecialButton*) GetControl( ID, DXUT_CONTROL_SPECIALBUTTON ); }
	CDXUTTabPage*       GetTabPage( int ID ) { return (CDXUTTabPage*)GetControl( ID , DXUT_CONTROL_TABPAGE ) ; }
	CMapControl*		GetMapControl(int ID) { return (CMapControl*) GetControl( ID , DXUT_CONTROL_MAPCONTROL ); }
	CDXUTPuzzle*        GetPuzzle( int ID ) { return ( CDXUTPuzzle* ) GetControl( ID , DXUT_CONTROL_PUZZLE ) ; }
    // ming add <<

    CDXUTControl* GetControl( int ID );
    CDXUTControl* GetControl( int ID, UINT nControlType );
    CDXUTControl* GetControlAtPoint( POINT pt );
	static CDXUTControl* GetSubDialogControl( int iSubDlgID , int iCtrlID , CDXUTDialog* pDialog ) ;
	CDXUTControl* GetSubDialogControl( int iSubDlgID1 , int iSubDlgID2 , ... ) ;
	static void BeginDragDrop();
	static void EndDragDrop();

    bool GetControlEnabled( int ID );
    void SetControlEnabled( int ID, bool bEnabled );

    void DeleteRadioButtonGroup( UINT nGroup );
    void ClearRadioButtonGroup( UINT nGroup );
    void ClearComboBox( int ID );

    // Access the default display Elements used when adding new controls
    HRESULT       SetDefaultElement( UINT nControlType, UINT iElement, CDXUTElement* pElement );
    CDXUTElement* GetDefaultElement( UINT nControlType, UINT iElement );

    // Methods called by controls
	uint SendEvent( UINT nEvent, bool bTriggeredByUser, CDXUTControl* pControl ,UINT nData = 0);
    static void RequestFocus(CDXUTControl* pControl);

	static void	EnableDebugInfo(bool bEnable);
	static bool	GetEnableDebugInfo();
    virtual void RequestDefaultFocus() ;

    // Render helpers
    HRESULT DrawRect(const RECT* pRect, D3DCOLOR color);
    HRESULT DrawFrameRect(const RECT* pRect, D3DCOLOR color );
    HRESULT DrawFineRect(const RECT* pRect, D3DCOLOR color );
    HRESULT DrawPolyLine(POINT* apPoints, UINT nNumPoints, D3DCOLOR color);
    HRESULT DrawSprite(CDXUTElement* pElement, RECT* prcDest, LPDIRECT3DTEXTURE9 pTexture = 0);
	HRESULT DrawSpriteScale(CDXUTElement* pElement, const RECT& rcDest );
    HRESULT CalcTextRect(LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, int nCount = -1);

	static INT CalcTextWidth(LPCWSTR strText, CDXUTElement* pElement);

    // DrawText methods
    HRESULT DrawPlainText(LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, bool bShadow = false, int nCount = -1);
	HRESULT DrawPlainText(LPCWSTR strText, CDXUTElement* pElement, const RECT* prcDest, bool bShadow = false, int nCount = -1);
    HRESULT DrawTextObject(CTextObject* pTextObject, CDXUTElement* pElement, RECT* prcDest, bool bShadow = false);
    HRESULT DrawLineObject(CLineObject* pLineObject, CDXUTElement* pElement, RECT* prcDest, bool bShadow = false);

    // Attributes
    bool GetVisible() const { return m_bVisible; }
    void SetVisible( bool bVisible );
    bool GetMinimized() { return m_bMinimized; }
    void SetMinimized( bool bMinimized ) { m_bMinimized = bMinimized; }
    void SetBackgroundColors( D3DCOLOR colorAllCorners ) { SetBackgroundColors( colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners ); }
    void SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight );
	void NOChangeColorAndAlpha( bool bVisible ) { m_bNOChColorAndAlpha = bVisible; }// Darkness add22
	void EnableCanChColor( bool bEnable ){ m_bCanChColor = bEnable; }           // Darkness add
    void EnableUseSideFrame( bool bEnable, int iFrameGauge=5 );                 // Darkness add
    void EnableAptoticSize( bool bEnable, int iWidth=0, int iHeight=0 );        // Darkness add
    void EnableSizeChange( bool bEnable, int iSideGauge=5 , int iChSzMode=0 );  // Darkness add
    void EnableDragToMove(bool bEnable);
    void EnableCaption( bool bEnable, RECT* prcCaption=0, bool bDrawCap=false );
	void Set9Bar(  int iW = 0, int iH = 0 );				// Darkness add
    int  GetCaptionHeight() const ;
    void GetCaptionRect(RECT& rcOut) const;
    void SetMaxSize( int iMaxWidth=0, int iMaxHeight=0 );                       // Darkness add
    void SetMinSize( int iMinWidth=0, int iMinHeight=0 );                       // Darkness add
    void SetCaptionText( const WCHAR *pwszText, RECT* prcCapText=0 ); // { StringCchCopy( m_wszCaption, sizeof(m_wszCaption)/sizeof(m_wszCaption[0]), pwszText); } ming mark
    void SetCaptionText( const string &szText, RECT* prcCapText=0 ); // ming add
	void SetCaptionTextColor( DWORD dwColor ); // By Levian
    void GetLocation( POINT &Pt ) const { Pt.x = m_x; Pt.y = m_y; }
    // ming edit >>
    void SetLocation( int x, int y, bool bAutoCorrect=true );
    void SetSize( int width, int height );
    // ming edit <<
    void MiniChange( HWND hWnd );  // Darkness add

    int  GetWidth() { return m_width; }
    int  GetHeight() { return m_height; }
    void GetRect(RECT& rcOut);

    void SetNextDialog( CDXUTDialog* pNextDialog );

    static void SetRefreshTime( float fTime ){ s_fTimeRefresh = fTime; }

    static CDXUTControl* GetNextControl( CDXUTControl* pControl );
    static CDXUTControl* GetPrevControl( CDXUTControl* pControl );

    bool RemoveControl( CDXUTControl* pControl ); // ming add
    void RemoveControl( int ID );
    void RemoveAllControls();
	

    // Sets the callback used to notify the app of control events
    void SetCallback( PCALLBACKDXUTGUIEVENT pCallback, void* pUserContext = NULL );
    void EnableNonUserEvents( bool bEnable ) { m_bNonUserEvents = bEnable; }
    void EnableKeyboardInput( bool bEnable ) { m_bKeyboardInput = bEnable; }
    void EnableMouseInput( bool bEnable ) { m_bMouseInput = bEnable; }

    // Device state notification
    void Refresh();
    virtual HRESULT OnRender(float fElapsedTime);
    virtual void RenderCore(float fElapsedTime);

    // Shared resource access. Indexed fonts and textures are shared among
    // all the controls.
    HRESULT       SetFont( UINT index, string strFaceName, LONG height, LONG weight );
    HRESULT       SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight );
    static DXUTFontNode* GetFont( UINT index );

    HRESULT          SetTexture( UINT index, LPCWSTR strFilename );
    DXUTTextureNode* GetTexture( UINT index );

    static CDXUTDialogResourceManager* GetManager() { return s_pManager; }
	static void							SetManager(CDXUTDialogResourceManager* s) { s_pManager = s; }

    static void ClearFocus();
    void FocusDefaultControl();

    bool m_bNonUserEvents;
    bool m_bKeyboardInput;
    bool m_bMouseInput;
  
    // �ثe�ƹ����쪺 Dialog
    static CDXUTDialog* MouseDialog() { return s_pMouseDialog; }
    // �ثe�ƹ����쪺 Dialog
    static CDXUTDialog* FocusDialog() { return s_pFocusDialog; }

    bool ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt )!=0; }
    void UpdateRects();
    void OnMouseEnter();
    void OnMouseLeave();
    
    // Dialog focus
    void OnDialogFocusOut();
    void OnDialogFocusIn();

    bool SetBackgroundTexture( const string &FileName, RECT* prcTexture=0, RECT* prcDraw=0 );
    bool SetBackgroundTexture2( int iNum, const string &FileName, RECT* prcTexture=0, RECT* prcDraw=0 );        // Darkness add ��ʳ]�w�K�ӥ~������ *:iNum:0�G����������, 1�G���W, 2�G�W��, 3�G�k�W, 4�G�k��, 5�G�k�U, 6�G�U��, 7�G���U, 8�G����, 9:CAPTION�U��������b
    bool SetBackgroundTextureAuto9( const string &FileName, RECT* prcTexture=0, int iGauge=0, RECT* prcDraw=0 );// Darkness add �۰ʳ]�w�K�ӥ~������ �]�d�򪽱�����Ӥj����RECT,���O���ϧ����CAPTION�U�������n�t�~�]�w�^�^
    bool SetBackgroundTexture( LPCWSTR FileName, RECT* prcTexture=0, RECT* prcDraw=0 );
    bool SetBackgroundTexture2( int iNum, LPCWSTR FileName, RECT* prcTexture=0, RECT* prcDraw=0 );              // Darkness add
    bool SetBackgroundTextureAuto9( LPCWSTR FileName, RECT* prcTexture=0, int iGauge=0, RECT* prcDraw=0 );        // Darkness add
    bool push_front();
    bool push_back();
    virtual void Show();
    virtual void Hide();
    void Move3Btn();                                                        // Darkness add
    void SetMinimizeBtuttonAuto();                                          // Darkness add
    void SetOriginBtuttonAuto();                                            // Darkness add
    void SetCloseBtuttonAuto();                                             // Darkness add
    void SetOriginalSize( int x, int y, int Width, int Height );            // Darkness add
    void SetOriginBtutton( int x=-1, int y=-1, int width=0, int height=0 ); // Darkness add
    void SetCloseBtutton( int x=-1, int y=-1, int width=0, int height=0 );
    void SetMinimizeBtutton( int x=-1, int y=-1, int width=0, int height=0 );
    void SetMinBtuttonTexture( const string &strNormal, const string &strFocus, RECT* prcNormal=0, RECT* prcFocus=0 );// Darkness add
    void SetOrigBtuttonTexture( const string &strNormal, const string &strFocus, RECT* prcNormal=0, RECT* prcFocus=0 );// Darkness add
    void SetCloseBtuttonTexture( const string &strNormal, const string &strFocus, RECT* prcNormal=0, RECT* prcFocus=0 );
	void SetMinBtnVisible( bool bVisible);		// Darkness add
	void SetOriBtnVisible( bool bVisible);		// Darkness add
	void SetClsBtnVisible( bool bVisible);		// Darkness add
	void SetColors( D3DCOLOR color );
    void SetColors_RGBA( int Red, int Green, int Blue, int Alpha=214 );		// Darkness add
    void SetAlpha(int Alpha );												// Darkness add
    int  GetAlpha(){ return m_ColorAlpha; }									// Darkness add
    void SetAlwaysFocus( bool bAlwaysFocus ) { m_bAlwaysFocus = bAlwaysFocus; }
	void SetControlOrder(bool bSet);//
    bool GetAlwaysFocus() { return m_bAlwaysFocus; }
	void AdjLayout();
	static void fnAdjLayout	(CDXUTDialog* pDlg,RECT& tmp);
    // ming add <<
    
    void SetPressedControl(CDXUTControl* pControl);
    
    // For temporarily hiding all UI for PrintScreen.
    // See UIManager::SaveVisibility(), UIManager::RestoreVisibility().
    void SaveVisibility();
    bool RestoreVisibility();
    
    void ClientToScreen(RECT* prcDest);
    
    bool IsMouseOver() { return m_bMouseOver; }
    DXUT_CONTROL_STATE GetCurrentState();
    
    const CDXUTElement& GetDialogElement() { return m_DiaElement; }

	void SeparateDialogList(); //����List ,�N���o���QManager�x��
	
	static CDXUTDialog* fnGetOnMouseDialog();
	
	inline DialogHandle	GetHandle		() const	{ return mHnd; }
	
	static HandleHookManager& GetHookMgr		()			{ return m_HookMgr; }
	
	bool		Raise					(bool bSet = true);
	void		SetAllControlEnable		(bool bEnable);
	
	inline CDXUTDialogDropTarget*		GetDropTarget()			{	return &mCDXUTDialogDropTarget ; }

protected:
	DialogHandle				mHnd;	//�޲z��
	static HandleHookManager	m_HookMgr;	
	DialogEventUserContext		mDEUC;
	typedef std::list<uint>		LockControlSet;
	LockControlSet				mLockControls;

	CDXUTDialogDropTarget		mCDXUTDialogDropTarget;

    bool m_bMouseOver; // ming add >> Mouse pointer is above control    

    static CDXUTDialog* s_pDefFocusDialog; // �w�]���J�I Dialog
    static CDXUTDialog* s_pFocusDialog;    // �ثe�J�I�� Dialog
    static CDXUTDialog* s_pMouseDialog;    // �ثe�ƹ����쪺 Dialog
	static void fnDisablePublicState(CDXUTDialog* pDlg);
    CDXUTElement m_CapElement;  // Element for the caption
    CDXUTElement m_DiaElement;  // Element for the dialog - ming add
    CDXUTElement m_SidElement[10];  // Element for the side   Darkness add

	D3DXCOLOR m_BackgroundColor;

    int m_dxDrag;
    int m_dyDrag;

	
    void Initialize();    

    // RenderCore() = RenerBackground() + RenderControls()
    void RenderBackground(float fElapsedTime);
    void RenderBackground2(float fElapsedTime);     // Darkness add
    void RenderControls(float fElapsedTime);

public:
	void RenderHintAll( int iWord_Max_Width, D3DCOLOR TextColor, D3DCOLOR FrameColor, D3DCOLOR BackgroundColor, CDXUTElement* pElement, POINT &MousePoint, const WCHAR* strHint, const WCHAR* strHintImgPath, bool bHintTurnRight );
	void RemoveFirst_SlashN( WCHAR *temp );

	static void ShowHint(bool bTrue){ sm_bShowHint = bTrue;}
	static bool IsShowHint(){ return sm_bShowHint; }
	void        SetHintAlpha( int Alpha ){ m_iALLHintAlpha = Alpha; }

	inline void SetID	(int nId) { m_ID = nId;};
	inline int GetID	() const { return m_ID ;};

	inline	void					SetParent	(CDXUTSubDialog* pCtrl) {m_pParent = pCtrl ;};
	inline	const CDXUTSubDialog*	GetParent	()	const {	return m_pParent;};
	inline	CDXUTSubDialog*	GetParent	()	{	return m_pParent;};
private:
    // Caption TextObject
    CTextObject* m_pTextObjectCaption;
    void DrawCaption();

	std::vector<IDXUTRender*> m_pPipeVec ; 

    // Visibility stack
    std::list<bool> m_listVis;
    void PushVis(bool bVal);
    bool PopVis(bool& bValOut);

    int m_nDefaultControlID;

    static double s_fTimeRefresh;
	static bool s_bEnableDebugInfo;
    double m_fTimeLastRefresh;

    // Initialize default Elements
    void InitDefaultElements_FdoStyle();
    void InitDefaultElements_HvoStyle();
	
    // Windows message handlers
    void OnMouseMove( POINT pt );
    void OnMouseUp( POINT pt );
    bool MouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool CaptionMouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool SideMouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);     // Darkness add
    bool ClientMouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool CheckCanPush	(UINT uMsg);
    bool DragToMove(const RECT& rcDrag, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool KeyboardProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool HandleMouseOutside(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool HandleMouseInside(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool HandleMouseOnDialog(UINT uMsg, POINT mousePoint, WPARAM wParam, LPARAM lParam);
    bool HandleMouseOnControls(UINT uMsg, POINT mousePoint, WPARAM wParam, LPARAM lParam);
    void MouseArea( WPARAM wParam, LPARAM lParam );
    void AptoticChangeSize( HWND hWnd );
    void DragToChnageSize( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    
    // Control events
    bool OnCycleFocus( bool bForward );

    static CDXUTControl* s_pControlFocus;        // The control which has focus
    static CDXUTControl* s_pControlPressed;      // The control currently pressed	

    CDXUTControl*	m_pControlMouseOver;           // The control which is hovered over
	CDXUTSubDialog* m_pParent;					

    bool m_bVisible;
    bool m_bCaption;
    bool m_bMinimized;
    bool m_bCanMinimized;
    bool m_bDrawCap;
    bool m_bDragToMove;
    bool m_bDragging;
    bool m_bCanSizeChange;                      // �O�_��size change���}��
    bool m_bSizeChange;                         // ���b����Size change
    bool m_bUseSideFrame;                       // �O�_�ϥΥ~��
    bool m_bAptoticSize;                        // �T�w�ܧ�j�p
    bool m_bCanChColor;                         // �i�H�ܧ��C��z���ת������}�� ���O�b�V�n�S���ϥ� �ҥH�����\��S���~�򧹦�
	bool m_bNOChColorAndAlpha;					// �������i����ALPHA�Ȥ��C��
	bool m_bControlOrder;						// ���󶶧� true = size -> 0 false = 0 -> size

    WCHAR m_wszCaption[256];

	int	m_ID;
    int m_x;
    int m_y;
    int m_x2;
    int m_y2;
    int m_x_r;                                  // Dialog���Ux�y��
    int m_y_b;                                  // Dialog���Uy�y��
    int m_width;
    int m_height;
    int m_width2;
    int m_height2;
    int m_iSubX;
    int m_iSubY;
    int m_nCaptionHeight;
    int m_iFrameGauge;                          // ø�s���~�ثp��
    int m_iArea;                                // �ƹ��Ҧb�d��
    int m_SideGauge;                            // �i�P�_�즲�d����ثp��
    int m_iMaxWidth;                            // ��j�e�׳̤j����
    int m_iMaxHeight;                           // ��j���׳̤j����
    int m_iMinWidth;                            // �Y�p�e�׳̤p����
    int m_iMinHeight;                           // �Y�p���׳̤p����
    int m_iOriginWidth;                         // ������l�e��
    int m_iOriginHeight;                        // ������l����
    int m_iOriginX;                             // ������l��mX
    int m_iOriginY;                             // ������l��mY
    int m_iChangeSizeMode;                      // �Ҧ��G0.�K��V���i�즲  1.�|�ӹ﨤�i�즲  2.�|����i�즲  3.�k,�k�U,�U�i�즲  4.�U�i�즲
    int m_ColorRed;
    int m_ColorGreen;
    int m_ColorBlue;
    int m_ColorAlpha;
	int m_i9Bar_L;								// 9���ؽu�����t(�PX���t)
	int m_i9Bar_T;								// 9���ؽu���W�t
	int m_i9Bar_R;								// 9���ؽu���k�t(�Pm_iwidth���t)
	int m_i9Bar_B;								// 9���ؽu���U�t
	bool m_bSet9BarSelf;						// �ۭq9���ؽu����m
	int	 m_iALLHintAlpha;						// �V�n�o��Q�n�����ܩҦ�HINT�z����	
    POINT m_LastMouse;
    string mousestr;

    D3DCOLOR m_colorTopLeft;
    D3DCOLOR m_colorTopRight;
    D3DCOLOR m_colorBottomLeft;
    D3DCOLOR m_colorBottomRight;

    static CDXUTDialogResourceManager* s_pManager;
    PCALLBACKDXUTGUIEVENT m_pCallbackEvent;
    void* m_pCallbackEventUserContext;	
	
	
	
    CGrowableArray< int > m_Textures;   // Index into m_TextureCache;
    CGrowableArray< int > m_Fonts;      // Index into m_FontCache;

    CGrowableArray< CDXUTControl* > m_Controls;
	typedef std::map<uint , CDXUTControl*> ControlCache;
	ControlCache					mControlCache;
    CGrowableArray< DXUTElementHolder* > m_DefaultElements;

    CDXUTDialog* m_pNextDialog;
    CDXUTDialog* m_pPrevDialog;
	CDXUTElement pTextureElement;

    // ming add >>
    RECT m_rcBoundingBox; // Dialog �� BoundingBox
    RECT m_rcDraw; // �e Dialog ���誺�۹��m
    RECT m_rcCaption; // Caption ���d��
    RECT m_rcCapText; // Caption ���d��
	RECT m_rcCaption1; // Caption ���d��(�Ƥ�)
	RECT m_rcCaption2; // Caption ���d��(�Y�p��)

    list<CDXUTDialog*> *m_pDialogList; // �x�s�o��Dialog��list�e��

    CDXUTButton *m_MinBtn;
	bool		mbUseCloseBtn;
    CDXUTButton *m_CloseBtn;
    CDXUTButton *m_OrigBtn;

    bool m_bAlwaysFocus;
    // ming add <<

    // Mouse message preprocessing.
    bool m_bLButtonDown;
    bool PreprocessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bStopOuter);
    void SwitchMouseOverControl( POINT pt );

	//<gino>
public:
	enum eSkinStyleType{
	    sstFDO,
		sstHVO,
	};

	enum eSystemButtonType{
		sbtMINIMUM,
		sbtMAXIMUM,
		sbtCLOSE,
		sbtORIGIN,
	};
private:
	static eSkinStyleType sm_skinStyle;
	static std::string sm_resizeMouseAnim; //resize��mouse anim
	static std::wstring sm_defaultFont; //�w�]�r��
	static std::wstring sm_hintTextMinimum; //�̤p��	// Darkness Edit
	static std::wstring sm_hintTextMaximum; //�̤j��	// Darkness Edit
	static std::wstring sm_hintTextClose; //����		// Darkness Edit
	static std::wstring sm_hintTextOrigin; //�_��		// Darkness Edit
	static bool sm_bShowHint;
public:
	static void SetDialogStyle(eSkinStyleType type){ sm_skinStyle = type; }
	static void SetResizeMouseAnimName(const char *name){ sm_resizeMouseAnim = name; } 
	static void SetDefaultFontName(const char16 *name){ sm_defaultFont = name; }
	static void EnableShowHint(bool bV){ sm_bShowHint = bV;}
	static bool IsEnableShowHint(){ return sm_bShowHint;}
	static void SetSystemButtonHintText(eSystemButtonType type, const WCHAR *text);	// Darkness Edit
	static void fnSetResourceManager(CDXUTDialogResourceManager* pManager );
	//</gino>
	
	inline std::list<CDXUTDialog*>* GetDialogList() { return m_pDialogList ; }
	template<typename TKey>
	static std::map<TKey,CDXUTDialog*>& GetSource()
	{
		typedef std::map<TKey,CDXUTDialog*>		DialogSet;
		static DialogSet mDlgs;
		return mDlgs;
	}

	//////////////////////////////////////////////////////////////////////////		


	static bool Register(DialogHandle key,CDXUTDialog* pdlg)
	{
		return Register<uint64>(key->GetKey(),pdlg);
	}
	static bool Register(const char* key,CDXUTDialog* pdlg)
	{
		return Register<string>(key,pdlg);
	}


	template<typename TKey>
	static bool Register(TKey key,CDXUTDialog* pdlg)
	{
		typedef std::map<TKey,CDXUTDialog*>		DialogSet;
		DialogSet& mDlgs = GetSource<TKey>();

		DialogSet::iterator it = mDlgs.find(key);
		if(it == mDlgs.end())
		{
			mDlgs.insert( make_pair(key,pdlg) );
			return true;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	static bool Unregister(const char* key)
	{
		return Unregister<string>(key);
	}
	static bool Unregister(DialogHandle key)
	{
		return Unregister<uint64>(key->GetKey());
	}


	template<typename TKey>
	static bool Unregister(TKey key)
	{

		typedef std::map<TKey,CDXUTDialog*>		DialogSet;
		DialogSet& mDlgs = GetSource<TKey>();

		DialogSet::iterator it = mDlgs.find(key);
		if(it != mDlgs.end())
		{
			CDXUTDialog* pDlg = it->second;

			mDlgs.erase(it);
			return true;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	static CDXUTDialog* Query(const char* key)
	{
		return Query<string>(key);
	}
	static CDXUTDialog* Query(DialogHandle key)
	{
		if(RS_HANDLEVALID(key))
			return Query<uint64>(key->GetKey());
		return 0;
	}

	template<typename TKey>
	static CDXUTDialog* Query(TKey key)
	{
		typedef std::map<TKey,CDXUTDialog*>		DialogSet;
		DialogSet& mDlgs = GetSource<TKey>();

		DialogSet::iterator it = mDlgs.find(key);
		if(it != mDlgs.end())
		{
			return it->second;
		}
		return 0;
	}

};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ViewDialog 
// �s�Wview�\�� , �i�H��ܭn�ݪ�����d��
typedef CDXUTDialog CDXUTViewDialog; // �w�w�]�p
// class CDXUTViewDialog : public CDXUTDialog
// {
// 	RECT	mrcView;
// 
// 	
// public:
// 	CDXUTViewDialog();
// 	CDXUTViewDialog(list<CDXUTDialog*> *pDialogList);
// 	virtual ~CDXUTViewDialog();
// 
// 
// };

#endif // __CDXUTDialog_h_
