#include "StdAfx.h"
#include "CDXUTUIBuilder.h"
#include "Layout.h"
#include "LooknFeel.h"
#include "PhenixD3D9RenderSystem.h"
#include "CDXUTControl.h"
namespace FDO
{
	
	float fnGetAlignPoint(int nMode,int nWndWH,int nScreenWH)
	{
		return (nScreenWH*(0.5f*nMode)) - (nWndWH*(0.5f*nMode));	
	}
	//////////////////////////////////////////////////////////////////////////

	bool fnGetTextureInfo(int iLabelOffset,const GUI::Element* pEle,string& spath,RECT& rc)
	{
		wchar_t* path = 0;
		pEle->Get(GUI::Common::Texture::PATH + iLabelOffset,path);

		int l,t,r,b;
		if(	pEle->Get(GUI::Common::Texture::LEFT + iLabelOffset,l) &&
			pEle->Get(GUI::Common::Texture::RIGHT + iLabelOffset,r)&&
			pEle->Get(GUI::Common::Texture::TOP + iLabelOffset,t)&&
			pEle->Get(GUI::Common::Texture::BOTTOM + iLabelOffset,b) && path)
		{

			spath = UnicodeToAnsi(path);			
			SetRect(&rc,l,t,r,b);

			return true;
		}

		if(	pEle->Get(GUI::Common::Texture::POSX + iLabelOffset,l)&&
			pEle->Get(GUI::Common::Texture::POSY + iLabelOffset,t)&&
			pEle->Get(GUI::Common::Texture::WIDTH + iLabelOffset,r)&&
			pEle->Get(GUI::Common::Texture::HEIGHT + iLabelOffset,b) && path)
		{			
			spath = UnicodeToAnsi(path);			
			SetRect(&rc,l,t,l+r,t+b);			
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CDXUTDialogBuilder::CDXUTDialogBuilder(CDXUTDialog* pDialog,UIListener* l)
		: mpDialog(pDialog) , mpLisener(l)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTDialogBuilder::~CDXUTDialogBuilder()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool CDXUTDialogBuilder::ReceiveStuff(int nClass,int nID,const GUI::Element* pEle)
	{
		if(nClass == UIBuilder::CLASS_LOOKNFEEL)
		{
			if(nID == GUI::LooknFeel::CONTROL)
			{
				mpDialog->SetControlOrder(true);
				int iFrameGauge;
				if(pEle->Get(GUI::LooknFeel::Control::FRAMEGAUGE,iFrameGauge))
				{
					mpDialog->EnableUseSideFrame(true,iFrameGauge);

				}
			}
			else if(nID == GUI::LooknFeel::CONTROL_ELEMENT)
			{

			}
			else if(nID == GUI::LooknFeel::CONTROL_ELEMENT_IMAGE)
			{
				const GUI::Element* pParent = pEle->GetParent();
				int nId;
				if(pParent->Get(GUI::LooknFeel::ControlElement::ID,nId))
				{				
					void* ptr;
					if(pEle->Get(GUI::LooknFeel::ControlElementImage::SOURCE,ptr))
					{
						const GUI::ImageSet::Data* pImg = (const GUI::ImageSet::Data*)ptr;
						RECT rect;
						rect.left = pImg->rect.left;
						rect.right = pImg->rect.right;
						rect.top = pImg->rect.top;
						rect.bottom = pImg->rect.bottom;
						mpDialog->SetBackgroundTexture2(nId,pImg->path,&rect);


						string p;
						p = pImg->path;												
						//mpLisener->Send(UICL::DataMessage,L"背景圖(%d) = %s(l:%d,t:%d,r:%d,b:%d)",nId,ANSItoUNICODE(p),rect.left,rect.top,rect.right,rect.bottom);
					}
					return true;
				}					
			}
		}
		if(nClass == UIBuilder::CLASS_LAYOUT)
		{

			if(nID == GUI::Layout::CONTROL)
			{

				int x,y,w,h;
				using namespace GUI::Layout;
				pEle->Get(Control::POSX,x);
				pEle->Get(Control::POSY,y);
				pEle->Get(Control::WIDTH,w);
				pEle->Get(Control::HEIGHT,h);		
				int nId = -1;
				pEle->Get(Control::ID,nId);		
				mpDialog->SetID(nId);

				int nAlginMode = 0;
				if(pEle->Get(Control::ALGINMODE,nAlginMode))
				{
					unsigned int cw = g_RenderSystemPtr->GetWindowWidth();
					unsigned int ch = g_RenderSystemPtr->GetWindowHeight();
					if( cw < 0 )
					{
						Assert( 0, "cw不合法 !!" );
					}
					if( ch < 0 )
					{
						Assert( 0, "ch不合法 !!" );
					}
					float fx,fy;
					//求出基準點
					fx = fnGetAlignPoint(nAlginMode%3,w,cw);
					fy = fnGetAlignPoint(nAlginMode/3,h,ch);

					//由基準點扣掉位移值 得到實際位置
					// 					fx -= float(x)/800.0f * cw;
					// 					fy -= float(y)/600.0f * ch;
					
					fx += float(x);
					fy += float(y);
					x = (int)fx;
					y = (int)fy;
				}

				
				mpDialog->SetLocation(x,y,false);

				mpDialog->SetSize(w,h);

				int nMsgMode;
				mpDialog->SetControlOrder(false);
				if(pEle->Get(Control::MSGORDER,nMsgMode))
				{
					if(nMsgMode == Control::OrderContrary)
					{
						mpDialog->SetControlOrder(true);
					}
				}

			}
			else if(nID == GUI::Layout::CONTROL_DIALOG)
			{
				using namespace GUI::Layout;
				int	nClose,nMin,nMax;
				pEle->Get(ControlDialog::CLOSE,nClose);
				pEle->Get(ControlDialog::MIN,nMin);
				pEle->Get(ControlDialog::MAX,nMax);

				if(nClose == 1)
					mpDialog->SetCloseBtuttonAuto();
			}
			else if(nID == GUI::Layout::CONTROL_DIALOG_CAPTION)
			{
				using namespace GUI::Layout;				

				RECT rc;
				SetRectEmpty(&rc);
				int nVal = 0;
				pEle->Get(ControlDialogCaption::LEFT,nVal);
				rc.left = nVal;
				pEle->Get(ControlDialogCaption::RIGHT,nVal) ;
				rc.right = nVal;
				pEle->Get(ControlDialogCaption::BOTTOM,nVal) ;
				rc.bottom = nVal;
				pEle->Get(ControlDialogCaption::TOP,nVal) ;
				rc.top = nVal;

				nVal = 0;
				if(pEle->Get(ControlDialogCaption::DRAWRECT,nVal))
					mpDialog->EnableCaption(true,&rc,nVal == 1);	
				else
					mpDialog->EnableCaption(true,&rc,false);	

				int nText;
				if(pEle->Get(ControlDialogCaption::TEXT,nText))
				{
					wstring text;
					text = gpTextDb->GetText(nText) ;//g_BinTable.mCodeInlineTextBin.pFastFindW( nText ,CodeInlineText_Text);
					mpDialog->SetCaptionText(text.c_str(),&rc);

					int nARGB;
					if(pEle->Get(ControlDialogCaption::COLOR,nARGB))
					{
						mpDialog->SetCaptionTextColor(nARGB);
					}
				}


			}

		}
		return true;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	ControlBuilder::ControlBuilder(CDXUTControl* pCtrl)
		: mpCtrl(pCtrl) 
	{

	}
	//////////////////////////////////////////////////////////////////////////
	ControlBuilder::~ControlBuilder()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ControlBuilder::Dump(UIListener* l,int nDepth)
	{
		wstring depth,fmt;
		depth.assign(nDepth,L'\t');

		fmt = depth + _strtable(STR_FMT_BUILD_CONTROL);
		l->Send(UICL::DataMessage,fmt.c_str(),L"Control");

		depth.assign(2,L'\t');

		fmt = depth + _strtable(STR_FMT_ID);
		l->Send(UICL::DataMessage,fmt.c_str(),mpCtrl->GetID());

		int w,h,x,y;
		mpCtrl->GetSize(w,h);
		mpCtrl->GetLocation(x,y);
		fmt = depth + _strtable(STR_FMT_RECT);
		l->Send(UICL::DataMessage,fmt.c_str(),x,y,w,h);		

		fmt = depth + _strtable(STR_FMT_ENABLE);
		l->Send(UICL::DataMessage,fmt.c_str(),mpCtrl->GetEnabled());		

		fmt = depth + _strtable(STR_FMT_VISIBLE);
		l->Send(UICL::DataMessage,fmt.c_str(),mpCtrl->GetVisible());

		CDXUTElement* pEle = 0;
		int idx = 0;
		CDXUTDialogResourceManager* pMgr = CDXUTDialog::GetManager();

		while (pEle = mpCtrl->GetElement(idx))
		{
			fmt = depth + L"Element %2d";
			l->Send(UICL::DataMessage,fmt.c_str(),idx);
			//////////////////////////////////////////////////////////////////////////
			// texture
			if(pEle->iTexture != -1)
			{
				DXUTTextureNode* pNode = pMgr->GetTextureNode(pEle->iTexture);
				if(pNode)
				{
					fmt = depth + L"\tTexture : %s ,%4d,%4d,%4d,%4d";
					l->Send(UICL::DataMessage,fmt.c_str(),pNode->strFilename,pEle->rcTexture.left,pEle->rcTexture.top,pEle->rcTexture.right,pEle->rcTexture.bottom);	
					fmt = depth + L"\t\tColors";
					l->Send(UICL::DataMessage,fmt.c_str());

					for (int i = 0 ; i < MAX_CONTROL_STATES ; ++i)
					{					
						fmt = depth + L"\t\t\t Color %d = 0x%x";
						l->Send(UICL::DataMessage,fmt.c_str(),i,pEle->TextureColor.States[i]);
					}				
				}
				else
				{
					l->Send(UICL::DataWarning,L"\tNo Texture Id = %d",pEle->iTexture);
				}
			}


			//////////////////////////////////////////////////////////////////////////
			// font
			if(pEle->iFont != -1)
			{
				DXUTFontNode* pNode = pMgr->GetFontNode(pEle->iFont);
				if(pNode)
				{
					fmt = depth + L"\tFont : %s \t, weight = %2d, height = %2d, Italic = %d , format = %x";
					l->Send(UICL::DataMessage,fmt.c_str(),pNode->strFace,pNode->nWeight,pNode->nHeight,pNode->bItalic,pEle->dwTextFormat);
					fmt = depth + L"\t\tColors";
					l->Send(UICL::DataMessage,fmt.c_str());

					for (int i = 0 ; i < MAX_CONTROL_STATES ; ++i)
					{					
						fmt = depth + L"\t\t\t Color %d = 0x%x";
						l->Send(UICL::DataMessage,fmt.c_str(),i,pEle->FontColor.States[i]);
					}
				}
				else
				{
					l->Send(UICL::DataWarning,L"\tNo Font Id = %d",pEle->iFont);
				}
			}


			idx++;
		}
		if(idx == 0)
		{
			l->Send(UICL::DataWarning,L"No Element");
		}
	}
	//////////////////////////////////////////////////////////////////////////

	bool ControlBuilder::XmlLabelLooknFeelControl(const GUI::Element* pEle)
	{

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLooknFeelControlElement(const GUI::Element* pEle)
	{
		using namespace GUI::LooknFeel;
		int nId;
		if(pEle->Get(ControlElement::ID,nId))
		{
			CDXUTElement Element;			
			RECT rcTexture;
			SetRectEmpty( &rcTexture ) ;
			Element.SetTexture( 0, &rcTexture );
			Element.SetFont( 0 );
			if( !mpCtrl->GetElement( nId ) )
				mpCtrl->SetElement(nId,&Element);			
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLooknFeelControlElementFont(const GUI::Element* pEle)
	{
		using namespace GUI::LooknFeel;
		int nId;				
		if(pEle->GetParent()->Get(ControlElement::ID,nId))
		{
			int nAlign;
			DWORD nFontFmt = 0;
			bool bChange = false;
			if(pEle->Get(ControlElementFont::ALIGN,nAlign))
			{
				bChange = true;
				nFontFmt = nAlign;				
			}
			if(pEle->Get(ControlElementFont::VALIGN,nAlign))
			{
				bChange = true;
				if(nAlign == ControlElementFont::Top)
				{
					nFontFmt |= DT_TOP;	
				}
				else if(nAlign == ControlElementFont::Center)
				{
					nFontFmt |= DT_VCENTER;	

				}
				else if(nAlign == ControlElementFont::Bottom)
				{
					nFontFmt |= DT_BOTTOM;

				}

			}
			if(pEle->Get(ControlElementFont::HALIGN,nAlign))
			{
				bChange = true;
				if(nAlign == ControlElementFont::Left)
				{
					nFontFmt |= DT_LEFT;	

				}
				else if(nAlign == ControlElementFont::Center)
				{
					nFontFmt |= DT_CENTER;	

				}
				else if(nAlign == ControlElementFont::Right)
				{
					nFontFmt |= (DT_RIGHT | DT_SINGLELINE);	
				}
			}
			if(bChange)
				mpCtrl->GetElement( nId )->dwTextFormat = nFontFmt;	
			int argb ;
			if(pEle->Get(ControlElementFont::RGBA,argb))
			{			
				mpCtrl->GetElement( nId )->FontColor.Init(argb) ;

			}
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLooknFeelControlElementFontColor(const GUI::Element* pEle)
	{
		using namespace GUI::LooknFeel;
		int nId;
		if(pEle->GetParent()->GetParent()->Get(ControlElement::ID,nId))
		{
			int argb ;
			int states;
			if(pEle->Get(ControlElementFontColor::RGBA,argb) && pEle->Get(ControlElementFontColor::STATE,states))
			{				
				mpCtrl->GetElement( nId )->FontColor.States[ states ] = argb;			


			}				
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLooknFeelControlElementImage(const GUI::Element* pEle)
	{
		using namespace GUI::LooknFeel;
		void* ptr;
		if(pEle->Get(ControlElementImage::SOURCE,ptr))
		{
			const GUI::ImageSet::Data* pImg =(const GUI::ImageSet::Data*)ptr; 
			RECT rect;
			rect.left = pImg->rect.left;
			rect.right = pImg->rect.right;
			rect.top = pImg->rect.top;
			rect.bottom = pImg->rect.bottom;

			int nId;				
			if(pEle->GetParent()->Get(ControlElement::ID,nId))
			{
				string path;
				path = pImg->path;

				mpCtrl->CDXUTControl::SetTexture( path, &rect, nId );

			}				
		}
		int argb ;
		if(pEle->Get(ControlElementImage::RGBA,argb))
		{
			int nId;				
			if(pEle->GetParent()->Get(ControlElement::ID,nId))
			{
				mpCtrl->GetElement( nId )->TextureColor.Init(argb);

			}
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLooknFeelControlElementImageColor(const GUI::Element* pEle)
	{
		using namespace GUI::LooknFeel;
		int nId;
		if(pEle->GetParent()->GetParent()->Get(ControlElement::ID,nId))
		{
			int argb ;
			int states;
			if(pEle->Get(ControlElementImageColor::RGBA,argb) && pEle->Get(ControlElementImageColor::STATE,states))
			{
				mpCtrl->GetElement( nId )->TextureColor.States[ states ] = argb;	

			}				
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLayoutControl(const GUI::Element* pEle)
	{
		using namespace GUI::Layout;
		int x = 0,y= 0,w= 0,h= 0,id = -1;
		pEle->Get(Control::ID , id);
		mpCtrl->SetID(id);

		if(pEle->Get(Control::POSX , x) && pEle->Get(Control::POSY , y) && pEle->Get(Control::WIDTH, w) && pEle->Get(Control::HEIGHT, h))
		{
			mpCtrl->SetLocation(x,y);
			mpCtrl->SetSize(w,h);
		}
		if(pEle->Get(Control::LEFT, x) && pEle->Get(Control::TOP , y) && pEle->Get(Control::RIGHT, w) && pEle->Get(Control::BOTTOM, h))
		{
			mpCtrl->SetLocation(x,y);
			mpCtrl->SetSize(w - x,h - y);
		}

		int nEnable;
		if(pEle->Get(Control::ENABLE, nEnable))
		{
			mpCtrl->SetEnabled(nEnable != 0);

		}
		int nVis;
		if(pEle->Get(Control::VISIBLE, nVis))
		{
			mpCtrl->SetVisible(nVis != 0);
		}		

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLayoutControlHint(const GUI::Element* pEle)
	{
		using namespace GUI::Layout;
		int nText;
		if(!pEle->Get(ControlHint::TEXT, nText))
		{

		}
		wchar_t* path;
		if(pEle->Get(ControlHint::PATH, path) == false)
		{
			path = 0;
		}
		wstring text;
		text = gpTextDb->GetText(nText) ;//g_BinTable.mCodeInlineTextBin.pFastFindW( nText ,CodeInlineText_Text);
		if(path == 0)
			mpCtrl->SetHint(text.c_str());
		else
			mpCtrl->SetHint(text.c_str(),path);


		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLayoutControlElement(const GUI::Element* pEle)
	{
		return false ;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLayoutControlElementFont(const GUI::Element* pEle)
	{
		using namespace GUI::Layout;
		int nId;				
		if(pEle->GetParent()->Get(ControlElement::ID,nId))
		{
			int nAlign;
			DWORD nFontFmt = 0;
			bool bChange = false;
			if(pEle->Get(ControlElementFont::ALIGN,nAlign))
			{
				bChange = true;
				nFontFmt = nAlign;				
			}
			if(pEle->Get(ControlElementFont::VALIGN,nAlign))
			{
				bChange = true;
				if(nAlign == ControlElementFont::Top)
				{
					nFontFmt |= DT_TOP;	
				}
				else if(nAlign == ControlElementFont::Center)
				{
					nFontFmt |= DT_VCENTER;	

				}
				else if(nAlign == ControlElementFont::Bottom)
				{
					nFontFmt |= DT_BOTTOM;

				}

			}
			if(pEle->Get(ControlElementFont::HALIGN,nAlign))
			{
				bChange = true;
				if(nAlign == ControlElementFont::Left)
				{
					nFontFmt |= DT_LEFT;	

				}
				else if(nAlign == ControlElementFont::Center)
				{
					nFontFmt |= DT_CENTER;	

				}
				else if(nAlign == ControlElementFont::Right)
				{
					nFontFmt |= (DT_RIGHT | DT_SINGLELINE);	
				}
			}
			if(bChange)
				mpCtrl->GetElement( nId )->dwTextFormat = nFontFmt;	
			int argb ;
			if(pEle->Get(ControlElementFont::RGBA,argb))
			{			
				mpCtrl->GetElement( nId )->FontColor.Init(argb) ;
			}
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLayoutControlElementFontColor(const GUI::Element* pEle)
	{
		using namespace GUI::Layout;
		int nId;
		if(pEle->GetParent()->GetParent()->Get(ControlElement::ID,nId))
		{
			int argb ;
			int states;
			if(pEle->Get(ControlElementFontColor::RGBA,argb) && pEle->Get(ControlElementFontColor::STATE,states))
			{				
				mpCtrl->GetElement( nId )->FontColor.States[ states ] = argb;			
			}				
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLayoutControlElementImage(const GUI::Element* pEle)
	{
		using namespace GUI::Layout;
		void* ptr;
		if(pEle->Get(ControlElementImage::SOURCE,ptr))
		{
			const GUI::ImageSet::Data* pImg =(const GUI::ImageSet::Data*)ptr; 
			RECT rect;
			rect.left = pImg->rect.left;
			rect.right = pImg->rect.right;
			rect.top = pImg->rect.top;
			rect.bottom = pImg->rect.bottom;

			int nId;				
			if(pEle->GetParent()->Get(ControlElement::ID,nId))
			{
				string path;
				path = pImg->path;

				mpCtrl->CDXUTControl::SetTexture( path, &rect, nId );

			}				
		}
		int argb ;
		if(pEle->Get(ControlElementImage::RGBA,argb))
		{
			int nId;				
			if(pEle->GetParent()->Get(ControlElement::ID,nId))
			{
				mpCtrl->GetElement( nId )->TextureColor.Init(argb);

			}
		}
		return false ;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ControlBuilder::XmlLabelLayoutControlElementImageColor(const GUI::Element* pEle)
	{
		using namespace GUI::Layout;
		int nId;
		if(pEle->GetParent()->GetParent()->Get(ControlElement::ID,nId))
		{
			int argb ;
			int states;
			if(pEle->Get(ControlElementImageColor::RGBA,argb) && pEle->Get(ControlElementImageColor::STATE,states))
			{
				mpCtrl->GetElement( nId )->TextureColor.States[ states ] = argb;	

			}				
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	BEGIN_LABEL_MAP(ControlBuilder,LabelTarget)		
		_SET_LOOKNFELL_LABEL_PARSE(GUI::LooknFeel::CONTROL,&ControlBuilder::XmlLabelLooknFeelControl)
		_SET_LOOKNFELL_LABEL_PARSE(GUI::LooknFeel::CONTROL_ELEMENT,&ControlBuilder::XmlLabelLooknFeelControlElement)
		_SET_LOOKNFELL_LABEL_PARSE(GUI::LooknFeel::CONTROL_ELEMENT_FONT,&ControlBuilder::XmlLabelLooknFeelControlElementFont)
		_SET_LOOKNFELL_LABEL_PARSE(GUI::LooknFeel::CONTROL_ELEMENT_FONT_COLOR,&ControlBuilder::XmlLabelLooknFeelControlElementFontColor)
		_SET_LOOKNFELL_LABEL_PARSE(GUI::LooknFeel::CONTROL_ELEMENT_IMAGE,&ControlBuilder::XmlLabelLooknFeelControlElementImage)
		_SET_LOOKNFELL_LABEL_PARSE(GUI::LooknFeel::CONTROL_ELEMENT_IMAGE_COLOR,&ControlBuilder::XmlLabelLooknFeelControlElementImageColor)
		_SET_LAYOUT_LABEL_PARSE(GUI::Layout::CONTROL,&ControlBuilder::XmlLabelLayoutControl)
		_SET_LAYOUT_LABEL_PARSE(GUI::Layout::CONTROL_HINT,&ControlBuilder::XmlLabelLayoutControlHint)
		_SET_LAYOUT_LABEL_PARSE(GUI::Layout::CONTROL_ELEMENT,&ControlBuilder::XmlLabelLayoutControlElement)
		_SET_LAYOUT_LABEL_PARSE(GUI::Layout::CONTROL_ELEMENT_FONT,&ControlBuilder::XmlLabelLayoutControlElementFont)
		_SET_LAYOUT_LABEL_PARSE(GUI::Layout::CONTROL_ELEMENT_FONT_COLOR,&ControlBuilder::XmlLabelLayoutControlElementFontColor)
		_SET_LAYOUT_LABEL_PARSE(GUI::Layout::CONTROL_ELEMENT_IMAGE,&ControlBuilder::XmlLabelLayoutControlElementImage)
		_SET_LAYOUT_LABEL_PARSE(GUI::Layout::CONTROL_ELEMENT_IMAGE_COLOR,&ControlBuilder::XmlLabelLayoutControlElementImageColor)
	END_LABEL_MAP()

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void fnBroadcastStuff(int nStuffClass,UIBuilder* pBuilder,const GUI::Element* pEle)
	{
		pBuilder->ReceiveStuff(nStuffClass,pEle->GetType(),pEle);
		std::list<const GUI::Element*> eles;
		pEle->GetChild(eles);
		std::list<const GUI::Element*>::const_iterator it = eles.begin();
		for( ; it != eles.end() ; ++it)
		{
			fnBroadcastStuff(nStuffClass,pBuilder,*it);
		}
	}		
};