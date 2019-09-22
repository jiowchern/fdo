#ifndef _CDXUTDialogCreator_20080902pm0415_
#define _CDXUTDialogCreator_20080902pm0415_
#include "Layout.h"
#include "LooknFeel.h"
#include "ImageSet.h"
#include "UIBuilder.h"
#include "Listener.h"
#include "LayoutReader.h"
class CDXUTDialogResourceManager;
namespace FDO
{
	class UICreator;	
	class ControlBuilder;
	struct UICreatorData
	{
		typedef void			(*PFN_INITLAYOUTEVENT)(GUILayoutReader* pLR);
		typedef ControlBuilder*	(*PFN_BUILDERCREATOR)(int nControlType,CDXUTControl* pControl);
		typedef void			(*PFN_BUILDERDESTORY)(ControlBuilder* pBuilder);
		typedef CDXUTControl*	(*PFN_CONTROLCREATOR)(int nControlType,CDXUTDialog* pDialog);


		PFN_INITLAYOUTEVENT	pInitLayoutEvent;
		PFN_BUILDERCREATOR	pBuilderCreator;
		PFN_BUILDERDESTORY	pBuilderDestory;
		PFN_CONTROLCREATOR	pControlCreator;
		
		UICreatorData(PFN_INITLAYOUTEVENT	pI,
			PFN_BUILDERCREATOR	pBC,
			PFN_BUILDERDESTORY	pBD,
			PFN_CONTROLCREATOR	pCC) : 
			pInitLayoutEvent(pI),
			pBuilderCreator(pBC),
			pBuilderDestory(pBD),
			pControlCreator(pCC)
			{

			}
	};
	
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class UICreator
	{
		GUI::LooknFeel::ControlSet	mLooknFeelSet;
		GUI::ImageSet				mImageSet;
		GUILayoutReader				mlfreader;

		list<CDXUTDialog*>*					mpDialogList;					
		list<CDXUTDialog*>*					mpAlwaysOnTop;					
		list<CDXUTDialog*>*					mpAlwaysOnBottom;					
		CDXUTDialogResourceManager*	mpResMgr;
		

		UICreatorData::PFN_INITLAYOUTEVENT	mpInitLayoutEvent;
		UICreatorData::PFN_BUILDERCREATOR	mpBuilderCreator;
		UICreatorData::PFN_BUILDERDESTORY	mpBuilderDestory;		
		UICreatorData::PFN_CONTROLCREATOR	mpControlCreator;
		bool								mbReleaseResource;

		void			_CreateControl	(CDXUTDialog* pBuilder,const GUI::Layout::Control* type,UIListener* l);
		void			_Build			(UIBuilder* pBuilder,const GUI::Layout::Control* pLayoutSrc,const GUI::LooknFeel::Control* pLooknFeelSrc);
		CDXUTDialog*	_Parse			(const wchar_t* buf,CDXUTDialog* pSrc,UICreateListener* pListener);
	public:
		static UICreator* gpUICreator;
										UICreator	(list<CDXUTDialog*>* pDialogList,list<CDXUTDialog*>* pAlwaysOnTop,list<CDXUTDialog*>* mpAlwaysOnBottom,CDXUTDialogResourceManager* pResMgr,const UICreatorData& data);
										~UICreator	();

		void							Initial				(const char* looknFeelPath,const char* ImageSetPath);		
		CDXUTDialog*					Create				(const GUI::Layout::Control* pSet,CDXUTDialog* pSrc = 0,UICreateListener* pListener = 0);
		CDXUTDialog*					Create				(const char* path,CDXUTDialog* pSrc = 0,UICreateListener* pListener = 0);
		inline	void					SetReleaseResource	(bool b) {mbReleaseResource = b;}
		inline	bool					GetReleaseResource	() const { return mbReleaseResource ;}
		inline	list<CDXUTDialog*>*		GetDialogListNormal	()	{ return mpDialogList ;}
		inline	list<CDXUTDialog*>*		GetDialogListTop	()	{ return mpAlwaysOnTop ;}
		inline	list<CDXUTDialog*>*		GetDialogListBottom	()	{ return mpAlwaysOnBottom ;}
	};

	
}
#endif