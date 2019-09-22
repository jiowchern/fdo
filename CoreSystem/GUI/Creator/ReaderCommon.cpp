#include "stdafx.h"
#include "ReaderCommon.h"

namespace GUI
{
	namespace Common
	{
		Texture::Texture(int nType,Element* pParent) : XmlElement(nType,pParent,COUNT)
		{
			_SetGUIPropertyCNV(0,POSX,CNV_INT);
			_SetGUIPropertyCNV(0,POSY,CNV_INT);
			_SetGUIPropertyCNV(0,WIDTH,CNV_INT);
			_SetGUIPropertyCNV(0,HEIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,LEFT,CNV_INT);
			_SetGUIPropertyCNV(0,TOP,CNV_INT);
			_SetGUIPropertyCNV(0,RIGHT,CNV_INT);
			_SetGUIPropertyCNV(0,BOTTOM,CNV_INT);
			_SetGUIPropertyCNV(0,PATH,CNV_WCHAR);
		}
		//////////////////////////////////////////////////////////////////////////
		Texture::~Texture()
		{

		}

		void Color::fnSetColor(GUI::Element* pEle,int iProperty,int iBGRA,const wchar_t* value)
		{
			int nRGBA = 0;			
			pEle->Get(iProperty ,nRGBA);
			char* rgba = (char*)&nRGBA;
			rgba[iBGRA] = _wtoi(value);
			pEle->Set(iProperty ,nRGBA);			
		}
		//////////////////////////////////////////////////////////////////////////
		void Color::fnSetAttrib(GUI::Element* pEle,const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
		{
			if(wcsicmp(attr,L"STATE") == 0)
			{
				pEle->Set(STATE , fnGetState(value));			
			}
			else if(wcsicmp(attr,L"R") == 0)
			{		
				fnSetColor(pEle,RGBA,2,value);								
			}
			else if(wcsicmp(attr,L"G") == 0)
			{
				fnSetColor(pEle,RGBA,1,value);								
			}
			else if(wcsicmp(attr,L"B") == 0)
			{
				fnSetColor(pEle,RGBA,0,value);					
			}
			else if(wcsicmp(attr,L"A") == 0)
			{
				fnSetColor(pEle,RGBA,3,value);					
			}
			else if(wcsicmp(attr,L"ARGB") == 0)
			{
				int argb;
				swscanf(value,L"0x%x",&argb);
				pEle->Set(RGBA ,argb);			
			}
		}
		//////////////////////////////////////////////////////////////////////////
		int Color::fnGetState(const wchar_t* val)
		{
			if(wcsicmp(val,L"Normal") == 0)
			{
				return Normal;
			}
			if(wcsicmp(val,L"Disable") == 0)
			{
				return Disable;
			}
			if(wcsicmp(val,L"Hidden") == 0)
			{
				return Hidden;
			}
			if(wcsicmp(val,L"Focus") == 0)
			{
				return Focus;
			}
			if(wcsicmp(val,L"MouseOver") == 0)
			{
				return MouseOver;
			}
			if(wcsicmp(val,L"Pressed") == 0)
			{
				return Pressed;
			}
			if(wcsicmp(val,L"Unfocus") == 0)
			{
				return Unfocus;
			}	
			assert(0);
			return Normal;
		}
		//////////////////////////////////////////////////////////////////////////
		void fnSetColorState(Element* pEle,int iProperty,const wchar_t* val)
		{
			pEle->Set(iProperty,Color::fnGetState(val));
		}
		//////////////////////////////////////////////////////////////////////////
		int	Font::fnGetAlign(const wchar_t* val)
		{
			int nVal = 0;				
			swscanf(val,L"0x%x",&nVal);
			return nVal;
		}
		//////////////////////////////////////////////////////////////////////////
		int	Font::fnGetVHAlign(const wchar_t* val)
		{
#define  _cmpType(type) if(wcsicmp(val,L#type) == 0) { return type; }
			_cmpType(Left);
			_cmpType(Top);
			_cmpType(Center);
			_cmpType(Bottom);
			_cmpType(Right);
			assert(0);
			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		void fnSetFontAlign(Element* pEle,int iProperty,const wchar_t* val)
		{			
			pEle->Set(iProperty,Font::fnGetVHAlign(val));
		}
		//////////////////////////////////////////////////////////////////////////
		void Font::fnSetAttrib(GUI::Element* pEle,const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
		{
			if(wcsicmp(attr,L"ALIGN") == 0)
			{
				pEle->Set(ALIGN,fnGetAlign(value));
				return;
			}
			if(wcsicmp(attr,L"VALIGN") == 0)
			{
				pEle->Set(VALIGN,fnGetVHAlign(value));
				return;
			}
			if(wcsicmp(attr,L"HALIGN") == 0)
			{
				pEle->Set(HALIGN,fnGetVHAlign(value));
				return;
			}
		}

	};
}