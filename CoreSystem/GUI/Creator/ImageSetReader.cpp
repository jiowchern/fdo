#include "stdafx.h"
#include "ImageSetReader.h"
namespace FDO
{

	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	GUIImageSetReader::GUIImageSetReader() 
	{
		IXmlElementEventBase* pEve;
		
		// «Ø¥ß¶¥¼h
		// 0
		mDegrees.push_back(new RootXmlDegree());		

		// 1
		pEve = new TXmlElementEvent<GUIImageSetReader>(
			this,
			&GUIImageSetReader::XmlStartImage,
			&GUIImageSetReader::XmlEndImage,
			&GUIImageSetReader::XmlAttribImage,
			0	);
		
		mDegrees.push_back(new XmlDegree(mDegrees[0],L"IMAGE",pEve));

		pEve = new TXmlElementEvent<GUIImageSetReader>(
			this,
			&GUIImageSetReader::XmlStartSource,
			&GUIImageSetReader::XmlEndSource,
			&GUIImageSetReader::XmlAttribSource,
			0	);
		
		mDegrees.push_back(new XmlDegree(mDegrees[0],L"SOURCE",pEve));

		_ReadyParse(mDegrees[0]);
	}
	//////////////////////////////////////////////////////////////////////////
	GUIImageSetReader::~GUIImageSetReader()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void GUIImageSetReader::LoadXML(const wchar_t* xmlText,GUI::ImageSet* pSet)
	{
		_ClearTempData();
		mTemp.pSet = pSet;		
		LoadInfo info;		
		info.xmlText	= xmlText;				
		_LoadXML(info);
		pSet->Finish();
	}
	//////////////////////////////////////////////////////////////////////////
	void GUIImageSetReader::_ClearTempData()
	{
		memset(&mTemp,0,sizeof(mTemp));		
	}
	//////////////////////////////////////////////////////////////////////////
	void GUIImageSetReader::XmlStartImage()
	{		
		mTemp.img.nSourceId= 0;
		mTemp.img.rect.left = 0;
		mTemp.img.rect.top = 0;
		mTemp.img.rect.right = 0;
		mTemp.img.rect.bottom = 0;
		mTemp.w = 0;
		mTemp.h = 0;
		mTemp.imagename.clear();
		
	}
	//////////////////////////////////////////////////////////////////////////
	void GUIImageSetReader::XmlEndImage()
	{
		if(mTemp.imagename.length())
		{
			if(mTemp.w)
				mTemp.img.rect.right = mTemp.img.rect.left + mTemp.w;
			if(mTemp.h)
				mTemp.img.rect.bottom = mTemp.img.rect.top + mTemp.h;			
			mTemp.pSet->AddImage(mTemp.imagename.c_str(),mTemp.img);
		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	void GUIImageSetReader::XmlAttribImage(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		wstring sattr;
		sattr = attr;
		if(sattr == L"NAME")
		{
			mTemp.imagename = value;
		}
		else if(sattr == L"X")
		{
			mTemp.img.rect.left = _wtoi(value);
		}
		else if(sattr == L"Y")
		{
			mTemp.img.rect.top = _wtoi(value);
		}
		else if(sattr == L"W")
		{
			mTemp.w = _wtoi(value);			
		}
		else if(sattr == L"H")
		{
			mTemp.h = _wtoi(value);
		}
		else if(sattr == L"LEFT")
		{
			mTemp.img.rect.left = _wtoi(value);
		}
		else if(sattr == L"TOP")
		{
			mTemp.img.rect.top = _wtoi(value);
		}
		else if(sattr == L"RIGHT")
		{
			mTemp.img.rect.right = _wtoi(value);			
		}
		else if(sattr == L"BOTTOM")
		{
			mTemp.img.rect.bottom = _wtoi(value);
		}
		else if(sattr == L"SOURCE")
		{
			mTemp.img.nSourceId =  _wtoi(value);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GUIImageSetReader::XmlStartSource()
	{
		mTemp.src.path.clear();
		mTemp.srcid = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void GUIImageSetReader::XmlEndSource()
	{
		if(mTemp.srcid)
		{
			mTemp.pSet->AddSource(mTemp.srcid,mTemp.src);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GUIImageSetReader::XmlAttribSource(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		wstring sattr;
		sattr = attr;
		if(sattr == L"ID")
		{
			mTemp.srcid = _wtoi(value);
		}
		else if(sattr == L"PATH")
		{
			mTemp.src.path = UnicodeToAnsi(value);
		}
	}
}

