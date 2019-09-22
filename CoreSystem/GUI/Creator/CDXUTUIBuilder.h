#ifndef _CDXUTUIBuilder_20080917pm0538_
#define _CDXUTUIBuilder_20080917pm0538_
//#include "StdAfx.h"
#include "UIBuilder.h"
#include "Listener.h"

namespace FDO
{
	
	//////////////////////////////////////////////////////////////////////////
	// base dialog
	class CDXUTDialogBuilder : public UIBuilder
	{
		CDXUTDialog*	mpDialog;
		UIListener*		mpLisener;
	public:
		CDXUTDialogBuilder	(CDXUTDialog* pDialog,UIListener* l);
		~CDXUTDialogBuilder	();

		virtual bool	ReceiveStuff	(int nClass,int nLabelID,const GUI::Element* pEle);
	};

	//////////////////////////////////////////////////////////////////////////
	// control 	
	class ControlBuilder : public LabelTarget
	{
	public:
	protected:
		CDXUTControl*			mpCtrl;	

	public:
		ControlBuilder(CDXUTControl* pCtrl);
		~ControlBuilder();
		bool	XmlLabelLooknFeelControl					(const GUI::Element* pEle);
		bool	XmlLabelLooknFeelControlElement				(const GUI::Element* pEle);
		bool	XmlLabelLooknFeelControlElementFont			(const GUI::Element* pEle);
		bool	XmlLabelLooknFeelControlElementFontColor	(const GUI::Element* pEle);
		bool	XmlLabelLooknFeelControlElementImage		(const GUI::Element* pEle);
		bool	XmlLabelLooknFeelControlElementImageColor	(const GUI::Element* pEle);

		bool	XmlLabelLayoutControl						(const GUI::Element* pEle);
		bool	XmlLabelLayoutControlHint					(const GUI::Element* pEle);
		
		bool    XmlLabelLayoutControlElement                (const GUI::Element* pEle);
		bool	XmlLabelLayoutControlElementFont			(const GUI::Element* pEle);
		bool	XmlLabelLayoutControlElementFontColor		(const GUI::Element* pEle);
		bool    XmlLabelLayoutControlElementImage           (const GUI::Element* pEle);
		bool    XmlLabelLayoutControlElementImageColor      (const GUI::Element* pEle);
		inline CDXUTControl* GetControl						()		{ return mpCtrl;};
		DECLARE_LABEL_MAP;

		virtual void Dump									(UIListener* l,int nDepth = 0);
	};
	
	//////////////////////////////////////////////////////////////////////////
	void fnBroadcastStuff(int nStuffClass,UIBuilder* pBuilder,const GUI::Element* pEle);
	

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	enum
	{
		STR_FMT_BUILD_CONTROL,		
		STR_FMT_ID,
		STR_FMT_RECT,
		STR_FMT_ENABLE,
		STR_FMT_VISIBLE,
		STR_COUNT
	};
	static const wchar_t* gStrTable[STR_COUNT] =
	{
		L"[\tBuild %s\t]",
		L"Control Id = %d",
		L"Location %4d,%4d ,Size %4d,%4d",	
		L"Enable = %d",	
		L"Visible = %d",	
	};
#define _strtable(idx) gStrTable[idx] 
	
	float fnGetAlignPoint(int nMode,int nWndWH,int nScreenWH);
	bool fnGetTextureInfo(int iLabelOffset,const GUI::Element* pEle,string& spath,RECT& rc);
};
#endif