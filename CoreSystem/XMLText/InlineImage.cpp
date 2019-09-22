//
// InlineImage.cpp
//
#include "StdAfx.h"
#include <assert.h>
#include "CDXUTDialogResourceManager.h"
#include "DrawPlane.h"
#include "InlineImage.h"


//#include "Global.h"

//==============================================================================================
// CInlineImage instance members
//==============================================================================================


	CInlineImage::CInlineImage()
	{
		m_pD3DTexture = NULL;
		m_pSrcRect = NULL;
		ZeroMemory(&m_SrcRect, sizeof(RECT));
		ZeroMemory(&m_OrgSrcRect, sizeof(RECT));
		ZeroMemory(&m_ImageSize, sizeof(SIZE));
		m_dwAlignFormat = DT_BOTTOM;
	}

	CInlineImage::~CInlineImage()
	{


	}

	void CInlineImage::SetImage(LPCWSTR szFilename, RECT* pSrcRect)
	// If pRect is NULL, the entire image will be rendered.
	{
		// Setup m_pD3DTexture.
		CDXUTDialogResourceManager* pMgr = GetDialogResourceManager();
		assert(pMgr);
		int iTextureNode = pMgr->AddTexture(szFilename);
		DXUTTextureNode* pTextureNode = pMgr->GetTextureNode(iTextureNode);
		assert(pTextureNode);
		m_pD3DTexture = pTextureNode->pTexture;
		assert(m_pD3DTexture);
	    
		// Setup m_pSrcRect.
		if (!pSrcRect) {
			m_pSrcRect = NULL;
			m_ImageSize.cx = pTextureNode->dwWidth;
			m_ImageSize.cy = pTextureNode->dwHeight;
		} else {
			CopyRect(&m_SrcRect, pSrcRect);
			CopyRect(&m_OrgSrcRect, pSrcRect);
			m_pSrcRect = &m_SrcRect;
			m_ImageSize.cx = RectWidth(m_SrcRect);
			m_ImageSize.cy = RectHeight(m_SrcRect);
		} // end else
	}

	void CInlineImage::Render(const RECT& rcDest,bool bStretch /*= false*/)
	// Ref: CDXUTDialog::DrawSprite()
	{
		assert(m_pD3DTexture);
		CDXUTDialogResourceManager* pMgr = GetDialogResourceManager();
		assert(pMgr);
		ID3DXSprite* pSprite = pMgr->m_pSprite;
		//RECT screen; 

		const LPDIRECT3DDEVICE9 pDevice = pMgr->GetD3DDevice() ; 
	    
		// Setup transformation matrix.
		D3DXMATRIXA16 matTransform;
		D3DXMatrixIdentity( &matTransform );
		pSprite->SetTransform( &matTransform );

		// �p�G rcDest �� size �� m_ImgaeSize �p�A�� clippling�C
		// �ثe�u�䴩 DT_BOTTOM�C
		if (RectHeight(rcDest) < m_ImageSize.cy) 
		{
			if (m_dwAlignFormat & DT_BOTTOM) 
			{
				m_SrcRect.top = m_SrcRect.top + m_ImageSize.cy - RectHeight(rcDest);
				m_SrcRect.bottom = m_SrcRect.top + RectHeight(rcDest);
				m_pSrcRect = &m_SrcRect;
			} // end if
		} 
		else 
		{
			// �^�_��Ӫ� SrcRect
			m_SrcRect = m_OrgSrcRect;
		}
	    
		// CInlineImage ���@�Ӱ��D�Am_pSrcRect �|����O�� CInlineImage::m_SrcRect �W�A
		// �y�� Render �����T�C�ѨM��k�O���s���w m_pSrcRect�C
		if (m_pSrcRect)
			m_pSrcRect = &m_SrcRect;

		FDO::CRenderer *pRender = CDXUTDialog::GetRenderer() ; 
	        
		// Setup position vector.
		D3DXVECTOR3 vPos( (float)rcDest.left, (float)rcDest.top, 0.0f ) ; 
		if(bStretch == false)	
		{
			// ø�s�ϧ�

			//SetRect(&screen , vPos.x , vPos.y ,  vPos.x + (m_pSrcRect->right - m_pSrcRect->left) , vPos.y + ( m_pSrcRect->bottom - m_pSrcRect->top)) ; 

			//if(pRender != NULL)
			//{
			//	pRender->CreateUiOp(m_pD3DTexture , screen , m_pSrcRect , m_ImageSize.cx , m_ImageSize.cy , 0xFFFFFFFF) ; 

			//}

			HRESULT hr = pSprite->Draw(m_pD3DTexture, m_pSrcRect, NULL, &vPos, 0xFFFFFFFF);
			if (S_OK != hr) {
				pSprite->Begin(0);
				pSprite->Draw(m_pD3DTexture, m_pSrcRect, NULL, &vPos, 0xFFFFFFFF);
				pSprite->End();
				return;
			} // end if
		}
		else
		{
			RECT rcDraw = m_SrcRect;
			rcDraw.right = rcDraw.left + (rcDest.right - rcDest.left);
			rcDraw.bottom = rcDraw.top + (rcDraw.bottom - rcDraw.top);

			//SetRect(&screen , vPos.x , vPos.y ,  vPos.x + (rcDraw.right - rcDraw.left) , vPos.y + ( rcDraw.bottom - rcDraw.top)) ; 

			//if(pRender != NULL)
			//{
			//	pRender->CreateUiOp(m_pD3DTexture , screen , &rcDraw , m_ImageSize.cx , m_ImageSize.cy , 0xFFFFFFFF) ; 

			//}

			pSprite->Draw(m_pD3DTexture, &rcDraw, NULL, &vPos, 0xFFFFFFFF);
		}
	    
	}



