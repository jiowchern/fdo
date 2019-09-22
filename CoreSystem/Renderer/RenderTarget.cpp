#include "StdAfx.h"
#include "RenderTarget.h"

namespace FDO
{

CRenderTarget::CRenderTarget(const char *strName, IDirect3DDevice9 *pRenderer, int w, int h, D3DFORMAT format, bool depthstencil)
: mpDevice(pRenderer)
, mWidth(w)
, mHeight(h)
, mFormat(format)
, mResName(strName)
{
	mbDepthStencil = depthstencil;
	mpTexture = NULL;
	mpBackBufferSurface = NULL;
	mpDepthStencil = NULL;
	mpTempBackBuffer = NULL;
	mpTempDepthStencil = NULL;
}

bool CRenderTarget::SetupResources()
{
    if(!mpBackBufferSurface)
	{		
		IDirect3DSurface9 *pDS = NULL;
		if(mbDepthStencil && !mpDepthStencil) 
		{
			if(FAILED(mpDevice->CreateDepthStencilSurface(mWidth,
				mHeight,
				D3DFMT_D16,
				D3DMULTISAMPLE_NONE, 
				0,
				TRUE,
				&pDS,
				NULL)))
			{
				_LOG(0, _T("Failed to create depth stencil!"));
				return false;
			}
			
			mpDepthStencil = pDS;
		}

		if(FAILED(mpDevice->CreateTexture( mWidth , mHeight , 1 ,D3DUSAGE_RENDERTARGET, 
									mFormat, D3DPOOL_DEFAULT, &mpTexture , NULL )))
		{
			_LOG(0, _T("Failed to create render target"));
			mpDepthStencil->Release();
			mpDepthStencil = NULL;
			return false;
		}

		if(FAILED(mpTexture->GetSurfaceLevel(0, &mpBackBufferSurface)))
		{
			_LOG(0, _T("Failed to Get render target surface"));
			mpTexture->Release();
			mpTexture = NULL;
			mpDepthStencil->Release();
			mpDepthStencil = NULL;
			return false;
		}
	}
	return true;
}

CRenderTarget::~CRenderTarget(void)
{

}



bool CRenderTarget::Begin()
{
	if(!SetupResources()) {
		// Remove this when fallbacks are done
		return false;
	}

	mpDevice->GetRenderTarget(0,&mpTempBackBuffer);
	mpDevice->GetDepthStencilSurface(&mpTempDepthStencil);

	bool bOK = (S_OK==mpDevice->SetRenderTarget(0,mpBackBufferSurface));
	bool bOK2 = (S_OK==mpDevice->SetDepthStencilSurface(mpDepthStencil));

	return bOK&&bOK2;
}

VOID CRenderTarget::End()
{
	if(mpTempBackBuffer) {
		mpDevice->SetRenderTarget(0, mpTempBackBuffer);
		mpTempBackBuffer->Release();
		mpTempBackBuffer = NULL;
	}

	if(mpTempDepthStencil) 
	{
		mpDevice->SetDepthStencilSurface(mpTempDepthStencil);
		mpTempDepthStencil->Release();
		mpTempDepthStencil = NULL;
	}
}

void CRenderTarget::OnLostDevice()
{
	SAFE_RELEASE(mpTexture); mpTexture = NULL;

	SAFE_RELEASE(mpDepthStencil);		mpDepthStencil = NULL;
	SAFE_RELEASE(mpBackBufferSurface);	mpBackBufferSurface = NULL;

	SAFE_RELEASE(mpTempDepthStencil);	mpTempDepthStencil = NULL;
	SAFE_RELEASE(mpTempBackBuffer);		mpTempBackBuffer = NULL;
}

void CRenderTarget::OnResetDevice()
{

}

void CRenderTarget::OnRestoreDevice()
{
	
}
};