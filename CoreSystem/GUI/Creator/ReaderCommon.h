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
				POSX,		//�����mx ���:����
				POSY,		//�����my ���:����
				POSDIMX,		//�����mx ���:�ʤ���
				POSDIMY	,	//�����my ���:�ʤ���
				WIDTH	,	//����e��  ���:����	
				HEIGHT	,	//���󰪫�  ���:����	
				WIDTHDIM,	//����e��  ���:�ʤ���	
				HEIGHTDIM,	//���󰪫�  ���:�ʤ���	
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
				POSX,		//�����mx ���:����
				POSY,		//�����my ���:����				
				WIDTH	,	//����e��  ���:����	
 				HEIGHT	,	//���󰪫�  ���:����	
// 				POSDIMX,		//�����mx ���:�ʤ���
// 				POSDIMY	,	//�����my ���:�ʤ���
// 				WIDTHDIM,	//����e��  ���:�ʤ���	
// 				HEIGHTDIM,	//���󰪫�  ���:�ʤ���	
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