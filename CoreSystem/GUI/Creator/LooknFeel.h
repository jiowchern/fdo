#ifndef _LooknFeel_20080822am1019_
#define _LooknFeel_20080822am1019_
#include "ReaderCommon.h"
#include "Element.h"
namespace GUI
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	namespace LooknFeel
	{
		enum
		{
			INVALID_TYPE,
			CONTROL,
			CONTROL_ELEMENT,
			CONTROL_ELEMENT_FONT,
			CONTROL_ELEMENT_FONT_COLOR,
			CONTROL_ELEMENT_IMAGE,			
			CONTROL_ELEMENT_IMAGE_COLOR,			
			CONTROL_TYPES
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		const wchar_t* fnGetLabel(int controlType);		
		

		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////		
		class Control : public XmlElement
		{

		public:
			enum
			{
				TYPE,
				BASE,
				FRAMEGAUGE,
				COUNT
			};

			// BASE types
			enum
			{
				Dialog,
				Button,
				Static,
				CheckBox,
				ComboBox,
				IMEEditBox,
				ListBox,
				ScrollBar,
				RadioBox,
				Slider,
				Image,
				IconListBox,
				ImageListBox,
				DynamicText,
				EditBox,
				TreeList,
				ImageBox,
				ProgressBar,				
				VariableHeightListBox,
				TDGItemBox,
				TabPage,
				SubDialog,
				ListView,
				Avatar,
				Textarea,
				// 在這裡新增元件類型 ...
			};
							Control		(Element* parent);
							~Control	();
			static int		fnGetBase	(const wchar_t* val);
			//////////////////////////////////////////////////////////////////////////			
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlElement : public XmlElement
		{
		public:
			enum
			{
				ID,
				COUNT
			};			
							ControlElement	(Element* parent);
							~ControlElement	();
			//virtual void	SetAttrib		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) ;
		};
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
		class ControlElementImage : public XmlElement
		{
		public:	
			enum
			{
				SOURCE,
				RGBA,
				COUNT
			};
							ControlElementImage		(Element* parent);
							~ControlElementImage	();						
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlElementImageColor : public XmlElement , public Common::Color
		{
		public:
			enum
			{
				COUNT = Common::Color::COUNT
			};			
							ControlElementImageColor	(Element* parent);
							~ControlElementImageColor	();			
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		class ControlSet : public ElementSet
		{
			std::map<wstring,Control*> mControls;
		public:

							ControlSet	();
			virtual			~ControlSet	();
			bool			Add			(const wchar_t* type,Control* ctrl);			
			const Control*	Get			(const wchar_t* type) const;
		};
		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// 以下新增Control Element

	};
};
#define GUI_LOOKNFEEL_XML_LABEL(control) GUI::LooknFeel::fnGetLabel(control)
#endif
