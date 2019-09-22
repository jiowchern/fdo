#include "stdafx.h"
#include "Element.h"

namespace GUI
{
	//////////////////////////////////////////////////////////////////////////
	// 		template<typename T>
	// 		bool Get(T& val) const 
	// 		{
	// 			if(bValid)
	// 				val = (T)valvp;
	// 			return bValid;
	// 		}
	//////////////////////////////////////////////////////////////////////////
	Property::Property() : valn64(0) ,nMemSize(0)
	{
		memset(buf,0,sizeof(char) * BUFFER_SIZE);
	}
	//////////////////////////////////////////////////////////////////////////
	Property::~Property()
	{
		Release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Property::Release()
	{
		_FreePointer(valvp);

	}
	//////////////////////////////////////////////////////////////////////////
	void Property::_FreePointer(void * p)
	{
		if(nMemSize )
		{
			delete [] valvp;
			nMemSize = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// operators
	//////////////////////////////////////////////////////////////////////////
	void Property::operator = (unsigned int val)
	{
		_FreePointer(valvp);
		valn32 = val;

	}
	//////////////////////////////////////////////////////////////////////////
	void Property::operator = (int val)
	{
		_FreePointer(valvp);
		valsn32 = val;

	}
	//////////////////////////////////////////////////////////////////////////
	void Property::operator = (float val)
	{
		_FreePointer(valvp);
		valf32 = val;

	}
	//////////////////////////////////////////////////////////////////////////
	void Property::operator = (const char* val)
	{
		_FreePointer(valvp);
		
		int nLen = strlen(val);
		vals8 = new char[nLen+1]; // +1 is \0
		memcpy(vals8,val,sizeof(char) * nLen);
		vals8[nLen] = 0;

		nMemSize = nLen+1;
	}
	//////////////////////////////////////////////////////////////////////////
	void Property::operator = (const wchar_t* val)
	{
		_FreePointer(valvp);

		
		int nLen = wcslen(val);
		vals16 = new wchar_t[nLen+1]; // +1 is \0
		memcpy(vals8,val,sizeof(wchar_t) * nLen);
		vals16[nLen] = 0;

		nMemSize = sizeof(wchar_t) * (nLen+1);
	}
	//////////////////////////////////////////////////////////////////////////
	void Property::operator = (void* val)
	{
		_FreePointer(valvp);
		valvp = val;

	}
	//////////////////////////////////////////////////////////////////////////
	void Property::operator = (const Property& p)
	{
		_FreePointer(valvp);
		if(p.nMemSize)
		{
			valvp = (void*)new uint8[p.nMemSize];
			memcpy(valvp,p.valvp,p.nMemSize);
		}
		else
		{
			memcpy(buf,p.buf,BUFFER_SIZE);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void* Property::operator new(size_t size)
	{
		return PropertyFactory::GetSingleton().Create();
	}
	//////////////////////////////////////////////////////////////////////////
	void Property::operator delete(void* p)
	{
		PropertyFactory::GetSingleton().Destroy(p);
	}
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	Element::Slot::Slot() : pProperty(0)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	Element::Slot::~Slot() 
	{
		Release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Slot::Init()
	{
		if(pProperty == 0)
		{
			pProperty = new Property;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Slot::Release()
	{
		delete pProperty;
		pProperty = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Element::Slot::IsValid() const 
	{
		return pProperty != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	Property* Element::Slot::Get()
	{
		return pProperty;
	}
	//////////////////////////////////////////////////////////////////////////

	Element::Element(int type,Element* pParent,int nPropertySize) 
		: mpParent(pParent) ,mPropertys(0) ,mnPropertySize(nPropertySize) ,mType(type)
	{
		assert(mPropertys == 0);
		
		if(nPropertySize)
		{
			mPropertys = new Slot[nPropertySize];		
		}
		if(pParent)
			pParent->Add(this);
	}
	//////////////////////////////////////////////////////////////////////////
	Element::~Element()
	{
		if(mPropertys)
		{
			for (int i = 0 ; i < mnPropertySize; i++)
			{
				if(mPropertys[i].IsValid())
					mPropertys[i].Release();
			}
			delete [] mPropertys;
			mPropertys = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::_CheckValid(int iIndex)
	{
		mPropertys[iIndex].Init();		
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Set(int iIndex,int val)
	{
		if(mnPropertySize > iIndex)
		{
			_CheckValid(iIndex);
			*mPropertys[iIndex].Get() = val;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Set(int iIndex,unsigned int val)
	{
		if(mnPropertySize > iIndex)
		{
			_CheckValid(iIndex);
			*mPropertys[iIndex].Get() = val;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Set(int iIndex,float val)
	{
		if(mnPropertySize > iIndex)
		{
			_CheckValid(iIndex);
			*mPropertys[iIndex].Get() = val;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Set(int iIndex,const char* val)
	{
		if(mnPropertySize > iIndex)
		{
			_CheckValid(iIndex);
			*mPropertys[iIndex].Get() = val;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Set(int iIndex,const wchar_t* val)
	{
		if(mnPropertySize > iIndex)
		{
			_CheckValid(iIndex);
			*mPropertys[iIndex].Get() = val;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Set(int iIndex,const char* buf,int size)
	{
		if(mnPropertySize > iIndex)
		{
			assert(0);
			//mPropertys[iIndex] = val;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Set(int iIndex,const Property* p)						
	{
		if(mnPropertySize > iIndex)
		{
			_CheckValid(iIndex);
			*mPropertys[iIndex].Get() = *p;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Set(int iIndex,void* p)
	{
		if(mnPropertySize > iIndex)		
		{
			_CheckValid(iIndex);
			*mPropertys[iIndex].Get() = p;
		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::Add(Element* pChild)
	{
		mChilds.push_back(pChild);
	}

	//////////////////////////////////////////////////////////////////////////

	bool Element::Get(int iIndex,int& val)const
	{	
		
		if(mnPropertySize > iIndex && mPropertys[iIndex].IsValid())
		{
			val = mPropertys[iIndex].Get()->valsn32;
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Element::Get(int iIndex,unsigned int& val)		const
	{
		
		if(mnPropertySize > iIndex && mPropertys[iIndex].IsValid())
		{
			val = mPropertys[iIndex].Get()->valn32;
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Element::Get(int iIndex,float& val)	const
	{
		
		if(mnPropertySize > iIndex && mPropertys[iIndex].IsValid())
		{
			val = mPropertys[iIndex].Get()->valf32;
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Element::Get(int iIndex,char*& val)	const
	{
		
		if(mnPropertySize > iIndex && mPropertys[iIndex].IsValid())
		{
			val = mPropertys[iIndex].Get()->vals8;
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Element::Get(int iIndex,wchar_t*& val)		const
	{
		
		if(mnPropertySize > iIndex && mPropertys[iIndex].IsValid())
		{
			val = mPropertys[iIndex].Get()->vals16;
			return true;
		}
		return false;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool Element::Get(int iIndex,char*& val,int size)	const
	{
		
		if(mnPropertySize > iIndex && mPropertys[iIndex].IsValid())
		{
			memcpy(val,mPropertys[iIndex].Get()->valvp,sizeof(Property));
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Element::Get(int iIndex,void*& val) const
	{
		
		if(mnPropertySize > iIndex && mPropertys[iIndex].IsValid())
		{
			val = mPropertys[iIndex].Get()->valvp;
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const Property*	Element::Get(int iIndex) const
	{
		return _Get(iIndex);
	}
	//////////////////////////////////////////////////////////////////////////
	Property* Element::_Get(int iIndex) const
	{
		if(mnPropertySize > iIndex && mPropertys[iIndex].IsValid())
			return mPropertys[iIndex].Get();
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	Element* Element::GetParent()
	{
		return mpParent;
	}
	//////////////////////////////////////////////////////////////////////////
	const Element*	Element::GetParent() const 
	{
		return mpParent;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Element::GetChild(std::list<const Element*>& set)	const
	{
		std::list<Element*>::const_iterator it = mChilds.begin();
		for ( ; it!=mChilds.end();++it)
		{
			set.push_back(*it);
		}
		return mChilds.size() > 0;		
	}
	//////////////////////////////////////////////////////////////////////////
	int Element::GetType() const
	{
		return mType;
	}
	//////////////////////////////////////////////////////////////////////////
	void Element::fnDestoryElementTree(Element* pRoot)
	{
		std::list<Element*>::iterator it = pRoot->mChilds.begin();
		for( ; it != pRoot->mChilds.end(); ++it)
		{
			fnDestoryElementTree(*it);
		}
		delete pRoot;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void fnSetGuiElement_Int(Element* pEle,int iProperty,const wchar_t* value)
	{
		pEle->Set(iProperty,_wtoi(value));
	}
	//////////////////////////////////////////////////////////////////////////
	void fnSetGuiElement_Float(Element* pEle,int iProperty,const wchar_t* value)
	{
		pEle->Set(iProperty,(float)_wtof(value));
	}
	//////////////////////////////////////////////////////////////////////////
	void fnSetGuiElement_wchar(Element* pEle,int iProperty,const wchar_t* value)
	{
		pEle->Set(iProperty,value);
	}
	//////////////////////////////////////////////////////////////////////////

	void fnSetGuiElement_ARGB(Element* pEle,int iProperty,const wchar_t* value)
	{
		int argb = -1;
		swscanf(value,L"0x%x",&argb);
		pEle->Set(iProperty,argb);
	}
	//////////////////////////////////////////////////////////////////////////
	void fnSetGuiElement_ARGBR(Element* pEle,int iProperty,const wchar_t* value)
	{
		int nRGBA = 0;			
		pEle->Get(iProperty ,nRGBA);
		char* rgba = (char*)&nRGBA;
		rgba[2] = _wtoi(value);
		pEle->Set(iProperty ,nRGBA);			
	}
	//////////////////////////////////////////////////////////////////////////
	void fnSetGuiElement_ARGBG(Element* pEle,int iProperty,const wchar_t* value)
	{
		int nRGBA = 0;			
		pEle->Get(iProperty ,nRGBA);
		char* rgba = (char*)&nRGBA;
		rgba[1] = _wtoi(value);
		pEle->Set(iProperty ,nRGBA);			
	}
	//////////////////////////////////////////////////////////////////////////
	void fnSetGuiElement_ARGBB(Element* pEle,int iProperty,const wchar_t* value)
	{
		int nRGBA = 0;			
		pEle->Get(iProperty ,nRGBA);
		char* rgba = (char*)&nRGBA;
		rgba[0] = _wtoi(value);
		pEle->Set(iProperty ,nRGBA);			
	}
	//////////////////////////////////////////////////////////////////////////
	void fnSetGuiElement_ARGBA(Element* pEle,int iProperty,const wchar_t* value)
	{
		int nRGBA = 0;			
		pEle->Get(iProperty ,nRGBA);
		char* rgba = (char*)&nRGBA;
		rgba[3] = _wtoi(value);
		pEle->Set(iProperty ,nRGBA);			
	}
	//////////////////////////////////////////////////////////////////////////
	void fnSetGuiElement_Hex(Element* pEle,int iProperty,const wchar_t* value)
	{
		int argb = -1;
		swscanf(value,L"0x%x",&argb);
		
		pEle->Set(iProperty,argb);
	}
	
	//////////////////////////////////////////////////////////////////////////
	XmlElement::PFN_SET_GUIELEMENT gvCNVs[] =
	{
		fnSetGuiElement_Int,
		fnSetGuiElement_Float,
		fnSetGuiElement_wchar,
		fnSetGuiElement_ARGB,
		fnSetGuiElement_ARGBR,
		fnSetGuiElement_ARGBG,
		fnSetGuiElement_ARGBB,
		fnSetGuiElement_ARGBA,
		fnSetGuiElement_Hex

	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	std::map<wstring,XmlElement::PFN_SET_GUIELEMENT> XmlElement::KeyWordMap;
	XmlElement::XmlElement(int type,Element* pParent,int nPropertySize)
		: Element(type,pParent,nPropertySize)
	{
		if(KeyWordMap.empty())
		{
			KeyWordMap[L"LEFT"] = fnSetGuiElement_Int;
			KeyWordMap[L"RIGHT"] = fnSetGuiElement_Int;
			KeyWordMap[L"TOP"] = fnSetGuiElement_Int;
			KeyWordMap[L"BOTTOM"] = fnSetGuiElement_Int;

			KeyWordMap[L"POSX"] = fnSetGuiElement_Int;
			KeyWordMap[L"POSY"] = fnSetGuiElement_Int;
			KeyWordMap[L"WIDTH"] = fnSetGuiElement_Int;
			KeyWordMap[L"HEIGHT"] = fnSetGuiElement_Int;

			KeyWordMap[L"POSDIMX"] = fnSetGuiElement_Float;
			KeyWordMap[L"POSDIMY"] = fnSetGuiElement_Float;
			KeyWordMap[L"WIDTHDIM"] = fnSetGuiElement_Float;
			KeyWordMap[L"HEIGHTDIM"] = fnSetGuiElement_Float;

			KeyWordMap[L"PATH"] = fnSetGuiElement_wchar;
			KeyWordMap[L"MIN"] = fnSetGuiElement_Int;
			KeyWordMap[L"MAX"] = fnSetGuiElement_Int;
			KeyWordMap[L"VALUE"] = fnSetGuiElement_Int;

			KeyWordMap[L"STYLE"] = fnSetGuiElement_Int;
			KeyWordMap[L"ROW"] = fnSetGuiElement_Int;
			KeyWordMap[L"COL"] = fnSetGuiElement_Int;
			KeyWordMap[L"ITEMWIDTH"] = fnSetGuiElement_Int;
			KeyWordMap[L"ITEMHEIGHT"] = fnSetGuiElement_Int;

			KeyWordMap[L"TEXT"] = fnSetGuiElement_Int;
			KeyWordMap[L"AMOUNT"] = fnSetGuiElement_Int;
			KeyWordMap[L"MONEY"] = fnSetGuiElement_Int;
			KeyWordMap[L"TEXTCOLOR"]	= fnSetGuiElement_ARGB;
			KeyWordMap[L"TEXTCOLOR_R"]	= fnSetGuiElement_ARGBR;
			KeyWordMap[L"TEXTCOLOR_G"]	= fnSetGuiElement_ARGBG;
			KeyWordMap[L"TEXTCOLOR_B"]	= fnSetGuiElement_ARGBB;
			KeyWordMap[L"TEXTCOLOR_A"]	= fnSetGuiElement_ARGBA;

		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	XmlElement::~XmlElement	()
	{
		
	}
	
	//////////////////////////////////////////////////////////////////////////
	bool XmlElement::SetAttrib(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) 
	{
		std::map<wstring,SetInfo>::iterator it =  mKeyWordIndexMap.find(attr);
		if(it != mKeyWordIndexMap.end())
		{
			assert(it->second.pfn);
			if(it->second.pfn)
			{
				it->second.pfn(this,it->second.iProperty,value)	;
				return true;
			}
		}
		else
		{
			
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlElement::_SetPropertySet(int index,const wchar_t* attrib,XmlElement::PFN_SET_GUIELEMENT pfnCustom /*= 0*/)
	{
		std::wstring attr(attrib);
		std::transform(attr.begin() , attr.end() , attr.begin() ,towupper);

		mKeyWordIndexMap[attr].iProperty = index;
		(pfnCustom == 0)?
			mKeyWordIndexMap[attr].pfn = fnGetPropertySets(attr.c_str()) :
			mKeyWordIndexMap[attr].pfn = pfnCustom;

		assert(mKeyWordIndexMap[attr].pfn);
		return mKeyWordIndexMap[attr].pfn!=0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlElement::_SetPropertySet(int index,const wchar_t* att,int nCNV)
	{
		std::wstring attrib(att);
		
		std::transform(attrib.begin() , attrib.end() , attrib.begin() ,towupper);

		mKeyWordIndexMap[attrib].iProperty = index;
		mKeyWordIndexMap[attrib].pfn = 0;

		if(nCNV < CNVS)
			mKeyWordIndexMap[attrib].pfn = gvCNVs[nCNV];

		assert(mKeyWordIndexMap[attrib].pfn);
		return mKeyWordIndexMap[attrib].pfn!=0;
	}
	//////////////////////////////////////////////////////////////////////////

	XmlElement::PFN_SET_GUIELEMENT XmlElement::fnGetPropertySets(const wchar_t* att)
	{
		std::wstring attrib(att);
		std::transform(attrib.begin() , attrib.end() , attrib.begin() ,towupper);
		std::map<wstring,PFN_SET_GUIELEMENT>::iterator it = KeyWordMap.find(attrib);
		if(it != KeyWordMap.end())
		{
			return it->second;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	ElementSet::ElementSet() 
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	ElementSet::~ElementSet()
	{
		struct deleteelement
		{
			void operator()(Element* ele)
			{
				delete ele;
			}
		};
		deleteelement de;
		for_each(mElements.begin(),mElements.end(),de);
		mElements.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void ElementSet::Add(Element* p)
	{
		mElements.push_back(p);
	}
	//////////////////////////////////////////////////////////////////////////
		
	/************************************************************************/	
	/*                                                                      		
		L"ROOT" , EleRoot , L"FONT_HEIGHT"	,nFontHeight
		L"ROOT" , EleRoot , L"FONT"			,fontName
		
		
	*/
	/************************************************************************/

}