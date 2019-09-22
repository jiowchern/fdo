#ifndef _Layout_20080911am1154_
#define _Layout_20080911am1154_
#include "Element.h"
#include "ReaderCommon.h"
#include "LooknFeel.h"
namespace GUI
{
	//////////////////////////////////////////////////////////////////////////
	namespace Layout
	{
		typedef GUI::XmlElement Element;
		enum
		{
			INVALID_TYPE,
			CONTROL,
			CONTROL_ELEMENT,
			CONTROL_ELEMENT_FONT,
			CONTROL_ELEMENT_FONT_COLOR,
			CONTROL_ELEMENT_IMAGE,
			CONTROL_ELEMENT_IMAGE_COLOR,
			CONTROL_TEXT,
			CONTROL_RADIOINFO,
			CONTROL_LISTBOXINFO,
			CONTROL_LISTBOXINFO_ITEM,
			CONTROL_IMAGE,
			CONTROL_SLIDER,
			CONTROL_ICONLISTBOX,
			CONTROL_ICONLISTBOX_ITEM,
			CONTROL_ICONLISTBOX_CELL_IMAGE,
			CONTROL_IMAGELISTBOX,
			CONTROL_IMAGELISTBOX_ITEM,
			CONTROL_COMBOBOX,
			CONTROL_COMBOBOX_ITEM,
			CONTROL_SCROLLBAR,
			CONTROL_HINT,
			CONTROL_DIALOG,
			CONTROL_DIALOG_CAPTION,
			CONTROL_DYNAMICTEXT,
			CONTROL_TREELIST,
			CONTROL_TREELIST_ITEM,
			CONTROL_TREELIST_ITEM_TEXT,
			CONTROL_TREELIST_ITEM_IMAGE,
			CONTROL_IMAGEBOX,
			CONTROL_PROGRESSBAR,
			CONTROL_PROGRESSBAR_BACKGROUND,			
			CONTROL_PROGRESSBAR_FORWARD,			
			CONTROL_BUTTON,
			CONTROL_BUTTON_NORMAL,
			CONTROL_BUTTON_NORMAL_IMAGE,
			CONTROL_BUTTON_FOCUS,
			CONTROL_BUTTON_FOCUS_IMAGE,
			CONTROL_TDGITEMBOX,
			CONTROL_TDGITEMBOX_FRAME,
			CONTROL_TDGITEMBOX_IMAGE,
			CONTROL_TDGITEMBOX_BACKGROUNDIMAGE,
			CONTROL_TABPAGE,
			CONTROL_TABPAGE_ITEM,
			CONTROL_TABPAGE_NORMAL,
			CONTROL_TABPAGE_FOCUS,
			CONTROL_TABPAGE_NORMALIMAGE,
			CONTROL_TABPAGE_FOCUSIMAGE,
			CONTROL_SUBDIALOG,
			CONTROL_SUBDIALOG_INFO,
			CONTROL_LISTVIEW,			
			CONTROL_AVATAR,			
			CONTROL_TEXTAREA,			
			//////////////////////////////////////////////////////////////////////////
			//新標籤只能往下添加 ...
			CONTROL_TYPES
		};
		
		class Control : public Element
		{
		public:
			enum
			{
				
				ID,		//元件唯一識別碼 (P.S 不可重複)
				TYPE,		//元件類型 對應於LooknFeel表的 CONTROL::TYPE
				LEVEL,		//階層設定,介面顯示階層
							/*
							Normal,
							AlwaysOnTop,
							AlwaysOnBottom,
							*/

				POSX,		//元件位置x 單位:像素
				POSY,		//元件位置y 單位:像素
				POSDIMX,		//元件位置x 單位:百分比
				POSDIMY	,	//元件位置y 單位:百分比
				WIDTH	,	//元件寬度  單位:像素	
				HEIGHT	,	//元件高度  單位:像素				
				WIDTHDIM,	//元件寬度  單位:百分比	
				HEIGHTDIM,	//元件高度  單位:百分比	
				LEFT,
				TOP,
				RIGHT,
				BOTTOM,
				ENABLE,		
				VISIBLE,				
				MSGORDER,
				ALGINMODE,
				COUNT
			};
			enum
			{ //對其模式
				LeftTop,
				CenterTop,
				RightTop,
				LeftCenter,
				CenterCenter,
				RightCenter,
				LeftBottom,
				CenterBottom,
				RightBottom
			};
			enum
			{
				OrderNormal,
				OrderContrary,				
			};
			enum
			{
				Normal,
				AlwaysOnTop,
				AlwaysOnBottom,
			};
			Control		(Element* parent);
			~Control	();
			static void fnSetAlginMode(Element* pEle,int iProperty,const wchar_t* value);
			static void fnSetLevel(Element* pEle,int iProperty,const wchar_t* value);
			static void fnMessageOrder(Element* pEle,int iProperty,const wchar_t* value);
			static int fnGetLevel(const wchar_t* value);

		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlElement : public Element
		{
		public :
			enum
			{
				ID ,
				COUNT ,
			} ;
			ControlElement(Element* parent);
			~ControlElement();
		} ;
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlElementFont : public XmlElement , public Common::Font
		{
		public:
			enum
			{
				RGBA = Common::Font::COUNT,
				COUNT 
			};			
			ControlElementFont	(Element* parent);
			~ControlElementFont	();
		
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlElementFontColor : public XmlElement , public Common::Color
		{
		public:
			enum
			{
				COUNT = Common::Color::COUNT
			};			
			ControlElementFontColor		(Element* parent);
			~ControlElementFontColor	();			
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlElementImage : public Element
		{
		public:
			enum
			{
				SOURCE ,
				RGBA ,
				COUNT
			} ;
			ControlElementImage(Element* parent); 
			~ControlElementImage() ;
		} ;
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlElementImageColor : public Element , public GUI::Common::Color
		{
		public:
			enum
			{
				COUNT = GUI::Common::Color::COUNT 
			} ;
			ControlElementImageColor(Element* parent) ;
			~ControlElementImageColor();
		} ;
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlText : public Element
		{
		public:
			//標籤的屬性
			/*
				ex.
					<TEXT ID = "1" COLOR = "0xffffffff" SHADOW ="1" ...
			*/
			enum 
			{
				ID,
				COLOR,
				SHADOW,
				STRING,
				VALIGN,
				HALIGN,
				ONELINE,
				COUNT
			};
			enum
			{
				Left,Right,Center,Top,Bottom
			};
			ControlText		(Element* parent);
			~ControlText	();

			static void fnVAlign(Element* pEle,int iProperty,const wchar_t* value);
			static void fnHAlign(Element* pEle,int iProperty,const wchar_t* value);
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlRadioInfo : public Element
		{
		public:
			enum
			{
				GROUP_ID,
				STYLE,
				CHECK,
				COUNT
			};

			enum
			{
				Style_Normal,Style_Button
			};

			static void fnStyle(Element* pEle,int iProperty,const wchar_t* value);
			ControlRadioInfo(Element* parent);
			~ControlRadioInfo();
			
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlListBoxInfo : public Element
		{
		public:
			enum
			{
				STYLE,
				DRAWSELECT,
				NORMAL_COLOR,
				SELECT_COLOR,
				SHOW_SCROLLBAR,
				COUNT
			};
			ControlListBoxInfo(Element* parent);
			~ControlListBoxInfo();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlListBoxInfoItem : public Element
		{
		public:
			enum 
			{
				TEXT,COLOR,COUNT
			};
			ControlListBoxInfoItem(Element* parent);
			~ControlListBoxInfoItem();			
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlImage : public Element , public Common::Rect
		{
		public:
			enum
			{
				PATH,
				RECT_BEGIN , //Common::Rect::LEFT
				RECT_END = RECT_BEGIN + Common::Rect::COUNT-1 ,
				COUNT
			};
			ControlImage(Element* parent);
			~ControlImage();
			
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlSlider: public Element 
		{
		public:
			enum
			{
				MIN,MAX,VALUE,
				COUNT
			};
			ControlSlider(Element* parent);
			~ControlSlider();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlIconListBox : public Element 
		{
		public:
			enum
			{
				STYLE,
				ROW,
				COL,
				ITEMWIDTH,
				ITEMHEIGHT,
				COUNT
			};
			
			ControlIconListBox(Element* parent);
			~ControlIconListBox();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// <CELLIMAGE PATH = "xxx.tga" LEFT = "1" TOP = "1" RIGHT = "1" BOTTOM ="1"/>
		struct ControlIconListBoxCellImage : public Common::Texture
		{
			ControlIconListBoxCellImage(int nType,Element* parent);
			~ControlIconListBoxCellImage();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlIconListBoxItem : public Element 
		{
		public:
			enum
			{
				PATH,
				AMOUNT,				
				TEXTCOLOR,				
				COUNT
			};

			ControlIconListBoxItem(Element* parent);
			~ControlIconListBoxItem();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlImageListBox : public Element 
		{
		public:
			enum
			{
				STYLE,COUNT
			};

			ControlImageListBox(Element* parent);
			~ControlImageListBox();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlImageListBoxItem : public Element 
		{
		public:
			enum
			{
				PATH,
				TEXT,
				AMOUNT,				
				TEXTCOLOR,				
				MONEY,
				COUNT
			};

			ControlImageListBoxItem(Element* parent);
			~ControlImageListBoxItem();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlComboBox : public Element 
		{
		public:
			enum
			{
				DROPHEIGHT,
				TEXTCOLOR,
				COUNT
			};

			ControlComboBox(Element* parent);
			~ControlComboBox();
		};

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlComboBoxItem : public Element 
		{
		public:
			enum
			{
				TEXT,
				TEXTCOLOR,
				COUNT
			};

			ControlComboBoxItem(Element* parent);
			~ControlComboBoxItem();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlScrollBar : public Element 
		{
		public:
			enum
			{
				START,
				END,
				VALUE,
				COUNT
			};
			ControlScrollBar(Element* parent);
			~ControlScrollBar();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlHint : public Element 
		{
		public:
			enum
			{
				TEXT,PATH,COUNT
			};
			ControlHint(Element* parent);
			~ControlHint();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlDialog : public Element 
		{
		public:
			enum
			{
				CLOSE,MIN,MAX,COUNT
			};
			ControlDialog(Element* parent);
			~ControlDialog();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlDialogCaption : public Element 
		{
		public:
			enum
			{	TEXT,LEFT,TOP,RIGHT,BOTTOM,COLOR,DRAWRECT,COUNT	};
			ControlDialogCaption(Element* parent);
			~ControlDialogCaption();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlDynamicText : public Element 
		{
		public:
			enum
			{	PLAY,SECONDPERPIXLE,COUNT	};
			ControlDynamicText(Element* parent);
			~ControlDynamicText();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlTreeList: public Element 
		{
		public:
			//mnIndent
			enum
			{	INDENT,ICONWIDTH,ICONHEIGHT,COUNT	};
			ControlTreeList(Element* parent);
			~ControlTreeList();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlTreeListItem: public Element 
		{
		public:
			enum
			{	LEVEL,COUNT	};
			ControlTreeListItem(Element* parent);
			~ControlTreeListItem();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlTreeListItemText: public Element 
		{
		public:
			enum
			{	ID,COLOR,COUNT	};
			ControlTreeListItemText(Element* parent);
			~ControlTreeListItemText();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlTreeListItemImage: public Element 
		{
		public:
			enum
			{	PATH,LEFT,RIGHT,TOP,BOTTOM,COUNT	};
			ControlTreeListItemImage(Element* parent);
			~ControlTreeListItemImage();
		};			

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlImageBox : public Element 
		{
		public:
			enum
			{	COUNT	};
			ControlImageBox(Element* parent);
			~ControlImageBox();
		};			
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlProgressBar : public Element 
		{
		public:
			enum
			{	MAX,VALUE,COUNT	};
			ControlProgressBar(Element* parent);
			~ControlProgressBar();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlButton : public Element 
		{
		public:
			enum
			{	COUNT	};
			ControlButton(Element* parent);
			~ControlButton();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlButtonState : public Element 
		{
		public:
			enum
			{	COUNT	};
			ControlButtonState(int nType,Element* parent);
			~ControlButtonState();
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
	
		//////////////////////////////////////////////////////////////////////////	
		/*		
		<CONTROL ID="1" TYPE="TDGItemBox" POSX="28" POSY="15" WIDTH="170" HEIGHT="100">
			<TDGItemBox ITEMID = "1001" >				
				<BACKGROUNDIMAGE	PATH = "xxx.tga" LEFT = "1" TOP = "1" RIGHT = "1" BOTTOM ="1"/>
				<FRAMEIMAGE			PATH = "xxx.tga" LEFT = "1" TOP = "1" RIGHT = "1" BOTTOM ="1"/>
			</TDGItemBox>			
		</CONTROL>
		*/
		//////////////////////////////////////////////////////////////////////////
		struct ControlTDGItemBox : public XmlElement
		{
			enum
			{
				ITEMID,
				ITEM_DRAG,		//是否可拖曳
				FRAME_SIZE ,	//邊框大小
				IMAGE_WIDTH , 
				IMAGE_HEIGHT ,
				
				COUNT
			};
			ControlTDGItemBox(int nType,Element* parent);
			~ControlTDGItemBox();
		};
		//////////////////////////////////////////////////////////////////////////
		// <BACKGROUNDIMAGE PATH = "xxx.tga" LEFT = "1" TOP = "1" RIGHT = "1" BOTTOM ="1"/>
		struct ControlTDGItemBoxBackImage: public Common::Texture
		{
			ControlTDGItemBoxBackImage(int nType,Element* parent);
			~ControlTDGItemBoxBackImage();
		};

		//////////////////////////////////////////////////////////////////////////
		// <FRAME ID = "1" PATH = "xxx.tga" LEFT = "1" TOP = "1" RIGHT = "1" BOTTOM ="1"/>
		struct ControlTDGItemBoxFrameImage : public XmlElement
		{
			enum
			{
				ID ,
				PATH ,
				LEFT ,
				TOP ,
				RIGHT ,
				BOTTOM ,
				POSX ,		
				POSY ,				
				WIDTH ,	
 				HEIGHT ,	
				COUNT ,
			};
			ControlTDGItemBoxFrameImage(int nType,Element* parent);
			~ControlTDGItemBoxFrameImage();
		} ;


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct ControlTabPage : public XmlElement
		{
			ControlTabPage(int nType,Element* parent);
			~ControlTabPage();
			enum
			{
				TABPAGEWIDTH ,
				TABPAGEHEIGHT ,
				BUTTONOFFSETSIZE ,
				BUTTONWIDTH ,
				BUTTONHEIGHT ,
				TABPAGESIZE ,
				PAGEOFFSETSIZE ,
				COUNT
			};
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct ControlTabPageItem : public XmlElement
		{
			ControlTabPageItem(int nType,Element* parent);
			~ControlTabPageItem();
			enum
			{
				TEXT,
				COUNT
			};
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct ControlTabPageNormal : public XmlElement
		{
			ControlTabPageNormal(int nType,Element* parent);
			~ControlTabPageNormal();
			enum
			{
				ID,
				COUNT
			};
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct ControlTabPageFocus : public XmlElement
		{
			ControlTabPageFocus(int nType,Element* parent);
			~ControlTabPageFocus();
			enum
			{
				ID,
				COUNT
			};
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct ControlSubDialog : public XmlElement
		{
			ControlSubDialog(int nType,Element* parent);
			~ControlSubDialog();
			enum
			{
				PATH,
				COUNT
			};
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct ControlSubDialogCustom : public XmlElement
		{
			ControlSubDialogCustom(int nType,Element* parent);
			~ControlSubDialogCustom();
			enum
			{
				
				COUNT
			};
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct ControlListView : public XmlElement
		{
			ControlListView(int nType,Element* parent);
			~ControlListView();
			enum
			{
				LIST_PATH,
				ITEM_PATH,
				SCROLLBAR_ID,
				COUNT
			};
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct ControlAvatar : public XmlElement
		{
			ControlAvatar(int nType,Element* parent);
			~ControlAvatar();

			enum
			{
				SHOWTYPE ,
				SCALE ,
				FACE,
				CLASS,
				RACE,
				GENDER,
				COLOR , 
				NUMBER ,
				RESETNUMBER ,
				SHOWHEAD ,
				MONSTERID ,
				EMTPATH , 
				COUNT ,
			};
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct ControlTextarea : public XmlElement
		{
			ControlTextarea	(int nType,Element* parent);
			~ControlTextarea();
			enum
			{
				PATH,
				COUNT 
			};
		};
	};
};
#endif