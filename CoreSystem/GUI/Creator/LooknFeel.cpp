#include "stdafx.h"
#include "LooknFeel.h"
#define _IsLabel(label,attrib) wcsicmp(label,attrib) == 0
#define _SetGUIPropertyCNV(offset,label,cnv_type) _SetPropertySet(offset + label,L#label,cnv_type)
namespace GUI
{
	namespace LooknFeel
	{

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		const wchar_t* gLabel[CONTROL_TYPES] =
		{
			L"",
			L"CONTROL",
			L"ELEMENT",
			L"FONT",
			L"COLOR",
			L"IMAGE",
			L"COLOR",
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		const wchar_t* fnGetLabel(int controlType)
		{
			if(controlType < CONTROL_TYPES)
				return gLabel[controlType];
			assert(0);
			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		void fnGetBase(Element* pEle,int iProperty,const wchar_t* val)
		{			
			pEle->Set(iProperty,Control::fnGetBase(val));
		}
		//////////////////////////////////////////////////////////////////////////
		Control::Control(Element* parent) : XmlElement(CONTROL,parent,COUNT)
		{
			assert( parent == 0 || parent->GetType() == CONTROL);
			_SetGUIPropertyCNV(0,TYPE,CNV_WCHAR);
			_SetGUIPropertyCNV(0,BASE,LooknFeel::fnGetBase);
			_SetGUIPropertyCNV(0,FRAMEGAUGE,CNV_INT);
		}
		//////////////////////////////////////////////////////////////////////////
		Control::~Control()
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
// 		void Control::SetAttrib(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) 
// 		{
// 			if( _IsLabel(attr,L"TYPE") )
// 			{			
// 				Set(TYPE,value);				
// 			}
// 			else if( _IsLabel(attr,L"BASE") )
// 			{
// 				Set(BASE,fnGetBase(value));
// 			}
// 			else if(_IsLabel(attr,L"FRAMEGAUGE"))
// 			{
// 				Set(FRAMEGAUGE,_wtoi(value));
// 			}
// 		}
		
		//////////////////////////////////////////////////////////////////////////
		int Control::fnGetBase(const wchar_t* val)
		{
			if(wcsicmp(L"Dialog",val) == 0)
			{
				return Dialog;
			}
			if(wcsicmp(L"Button",val) == 0)
			{
				return Button;
			}
			if(wcsicmp(L"Static",val) == 0)
			{
				return Static;
			}
			if(wcsicmp(L"CheckBox",val) == 0)
			{
				return CheckBox;
			}
			if(wcsicmp(L"ComboBox",val) == 0)
			{
				return ComboBox;
			}
			if(wcsicmp(L"IMEEditBox",val) == 0)
			{
				return IMEEditBox;
			}
			if(wcsicmp(L"ListBox",val) == 0)
			{
				return ListBox;
			}
			if(wcsicmp(L"ScrollBar",val) == 0)
			{
				return ScrollBar;
			}
			if(wcsicmp(L"RadioBox",val) == 0)
			{
				return RadioBox;
			}
			if(wcsicmp(L"Image",val) == 0)
			{
				return Image;
			}
			if(wcsicmp(L"Slider",val) == 0)
			{
				return Slider;
			}
			if(wcsicmp(L"IconListBox",val) == 0)
			{
				return IconListBox;
			}
			if(wcsicmp(L"ImageListBox",val) == 0)
			{
				return ImageListBox;
			}
			if(wcsicmp(L"EditBox",val) == 0)
			{
				return EditBox;
			}
			if(wcsicmp(L"DynamicText",val) == 0)
			{
				return DynamicText;
			}
			if(wcsicmp(L"TreeList",val) == 0)
			{
				return TreeList;
			}
			if(wcsicmp(L"ImageBox",val) == 0)
			{
				return ImageBox;
			}
			if(wcsicmp(L"ProgressBar",val) == 0)
			{
				return ProgressBar;
			}
			if(wcsicmp(L"VariableHeightListBox",val) == 0)
			{
				return VariableHeightListBox;
			}
			if(wcsicmp(L"TDGItemBox",val) == 0)
			{
				return TDGItemBox;
			}
			if(wcsicmp(L"TabPage",val) == 0)
			{
				return TabPage;
			}
			if(wcsicmp(L"SubDialog",val) == 0)
			{
				return SubDialog;
			}
			if(wcsicmp(L"ListView",val) == 0)
			{
				return ListView;
			}
			if(wcsicmp(L"Avatar",val) == 0)
			{
				return Avatar;
			}
			if(wcsicmp(L"Textarea",val) == 0)
			{
				return Textarea;
			}
			
			assert(0);
			return Button;
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlElement::ControlElement(Element* parent) :  XmlElement(CONTROL_ELEMENT,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL);
			_SetGUIPropertyCNV(0,ID,CNV_INT);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlElement::~ControlElement	()
		{

		}
		//////////////////////////////////////////////////////////////////////////
// 		void ControlElement::SetAttrib(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) 
// 		{
// 			if( _IsLabel(attr,L"ID") )
// 				Set(GUI::LooknFeel::ControlElement::ID,_wtoi(value));
// 		}
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
// 		void ControlElementFont::SetAttrib(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) 
// 		{
// 			if(wcsicmp(attr,L"R") == 0)
// 			{		
// 				Common::Color::fnSetColor(this,RGBA,2,value);								
// 			}
// 			else if(wcsicmp(attr,L"G") == 0)
// 			{
// 				Common::Color::fnSetColor(this,RGBA,1,value);								
// 			}
// 			else if(wcsicmp(attr,L"B") == 0)
// 			{
// 				Common::Color::fnSetColor(this,RGBA,0,value);					
// 			}
// 			else if(wcsicmp(attr,L"A") == 0)
// 			{
// 				Common::Color::fnSetColor(this,RGBA,3,value);					
// 			}
// 			else
// 			{
// 				fnSetAttrib(this,attr,nAttrLen,value,nValLen);
// 			}			
// 		}
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
		ControlElementImage::ControlElementImage(Element* parent) 
			: XmlElement(CONTROL_ELEMENT_IMAGE,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL_ELEMENT);
			/************************************************************************/
			/* SOURCE 在上衣曾被處理掉了                                                                     */
			/************************************************************************/
			_SetPropertySet(RGBA,L"ARGB",CNV_ARGB);
			_SetPropertySet(RGBA,L"R",CNV_ARGBR);
			_SetPropertySet(RGBA,L"G",CNV_ARGBG);
			_SetPropertySet(RGBA,L"B",CNV_ARGBB);
			_SetPropertySet(RGBA,L"A",CNV_ARGBA);
		}
		//////////////////////////////////////////////////////////////////////////			
		ControlElementImage::~ControlElementImage()
		{

		}
			
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlElementImageColor::ControlElementImageColor(Element* parent)
			: XmlElement(CONTROL_ELEMENT_IMAGE_COLOR,parent,COUNT)
		{
			assert(parent->GetType() == CONTROL_ELEMENT_IMAGE);
			_SetGUIPropertyCNV(0,STATE,Common::fnSetColorState);
			_SetPropertySet(RGBA,L"ARGB",CNV_ARGB);
			_SetPropertySet(RGBA,L"R",CNV_ARGBR);
			_SetPropertySet(RGBA,L"G",CNV_ARGBG);
			_SetPropertySet(RGBA,L"B",CNV_ARGBB);
			_SetPropertySet(RGBA,L"A",CNV_ARGBA);
		}
		//////////////////////////////////////////////////////////////////////////
		ControlElementImageColor::~ControlElementImageColor()
		{

		}		

		///////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ControlSet::ControlSet	()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		ControlSet::~ControlSet	()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		bool ControlSet::Add(const wchar_t* type,Control* ctrl)
		{
			wstring key = type;
			std::map<wstring,Control*>::iterator it =  mControls.find(key);
			if(it == mControls.end())
			{
				mControls[key] = ctrl;
				return true;
			}
			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		const Control* ControlSet::Get(const wchar_t* type) const
		{
			wstring key = type;
			std::map<wstring,Control*>::const_iterator it =  mControls.find(key);
			if(it != mControls.end())
			{
				return it->second;
			}
			return 0;
		}
		
	};
	
}