#include "stdafx.h"
#include "CDXUTText.h"

CDXUTItemRender::CDXUTItemRender( CDXUTDialog *pDialog ): CDXUTControl(pDialog) 
{

}
//////////////////////////////////////////////////////////////////////////
CDXUTItemRender::~CDXUTItemRender()
{

}

//////////////////////////////////////////////////////////////////////////
HRESULT	CDXUTItemRender::OnInit() 
{
	ElementQueryer queryer(this);
	_FOREACH(RenderSet , mRenders , it)
	{
		RenderInfo& inf = *it;
		inf.rcClient = inf.pRender->Initial(m_rcBoundingBox , queryer).rcClient;
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTItemRender::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) 
{
	_FOREACH(RenderSet , mRenders , it)
	{
		RenderInfo& inf = *it;
		CDXUTTextItemRenderPtr pRender = inf.pRender;
		assert(pRender);

		POINT ptDlg;
		m_pDialog->GetLocation(ptDlg);

		RECT rcScreen(inf.rcClient);
		OffsetRect(&rcScreen,ptDlg.x + m_rcBoundingBox.left,ptDlg.y + m_rcBoundingBox.top);
		RECT rcClient(inf.rcClient);

		OffsetRect(&rcClient,m_rcBoundingBox.left,m_rcBoundingBox.top);
		
		CDXUTTextItemRender::RenderInfo renderinf(rcScreen,rcClient,m_pDialog);		
		pRender->Render(renderinf);
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTItemRender::AddRender(CDXUTTextItemRenderPtr pRender)
{
	RenderInfo inf;
	inf.pRender = pRender;
	
	mRenders.push_back(inf);
	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTItemRender::ClearRender()
{
	mRenders.clear();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CDXUTTextItem::CDXUTTextItem(CDXUTTextItemEnvironment* pEnv , CDXUTDialog *pDialog)
: CDXUTItemRender(pDialog) , mpEnv(pEnv) , mRender(pEnv)
{
	AddRender(&mRender);
	mhClose = pEnv->hMouseOutEvent;
}
//////////////////////////////////////////////////////////////////////////
CDXUTTextItem::~CDXUTTextItem()
{
	CDXUTTextItemEnvironment::fnRunCommand(mhClose);
}
//////////////////////////////////////////////////////////////////////////
bool CDXUTTextItem::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{

	if(uMsg == WM_LBUTTONDOWN)
	{
		mpEnv->DoEvent_MouseLDown();
		return true;
	}
	return CDXUTItemRender::HandleMouse(uMsg,pt,wParam,lParam);
}

//////////////////////////////////////////////////////////////////////////
void CDXUTTextItem::OnMouseEnter() 
{
	
	if(mpEnv->bHasHypelink)
	{
		mpEnv->bHypelink = true;
	}
	mpEnv->DoEvent_MouseIn();
	CDXUTItemRender::OnMouseEnter();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItem::OnMouseLeave() 
{
	if(mpEnv->bHasHypelink)
	{
		mpEnv->bHypelink = false;
	}
	mpEnv->DoEvent_MouseOut();
	CDXUTItemRender::OnMouseLeave();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItem::SetText(const char16* text)
{
	mRender.SetText(text);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItem::SetAlign(uint nMode) 
{
	mRender.SetAlign(nMode);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CDXUTItemRender::ElementQueryer::ElementQueryer(CDXUTItemRender* pCtrl) : mpCtrl(pCtrl) , mnCount(BEGIN_SN)
{
	
}
//////////////////////////////////////////////////////////////////////////
CDXUTItemRender::ElementQueryer::~ElementQueryer()
{

}
//////////////////////////////////////////////////////////////////////////
CDXUTElement* CDXUTItemRender::ElementQueryer::Query()
{
	CDXUTElement* pEle = mpCtrl->GetElement(mnCount);
	if(pEle == 0)
	{
		CDXUTElement ele;
		mpCtrl->SetElement(mnCount , &ele);
		pEle = mpCtrl->GetElement(mnCount);
	}

	++mnCount;
	return pEle;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CDXUTTextItemRenderString::CDXUTTextItemRenderString(CDXUTTextItemEnvironment* pEnv) : mpEnv(pEnv)
{

}
//////////////////////////////////////////////////////////////////////////
CDXUTTextItemRenderString::~CDXUTTextItemRenderString()
{
	
}
//////////////////////////////////////////////////////////////////////////
CDXUTTextItemRender::InitialResult CDXUTTextItemRenderString::Initial(const RECT& rcCtrl , CDXUTTextItem::ElementQueryer& q) 
{
	CDXUTTextItemRender::InitialResult ir;
	SetRect(&ir.rcClient , 0,0,RectWidth(rcCtrl) , RectHeight(rcCtrl));	

	mpElement						= q.Query();
	mpElement->iFont				= mpEnv->iFont;			
	mpElement->FontColor.Current	= mpEnv->nFontColor;
	mpElement->dwTextFormat			= DT_TOP | DT_LEFT;

	
	return ir;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemRenderString::SetAlign(uint nMode) 
{
	if(mpElement)
		mpElement->dwTextFormat	= nMode;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemRenderString::Render(const RenderInfo& inf) 
{
	if(mpEnv->bHypelink)
	{
		CDXUTElement ele = *mpElement;
		ele.FontColor.Current = 0xFF8064a2;
		inf.rendersys.DrawPlainText(mString.c_str() , &ele , &inf.rcClient,false) ;	
	}
	else
	{
		inf.rendersys.DrawPlainText(mString.c_str() , mpElement , &inf.rcClient,false) ;	
	}
	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemRenderString::SetText(const char16* text)
{
	mString = text;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CDXUTSystemIcon::CDXUTSystemIcon(CDXUTTextItemEnvironment* pEnv , CDXUTDialog *pDialog) :
	CDXUTItemRender(pDialog) , mpEnv(pEnv)
{
	AddRender(&mRender);
}
//////////////////////////////////////////////////////////////////////////
CDXUTSystemIcon::~CDXUTSystemIcon()
{

}
//////////////////////////////////////////////////////////////////////////
// Ä~©Ó¦Û CDXUTControl	
bool CDXUTSystemIcon::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{
	return false;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSystemIcon::SetIcon(uint nIcon)
{
	CDXUTElement* pEle = mRender.GetElement();
	if(pEle)
	{
		GetSysIconElement(nIcon,*pEle);
	}		
}
//////////////////////////////////////////////////////////////////////////
CDXUTSystemIcon::IconRender::IconRender() : mpElement(0)
{
	
}
//////////////////////////////////////////////////////////////////////////
CDXUTSystemIcon::IconRender::~IconRender()
{

}
//////////////////////////////////////////////////////////////////////////
CDXUTTextItemRender::InitialResult CDXUTSystemIcon::IconRender::Initial(const RECT& rcCtrl , CDXUTItemRender::ElementQueryer& q) 
{
	
	
	mpElement = q.Query();

	CDXUTTextItemRender::InitialResult ir;	
	SetRect(&ir.rcClient , 0,0,RectWidth(rcCtrl) , RectHeight(rcCtrl));	
	return ir;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSystemIcon::IconRender::Render(const RenderInfo& inf) 
{
	RECT rc(inf.rcClient);
	inf.rendersys.DrawSprite(mpElement , &rc );
}
//////////////////////////////////////////////////////////////////////////