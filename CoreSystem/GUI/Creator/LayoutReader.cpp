#include "stdafx.h"
#include "LayoutReader.h"
namespace FDO
{

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	GUILayoutReader::GUILayoutReader(const GUI::LooknFeel::ControlSet*	pLooknFeel,const GUI::ImageSet* pSet)
		: mpLooknFeels(pLooknFeel) ,mnDepth(0) , mpImgs(pSet)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	GUILayoutReader::~GUILayoutReader()
	{

	}
#define LABEL_START		L"<START>%s"
#define LABEL_END		L"<END>"
#define LABEL_ATTRIB	L"<ATTRIB>%s = %s"
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::LoadXML(const wchar_t* xmlText,GUI::Layout::Control*& pSet,UICreateListener* l)
	{
		mpSet = 0;
		mnDepth = 0;
		LoadInfo i;
		i.xmlText = xmlText;
		_LoadXML(i,l);
		pSet = (GUI::Layout::Control*)mpSet;
		
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControl()
	{
		mpSet = new GUI::Layout::Control(mpSet);		
		_LogStart(L"Control");
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlEndControl()
	{
		GUI::XmlElement* pParent = static_cast<GUI::XmlElement*>(mpSet->GetParent());
		if(pParent != 0)
		{
			mpSet = pParent;
		}
		assert(mnDepth != 0);
		_LogEnd();
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlAttribControl(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		if(wcsicmp(L"TYPE", attr) == 0)
		{
			const GUI::LooknFeel::Control* pCtrl =  mpLooknFeels->Get(value);
			if(pCtrl)
			{
				mpSet->Set(GUI::Layout::Control::TYPE,(void*)pCtrl);
				_LogAttrib(attr,value);				
			}			
		}
		else 
		{
			XmlAttribCommon(attr,nAttrLen,value,nValLen);
		}
	}
	///////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlElement()
	{
		mpSet = new GUI::Layout::ControlElement(mpSet);
		_LogStart( L"ControlElement" ) ;
	}
	///////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlElementFont()
	{
		mpSet = new GUI::Layout::ControlElementFont(mpSet);
		_LogStart( L"ControlElementFont" ) ;
	}
	///////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlElementFontColor()
	{
		mpSet = new GUI::Layout::ControlElementFontColor(mpSet);
		_LogStart( L"ControlElementFontColor" ) ;
	}		
	///////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlElementImage()
	{
		_XmlStartCommon(new GUI::Layout::ControlElementImage(mpSet));
		_LogStart( L"ControlElementImage" ) ;
	}
	///////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlElementImageColor()
	{
		_XmlStartCommon(new GUI::Layout::ControlElementImageColor(mpSet));
		_LogStart( L"ControlElementImageColor" ) ;
	}
	///////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlText()
	{
		mpSet = new GUI::Layout::ControlText(mpSet);
		_LogStart(L"ControlText");	
	}	
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlRadioInfo()
	{
		_XmlStartCommon(new GUI::Layout::ControlRadioInfo(mpSet));
		_LogStart(L"ControlRadioInfo");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlListBoxInfo()
	{
		_XmlStartCommon(new GUI::Layout::ControlListBoxInfo(mpSet));
		_LogStart(L"ControlListBoxInfo");	
	}	
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlListBoxInfoItem()
	{
		_XmlStartCommon(new GUI::Layout::ControlListBoxInfoItem(mpSet));
		_LogStart(L"ControlListBoxInfoItem");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlImage()
	{
		_XmlStartCommon(new GUI::Layout::ControlImage(mpSet));
		_LogStart(L"ControlImage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlSlider()
	{
		_XmlStartCommon(new GUI::Layout::ControlSlider(mpSet));
		_LogStart(L"ControlSlider");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlIconListBox()
	{
		_XmlStartCommon(new GUI::Layout::ControlIconListBox(mpSet));
		_LogStart(L"ControlIconListBox");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlIconListBoxCellImage()
	{
		_XmlStartCommon(new GUI::Layout::ControlIconListBoxCellImage(GUI::Layout::CONTROL_ICONLISTBOX_CELL_IMAGE,mpSet));
		_LogStart(L"ControlIconListBoxCellImage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlIconListBoxItem()
	{
		_XmlStartCommon(new GUI::Layout::ControlIconListBoxItem(mpSet));
		_LogStart(L"ControlIconListBoxItem");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlImageListBox()
	{
		_XmlStartCommon(new GUI::Layout::ControlImageListBox(mpSet));
		_LogStart(L"ControlImageListBox");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlImageListBoxItem()
	{
		_XmlStartCommon(new GUI::Layout::ControlImageListBoxItem(mpSet));
		_LogStart(L"ControlImageListBoxItem");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlComboBox()
	{
		_XmlStartCommon(new GUI::Layout::ControlComboBox(mpSet));
		_LogStart(L"ControlComboBox");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlComboBoxItem()
	{
		_XmlStartCommon(new GUI::Layout::ControlComboBoxItem(mpSet));
		_LogStart(L"ControlComboBoxItem");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlScrollBar()
	{
		_XmlStartCommon(new GUI::Layout::ControlScrollBar(mpSet));
		_LogStart(L"ControlScrollBar");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlHint()
	{
		_XmlStartCommon(new GUI::Layout::ControlHint(mpSet));
		_LogStart(L"ControlHint");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlDialog()
	{
		_XmlStartCommon(new GUI::Layout::ControlDialog(mpSet));
		_LogStart(L"ControlDialog");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlDialogCaption()
	{
		_XmlStartCommon(new GUI::Layout::ControlDialogCaption(mpSet));
		_LogStart(L"ControlDialogCaption");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlDynamicText()
	{
		_XmlStartCommon(new GUI::Layout::ControlDynamicText(mpSet));
		_LogStart(L"ControlDynamicText");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTreeList()
	{
		_XmlStartCommon(new GUI::Layout::ControlTreeList(mpSet));
		_LogStart(L"ControlTreeList");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTreeListItem()
	{
		_XmlStartCommon(new GUI::Layout::ControlTreeListItem(mpSet));
		_LogStart(L"ControlTreeListItem");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTreeListItemText()
	{
		_XmlStartCommon(new GUI::Layout::ControlTreeListItemText(mpSet));
		_LogStart(L"ControlTreeListItemText");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTreeListItemImage()
	{
		_XmlStartCommon(new GUI::Layout::ControlTreeListItemImage(mpSet));
		_LogStart(L"ControlTreeListItemImage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlImageBox()
	{
		_XmlStartCommon(new GUI::Layout::ControlImageBox(mpSet));
		_LogStart(L"ControlImageBox");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlProgressBar()
	{
		_XmlStartCommon(new GUI::Layout::ControlProgressBar(mpSet));
		_LogStart(L"ControlProgressBar");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlProgressBarBackground()
	{
		_XmlStartCommon(new GUI::Common::Texture(GUI::Layout::CONTROL_PROGRESSBAR_BACKGROUND,mpSet));
		_LogStart(L"ControlProgressBackground");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlProgressBarForward()
	{
		_XmlStartCommon(new GUI::Common::Texture(GUI::Layout::CONTROL_PROGRESSBAR_FORWARD,mpSet));
		_LogStart(L"ControlProgressForward");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlButton()
	{
		_XmlStartCommon(new GUI::Layout::ControlButton(mpSet));
		_LogStart(L"ControlButton");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlButtonNormal()
	{
		_XmlStartCommon(new GUI::Layout::ControlButtonState(GUI::Layout::CONTROL_BUTTON_NORMAL,mpSet));
		_LogStart(L"ControlButtonNormal");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlButtonNormalImage()
	{
		_XmlStartCommon(new GUI::Common::Texture(GUI::Layout::CONTROL_BUTTON_NORMAL_IMAGE,mpSet));
		_LogStart(L"ControlButtonNormalImage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlButtonFocus()
	{
		_XmlStartCommon(new GUI::Layout::ControlButtonState(GUI::Layout::CONTROL_BUTTON_FOCUS,mpSet));
		_LogStart(L"ControlButtonFocus");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlButtonFocusImage()
	{
		_XmlStartCommon(new GUI::Common::Texture(GUI::Layout::CONTROL_BUTTON_FOCUS_IMAGE,mpSet));
		_LogStart(L"ControlButtonFocusImage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTDGItemBox()
	{
		_XmlStartCommon(new GUI::Layout::ControlTDGItemBox(GUI::Layout::CONTROL_TDGITEMBOX,mpSet));
		_LogStart(L"ControlTDGItemBox");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTDGItemBoxFrame()
	{
		_XmlStartCommon(new GUI::Layout::ControlTDGItemBoxFrameImage(GUI::Layout::CONTROL_TDGITEMBOX_FRAME,mpSet));
		_LogStart(L"ControlTDGItemBoxFrame");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTDGItemBoxImage()
	{
		_XmlStartCommon(new GUI::Layout::ControlTDGItemBoxBackImage(GUI::Layout::CONTROL_TDGITEMBOX_IMAGE,mpSet));
		_LogStart(L"ControlTDGItemBoxImage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTDGItemBoxBackgroundImage()
	{
		_XmlStartCommon(new GUI::Layout::ControlTDGItemBoxBackImage(GUI::Layout::CONTROL_TDGITEMBOX_BACKGROUNDIMAGE,mpSet));
		_LogStart(L"ControlTDGItemBoxBackImage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlSubDialog()
	{
		_XmlStartCommon(new GUI::Layout::ControlSubDialog(GUI::Layout::CONTROL_SUBDIALOG,mpSet));
		_LogStart(L"ControlSubDialog");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlListView()
	{
		_XmlStartCommon(new GUI::Layout::ControlListView(GUI::Layout::CONTROL_LISTVIEW,mpSet));
		_LogStart(L"ControlListView");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTextarea()
	{
		_XmlStartCommon(new GUI::Layout::ControlTextarea(GUI::Layout::CONTROL_TEXTAREA,mpSet));
		_LogStart(L"ControlTextarea");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlAvatar()
	{
		_XmlStartCommon(new GUI::Layout::ControlAvatar(GUI::Layout::CONTROL_AVATAR,mpSet));
		_LogStart(L"ControlAvatar");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlSubDialogCustomInfo()
	{
		_XmlStartCommon(new GUI::Layout::ControlSubDialogCustom(GUI::Layout::CONTROL_SUBDIALOG_INFO,mpSet));
		_LogStart(L"ControlSubDialogCustomInfo");	
	}	
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTabPage()
	{
		_XmlStartCommon(new GUI::Layout::ControlTabPage(GUI::Layout::CONTROL_TABPAGE,mpSet));
		_LogStart(L"ControlTabPage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTabPageItem()
	{
		_XmlStartCommon(new GUI::Layout::ControlTabPageItem(GUI::Layout::CONTROL_TABPAGE_ITEM,mpSet));
		_LogStart(L"ControlTabPageItem");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTabPageNormal()
	{
		_XmlStartCommon(new GUI::Layout::ControlTabPageNormal(GUI::Layout::CONTROL_TABPAGE_NORMAL,mpSet));
		_LogStart(L"ControlTabPageNormal");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTabPageFocus()
	{
		_XmlStartCommon(new GUI::Layout::ControlTabPageFocus(GUI::Layout::CONTROL_TABPAGE_FOCUS,mpSet));
		_LogStart(L"ControlTabPageFocus");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTabPageNormalImage()
	{
		_XmlStartCommon(new GUI::Common::Texture(GUI::Layout::CONTROL_TABPAGE_NORMALIMAGE,mpSet));
		_LogStart(L"ControlTabPageNormalImage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlStartControlTabPageFocusImage()
	{
		_XmlStartCommon(new GUI::Common::Texture(GUI::Layout::CONTROL_TABPAGE_FOCUSIMAGE,mpSet));
		_LogStart(L"ControlTabPageFocusImage");	
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlEndCommon()
	{
		mpSet = static_cast<GUI::XmlElement*>(mpSet->GetParent());
		_LogEnd();
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlAttribCommon(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		if(mpSet->SetAttrib(attr,nAttrLen,value,nValLen))
		{
			_LogAttrib(attr,value);
		}
		else
		{
			wstring fmt;
			fmt.assign(mnDepth,L'\t');
			fmt += L"Unknown Attrib : %s = \"%s\" .";
			mpListener->Send(UICL::XMLLog,fmt.c_str(),attr,value);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::_XmlStartCommon(GUI::XmlElement* pNew)
	{
		mpSet = pNew;		
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::_LogStart(const wchar_t* label)
	{
		wstring fmt;
		fmt.assign(mnDepth++,L'\t');
		fmt += LABEL_START;
		mpListener->Send(UICL::XMLLog,fmt.c_str(),label);
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::_LogEnd()
	{
		wstring fmt;
		fmt.assign(--mnDepth,L'\t');
		fmt += LABEL_END;
		mpListener->Send(UICL::XMLLog,fmt.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::_LogAttrib(const wchar_t* attr,const wchar_t* value)
	{
		wstring fmt;
		fmt.assign(mnDepth,L'\t');
		fmt += LABEL_ATTRIB;
		mpListener->Send(UICL::XMLLog,fmt.c_str(),attr,value);
	}

	//////////////////////////////////////////////////////////////////////////
	void GUILayoutReader::XmlAttribElementImage(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		if(wcsicmp(attr,L"SOURCE") == 0)
		{
			const GUI::ImageSet::Data* pData = mpImgs->Find(value);
			if(pData)
			{
				mpSet->Set(GUI::Layout::ControlElementImage::SOURCE, (void*)pData);
			}			
			//assert(pData);
		}
		else
		{
			XmlAttribCommon(attr,nAttrLen,value,nValLen);
		}
	}

}