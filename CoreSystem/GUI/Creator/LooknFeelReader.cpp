#include "stdafx.h"
#include "LooknFeelReader.h"

#define _set_degree(class_name,pfn_start,pfn_end,pfn_attrib,parent_index,control_type) \
	pEve = new TXmlElementEvent<class_name>( \
	this, \
	&pfn_start,\
	&pfn_end,\
	&pfn_attrib,\
	0	);mDegrees.push_back(new XmlDegree(mDegrees[parent_index],GUI_LOOKNFEEL_XML_LABEL(control_type),pEve))
	
/*
pEve = new TXmlElementEvent<GUILooknFeelReader>(
this,
&GUILooknFeelReader::XmlStartControl,
&GUILooknFeelReader::XmlEndControl,
&GUILooknFeelReader::XmlAttribControl,
0	);
mEvents.push_back(pEve);
mDegrees.push_back(new XmlDegree(mDegrees[0],GUI_LOOKNFEEL_XML_LABEL(CONTROL),pEve));
*/
namespace FDO
{

	GUILooknFeelReader::GUILooknFeelReader(const GUI::ImageSet* pSet) : mpSet(0) ,mpImgs(pSet)
	{
		IXmlElementEventBase* pEve;

		// 建立階層
		// 0 // root
		mDegrees.push_back(new RootXmlDegree());		


		// 1 // control
		pEve = new TXmlElementEvent<GUILooknFeelReader>(
			this,
			&GUILooknFeelReader::XmlStartControl,
			&GUILooknFeelReader::XmlEndControl,
			&GUILooknFeelReader::XmlAttribControl,
			0	);
		
		mDegrees.push_back(new XmlLoopDegree(mDegrees[0],GUI_LOOKNFEEL_XML_LABEL(GUI::LooknFeel::CONTROL),pEve));
		
		// 2 control//element
		_set_degree(GUILooknFeelReader
			,GUILooknFeelReader::XmlStartControlElement
			,GUILooknFeelReader::XmlEndCommon
			,GUILooknFeelReader::XmlAttribCommon
			,1
			,GUI::LooknFeel::CONTROL_ELEMENT);

		// 3 control//element//font
		_set_degree(GUILooknFeelReader
			,GUILooknFeelReader::XmlStartControlElementFont
			,GUILooknFeelReader::XmlEndCommon
			,GUILooknFeelReader::XmlAttribCommon
			,2
			,GUI::LooknFeel::CONTROL_ELEMENT_FONT);

		// 4 control//element//font/color
		_set_degree(GUILooknFeelReader
			,GUILooknFeelReader::XmlStartControlElementFontColor
			,GUILooknFeelReader::XmlEndCommon
			,GUILooknFeelReader::XmlAttribCommon
			,3
			,GUI::LooknFeel::CONTROL_ELEMENT_FONT_COLOR);
		// 5 control//element//image
		_set_degree(GUILooknFeelReader
			,GUILooknFeelReader::XmlStartControlElementImage
			,GUILooknFeelReader::XmlEndCommon
			,GUILooknFeelReader::XmlAttribElementImage
			,2
			,GUI::LooknFeel::CONTROL_ELEMENT_IMAGE);

		// 6 control//element//image//color
		_set_degree(GUILooknFeelReader
			,GUILooknFeelReader::XmlStartControlElementImageColor
			,GUILooknFeelReader::XmlEndCommon
			,GUILooknFeelReader::XmlAttribCommon
			,5
			,GUI::LooknFeel::CONTROL_ELEMENT_IMAGE_COLOR);


	

		_ReadyParse(mDegrees[0]);
	}
	//////////////////////////////////////////////////////////////////////////
	GUILooknFeelReader::~GUILooknFeelReader()
	{
		
	}
	
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::LoadXML(const wchar_t* xmlText,GUI::LooknFeel::ControlSet* pMgr)
	{
		_Clear();
		mpSet = pMgr;				
		LoadInfo info;		
		info.xmlText = xmlText;
		_LoadXML(info);
		mpSet = 0;
	}
	
	
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::_Clear()
	{
		// 清除暫存資料		
		mpCur = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlStartControl()
	{		
		_XmlStartCommon(new GUI::LooknFeel::Control(mpCur));
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlEndControl()
	{
		XmlEndCommon();		
	}
	
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlAttribControl(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		mpCur->SetAttrib(attr,nAttrLen,value,nValLen);
		if( wcsicmp(attr,L"TYPE") == 0)
			mpSet->Add(value,(GUI::LooknFeel::Control*)mpCur);
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::_XmlStartCommon(GUI::XmlElement* pNew)
	{
		mpCur = pNew;
		mpSet->ElementSet::Add(pNew);
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlEndCommon()
	{
		mpCur = static_cast<GUI::XmlElement*>(mpCur->GetParent());
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlAttribCommon(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		mpCur->SetAttrib(attr,nAttrLen,value,nValLen);
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlStartControlElement()
	{		
		_XmlStartCommon(new GUI::LooknFeel::ControlElement(mpCur));
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlStartControlElementFont()
	{		
		_XmlStartCommon(new GUI::LooknFeel::ControlElementFont(mpCur));				
	}	
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlStartControlElementFontColor()
	{
		_XmlStartCommon(new GUI::LooknFeel::ControlElementFontColor(mpCur));		
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlStartControlElementImage()
	{
		_XmlStartCommon(new GUI::LooknFeel::ControlElementImage(mpCur));		
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlAttribElementImage(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		if(wcsicmp(attr,L"SOURCE") == 0)
		{
			const GUI::ImageSet::Data* pData = mpImgs->Find(value);
			if(pData)
			{
				mpCur->Set(GUI::LooknFeel::ControlElementImage::SOURCE, (void*)pData);
			}			
			//assert(pData);
		}
		else
		{
			XmlAttribCommon(attr,nAttrLen,value,nValLen);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILooknFeelReader::XmlStartControlElementImageColor()
	{
		_XmlStartCommon(new GUI::LooknFeel::ControlElementImageColor(mpCur));		
	}
	//////////////////////////////////////////////////////////////////////////

}