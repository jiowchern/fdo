#pragma once
#include "Renderer.h"

namespace FDO
{
	
class CRenderTarget : public IRendererResource
{
public:
	CRenderTarget(const char *strName, IDirect3DDevice9 *pRenderer, int w, int h, D3DFORMAT format, bool depthstencil);
	~CRenderTarget(void);
	const char *GetResName(){ return mResName.c_str(); }
	IDirect3DTexture9 *GetTexture(){ return mpTexture; }

	bool Begin();
	void End();

	void OnLostDevice();
	void OnResetDevice();
	void OnRestoreDevice();
	INT GetWidth() { return mWidth; }
	INT GetHeight(){ return mHeight; }
private:
	bool SetupResources();

	bool mbDepthStencil;

	IDirect3DDevice9* mpDevice;

	IDirect3DTexture9* mpTexture;

	IDirect3DSurface9* mpDepthStencil;
	IDirect3DSurface9* mpBackBufferSurface;
	IDirect3DSurface9* mpTempBackBuffer;
	IDirect3DSurface9* mpTempDepthStencil;

	INT mWidth;
	INT mHeight;
	D3DFORMAT mFormat;
	std::string mResName;
};

};