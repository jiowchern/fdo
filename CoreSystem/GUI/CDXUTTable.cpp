#include "StdAfx.h"
#include "CDXUTTable.h"
namespace FDO
{
CDXUTPanel::CDXUTPanel(CDXUTDialog *pDialog) : CDXUTControl( pDialog )
{
	m_Type = DXUT_CONTROL_PANEL;
}
CDXUTPanel::~CDXUTPanel()
{

}
bool CDXUTPanel::AddControl(CDXUTControl* pCtrl , uint nAlignMode ,bool bResize)
{
	assert(pCtrl);
	if(mpNode)
	{
		CDXUTControl::Node* pNew = mpNode->InsertChild();
		pNew->SetObject(pCtrl);
		if(pCtrl)
		{
			pCtrl->SetParentNode(pNew);		
			pCtrl->SetAlign(nAlignMode);
			if(bResize)
			{
				RECT rc = GetStretchRect();
				m_x = rc.left;
				m_y = rc.top;
				m_width = RectWidth(rc);
				m_height = RectHeight(rc);
				CDXUTControl::UpdateRects();
				//SetBody(rc.left,rc.top,RectWidth(rc),RectHeight(rc));
			}
		}
		
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
RECT CDXUTPanel::GetStretchRect() const
{
	// 一造所包含的元件(不包含panel)
	// 計算出涵蓋的最大區塊

	CDXUTControl::Node::ChildIterator itcur =  mpNode->GetChildsIteratorBegin();
	CDXUTControl::Node::ChildIterator itend	=  mpNode->GetChildsIteratorEnd();

	RECT rcPanel;	
	SetRectEmpty(&rcPanel);
	while(itcur != itend)
	{

		CDXUTControl* pCtrl = (*itcur)->GetObject();
		++itcur;
		if(pCtrl == 0 )
			continue;
		if(pCtrl->GetType() != DXUT_CONTROL_PANEL)
		{
			rcPanel = pCtrl->GetBoundingBox();	
			break;
		}		
	}
	for ( ; itcur != itend; ++itcur)
	{
		CDXUTControl* pCtrl = (*itcur)->GetObject();
		if(pCtrl == 0)
			continue;

		if(pCtrl->GetType() != DXUT_CONTROL_PANEL)
		{
			const RECT& rcCtrl = pCtrl->GetBoundingBox();			
			if(rcCtrl.left < rcPanel.left)
				rcPanel.left = rcCtrl.left;
			if(rcCtrl.top < rcPanel.top)
				rcPanel.top = rcCtrl.top;
			if(rcCtrl.right > rcPanel.right)
				rcPanel.right = rcCtrl.right;
			if(rcCtrl.bottom > rcPanel.bottom)
				rcPanel.bottom = rcCtrl.bottom;			
		}		
	}
	return rcPanel;
	//SetBody(rcPanel.left,rcPanel.top,RectWidth(rcPanel),RectHeight(rcPanel));
}
//////////////////////////////////////////////////////////////////////////
void CDXUTPanel::UpdatePanel()
{
	RECT empty;
	SetRectEmpty(&empty);
	UpdatePanel(m_rcBoundingBox,empty);
		
	

}
//////////////////////////////////////////////////////////////////////////
bool CDXUTPanel::IsRoot() const
{
	if(mpNode)
	{
		return mpNode->GetParent() == 0;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTPanel::UpdatePanel(const RECT& rcNow,const RECT& rcDiff)
{
	CDXUTControl::Node::ChildIterator itcur =  mpNode->GetChildsIteratorBegin();
	CDXUTControl::Node::ChildIterator itend	=  mpNode->GetChildsIteratorEnd();
	typedef std::multimap<uint,CDXUTControl*,std::greater<uint>> PanelSet;
	PanelSet ctrls;
	for ( ; itcur != itend; ++itcur)
	{
	
		CDXUTControl* pCtrl = (*itcur)->GetObject();
		if(pCtrl->GetType() != DXUT_CONTROL_PANEL)
		{
			int x,y;
			pCtrl->GetLocation(x,y);
			pCtrl->SetLocation(x + rcDiff.left,y + rcDiff.top);
		}
		else
		{
			ctrls.insert( make_pair(pCtrl->GetAlign() , pCtrl) ) ; 
		}
	}

	{
		RECT rcCurrent = rcNow; // 剩餘的區塊
		_FOREACH(PanelSet::iterator , ctrls , it)
		{
			CDXUTPanel* pCtrl = (CDXUTPanel*)it->second;			
			const RECT rcCtrl = pCtrl->GetStretchRect();
			RECT rcNew = rcCtrl;
			if(pCtrl->GetAlign() == CDXUTControl::ALIGN_TOP)
			{								
				int nValue = RectHeight(rcCtrl);// 高不變
				SetRect(&rcNew,rcCurrent.left,rcCurrent.top,rcCurrent.right,rcCurrent.top + nValue); // 吃掉頂端
				//更新剩下的區塊
				rcCurrent.top += nValue; // 吃掉頂端				
			}
			else if(pCtrl->GetAlign() == CDXUTControl::ALIGN_BOTTOM)
			{
				// 高不變 
				int nValue = RectHeight(rcCtrl);// 高不變				
				SetRect(&rcNew,rcCurrent.left,rcCurrent.bottom - nValue ,rcCurrent.right,rcCurrent.top + nValue); // 吃掉底端
				//更新剩下的區塊
				rcCurrent.bottom -= nValue; // 吃掉底端
			}
			else if(pCtrl->GetAlign() == CDXUTControl::ALIGN_LEFT)
			{								
				int nValue = RectWidth(rcCtrl);// 寬不變
				SetRect(&rcNew,rcCurrent.left,rcCurrent.top,rcCurrent.right + nValue,rcCurrent.bottom); // 吃掉左端
				//更新剩下的區塊
				rcCurrent.right += nValue; // 吃掉左端				
			}
			else if(pCtrl->GetAlign() == CDXUTControl::ALIGN_RIGHT)
			{								
				int nValue = RectWidth(rcCtrl);// 寬不變
				SetRect(&rcNew,rcCurrent.right - nValue,rcCurrent.top,rcCurrent.right + nValue,rcCurrent.bottom); // 吃掉右端
				//更新剩下的區塊
				rcCurrent.right -= nValue; // 吃掉右端				
			}
			else if(pCtrl->GetAlign() == CDXUTControl::ALIGN_CLIENT)
			{

				SetRect(&rcNew,rcCurrent.left,rcCurrent.top,rcCurrent.right,rcCurrent.bottom); // 全吃
				//更新剩下的區塊
				rcCurrent.right = rcCurrent.left;
				rcCurrent.bottom = rcCurrent.top;
			}
			else if(pCtrl->GetAlign() == CDXUTControl::ALIGN_NONE)
			{
				// 更新座標 不吃任和區塊
				int x,y;
				pCtrl->GetLocation(x,y);
				pCtrl->SetLocation(x + rcDiff.left,y + rcDiff.top);	

				continue;
			}

			// 更新對應物件
			pCtrl->SetBody(rcNew.left,rcNew.top,RectWidth(rcNew) , RectHeight(rcNew));		
		}
	}
}
//////////////////////////////////////////////////////////////////////////

void CDXUTPanel::UpdateRects()
{
	RECT rcNow;//現在
	RECT rcBefore = m_rcBoundingBox; //之前;
	SetRect( &rcNow, m_x, m_y, m_x + m_width, m_y + m_height );	
	
	RECT rcDiff;
	rcDiff.left = rcNow.left - rcBefore.left   ;
	rcDiff.top = rcNow.top - rcBefore.top    ;
	rcDiff.right = rcNow.right - rcBefore.right  ;
	rcDiff.bottom= rcNow.bottom - rcBefore.bottom;

	if((rcDiff.left == 0 &&
		rcDiff.top == 0 &&
		rcDiff.right == 0 &&
		rcDiff.bottom == 0 ) ||
		mpNode == 0)
	{
		CDXUTControl::UpdateRects();
		return ;
	}
	
	UpdatePanel(rcNow,rcDiff);

	CDXUTControl::UpdateRects();


}
#ifdef _DEBUG		
void CDXUTPanel::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
// 	if(GetConfig("DebugGUI") != "Yes" && m_pDialog)
// 	{
// 		RECT rcScreen(GetRect());
// 		//OffsetRect(&rcScreen,-m_x,-m_y);
//  		srand((int)this);
//  		if(IsRectEmpty(&rcScreen) == FALSE)
//  			m_pDialog->DrawRect(&rcScreen,(D3DCOLOR(random(256) * 0x100 + random(256) * 0x10000 + random(256) * 0x1) & 0x00ffffff) | 0x80000000);				
// 	}
	
}
#endif


}