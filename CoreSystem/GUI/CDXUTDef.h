//
// CDXUTDef.h
//
#ifndef __CDXUTDef_h__
#define __CDXUTDef_h__

using namespace std;

#include "CGrowableArray.h"

#define _MAX_TALK_TEXT_LEN_    512

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class CDXUTDialog;
class CDXUTDialogResourceManager;
class CDXUTControl;
class CDXUTButton;
class CDXUTStatic;
class CDXUTCheckBox;
class CDXUTRadioButton;
class CDXUTComboBox;
class CDXUTSlider;
class CDXUTEditBox;
class CDXUTIMEEditBox;
class CDXUTListBox;
class CDXUTScrollBar;
class CDXUTElement;
// ming add >> �B�~�s�@������
class CTalkBarImageControl;
class moCDXUTBtnListBox;// �tButton��Listbox
class CDXUTImage; // �ϧΤ���
class CDXUTIconImage;
class CDXUTIconImageBox;
class CDXUTImageEX; // �~�ӹϧΤ���A�i����h������A�s�y�ʵe�ĪG (Not implemented.)
class CDXUTImageListBox; // �]�t�ϧΪ�ListBox
class CDXUTBtnImageListBox; // �tButton��ImageListBox
class CDXUTImageBox; // �ΨӸ�Image���e���A�i���Image���W
class CDXUTImageBoxList; // �u��ImageBox��ListBox(�W�ٷ����PImageListBox�V�c)
class CDXUTIconListBox;
class CDXUTProgressBar; // �i�ױ��A�i�Ω�HP Bar
class CDXUTSpecialButton; // �S��γ~���s
class CMapControl;		//�W�[�p�a�ϱ���
class CDXUTPuzzle;		
class CDXUTTabPage;
// ming add <<
struct DXUTElementHolder;
struct DXUTTextureNode;
struct DXUTFontNode;
typedef VOID (CALLBACK *PCALLBACKDXUTGUIEVENT) ( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

//--------------------------------------------------------------------------------------
// Defines and macros 
//--------------------------------------------------------------------------------------
#define EVENT_CONTROL_MOUSE_IN				0x0001
#define EVENT_CONTROL_MOUSE_OUT				0x0002
#define EVENT_CONTROL_DRAG					0x0003
#define EVENT_CONTROL_DROP					0x0004
#define EVENT_CONTROL_PRESSED				0x0005
#define EVENT_CONTROL_CLICKED               0x0006
#define EVENT_CONTROL_RELEASED              0x0007
#define EVENT_CONTROL_DBLCLK				0x0008
#define EVENT_CONTROL_RMOUSEBUTTONDOWN		0x0009
#define EVENT_CONTROL_DROP_NOT_TARGET		0x000A
#define EVENT_CONTROL_FOCUS_IN				0x000B
#define EVENT_CONTROL_FOCUS_OUT				0x000C
#define EVENT_CONTROL_MOUSEWHEEL			0x000D

#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_BUTTON_MOUSEMOVE              0x0102
#define EVENT_BUTTON_PRESSED                0x0103
#define EVENT_BUTTON_RELEASED               0x0104
#define EVENT_BUTTON_MOUSE_IN				0x0105
#define EVENT_BUTTON_MOUSE_OUT				0x0106

#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_EDITBOX_STRING                0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_EDITBOX_SHIFT_ENTER           0x0603
#define EVENT_EDITBOX_CTRL_ENTER            0x0604
#define EVENT_EDITBOX_ALT_ENTER             0x0605

#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.

#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_ITEM_RDBLCLK          0x0703 // �k����������
#define EVENT_LISTBOX_RSELECTION            0x0704 // �k���I��
#define EVENT_LISTBOX_CLICK                 0x0705
#define EVENT_LISTBOX_RBUTTONDBLCLK         0x0706 // �k������
#define EVENT_ICONLISTBOX_CELLCROSS			0x0707 // �ƹ����V���P����l
//CDXUTTreeList�ƥ�
#define EVENT_TREELIST_OPEN		            0x0708 //�}�Ҹ`�I
#define EVENT_TREELIST_CLOSE                0x0709 //�����`�I
#define EVENT_LISTBOX_LOSTSELECTION         0x070A

// ming add �ϧΩ��ƥ�
#define EVENT_ITEM_DROP                     0x0801
#define EVENT_ITEM_DRAG_AND_DROP_OUT        0x0802
// Dialog �ƥ�
#define EVENT_DIALOG_SHOW                   0x0901
#define EVENT_DIALOG_HIDE                   0x0902
#define EVENT_DIALOG_MOVE                   0x0903
#define EVENT_DIALOG_CLOSE                  0x0904   
#define EVENT_DIALOG_MOVE_STOP              0x0905
#define EVENT_DIALOG_CHANGECOLOR            0x0906 // �C���ܤ� 
#define EVENT_DIALOG_MOUSELEAVE             0x0907
#define EVENT_DIALOG_MOUSEENTER             0x0908 
#define EVENT_DIALOG_CHANGEMINI             0x0909 // �����̤p��
#define EVENT_DIALOG_FOCUSIN				0x090A
#define EVENT_DIALOG_FOCUSOUT				0x090B
#define EVENT_DIALOG_CHANGEBIG              0x090C // �����ܤj�]��_�^
#define EVENT_DIALOG_MOUSE_CLICKED          0x090D // ���@�U
#define EVENT_DIALOG_MOUSEWHEEL				0x090E
#define EVENT_DIALOG_DRAG					0x090F
#define EVENT_DIALOG_DROP					0x0910
#define EVENT_DIALOG_MOUSE_DOUBLECLICKED    0x0911 // ��2�U
#define EVENT_DIALOG_MOUSE_RCLICKED         0x0912 // �k����@�U
#define EVENT_DIALOG_MOUSE_LCLICKED			0x0913 // ������@�U
#define EVENT_DIALOG_DROP_NOT_TARGET		0x0914
#define EVENT_DIALOG_BE_HOOK				0x0915
#define EVENT_DIALOG_UNHOOK					0x0916

#define EVENT_COMPONENT_CD_INTERVAL			0x0917


// Puzzle�ƥ�
#define EVENT_PUZZLE_CROSSCELL				0x0C00
#define EVENT_PUZZLE_SELECTCELL				0x0C01
// TabPage�ƥ�
#define EVENT_TABPAGE_SELECTION             0x0D00

// ScrollBar �ƥ�
#define EVENT_SCROLLBAR_VALUE_CHANGED       0x0A01
//DynamicText �ƥ�
#define EVENT_PLAY_DONE				        0x0B01

#define EVENT_USER_DEFINE					0xF000

struct DXUT_SCREEN_VERTEX
{
    float x, y, z, h;
    D3DCOLOR color;
    float tu, tv;

    static DWORD FVF;
};
extern const int size_of_DXUT_SCREEN_VERTEX;

//--------------------------------------------------------------------------------------
// Define Locale ID of Country										DarknessAdd
//--------------------------------------------------------------------------------------
#define LID_CHINA							0x0804 // �j��
#define LID_JAPANESE						0x0411 // �饻
#define LID_TAIWAN							0x0404 // �x�W
#define LID_THAI							0x041E // ����
#define	LID_VIETNAMESE						0x042A // �V�n

//--------------------------------------------------------------------------------------
enum MessageResult
{
    Wait,
    Next,
    Close,
    Yes,
    No,
    Enter,
    OK,
    Cancel,
    Buy,
    Sell
};
//--------------------------------------------------------------------------------------
// Enums for pre-defined control types
//--------------------------------------------------------------------------------------
enum DXUT_CONTROL_TYPE 
{ 
    DXUT_CONTROL_BUTTON,
    DXUT_CONTROL_STATIC, 
    DXUT_CONTROL_CHECKBOX,
    DXUT_CONTROL_RADIOBUTTON,
    DXUT_CONTROL_COMBOBOX,
    DXUT_CONTROL_SLIDER,
    DXUT_CONTROL_EDITBOX,
    DXUT_CONTROL_IMEEDITBOX,
    DXUT_CONTROL_LISTBOX,
    DXUT_CONTROL_SCROLLBAR,
    // ming add >>
    DXUT_CONTROL_IMAGE,
    DXUT_CONTROL_IMAGEBOX,
    DXUT_CONTROL_PROGRESSBAR,
    DXUT_CONTROL_SPECIALBUTTON,
	DXUT_CONTROL_MAPCONTROL,
    // ming add <<
	DXUT_CONTROL_DISPLAYROLE,
	DXUT_CONTROL_SUBDIALOG_BEGIN,
	DXUT_CONTROL_SUBDIALOG = DXUT_CONTROL_SUBDIALOG_BEGIN,
	DXUT_CONTROL_TEXTAREA,
	DXUT_CONTROL_LISTVIEW ,
	DXUT_CONTROL_SUBDIALOG_END = DXUT_CONTROL_LISTVIEW,
	DXUT_CONTROL_PUZZLE ,
	DXUT_CONTROL_TABPAGE,
	DXUT_CONTROL_REMOTEVIEW ,
	DXUT_CONTROL_PANEL,
	
	DXUT_CONTROL_CUSTOM
};

enum DXUT_CONTROL_STATE
{
    DXUT_STATE_NORMAL = 0,
    DXUT_STATE_DISABLED,
    DXUT_STATE_HIDDEN,
    DXUT_STATE_FOCUS,
    DXUT_STATE_MOUSEOVER,
    DXUT_STATE_PRESSED,
    DXUT_STATE_UNFOCUS, // ming add
};

// ming edit >>
//#define MAX_CONTROL_STATES 6
const int MAX_CONTROL_STATES = 7;
// ming edit <<

struct DXUTBlendColor
{
    void Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128),
        D3DCOLOR hiddenColor = 0, D3DCOLOR unfocusedColor = D3DCOLOR_ARGB(200, 200, 200, 200) );

    void Blend( UINT iState, float fElapsedTime, float fRate = 0.7f );
    
    D3DCOLOR  States[ MAX_CONTROL_STATES ]; // Modulate colors for all possible control states
    D3DXCOLOR Current;
};

struct IDXUTRender
{
	virtual ~IDXUTRender() {}
	virtual void RenderPipeline() = 0 ; 
	virtual void Update(float fElapsedTime) = 0 ; 
	virtual void SetViewPosition(float left , float top) = 0 ; 
	virtual void SetViewSize(unsigned int width , unsigned int height) = 0 ; 
};

//-----------------------------------------------------------------------------
// Contains all the display tweakables for a sub-control
//-----------------------------------------------------------------------------
class CDXUTElement
{
public:
	CDXUTElement()
	{
		iTexture = -1; 
		iFont = 0;
	};
	void SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor = D3DCOLOR_ARGB(255, 255, 255, 255) );
    void SetFont( UINT iFont, D3DCOLOR defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DT_CENTER | DT_VCENTER );
    
    void Refresh();
    
    UINT iTexture;          // Index of the texture for this Element
    UINT iFont;             // Index of the font for this Element
    DWORD dwTextFormat;     // The format argument to DrawText

    RECT rcTexture;         // Bounding rect of this element on the composite texture
    
    DXUTBlendColor TextureColor;
    DXUTBlendColor FontColor;
};

//--------------------------------------------------------------------------------------
// Structs for shared resources
//--------------------------------------------------------------------------------------
struct DXUTTextureNode
{
	DXUTTextureNode() :pTexture(0)
	{

	}
	
	~DXUTTextureNode()
	{
		if(pTexture)
			pTexture->Release();
	}
    WCHAR strFilename[MAX_PATH];
    IDirect3DTexture9* pTexture;
    DWORD dwWidth;
    DWORD dwHeight;
};

struct DXUTFontNode
{
    WCHAR strFace[MAX_PATH];
    ID3DXFont* pFont;
    LONG  nHeight;
    LONG  nWeight;
    BOOL  bItalic;
};

//--------------------------------------------------------------------------------------
#undef min // use __min instead
#undef max // use __max instead

// ming add >>
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

//#define DXTRACE_MSG(str)              (0L)
//#define DXTRACE_ERR(str,hr)           (hr)
//#define DXTRACE_ERR_MSGBOX(str,hr)    (hr)

#define DXUT_ERR(str,hr)           (hr)
#define DXUT_ERR_MSGBOX(str,hr)    (hr)
#define DXUTTRACE                  {}

#ifndef V
#define V(x)           { hr = x; }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif
// ming add <<

#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B // (not always defined)
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C // (not always defined)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A // (not always defined)
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120 // (not always defined)
#endif

//--------------------------------------------------------------------------------------

#define RectWidth(refRECT) ((refRECT).right - (refRECT).left)
#define RectHeight(refRECT) ((refRECT).bottom - (refRECT).top)

// ���X��U��X
void ANSItoUNICODE( WCHAR *Dest, const std::string &Source );
// ���X��U��X
WCHAR *ANSItoUNICODE( const std::string &Source );
// �U��X�����X
string UNICODEtoANSI( const WCHAR *Source );
// �U��X��V�n(VISCII)
string UNICODEtoVISCII( const WCHAR *Source );

#include "DXUTutil.h"

//--------------------------------------------------------------------------------------
#endif // __CDXUTDef_h__
