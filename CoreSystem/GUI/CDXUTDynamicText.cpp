#include "stdafx.h"
#include "CDXUTDynamicText.h"
namespace FDO
{

	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CElementHanlder::CElementHanlder(CElement* pElement,const POINT& ptBegin) 
		: mpElement(pElement) , mptDraw(ptBegin)
	{
		assert(mpElement);		
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CElementHanlder::~CElementHanlder()
	{

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CElementHanlder::Draw(int nBegin,int nEnd,const POINT& ptScreen,CDXUTDialogResourceManager* pManager)
	{
		assert(mpElement);
		UINT nformat = DT_LEFT | DT_TOP;

		SIZE eleSize;
		mpElement->GetExtent(eleSize);

		RECT rcDraw ;
		rcDraw.left		= mptDraw.x;
		rcDraw.top		= mptDraw.y;
		rcDraw.right	= eleSize.cx + rcDraw.left;
		rcDraw.bottom	= eleSize.cy + rcDraw.top;

		LONG& nHead = ((nBegin > nEnd)? rcDraw.left : rcDraw.right); 
		LONG& nTail = ((nBegin > nEnd)? rcDraw.right : rcDraw.left); 

		if(nTail > nBegin)
		{
			nTail = nBegin;
		}

		if(nHead < nEnd)
		{			
			if(mpElement->IsImage() == false)
			{
				nHead = nEnd;
				nformat = DT_RIGHT | DT_TOP;
			}
			else
			{
				// 圖形不顯示 ...
				//nHead = nTail = 0;
				return;
			}
		}		
		
		 
		rcDraw.left		+= ptScreen.x;
		rcDraw.top		+= ptScreen.y;
		rcDraw.right	+= ptScreen.x;
		rcDraw.bottom	+= ptScreen.y;

		mpElement->SetRect(rcDraw);
		//IDirect3DDevice9* pDev= pManager->GetD3DDevice();

		/*pDev->SetRenderState(D3DRS_ZENABLE,FALSE);
		pDev->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		pDev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);*/
		/*

		*/
		

		mpElement->DrawTextW(nformat,0,false,true);

		//mpElement->DrawTextW(nformat,pManager->m_pSprite,false,true);


		/*pDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

		pDev->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		pDev->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		pDev->SetRenderState(D3DRS_ZENABLE,TRUE);*/


	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CElementHanlder::Move(int nStep)
	{		
		mptDraw.x += nStep;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CElementHanlder::SetPos(int x)
	{
		mptDraw.x = x;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int CElementHanlder::GetTailPos()
	{
		SIZE eleSize;
		mpElement->GetExtent(eleSize);		
		return mptDraw.x + eleSize.cx;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CElementHanlder::HasDrawTail(int nBegin)
	{
		SIZE eleSize;
		mpElement->GetExtent(eleSize);		
		return mptDraw.x + eleSize.cx < nBegin;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CElementHanlder::HasDraw(int nEnd)			
	{
		SIZE eleSize;
		mpElement->GetExtent(eleSize);
		return mptDraw.x + eleSize.cx > nEnd;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CDXUTDynamicText::CDXUTDynamicText( CDXUTDialog *pDialog ) : CDXUTStatic(pDialog) , mfPrevStepTime(0.0f) ,mfSecondPerPixle(0.05f)
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CDXUTDynamicText::~CDXUTDynamicText()
	{

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CDXUTDynamicText::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
	{
		assert(m_pDialog);
		if( m_bVisible == false || m_pTextObject == 0)
			return;
		const float fStepTime = mfSecondPerPixle;
		int		nBeginPos	= _GetBeginPos();
		int		nEndPos		= _GetEndPos();
		int		nStep		= 0;
		int		i;
		POINT	ptScreen;
		m_pDialog->GetLocation(ptScreen);
		mfPrevStepTime += fElapsedTime;

		if(mfPrevStepTime > fStepTime)
		{
			nStep = (int)(_GetStep() * (mfPrevStepTime/fStepTime));
			mfPrevStepTime = 0.0f;
		}

		DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

		if( m_bEnabled == false )
			iState = DXUT_STATE_DISABLED;

		CDXUTElement* pElement = GetElement( 0 ) ;

		pElement->FontColor.Blend( iState, fElapsedTime );
		m_pTextObject->ChangeDefaultColor(pElement->FontColor.Current);

		CElementHanlder* pPrev = 0;
		CElementHanlder* pCur = 0;
		int eleSize = mEleHandlers.size();
		if(eleSize)
		{
			mEleHandlers[0]->Move(nStep);

			for( i = 0 ; i < eleSize; ++i)
			{
				pCur = mEleHandlers[i];

				if(pPrev && pPrev->HasDrawTail(nBeginPos) == false)
				{
					break;
				}
				if(nStep != 0 && pPrev)
				{
					pCur->SetPos(pPrev->GetTailPos());
				}			

				pCur->Draw(nBeginPos,nEndPos,ptScreen,m_pDialog->GetManager());

				pPrev = pCur;
			}

		}


		if(pCur != 0 && pCur == pPrev && pCur->HasDraw(nEndPos) == false)
		{
			// draw over...
			_DestoryEles(mEleHandlers);
			pCur = 0;
			pPrev = 0;


			m_pDialog->SendEvent(EVENT_PLAY_DONE,true,this);
		}

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CDXUTDynamicText::SetSecondPerPixle(float fSecond)
	{
		mfSecondPerPixle = fSecond;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CDXUTDynamicText::Play(const wchar_t* str)
	{
		SetText(str);
		if(m_pTextObject)
		{
			_DestoryEles(mEleHandlers);
			_InitPlay(m_pTextObject);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int	CDXUTDynamicText::_GetBeginPos()
	{
		return m_rcBoundingBox.right;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int CDXUTDynamicText::_GetEndPos()
	{

		return m_rcBoundingBox.left;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int CDXUTDynamicText::_GetStep()
	{
		return (_GetBeginPos() > _GetEndPos())? -1 : 1 ;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CDXUTDynamicText::_InitPlay(CTextObject* pObj)
	{
		// 初始化元素容器
		CLineObjectIter lineIt = m_pTextObject->m_Lines.begin();
		for( ; lineIt != m_pTextObject->m_Lines.end(); ++lineIt )
		{
			CElementIter eleIt ; 
			if((*lineIt).GetElementIter(eleIt))
			{				
				_CreateEles(mEleHandlers,eleIt,(*lineIt).TotalElements());			
			}
		}

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CDXUTDynamicText::_CreateEles(CElementHanlderSet& set,CElementIter& it,int nEleCount)
	{
		size_t i ;
		i = set.size();
		set.resize(set.size() + nEleCount);
		POINT ptBegin ;
		ptBegin.x = _GetBeginPos();
		ptBegin.y = m_rcBoundingBox.top;

		for( i ; i < set.size(); ++i)
		{
			CElementHanlder* pHnd = new CElementHanlder(&*it,ptBegin);
			set[i] = pHnd;
			++it;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CDXUTDynamicText::_DestoryEles(CElementHanlderSet& set)
	{
		size_t i;
		for( i = 0 ; i < set.size(); ++i)
		{
			delete set[i];
		}
		set.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool CDXUTDynamicText::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
	{
		return CDXUTStatic::HandleMouse(uMsg,pt,wParam,lParam);
	}
};