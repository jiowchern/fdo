#ifndef _LayoutReader_20080902pm0406_
#define _LayoutReader_20080902pm0406_
#include "XmlLoader.h"
#include "Layout.h"
#include "ImageSet.h"
namespace FDO
{

	class GUILayoutReader : public XmlLoader 
	{
	public:
		
	private:
		const GUI::LooknFeel::ControlSet*	mpLooknFeels;
		GUI::XmlElement*					mpSet;
		const GUI::ImageSet*		    	mpImgs;		
		int									mnDepth;

		void	_LogStart				(const wchar_t* label);
		void	_LogEnd					();
		void	_LogAttrib				(const wchar_t* attr,const wchar_t* value);

		void	_XmlStartCommon			(GUI::XmlElement* pNew);

	public:
				GUILayoutReader			(const GUI::LooknFeel::ControlSet*	pLooknFeel,const GUI::ImageSet* pSet);
				~GUILayoutReader		();

		inline	GUI::XmlElement*		GetCurElement() {	return mpSet; }
		void	LoadXML					(const wchar_t* xmlText,GUI::Layout::Control*& pSet,UICreateListener* l = 0);
		void	XmlStartControl			();
		void	XmlEndControl			();
		void	XmlAttribControl		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);

		void    XmlStartControlElement();
		void    XmlStartControlElementFont();
		void    XmlStartControlElementFontColor();
		void    XmlStartControlElementImage();
		void    XmlStartControlElementImageColor();
		void	XmlStartControlText		();		
		void	XmlStartControlRadioInfo();
		void	XmlStartControlListBoxInfo();
		void	XmlStartControlListBoxInfoItem();
		void	XmlStartControlImage();
		void	XmlStartControlSlider();
		void	XmlStartControlIconListBox();
		void	XmlStartControlIconListBoxCellImage();
		void	XmlStartControlIconListBoxItem();
		void	XmlStartControlImageListBox();
		void	XmlStartControlImageListBoxItem();
		void	XmlStartControlComboBox();
		void	XmlStartControlComboBoxItem();
		void	XmlStartControlScrollBar();
		void	XmlStartControlHint();
		void	XmlStartControlDialog();
		void	XmlStartControlDialogCaption();
		void	XmlStartControlDynamicText();
		void	XmlStartControlTreeList();
		void	XmlStartControlTreeListItem();
		void	XmlStartControlTreeListItemText();
		void	XmlStartControlTreeListItemImage();
		void	XmlStartControlImageBox();
		void	XmlStartControlProgressBar();
		void	XmlStartControlProgressBarBackground();
		void	XmlStartControlProgressBarForward();
		void	XmlStartControlButton();
		void	XmlStartControlButtonNormal();
		void	XmlStartControlButtonNormalImage();
		void	XmlStartControlButtonFocus();
		void	XmlStartControlButtonFocusImage();
		void	XmlStartControlTDGItemBox();
		void	XmlStartControlTabPage();
		void	XmlStartControlTabPageItem();
		void    XmlStartControlTabPageNormal() ;
		void	XmlStartControlTabPageFocus() ;
		void    XmlStartControlTabPageNormalImage() ;
		void    XmlStartControlTabPageFocusImage() ;
		void    XmlStartControlTDGItemBoxFrame() ;
		void    XmlStartControlTDGItemBoxImage() ;
		void	XmlStartControlTDGItemBoxBackgroundImage();

		void	XmlStartControlSubDialog();
		void	XmlStartControlSubDialogCustomInfo();

		void	XmlStartControlListView();

		void	XmlStartControlAvatar();

		void	XmlStartControlTextarea();

		void    XmlAttribElementImage(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) ;

		void	XmlEndCommon			();
		void	XmlAttribCommon			(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);

	};

	
	
};
#endif