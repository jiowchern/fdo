#ifndef __UIBuilder_20080911pm0422__
#define __UIBuilder_20080911pm0422__
#include "Element.h"
#include "LooknFeel.h"
#include "Layout.h"
#include "ImageSet.h"
#include "Common.h"
struct UIBuilder
{
	UIBuilder();
	virtual ~UIBuilder();
	
	static const ITextDatabase*	gpTextDb;
	
	
	static void fnSetTextDatabase(const ITextDatabase* p)
	{
		gpTextDb = p;
	}
	
	enum
	{
		CLASS_LAYOUT,CLASS_LOOKNFEEL,CLASS_COUNT
	};
	//////////////////////////////////////////////////////////////////////////
	virtual bool ReceiveStuff	(int nClass,int nLabelID,const GUI::Element* pEle) { return false;};
};



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define DECLARE_LABEL_MAP \
		static LabelParseEntry	vLabelParser[]; \
		static LabelMap			labelMap; \
		virtual LabelMap* GetLabelMap() const;
//////////////////////////////////////////////////////////////////////////


#define BEGIN_LABEL_MAP(theClass, baseClass) \
	theClass::LabelMap* theClass::GetLabelMap() const \
	{ return &theClass::labelMap; } \
	theClass::LabelMap theClass::labelMap = \
	{ &(baseClass::labelMap), \
		(theClass::LabelParseEntry*) &(theClass::vLabelParser) }; \
		theClass::LabelParseEntry theClass::vLabelParser[] = \
{

//////////////////////////////////////////////////////////////////////////
#define _SET_LABEL_PARSE(stuff_class,label_id,call_back) \
					{stuff_class,label_id, static_cast<LabelTarget::PFN_LABEL_PARSER>(call_back)} , 
#define _SET_LAYOUT_LABEL_PARSE(label_id,call_back) {UIBuilder::CLASS_LAYOUT,label_id,static_cast<LabelTarget::PFN_LABEL_PARSER>(call_back)},
#define _SET_LOOKNFELL_LABEL_PARSE(label_id,call_back) {UIBuilder::CLASS_LOOKNFEEL,label_id,static_cast<LabelTarget::PFN_LABEL_PARSER>(call_back)},
//////////////////////////////////////////////////////////////////////////

#define END_LABEL_MAP() \
{ 0, 0, (LabelTarget::PFN_LABEL_PARSER)0 } \
};

class LabelTarget : public UIBuilder
{
protected:
	
public:
	typedef bool (LabelTarget::*PFN_LABEL_PARSER)(const GUI::Element* pEle);	
	struct LabelParseEntry
	{				
		int nClass;
		int nLabel;
		PFN_LABEL_PARSER pfn;
	};
	//////////////////////////////////////////////////////////////////////////
	struct LabelMap
	{
		LabelMap*			pBase;
		LabelParseEntry*	pEntries;
	};
	//////////////////////////////////////////////////////////////////////////
	virtual bool ReceiveStuff	(int nClass,int nLabelID,const GUI::Element* pEle) ;
	
	DECLARE_LABEL_MAP;
};



#endif