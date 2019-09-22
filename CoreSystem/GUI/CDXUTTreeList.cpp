#include "stdafx.h"
#include "CDXUTTreeList.h"
#include "ListBoxItem.h"
#include <strsafe.h>
#include "CDXUTDef.h"
namespace FDO
{
	CDXUTTreeNode::CDXUTTreeNode(CDXUTTreeNode* pParent) : Tree::TreeNode<CDXUTTreeNode>(pParent)
	,mnCurState(CDXUTTreeNode::Open) ,mpData(0) 

	{
		memset(&mTxr,0,sizeof(mTxr));
		memset(mnText,0,sizeof(mnText));
		
	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTTreeNode::~CDXUTTreeNode()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeNode::Initial(void* pData,const char16* wstext,const CDXUTElement* pTxr)
	{
		mpData = pData;
		StringCchCopyW(mnText,TEXT_LEN,wstext);
		RECT rc;
		mTxr.SetTexture(-1,&rc);
		mTxr.SetFont(0);
		mTxr = (pTxr)? *pTxr : mTxr;				
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeNode::SetOpen(bool bOpen)
	{		
		mnCurState = (bOpen || HasChilds() == false)?CDXUTTreeNode::Open : CDXUTTreeNode::Close ; 
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeNode::SetText(const wchar_t* text)
	{
		wcsncpy(mnText,text,TEXT_LEN);
		mnText[TEXT_LEN-1] = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeNode::SetData(void* pData)
	{
		mpData = pData;
	}
	//////////////////////////////////////////////////////////////////////////
	void* CDXUTTreeNode::GetData()	const 
	{
		return mpData;
	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTElement* CDXUTTreeNode::GetCurElement()	
	{
		return &mTxr;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	DXUTTreeItem::DXUTTreeItem(const TreeIterator::NodeInfo& inf)
		: pNode(inf.pNode) , nLevel(inf.nLevel)
	{		
		SetRect(&mrcTrigger,0,0,0,0);
	}
	//////////////////////////////////////////////////////////////////////////
	DXUTTreeItem::~DXUTTreeItem()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void DXUTTreeItem::Blend(float fElapsedTime)
	{
		CDXUTElement* pTxr = pNode->GetCurElement();
		pTxr->TextureColor.Blend(DXUT_STATE_NORMAL,fElapsedTime);
		pTxr->FontColor.Blend(DXUT_STATE_NORMAL,fElapsedTime);
	}
	//////////////////////////////////////////////////////////////////////////
	void DXUTTreeItem::SetTriggerRect(const RECT& rc)
	{
		mrcTrigger = rc;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DXUTTreeItem::InTriggerRect(const POINT& pt)
	{
		return PtInRect(&mrcTrigger,pt) == TRUE;
	}
	//////////////////////////////////////////////////////////////////////////		
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CDXUTTreeList::CDXUTTreeList( CDXUTDialog *pDialog ) 
		: mpRoot(0),mnBeginIndex(0) ,mnIndent(20) , CDXUTListBox(pDialog) ,mpHideSelectNode(0)
	{
		CDXUTElement ele;
		ele.SetTexture(-1,0);
		for (int i = Open ; i < Count ; ++i)
		{
			SetElement(i,&ele);
		}	
		m_ScrollBar.SetEnabled(false);
		m_ScrollBar.SetVisible(false);
		
	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTTreeList::~CDXUTTreeList()
	{
		DestoryRoot();
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::Initital(int ID, int x, int y, int width, int height, const Style* pStyle /*= 0*/)
	{		
		SetID( ID );
		SetLocation( x, y );
		SetSize( width, height );
		SetStyle( 0 );	
		
		if(pStyle)
		{
			SetStyle( pStyle->dwStyle );						
			if(pStyle->closeIcon)
				*GetElement(Close) = *pStyle->closeIcon;
			if(pStyle->openIcon)
				*GetElement(Open) = *pStyle->openIcon;
			if(pStyle->nodeIcon)
				*GetElement(Node) = *pStyle->nodeIcon;
			
			SetIconSize(pStyle->szIcon.cx,pStyle->szIcon.cy);			
		}
		else
		{
			// add test code ...
#ifdef _DEBUG
			CDXUTElement empty;			
			RECT rc;			
			empty.TextureColor.Init(-1);
			empty.SetFont(0);

			::SetRect(&rc,18,3,32,17);  
			empty.SetTexture(1,&rc);
			*GetElement(Close) = empty;			

			::SetRect(&rc,38,4,52,16);  
			empty.SetTexture(1,&rc);
			*GetElement(Open) = empty;			

			::SetRect(&rc,3,86,21,103);  
			empty.SetTexture(1,&rc);
			*GetElement(Node) = empty;			

			SetIconSize(16,16);			
#endif
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
	{
		{
			// 處理 icon 的顏色
			for (int i = 0 ; i < m_Items.GetSize() ; ++i)
			{
				DXUTTreeItem* pItem = (DXUTTreeItem*)m_Items.GetAt(i);
				pItem->Blend(fElapsedTime);
			}

			for (int i = Open ; i < Count ; ++i)
			{				
				GetElement(i)->TextureColor.Blend(DXUT_STATE_NORMAL,fElapsedTime);				
				
			}
			
		}
		
		CDXUTListBox::Render(pd3dDevice,fElapsedTime);
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::DrawItemText(DXUTListBoxItem* pItem, CDXUTElement* pElement, RECT* pRect)
	{
		DXUTTreeItem* pTreeItem = (DXUTTreeItem*)pItem;
		if (!pRect)
			pRect = &pTreeItem->rcActive;

		DXUTFontNode* pFontNode = m_pDialog->GetManager()->GetFontNode( GetElement( 0 )->iFont );
		int nIndent = pTreeItem->nLevel * mnIndent;
		int nTxrSize = RectHeight(*pRect) ;
		int nFontHeight = pFontNode->nHeight;
		int nOffset = (nTxrSize - nFontHeight ) / 2 ;
		
		RECT rcTxr ;		
		CDXUTElement* pTxr = 0;
		// render open/close icon
		rcTxr = *pRect;
		rcTxr.left += nIndent ;
		rcTxr.right = rcTxr.left + nTxrSize;
		if(pTreeItem->pNode->HasChilds())
			pTxr = GetElement((pTreeItem->pNode->IsOpen())?Open : Close) ;
			
		if(pTxr && pTxr->iTexture != -1)
		{
			RECT rcTxr2;			
			SetRect(&rcTxr2,0,0,nFontHeight,nFontHeight);
			OffsetRect(&rcTxr2,rcTxr.left + nOffset,rcTxr.top + nOffset);			
			pTreeItem->SetTriggerRect(rcTxr2);
			m_pDialog->DrawSprite(pTxr,&rcTxr2);
		}

		// render item icon		
		rcTxr.left += nTxrSize;		
		rcTxr.right = rcTxr.left + nTxrSize;
		pTxr = pTreeItem->pNode->GetCurElement();
		if(pTxr && pTxr->iTexture != -1)
			m_pDialog->DrawSprite(pTxr,&rcTxr);

		// render text
		RECT rcText = *pRect;
		rcText.left += nIndent + nTxrSize /* o/c icon*/ + nTxrSize /*item icon*/ + nOffset;
		OffsetRect(&rcText,0,nOffset);
		CDXUTElement* pText = pTreeItem->pNode->GetCurElement();		
		pTreeItem->TextObject.ChangeDefaultColor(pText->FontColor.Current);			
 		m_pDialog->DrawTextObject(&pItem->TextObject, pElement, &rcText);		
	}
	
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::SetBegin(int nIdx,bool bUpdate /*= false*/)
	{
		mnBeginIndex = nIdx;
		if(bUpdate)
			UpdateList();
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::SetIconSize(int cx,int cy)
	{
		mszIcon.cx = cx;
		mszIcon.cy = cy;
		UpdateRects();
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::SetIndent(int nIndent)
	{
		mnIndent = nIndent;
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::UpdateList()
	{
		const int gMaxLevel = 0x7fffffff;		
		DXUTTreeItem* pTreeItem = (DXUTTreeItem*)CDXUTListBox::GetSelectedItem();				
		CDXUTTreeNode*	pSelectNode = 0;
		if(pTreeItem)
		{
			pSelectNode = pTreeItem->pNode;
			mpHideSelectNode = pSelectNode; // 假定為隱藏SelectNode
		}
		else
		{
			pSelectNode = mpHideSelectNode;
		}
		CDXUTListBox::RemoveAllItems();			
		if(mpRoot)
		{
			
			int nSize = CDXUTListBox::GetPageSize();			
			TreeIterator it(mpRoot);
			int nBegin = mnBeginIndex;
			int nHideLevel = gMaxLevel;
			int nAddCount = 0;
			int nLoopCount = 0;			
			for ( ; it.IsEnd() == false ; ++it)
			{
				TreeIterator::NodeInfo inf = it.Get();								
				CDXUTTreeNode*	pNode = inf.pNode;
				int				nLevel = inf.nLevel;
				
				if(nLevel > nHideLevel)
					continue;				

				nHideLevel = (pNode->IsOpen() == false)? nLevel : gMaxLevel;
				++nLoopCount;

				if(nBegin != 0)
				{
					--nBegin;
					continue;
				}

				
				if(nAddCount < nSize)
				{
					
					_AddItem(inf);	
					if(pNode == pSelectNode)
					{
						CDXUTListBox::SetSelected(CDXUTListBox::GetItemSize()-1);	
						mpHideSelectNode = 0; // 確定有SelectNode 清除隱藏Node
					}
					++nAddCount ;
				}
				
			}
			mDXUTScrollBarVisitor.AcceptMaxSize(nLoopCount , nSize);						
		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::_AddItem(TreeIterator::NodeInfo& inf)
	{
		DXUTTreeItem *pNewItem = new DXUTTreeItem(inf);
		if( !pNewItem )
			return ;
		
		StringCchCopy( pNewItem->strText, _MaxItemTextSize_, pNewItem->pNode->GetText() );
		pNewItem->pData = pNewItem->pNode->GetData();
		SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
		pNewItem->bSelected = false;			
		pNewItem->FontColor = 0xffff00ff;		
		int nTextWidth = RectWidth(m_rcText) - pNewItem->nLevel * mnIndent; 
		// Create a CTextObject for this item.
		CTextObject* pTextObject = g_FDOXML.MakeTextObject(
			g_pD3DDevice, NULL, 0xffff00ff, pNewItem->pNode->GetText() , -1, nTextWidth);
		pNewItem->TextObject = *pTextObject;
		delete pTextObject;

		HRESULT hr = m_Items.Add( pNewItem );
		if( FAILED(hr) )
		{
			SAFE_DELETE( pNewItem );
		}
		else
		{
			UpdateTrackRange();
		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTTreeNode* CDXUTTreeList::CreateRoot()
	{
		if(mpRoot == 0)
		{
			mpRoot = CreateTreeNode(0);
		}		
		return mpRoot ;
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::DestoryRoot()
	{
		
		if(mpRoot)
		{
			DestoryTreeNode(mpRoot);
			mpRoot = 0;
		}
		UpdateList();
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	CDXUTTreeNode* CDXUTTreeList::InsertChild(CDXUTTreeNode* pNode)
	{
		CDXUTTreeNode* pNew = 0;
		if(pNode != 0)
		{
			pNew = CreateTreeNode(pNode);
		}
		else if(/*pNode == 0 && */mpRoot == 0)
		{
			mpRoot = CreateTreeNode(0);
			pNew = mpRoot;			
		}		
		return pNew;
	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTTreeNode* CDXUTTreeList::InsertSib(CDXUTTreeNode* pNode)
	{
		CDXUTTreeNode* pNew = 0;
		if(pNode != 0 && pNode->GetParent() != 0)
		{
			CDXUTTreeNode* pParent = pNode->GetParent();
			pNew = CreateTreeNode(pParent);	
		}		
		return pNew;
	}	
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::RemoveNode(CDXUTTreeNode* pNode)
	{
		if(mpRoot == pNode)
		{
			mpRoot = 0;
		}
		DestoryTreeNode(pNode);
	}
	//////////////////////////////////////////////////////////////////////////
	bool CDXUTTreeList::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
	{
		if( !m_bEnabled || !m_bVisible )
			return false;
		switch( uMsg )
		{		
		case WM_LBUTTONDOWN:		
			{
				DXUTTreeItem* pItem = (DXUTTreeItem*)GetSelectedItem();
				if(pItem)
				{
					if(pItem->InTriggerRect(pt))
					{
						pItem->pNode->SetOpen(!pItem->pNode->IsOpen());
						UpdateList();
						if( pItem->pNode->IsOpen() )
							m_pDialog->SendEvent( EVENT_TREELIST_OPEN , true, this ) ;
						else
							m_pDialog->SendEvent( EVENT_TREELIST_CLOSE , true, this ) ;
					}					
				}

			}
		}
		return CDXUTListBox::HandleMouse(uMsg,pt,wParam,lParam);
	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTTreeNode* CDXUTTreeList::InsertChild(CDXUTTreeNode* pParent,void* pData,const char16* wstext,const CDXUTElement* pTxr)
	{
		CDXUTTreeNode* pNew = InsertChild(pParent);
		if(pNew)
			pNew->Initial(pData,wstext,(pTxr)?pTxr : GetElement(Node));

		return pNew;
	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTTreeNode* CDXUTTreeList::InsertSib(CDXUTTreeNode* pSib,void* pData,const char16* wstext,const CDXUTElement* pTxr)
	{
		CDXUTTreeNode* pNew = InsertSib(pSib);
		if(pNew)
			pNew->Initial(pData,wstext,(pTxr)?pTxr : GetElement(Node));

		return pNew;
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::UpdateRects()
	{
		// UPDATE M_NtextHeight
		if(m_nTextHeight < mszIcon.cx )
		{
			m_nTextHeight = mszIcon.cx ;
		}
		CDXUTListBox::UpdateRects();
		
		UpdateList();
	}
	//////////////////////////////////////////////////////////////////////////
	void CDXUTTreeList::Visit(ScrollBarEvent::CurPos& e) 
	{		
		SetBegin(e.nCurPos , true);		
	}
}