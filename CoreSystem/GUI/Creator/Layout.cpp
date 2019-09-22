#include "stdafx.h"
#include "Layout.h"
namespace GUI
{
	namespace Layout
	{
		void Control::fnSetLevel(Element* pEle,int iProperty,const wchar_t* value)
		{
			pEle->Set(iProperty,GUI::Layout::Control::fnGetLevel(value));
		}
		//////////////////////////////////////////////////////////////////////////
		void Control::fnMessageOrder(Element* pEle,int iProperty,const wchar_t* value)
		{
			if(wcsicmp(value,L"Normal") == 0)
			{
				pEle->Set(iProperty,OrderNormal);
			}
			else if(wcsicmp(value,L"Contrary") == 0)
			{
				pEle->Set(iProperty,OrderContrary);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		Control::Control(Element* parent) : XmlElement(CONTROL,parent,COUNT)
		{
			_SetGUIPropertyCNV(0,ID,CNV_INT);
			_SetGUIPropertyCNV(0,LEVEL,fnSetLevel);
			_SetGUIPropertyCNV(0,POSX,CNV_INT);
			_SetGUIPropertyCNV(0,POSY,CNV_INT);
			_SetGUIPropertyCNV(0,WIDTH,CNV_INT);
			_SetGUIPropertyCNV(0,HEIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,POSDIMX,CNV_FLOAT);
			_SetGUIPropertyCNV(0,POSDIMY,CNV_FLOAT);
			_SetGUIPropertyCNV(0,WIDTHDIM,CNV_FLOAT);
			_SetGUIPropertyCNV(0,HEIGHTDIM,CNV_FLOAT);
			_SetGUIPropertyCNV(0,ENABLE,CNV_INT);			
			_SetGUIPropertyCNV(0,LEFT,CNV_INT);
			_SetGUIPropertyCNV(0,TOP,CNV_INT);
			_SetGUIPropertyCNV(0,RIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,BOTTOM,CNV_INT);
			_SetGUIPropertyCNV(0,VISIBLE,CNV_INT);
			_SetGUIPropertyCNV(0,MSGORDER,fnMessageOrder);											
			_SetGUIPropertyCNV(0,ALGINMODE,fnSetAlginMode);
		}
		//////////////////////////////////////////////////////////////////////////
		Control::~Control()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		void Control::fnSetAlginMode(Element* pEle,int iProperty,const wchar_t* value)
		{
			int nMode = 0;
			if(wcsicmp(value , L"LeftTop") == 0)
			{
				nMode = LeftTop;
			}
			if(wcsicmp(value , L"CenterTop") == 0)
			{
				nMode = CenterTop;
			}
			if(wcsicmp(value , L"RightTop") == 0)
			{
				nMode = RightTop;
			}
			if(wcsicmp(value , L"LeftCenter") == 0)
			{
				nMode = LeftCenter;
			}
			if(wcsicmp(value , L"CenterCenter") == 0)
			{
				nMode = CenterCenter;
			}
			if(wcsicmp(value , L"RightCenter") == 0)
			{
				nMode = RightCenter;
			}
			if(wcsicmp(value , L"LeftBottom") == 0)
			{
				nMode = LeftBottom;
			}
			if(wcsicmp(value , L"CenterBottom") == 0)
			{
				nMode = CenterBottom;
			}
			if(wcsicmp(value , L"RightBottom") == 0)
			{
				nMode = RightBottom;
			}
			pEle->Set(iProperty,nMode);
		}
		//////////////////////////////////////////////////////////////////////////
		int Control::fnGetLevel(const wchar_t* value)
		{
			if(wcsicmp(value , L"Normal") == 0)
			{
				return Normal;
			}
			if(wcsicmp(value , L"AlwaysOnTop") == 0)
			{
				return AlwaysOnTop;
			}
			if(wcsicmp(value , L"AlwaysOnBottom") == 0)
			{
				return AlwaysOnBottom;
			}
			return Normal;
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlElement::ControlElement(Element* parent) : XmlElement(Layout::CONTROL_ELEMENT,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIPropertyCNV(0,ID,CNV_INT);
		}
		ControlElement::~ControlElement()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlElementFont::ControlElementFont(Element* parent)
			:  XmlElement(CONTROL_ELEMENT_FONT,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL_ELEMENT);
			_SetPropertySet(RGBA,L"R",CNV_ARGBR);
			_SetPropertySet(RGBA,L"G",CNV_ARGBG);
			_SetPropertySet(RGBA,L"B",CNV_ARGBB);
			_SetPropertySet(RGBA,L"A",CNV_ARGBA);
			_SetPropertySet(ALIGN,L"ALIGN",CNV_HEX);
			_SetPropertySet(HALIGN,L"HALIGN",Common::fnSetFontAlign);
			_SetPropertySet(VALIGN,L"VALIGN",Common::fnSetFontAlign);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlElementFont::~ControlElementFont()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlElementFontColor::ControlElementFontColor(Element* parent)
			:  XmlElement(CONTROL_ELEMENT_FONT_COLOR,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL_ELEMENT_FONT);

			_SetGUIPropertyCNV(0,STATE,Common::fnSetColorState);
			_SetPropertySet(RGBA,L"ARGB",CNV_ARGB);
			_SetPropertySet(RGBA,L"R",CNV_ARGBR);
			_SetPropertySet(RGBA,L"G",CNV_ARGBG);
			_SetPropertySet(RGBA,L"B",CNV_ARGBB);
			_SetPropertySet(RGBA,L"A",CNV_ARGBA);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlElementFontColor::~ControlElementFontColor()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlElementImage::ControlElementImage(Element* parent) : XmlElement( Layout::CONTROL_ELEMENT_IMAGE,parent,COUNT)
		{
			assert(parent->GetType() == Layout::CONTROL_ELEMENT);
			//_SetGUIPropertyCNV(0,ControlElementImage::SOURCE,CNV_WCHAR);
			_SetPropertySet(RGBA,L"ARGB",CNV_ARGB);
			_SetPropertySet(RGBA,L"R",CNV_ARGBR);
			_SetPropertySet(RGBA,L"G",CNV_ARGBG);
			_SetPropertySet(RGBA,L"B",CNV_ARGBB);
			_SetPropertySet(RGBA,L"A",CNV_ARGBA);
		}
		ControlElementImage::~ControlElementImage()
		{
		}

		ControlElementImageColor::ControlElementImageColor(Element* parent) : XmlElement( Layout::CONTROL_ELEMENT_IMAGE_COLOR,parent,COUNT)
		{
			assert(parent->GetType() == Layout::CONTROL_ELEMENT_IMAGE);
			_SetGUIPropertyCNV(0,STATE,Common::fnSetColorState);
			_SetPropertySet(RGBA,L"ARGB",CNV_ARGB);
			_SetPropertySet(RGBA,L"R",CNV_ARGBR);
			_SetPropertySet(RGBA,L"G",CNV_ARGBG);
			_SetPropertySet(RGBA,L"B",CNV_ARGBB);
			_SetPropertySet(RGBA,L"A",CNV_ARGBA);
		}
		ControlElementImageColor::~ControlElementImageColor()
		{
		}
		ControlText::ControlText(Element* parent) : XmlElement(CONTROL_TEXT,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL);

			/*
				設定屬性的資料類型

				_SetGUIPropertyCNV(0 , 列舉 , 轉換類型列舉)
				_SetGUIPropertyCNV(0 , 列舉 , 函式指標)

				_SetPropertySet(列舉, 解析的字串 , 轉換類型列舉 );
				_SetPropertySet(列舉, 解析的字串 , 函式指標 );
			*/

			_SetGUIPropertyCNV(0,ID,CNV_INT);
			_SetGUIPropertyCNV(0,ONELINE,CNV_INT);
			_SetGUIPropertyCNV(0,SHADOW,CNV_INT);
			_SetGUIPropertyCNV(0,STRING,CNV_WCHAR);
			_SetGUIPropertyCNV(0,COLOR,CNV_ARGB);
			_SetPropertySet(COLOR,L"R",CNV_ARGBR);
			_SetPropertySet(COLOR,L"G",CNV_ARGBG);
			_SetPropertySet(COLOR,L"B",CNV_ARGBB);
			_SetPropertySet(COLOR,L"A",CNV_ARGBA);
			_SetGUIPropertyCNV(0,VALIGN,fnVAlign);
			_SetGUIPropertyCNV(0,HALIGN,fnHAlign);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlText::~ControlText()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		void ControlText::fnVAlign(Element* pEle,int iProperty,const wchar_t* value)
		{

			int nType ;
			if(wcsicmp(value,L"Center") == 0) 
			{
				nType = Center;
			}
			if(wcsicmp(value,L"Top") == 0) 
			{
				nType = Top;
			}
			if(wcsicmp(value,L"Bottom") == 0) 
			{
				nType = Bottom;
			}
			
			pEle->Set(iProperty,nType);
		}
		//////////////////////////////////////////////////////////////////////////
		void ControlText::fnHAlign(Element* pEle,int iProperty,const wchar_t* value)
		{
			int nType ;
			if(wcsicmp(value,L"Left") == 0) 
			{
				nType = Left;
			}
			if(wcsicmp(value,L"Right") == 0) 
			{
				nType = Right;
			}
			if(wcsicmp(value,L"Center") == 0) 
			{
				nType = Center;
			}
			pEle->Set(iProperty,nType);
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlRadioInfo::ControlRadioInfo(Element* parent) : XmlElement(CONTROL_RADIOINFO,parent,COUNT)
		{			
			assert(parent->GetType() == CONTROL);
			_SetGUIPropertyCNV(0,GROUP_ID,CNV_INT);
			_SetGUIPropertyCNV(0,STYLE,fnStyle);
			_SetGUIPropertyCNV(0,CHECK,CNV_INT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlRadioInfo::~ControlRadioInfo()
		{
			
		}	
		//////////////////////////////////////////////////////////////////////////
		void ControlRadioInfo::fnStyle(Element* pEle,int iProperty,const wchar_t* value)
		{
			if(wcsicmp(value,L"Normal") == 0)
			{
				pEle->Set(iProperty,Style_Normal);
			}
			else if(wcsicmp(value,L"Button") == 0)
			{
				pEle->Set(iProperty,Style_Button);
			}	
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlListBoxInfo::ControlListBoxInfo(Element* parent)
			: XmlElement(CONTROL_LISTBOXINFO,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIPropertyCNV(0,STYLE,CNV_INT);
			_SetGUIPropertyCNV(0,DRAWSELECT,CNV_INT);
			_SetGUIPropertyCNV(0,SHOW_SCROLLBAR,CNV_INT);

			_SetGUIPropertyCNV(0,NORMAL_COLOR,CNV_ARGB);
			_SetPropertySet(NORMAL_COLOR,L"NORMAL_COLORR",CNV_ARGBR);
			_SetPropertySet(NORMAL_COLOR,L"NORMAL_COLORG",CNV_ARGBG);
			_SetPropertySet(NORMAL_COLOR,L"NORMAL_COLORB",CNV_ARGBB);
			_SetPropertySet(NORMAL_COLOR,L"NORMAL_COLORA",CNV_ARGBA);

			_SetGUIPropertyCNV(0,SELECT_COLOR,CNV_ARGB);
			_SetPropertySet(SELECT_COLOR,L"SELECT_COLORR",CNV_ARGBR);
			_SetPropertySet(SELECT_COLOR,L"SELECT_COLORG",CNV_ARGBG);
			_SetPropertySet(SELECT_COLOR,L"SELECT_COLORB",CNV_ARGBB);
			_SetPropertySet(SELECT_COLOR,L"SELECT_COLORA",CNV_ARGBA);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlListBoxInfo::~ControlListBoxInfo()
		{
			
		}		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlListBoxInfoItem::ControlListBoxInfoItem(Element* parent)
			: XmlElement(CONTROL_LISTBOXINFO_ITEM,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL_LISTBOXINFO);

			_SetGUIPropertyCNV(0,TEXT,CNV_INT);
			_SetGUIPropertyCNV(0,COLOR,CNV_ARGB);
			_SetPropertySet(COLOR,L"R",CNV_ARGBR);
			_SetPropertySet(COLOR,L"G",CNV_ARGBG);
			_SetPropertySet(COLOR,L"B",CNV_ARGBB);
			_SetPropertySet(COLOR,L"A",CNV_ARGBA);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlListBoxInfoItem::~ControlListBoxInfoItem()
		{

		}
		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlImage::ControlImage(Element* parent)
			: XmlElement(CONTROL_IMAGE,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIProperty(0,PATH);
			_SetGUIProperty(RECT_BEGIN,LEFT);
			_SetGUIProperty(RECT_BEGIN,TOP);
			_SetGUIProperty(RECT_BEGIN,RIGHT);
			_SetGUIProperty(RECT_BEGIN,BOTTOM);
			_SetGUIProperty(RECT_BEGIN,POSX);
			_SetGUIProperty(RECT_BEGIN,POSY);
			_SetGUIProperty(RECT_BEGIN,WIDTH);
			_SetGUIProperty(RECT_BEGIN,HEIGHT);
			_SetGUIProperty(RECT_BEGIN,POSDIMX);
			_SetGUIProperty(RECT_BEGIN,POSDIMY);
			_SetGUIProperty(RECT_BEGIN,WIDTHDIM);
			_SetGUIProperty(RECT_BEGIN,HEIGHTDIM);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlImage::~ControlImage()
		{

		}
		//////////////////////////////////////////////////////////////////////////		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlSlider::ControlSlider(Element* parent)
			: XmlElement(CONTROL_SLIDER,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIProperty(0,MIN);
			_SetGUIProperty(0,MAX);
			_SetGUIProperty(0,VALUE);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlSlider::~ControlSlider()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlIconListBox::ControlIconListBox(Element* parent)
			: XmlElement(CONTROL_ICONLISTBOX,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);

			_SetGUIProperty(0,STYLE);
			_SetGUIProperty(0,ROW);
			_SetGUIProperty(0,COL);
			_SetGUIProperty(0,ITEMWIDTH);
			_SetGUIProperty(0,ITEMHEIGHT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlIconListBox::~ControlIconListBox()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlIconListBoxItem::ControlIconListBoxItem(Element* parent)
			: XmlElement(CONTROL_ICONLISTBOX_ITEM,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL_ICONLISTBOX);

			_SetGUIProperty(0,PATH);
			_SetGUIProperty(0,AMOUNT);			
			_SetGUIProperty(0,TEXTCOLOR);
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_R");
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_G");
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_B");
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_A");			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlIconListBoxItem::~ControlIconListBoxItem()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlIconListBoxCellImage::ControlIconListBoxCellImage(int nType,Element* parent) 
			: Common::Texture(nType,parent)
		{
			assert(parent->GetType() == CONTROL_ICONLISTBOX);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlIconListBoxCellImage::~ControlIconListBoxCellImage()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlImageListBox::ControlImageListBox(Element* parent)
			: XmlElement(CONTROL_IMAGELISTBOX,parent,COUNT) 
			
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIProperty(0,STYLE);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlImageListBox::~ControlImageListBox()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlImageListBoxItem::ControlImageListBoxItem(Element* parent)
			: XmlElement(CONTROL_IMAGELISTBOX_ITEM,parent,COUNT) 

		{
			assert(parent->GetType() == CONTROL_IMAGELISTBOX);
			_SetGUIProperty(0,PATH);			
			_SetGUIProperty(0,MONEY);
			_SetGUIProperty(0,AMOUNT);			
			_SetGUIProperty(0,TEXTCOLOR);
			_SetGUIProperty(0,TEXT);
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_R");
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_G");
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_B");
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_A");			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlImageListBoxItem::~ControlImageListBoxItem()
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlComboBox::ControlComboBox(Element* parent)
			: XmlElement(CONTROL_COMBOBOX,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);		
			_SetGUIPropertyCNV(0,DROPHEIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,TEXTCOLOR,CNV_ARGB);
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_R",CNV_ARGBR);
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_G",CNV_ARGBG);
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_B",CNV_ARGBB);
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_A",CNV_ARGBA);
			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlComboBox::~ControlComboBox()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlComboBoxItem::ControlComboBoxItem(Element* parent)
			: XmlElement(CONTROL_COMBOBOX_ITEM,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL_COMBOBOX);			
			_SetGUIProperty(0,TEXT);
			_SetGUIProperty(0,TEXTCOLOR);
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_R");
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_G");
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_B");
			_SetPropertySet(TEXTCOLOR,L"TEXTCOLOR_A");
		}
		//////////////////////////////////////////////////////////////////////////
		ControlComboBoxItem::~ControlComboBoxItem()
		{
						
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlScrollBar::ControlScrollBar(Element* parent)
			: XmlElement(CONTROL_SCROLLBAR,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);			

			_SetGUIPropertyCNV(0,START,CNV_INT);
			_SetGUIPropertyCNV(0,END,CNV_INT);
			_SetGUIPropertyCNV(0,VALUE,CNV_INT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlScrollBar::~ControlScrollBar()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlHint::ControlHint(Element* parent)
			: XmlElement(CONTROL_HINT,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);			

			_SetGUIPropertyCNV(0,TEXT,CNV_INT);
			_SetGUIPropertyCNV(0,PATH,CNV_WCHAR);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlHint::~ControlHint()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlDialog::ControlDialog(Element* parent)
			: XmlElement(CONTROL_DIALOG,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);			

			_SetGUIPropertyCNV(0,CLOSE,CNV_INT);
			_SetGUIPropertyCNV(0,MIN,CNV_INT);
			_SetGUIPropertyCNV(0,MAX,CNV_INT);
			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlDialog::~ControlDialog()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlDialogCaption::ControlDialogCaption(Element* parent)
			: XmlElement(CONTROL_DIALOG_CAPTION,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL_DIALOG);			
			_SetGUIPropertyCNV(0,TEXT,CNV_INT);
			_SetGUIPropertyCNV(0,LEFT,CNV_INT);
			_SetGUIPropertyCNV(0,TOP,CNV_INT);
			_SetGUIPropertyCNV(0,RIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,BOTTOM,CNV_INT);
			_SetGUIPropertyCNV(0,DRAWRECT,CNV_INT);
			_SetGUIPropertyCNV(0,COLOR,CNV_ARGB);
			_SetPropertySet(COLOR,L"R",CNV_ARGBR);
			_SetPropertySet(COLOR,L"G",CNV_ARGBG);
			_SetPropertySet(COLOR,L"B",CNV_ARGBB);
			_SetPropertySet(COLOR,L"A",CNV_ARGBA);

		}
		//////////////////////////////////////////////////////////////////////////
		ControlDialogCaption::~ControlDialogCaption()			
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlDynamicText::ControlDynamicText(Element* parent)
			: XmlElement(CONTROL_DYNAMICTEXT,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIPropertyCNV(0,PLAY,CNV_WCHAR);
			_SetGUIPropertyCNV(0,SECONDPERPIXLE,CNV_FLOAT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlDynamicText::~ControlDynamicText()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlTreeList::ControlTreeList(Element* parent)
			: XmlElement(CONTROL_TREELIST,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIPropertyCNV(0,ICONWIDTH,CNV_INT);
			_SetGUIPropertyCNV(0,ICONHEIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,INDENT,CNV_INT);
			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTreeList::~ControlTreeList()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlTreeListItem::ControlTreeListItem(Element* parent)
			: XmlElement(CONTROL_TREELIST_ITEM,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL_TREELIST);
			_SetGUIPropertyCNV(0,LEVEL,CNV_INT);			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTreeListItem::~ControlTreeListItem()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlTreeListItemText::ControlTreeListItemText(Element* parent)
			: XmlElement(CONTROL_TREELIST_ITEM_TEXT,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL_TREELIST_ITEM);
			
			_SetGUIPropertyCNV(0,ID,CNV_INT);			

			_SetGUIPropertyCNV(0,COLOR,CNV_ARGB);
			_SetPropertySet(COLOR,L"R",CNV_ARGBR);
			_SetPropertySet(COLOR,L"G",CNV_ARGBG);
			_SetPropertySet(COLOR,L"B",CNV_ARGBB);
			_SetPropertySet(COLOR,L"A",CNV_ARGBA);

		}
		//////////////////////////////////////////////////////////////////////////
		ControlTreeListItemText::~ControlTreeListItemText()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlTreeListItemImage::ControlTreeListItemImage(Element* parent)
			: XmlElement(CONTROL_TREELIST_ITEM_IMAGE,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL_TREELIST_ITEM);
			_SetGUIPropertyCNV(0,PATH,CNV_WCHAR);
			_SetGUIPropertyCNV(0,LEFT,CNV_INT);
			_SetGUIPropertyCNV(0,RIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,TOP,CNV_INT);
			_SetGUIPropertyCNV(0,BOTTOM,CNV_INT);
			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTreeListItemImage::~ControlTreeListItemImage()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlImageBox::ControlImageBox(Element* parent)
			: XmlElement(CONTROL_IMAGEBOX,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);

		}
		//////////////////////////////////////////////////////////////////////////
		ControlImageBox::~ControlImageBox()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlProgressBar::ControlProgressBar(Element* parent)
			: XmlElement(CONTROL_PROGRESSBAR,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIPropertyCNV(0,MAX,CNV_INT);
			_SetGUIPropertyCNV(0,VALUE,CNV_INT);			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlProgressBar::~ControlProgressBar()
		{
			

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlButton::ControlButton(Element* parent)
			: XmlElement(CONTROL_BUTTON,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlButton::~ControlButton()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlButtonState::ControlButtonState(int nType,Element* parent)
			: XmlElement(nType,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL_BUTTON);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlButtonState::~ControlButtonState()
		{

		}

		//////////////////////////////////////////////////////////////////////////
		//
		//////////////////////////////////////////////////////////////////////////		
		ControlTDGItemBox::ControlTDGItemBox(int nType,Element* parent)
			: XmlElement(nType,parent,COUNT)
		{		
			assert(parent->GetType() == CONTROL);

			_SetGUIPropertyCNV(0,ITEMID,CNV_INT);
			_SetGUIPropertyCNV(0,ITEM_DRAG,CNV_INT);
			_SetGUIPropertyCNV(0,FRAME_SIZE,CNV_INT);
			_SetGUIPropertyCNV(0,IMAGE_WIDTH,CNV_INT);
			_SetGUIPropertyCNV(0,IMAGE_HEIGHT,CNV_INT);
			//_SetPropertySet(ITEMID,L"ITEMID",GUIElement::CNV_INT);		
		}
		//////////////////////////////////////////////////////////////////////////	
		ControlTDGItemBox::~ControlTDGItemBox()
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTDGItemBoxBackImage::ControlTDGItemBoxBackImage(int nType,Element* parent)
			: Common::Texture(nType,parent)
		{
			assert(parent->GetType() == CONTROL_TDGITEMBOX);

// 			_SetGUIPropertyCNV(0,PATH,CNV_WCHAR);
// 			_SetGUIPropertyCNV(0,LEFT,CNV_INT);
// 			_SetGUIPropertyCNV(0,RIGHT,CNV_INT);
// 			_SetGUIPropertyCNV(0,TOP,CNV_INT);
// 			_SetGUIPropertyCNV(0,BOTTOM,CNV_INT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTDGItemBoxBackImage::~ControlTDGItemBoxBackImage()
		{
		
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlTDGItemBoxFrameImage::ControlTDGItemBoxFrameImage(int nType,Element* parent )
			: XmlElement(nType,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL_TDGITEMBOX);
			_SetGUIPropertyCNV(0,ID,CNV_INT);
			_SetGUIPropertyCNV(0,PATH,CNV_WCHAR);
			_SetGUIPropertyCNV(0,POSX,CNV_INT);
			_SetGUIPropertyCNV(0,POSY,CNV_INT);
			_SetGUIPropertyCNV(0,WIDTH,CNV_INT);
			_SetGUIPropertyCNV(0,HEIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,LEFT,CNV_INT);
			_SetGUIPropertyCNV(0,TOP,CNV_INT);
			_SetGUIPropertyCNV(0,RIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,BOTTOM,CNV_INT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTDGItemBoxFrameImage::~ControlTDGItemBoxFrameImage()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		ControlTabPage::ControlTabPage(int nType,Element* parent)
			: XmlElement(nType,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL);

			_SetGUIPropertyCNV (0,TABPAGEWIDTH ,CNV_INT);
			_SetGUIPropertyCNV (0,TABPAGEHEIGHT ,CNV_INT);
			_SetGUIPropertyCNV (0,BUTTONOFFSETSIZE ,CNV_INT);
			_SetGUIPropertyCNV (0,BUTTONWIDTH ,CNV_INT);
			_SetGUIPropertyCNV (0,BUTTONHEIGHT ,CNV_INT);
			_SetGUIPropertyCNV (0,TABPAGESIZE ,CNV_INT);
			_SetGUIPropertyCNV (0,PAGEOFFSETSIZE ,CNV_INT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTabPage::~ControlTabPage()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlTabPageItem::ControlTabPageItem(int nType,Element* parent)
			: XmlElement(nType,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL_TABPAGE);

			_SetGUIPropertyCNV (0,TEXT , CNV_WCHAR);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTabPageItem::~ControlTabPageItem()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlTabPageNormal::ControlTabPageNormal(int nType,Element* parent)
			: XmlElement(nType,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL_TABPAGE);

			_SetGUIPropertyCNV (0 , ID , CNV_INT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTabPageNormal::~ControlTabPageNormal()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlTabPageFocus::ControlTabPageFocus(int nType,Element* parent)
			: XmlElement(nType,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL_TABPAGE);

			_SetGUIPropertyCNV (0 , ID , CNV_INT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTabPageFocus::~ControlTabPageFocus()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlSubDialog::ControlSubDialog(int nType,Element* parent)
			: XmlElement(nType,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL);

			_SetGUIPropertyCNV (0 , PATH , CNV_WCHAR);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlSubDialog::~ControlSubDialog()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlSubDialogCustom::ControlSubDialogCustom(int nType,Element* parent)
			: XmlElement(nType,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL_SUBDIALOG);
			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlSubDialogCustom::~ControlSubDialogCustom()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlListView::ControlListView(int nType,Element* parent): XmlElement(nType,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL);

			_SetGUIPropertyCNV (0 , LIST_PATH , CNV_WCHAR);
			_SetGUIPropertyCNV (0 , ITEM_PATH , CNV_WCHAR);
			_SetGUIPropertyCNV (0 , SCROLLBAR_ID , CNV_INT);
			
		}
		//////////////////////////////////////////////////////////////////////////
		ControlListView::~ControlListView()
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlAvatar::ControlAvatar(int nType,Element* parent): XmlElement(nType,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL);

			_SetGUIPropertyCNV (0 , SHOWTYPE , CNV_INT) ; 
			_SetGUIPropertyCNV (0 , SCALE , CNV_FLOAT) ; 
			_SetGUIPropertyCNV (0 , FACE, CNV_INT);
			_SetGUIPropertyCNV (0 , CLASS, CNV_INT);
			_SetGUIPropertyCNV (0 , RACE, CNV_INT);
			_SetGUIPropertyCNV (0 , GENDER, CNV_INT);
			_SetGUIPropertyCNV (0 , COLOR, CNV_ARGB);
			_SetGUIPropertyCNV (0 , NUMBER, CNV_INT);
			_SetGUIPropertyCNV (0 , RESETNUMBER, CNV_INT);
			_SetGUIPropertyCNV (0 , SHOWHEAD, CNV_INT);
			_SetGUIPropertyCNV (0 , MONSTERID, CNV_INT);
			_SetGUIPropertyCNV (0 , EMTPATH, CNV_WCHAR);

		}
		//////////////////////////////////////////////////////////////////////////
		ControlAvatar::~ControlAvatar()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlTextarea::ControlTextarea(int nType,Element* parent): XmlElement(nType,parent,COUNT) 
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIPropertyCNV (0 , PATH, CNV_WCHAR);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlTextarea::~ControlTextarea()
		{

		}


	}
}