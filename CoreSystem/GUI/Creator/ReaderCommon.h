#ifndef _ReaderCommon_H_20090717_
#define _ReaderCommon_H_20090717_
#include "Element.h"

namespace GUI
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	namespace Common
	{
		/*

		*/
		struct Color
		{
			enum
			{
				Normal,
				Disable,
				Hidden,
				Focus,
				MouseOver,
				Pressed,
				Unfocus
			};
			enum
			{	STATE,RGBA,COUNT	};
			static int	fnGetState	(const wchar_t* val);
			static void	fnSetAttrib	(GUI::Element* pEle,const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);
			static void fnSetColor	(GUI::Element* pEle,int iProperty,int iARGB,const wchar_t* value);
		};
		//////////////////////////////////////////////////////////////////////////
		struct Font
		{

			enum
			{
				ALIGN,
				VALIGN,
				HALIGN,
				COUNT
			};

			enum
			{
				Left,Top,Center,Bottom,Right
			};
			static int	fnGetAlign		(const wchar_t* val);
			static int	fnGetVHAlign	(const wchar_t* val);
			static void	fnSetAttrib		(GUI::Element* pEle,const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);
		};

		struct Rect
		{
			enum
			{
				LEFT,
				TOP,
				RIGHT,
				BOTTOM,
				POSX,		//元件位置x 單位:像素
				POSY,		//元件位置y 單位:像素
				POSDIMX,		//元件位置x 單位:百分比
				POSDIMY	,	//元件位置y 單位:百分比
				WIDTH	,	//元件寬度  單位:像素	
				HEIGHT	,	//元件高度  單位:像素	
				WIDTHDIM,	//元件寬度  單位:百分比	
				HEIGHTDIM,	//元件高度  單位:百分比	
				COUNT
			};
			
		};

		class Texture : public GUI::XmlElement
		{
		public:
			enum
			{
				PATH,
				LEFT,
				TOP,
				RIGHT,
				BOTTOM,
				POSX,		//元件位置x 單位:像素
				POSY,		//元件位置y 單位:像素				
				WIDTH	,	//元件寬度  單位:像素	
 				HEIGHT	,	//元件高度  單位:像素	
// 				POSDIMX,		//元件位置x 單位:百分比
// 				POSDIMY	,	//元件位置y 單位:百分比
// 				WIDTHDIM,	//元件寬度  單位:百分比	
// 				HEIGHTDIM,	//元件高度  單位:百分比	
				COUNT
			};
			Texture(int nType,Element* pParent);
			~Texture();
		};

		void fnSetColorState(Element* pEle,int iProperty,const wchar_t* val) ;
		void fnSetFontAlign(Element* pEle,int iProperty,const wchar_t* val) ;
	};

	
}









#endif