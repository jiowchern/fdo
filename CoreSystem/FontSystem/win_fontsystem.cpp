#include "StdAfx.h"
#include "win_fontsystem.h"
#include "PhenixD3D9RenderSystem.h"
#include "DrawPlane.h"
WinFont::WinFont(FontSystem::FontHandle hnd,const WinFont::Style& fs) : FontSystem::Font(hnd,fs)
{

}
//////////////////////////////////////////////////////////////////////////
WinFont::~WinFont()
{

}
//////////////////////////////////////////////////////////////////////////
void WinFont::Initial() 
{
	
	HFONT hFont = ::CreateFontW(mstyle.size,0,0,0,mstyle.weight,mstyle.italic,mstyle.underline,mstyle.strikeout,DEFAULT_CHARSET , 
		OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS,PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE,mstyle.face.c_str());
//	
	if(hFont == NULL)//L"文鼎中粗隸"
		return ;
	
	mhFont = hFont;
	//////////////////////////////////////////////////////////////////////////

	uint nHeight;
	uint nAscent;
	uint renderMode;
	uint nMaxWidth;
	uint nGlyphsSupported;

	HDC hDC = ::CreateCompatibleDC(NULL);
	if(!hDC) 
		return ;

	mhDC = hDC;
	SelectObject(hDC,hFont);

	TEXTMETRIC tm;
	::GetTextMetrics(hDC,&tm);
	nHeight = tm.tmHeight;
	nAscent = tm.tmAscent;
	mnAscent = nAscent;

	//put this as a parameter (bitmap or 8bit gray levels)
	if(nHeight >= 18 || (nHeight >= 14 && mstyle.weight == 1000)) {
		renderMode = GGO_GRAY8_BITMAP;
	}
	else {
		renderMode = GGO_BITMAP;	
	}
	nMaxWidth = tm.tmMaxCharWidth;

	//POPULATE KERNING MAP
// 	DWORD dwSize=GetKerningPairs(hDC,0,NULL);
// 
// 	if(dwSize)
// 	{
// 		KERNINGPAIR* pKerningPairs=
// 			(KERNINGPAIR*)new BYTE[dwSize*sizeof(KERNINGPAIR)];
// 
// 		if(!GetKerningPairs(hDC,dwSize,pKerningPairs))
// 			return FALSE;
// 
// 		for(DWORD n = 0; n < dwSize; n++) {
// 			KERNINGPAIR *kp = &pKerningPairs[n];
// 			if(kp->iKernAmount == 0) continue;
// 			UINT kkey = _MAKE_KERNING_KEY(kp->wFirst,kp->wSecond);
// 			kerningMap.insert(KerningMap::value_type(kkey,kp->iKernAmount));
// 		}
// 
// 		delete pKerningPairs;
// 		bHasKerning = TRUE;
// 	}
// 	else {
// 		bHasKerning = FALSE;
// 	}

	DWORD rgsize = ::GetFontUnicodeRanges(hDC,NULL);
	LPGLYPHSET pRanages = (LPGLYPHSET)malloc(rgsize);
	::GetFontUnicodeRanges(hDC,pRanages);
	nGlyphsSupported = pRanages->cGlyphsSupported;
	free(pRanages);

	mrenderMode = renderMode;
	return ;
}
//////////////////////////////////////////////////////////////////////////
void WinFont::Release() 
{
	::DeleteObject(mhFont);
	::DeleteDC(mhDC);
}	


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct _RasterizedGlyph 
{
	_RasterizedGlyph(){}
	~_RasterizedGlyph(){}
	BYTE *data;
	DWORD bpp;
	DWORD pitch;	
	UINT h;
	UINT w;
	UINT xoffset;
	UINT yoffset;
	UINT advance;	
};
//////////////////////////////////////////////////////////////////////////
struct LineRecord;
typedef 	ObjFactory<LineRecord>	LineRecordFactory;
static LineRecordFactory glrf;

struct LineRecord
{	
	sint						iBegin;
	sint						iEnd;
	WinFontSystem::TextureCode*	codes[1024];
	uint						codecount;
	uint						width;
	uint						height;

	LineRecord() : iBegin(0),iEnd(0),width(0),height(0),codecount(0)
	{
		memset(codes , 0 , 1024*4);
	}
	void* operator new(size_t size)
	{
		return glrf.Create();
	}
	// 	//////////////////////////////////////////////////////////////////////////
	void operator delete(void* p)
	{
		glrf.Destroy(p);
	}
};



typedef std::list<LineRecord*> LineRecordSet;
static LineRecordSet	lines;


//////////////////////////////////////////////////////////////////////////
WinFontSystem::WinFontSystem()
{

}
//////////////////////////////////////////////////////////////////////////
WinFontSystem::~WinFontSystem()
{
	
}
//////////////////////////////////////////////////////////////////////////
WinTexture WinFontSystem::_CreateTexture(uint w,uint h) 
{
	WinTexture wt;
	D3D9RenderSystem& render =  g_RenderSystem;
	WinTexture::Texture& d3dtex = wt.txr;

	uint ntexturesize = 1;
	uint nsize = max(w,h);
	while (ntexturesize < nsize)
	{
		ntexturesize*=2;
	}
	
	
	const LPDIRECT3DDEVICE9	 dev = render.GetD3DDevice();
	HRESULT hr = dev->CreateTexture(
		w,
		h,
		1,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED, 
		&d3dtex,
		NULL);


	_ASSERT(SUCCEEDED(hr));

	D3DXIMAGE_INFO& info = wt.info;
	info.Depth = 1;
	info.Format = D3DFMT_A8R8G8B8;
	info.Width = w;
	info.Height = h;
	info.MipLevels = d3dtex->GetLevelCount();
	info.ImageFileFormat = D3DXIFF_BMP;
	info.ResourceType = D3DRTYPE_TEXTURE;	

	return wt;
}
//////////////////////////////////////////////////////////////////////////

void WinFontSystem::_DestoryTexture(Texture& txr) 
{
	IDirect3DTexture9* pTexture = txr.txr;



	if( GetConfig( "OutputFontTexture" ) == "Yes" )
	{
		LARGE_INTEGER nLintTC;
		QueryPerformanceCounter(&nLintTC);

		IDirect3DSurface9 *pSurface = 0;
		pTexture->GetSurfaceLevel(0 , &pSurface ) ; 
		wstring wszFilename;
		StringFormat16(wszFilename,L"fonttexture_%I64d.tga" , nLintTC.QuadPart);
		D3DXSaveSurfaceToFile( wszFilename.c_str(), D3DXIFF_TGA, pSurface, NULL, NULL );
		pSurface->Release() ; 
	}	

	pTexture->Release();
}
//////////////////////////////////////////////////////////////////////////
void WinFontSystem::_MakeFontTexture(WinFont* font, WinTexture& txr,Rect32& rc,Code code) 
{
	uint	codepoint	= code;
	uint	rendermode  = font->GetRenderMode();
	HDC		hDC			= font->GetHDC();	
	
	//RASTERIZES THE GLYPH
	GLYPHMETRICS gm;
	memset(&gm , 0 ,sizeof(GLYPHMETRICS));
	GLYPHMETRICS gm2;
	memset(&gm2 , 0 ,sizeof(GLYPHMETRICS));

	MAT2 mat = {0};
	::ZeroMemory(&mat,sizeof(MAT2));
	mat.eM11.value = 1;
	mat.eM22.value = 1;

	DWORD gsize = GetGlyphOutlineW(hDC,codepoint,rendermode,&gm,0,NULL,&mat);
	//DWORD err = ::GetLastError();
	if(gsize == GDI_ERROR)
		return ;
	
	BYTE *data = (gsize >0)? new BYTE[gsize] : 0;
	gsize = GetGlyphOutlineW(hDC,codepoint,rendermode,&gm2,gsize,data,&mat);
	if(gsize == GDI_ERROR)
		return ;

	
	

	UINT w = gm.gmBlackBoxX;
	UINT h = gm.gmBlackBoxY;
	UINT bbp = rendermode == GGO_BITMAP?1:8;
	UINT pitch;

	switch(bbp) {
	case 1:	// 1 bit
		pitch = w%32>0?((w+(32-(w%32)))/8):w/8;
		break;
	case 4: // 4 bits
	case 8: // 8 bits
		pitch = (w%4)?w + (4-(w%4)):w;
		break;
	default:
		_ASSERT(0);
	}

	//experiment
	if(rendermode == GGO_GRAY8_BITMAP) {
		UINT estimatedsize = pitch*h;
		if((estimatedsize != gsize) && ((w%4) == 0)) {

			if(estimatedsize + pitch == gsize) {
				h++;
			}
			else {
				pitch = pitch + 4;
				h = gsize/pitch;
			}
		}
	}

	_RasterizedGlyph rg ;
	rg.data = data;
	rg.w = w;
	rg.h = h;

	rg.xoffset = gm.gmptGlyphOrigin.x;
	txr.offsetx = rg.xoffset;
	
	rg.yoffset = gm.gmptGlyphOrigin.y;
	txr.offsety = font->GetAscent() - rg.yoffset ;

	txr.nWidth = gm.gmBlackBoxX;;
	txr.nHeight = gm.gmBlackBoxY;

	rg.advance = gm.gmCellIncX;
	
	//txr.advance = rg.advance;

	rg.bpp = bbp;
	rg.pitch = pitch;
// 
// 	mmapRasterizedGlyphs.insert(RasterizedGlyphsMap::value_type(codepoint,ret));

	{
		BYTE *data = rg.data;
		///////////////////////////////////////////////////////////////////////
		

		INT w = rg.w;
		INT h = rg.h;

		RECT glyphrect;
		glyphrect.left = rc.left;
		glyphrect.top = rc.top;
		glyphrect.bottom = rc.top + h;
		glyphrect.right = rc.left + (rc.right - rc.left);

		///////////////////////////////////////////////////////////////////////
		//rasterize
		D3DLOCKED_RECT lr;

		UINT bmppitch = rg.pitch;
		
		UINT mnSlotWidth = glyphrect.right - glyphrect.left;
		UINT mnSlotHeight = glyphrect.bottom - glyphrect.top;
		HRESULT hr;
		if(SUCCEEDED(hr = txr.txr->LockRect(0,&lr,&glyphrect,0)))
		{
			//CLEARS SLOT
			BYTE *b = (BYTE *)lr.pBits;
			INT pitch = lr.Pitch;
			for(UINT sy = 0; sy< mnSlotHeight; sy++) 
			{
				DWORD *p = (DWORD *)b;
				for(UINT sx = 0; sx< mnSlotWidth; sx++) 
				{
					*p = 0;
					p++;
				}
				b += pitch;
			}
			if(data)
			{
				switch(rg.bpp) 
				{
				case 1: {
					//RASTERIZE GLYPH
					b = (BYTE *)lr.pBits;
					for(int by = 0; by < h; by++) 
					{
						DWORD *p = (DWORD *)b;
						for(int bx = 0; bx < w; bx++)
						{
							int idx = by*bmppitch + bx/8;
							char byte = data[idx];
							int bit = bx%8;
							unsigned int mask = 0x80 >> bit;
							*p = D3DCOLOR_ARGB((byte & mask)?0xFF:0x00,0xFF,0xFF,0xFF);


							p++;
						}
						b += pitch;
					}
						}
						break;
				case 4:
				case 8: {
					FLOAT multiplyer = 1.0f/63;
					if(rg.bpp == 4) multiplyer = 1.0f/15;
					b = (BYTE *)lr.pBits;
					for(int by = 0; by < h; by++) {
						DWORD *p = (DWORD *)b;
						for(int bx = 0; bx < w; bx++)
						{
							int idx = by*bmppitch + bx;
							char byte = data[idx];

							*p = D3DCOLOR_ARGB(std::min<uint>(uint(255),uint((byte*(1.0f/63))*255)),0xFF,0xFF,0xFF);

							p++;
						}
						b += pitch;
					}
						}
						break;
				default:
					_ASSERT(0);
				}	
			}
			
			txr.txr->UnlockRect(0);
		}
		
 		rc.left = glyphrect.left;
 		rc.right = glyphrect.left + rg.advance ;
// 		rc.bottom = glyphrect.bottom;
// 		rc.top = glyphrect.top;
		
		////////////////////////////////////////////////////////////////////////
		//END NEW GLYPH
		////////////////////////////////////////////////////////////////////////
	}


	delete rg.data;
}
//////////////////////////////////////////////////////////////////////////

void fnDrawLineText(uint nAlignMode,sint offx,sint offy,const LineRecord* plr,const WinTextDrawInfo& di,const RECT& rcScreen)
{
	ID3DXSprite* pID3DXSprite = CDXUTDialog::GetManager()->m_pSprite;
	D3D9RenderSystem& render = g_RenderSystem;	
	const LPDIRECT3DDEVICE9 pDevice = render.GetD3DDevice();

	CRenderer *pRender = CDXUTDialog::GetRenderer() ; 

//	RECT screen; 
	sint x = rcScreen.left + offx;
	for (int i = plr->iBegin ; i < plr->iEnd ; ++i)
	{
		WinFontSystem::TextureCode* codeptr = plr->codes[i-plr->iBegin];
		if(codeptr == 0)
			continue;
		WinTexture& txr = codeptr->txr;			
		if(txr.nHeight <= 0 || txr.nWidth <= 0)
			continue;
		D3DXVECTOR3 vPos( (float)x + txr.offsetx  , (float)rcScreen.top + txr.offsety + offy, 1.0f );					
		RECT srcRect;			
		SetRect(&srcRect,0,0,txr.nWidth , txr.nHeight );
		OffsetRect(&srcRect , x , (rcScreen.top + offy));
		if(IntersectRect(&srcRect,&srcRect,&rcScreen) == FALSE)
		{
			x += (codeptr->rect.right - codeptr->rect.left);
			continue;
		}
		
		OffsetRect(&srcRect , -x , -(rcScreen.top + offy));
		OffsetRect(&srcRect ,codeptr->rect.left ,codeptr->rect.top ) ; 
		if(nAlignMode & DT_BOTTOM)
		{						
			OffsetRect(&srcRect ,0,txr.nHeight - RectHeight(srcRect) ) ;

		}

		if( (srcRect.top - codeptr->rect.top) >= codeptr->txr.nHeight )
		{
			x += (codeptr->rect.right - codeptr->rect.left);
			continue;
		}

// 		if(IntersectRect(&srcRect,&srcRect,&codeptr->rect) == FALSE)
// 			continue;
		
		/*
			D3DXVECTOR3 vPos( (float)x + txr.offsetx  , (float)rcScreen.top + txr.offsety + offy , 1.0f );			
					
					RECT srcRect;			
					SetRect(&srcRect,0,-offy,RectWidth(codeptr->rect) , RectHeight(codeptr->rect));
					
					OffsetRect(&srcRect ,x,(rcScreen.top ));
			
					IntersectRect(&srcRect,&srcRect,&rcScreen);
			
					OffsetRect(&srcRect ,-x,-(rcScreen.top ));
			
			 		OffsetRect(&srcRect ,codeptr->rect.left ,codeptr->rect.top ) ;
			 		srcRect.top -= offy;
					if(srcRect.top >= srcRect.bottom)
						continue;
						*/
			

		if(1)
		{
			if(pRender != NULL)
			{
				if(di.nStyle & WinTextDrawInfo::STYLE_SHADOW)
				{
					D3DXVECTOR3 vPos2 = vPos;			
					vPos2.x += 1;
					vPos2.y += 1;			
					pID3DXSprite->Draw(txr.txr,&srcRect,0,&vPos2,0x80000000);
				}

				if(di.nStyle & WinTextDrawInfo::STYLE_OUTLINE) 
				{
					uint nColor = (~di.nColor) | 0xff000000 ;
					D3DXVECTOR3 vPos2 = vPos;			
					vPos2.x += -1;
					vPos2.y += 0;				

					pID3DXSprite->Draw(txr.txr,&srcRect,0,&vPos2,nColor);
					vPos2 = vPos;			
					vPos2.x += 1;
					vPos2.y += 0;

					pID3DXSprite->Draw(txr.txr,&srcRect,0,&vPos2,nColor);
					vPos2 = vPos;			
					vPos2.x += 0;
					vPos2.y += -1;

					pID3DXSprite->Draw(txr.txr,&srcRect,0,&vPos2,nColor);
					vPos2 = vPos;			
					vPos2.x += 0;
					vPos2.y += 1;
					pID3DXSprite->Draw(txr.txr,&srcRect,0,&vPos2,nColor);
				}
			}




#ifndef _DEBUG		
			pID3DXSprite->Draw(txr.txr,&srcRect,0,&vPos,di.nColor);
#else


			//SetRect(&screen , vPos.x , vPos.y ,  vPos.x + (srcRect.right - srcRect.left) , vPos.y + ( srcRect.bottom - srcRect.top)) ; 

			//if(pRender != NULL)
			//{
			//	pRender->CreateUiOp(txr.txr , screen , &srcRect , txr.info.Width , txr.info.Height , di.nColor) ; 
			//}

			HRESULT hr = pID3DXSprite->Draw(txr.txr,&srcRect,0,&vPos,di.nColor);
			if(hr != S_OK)
			{
				//assert(0);

			}

#endif
		}
		else
		{
			assert(0);
		}
		

		x += (codeptr->rect.right - codeptr->rect.left);
		
	}

// 	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, vals[0] );
// 	pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, vals[0] );
// 	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, vals[0] );


}
//////////////////////////////////////////////////////////////////////////
void WinFontSystem::_Draw(const Rect32& rcDraw,const char16* text,CodeQueryer& codes,const WinTextDrawInfo& di) 
{
	D3DXMATRIX mat ; 

	D3DXMatrixIdentity(&mat) ; 

	D3DXMatrixScaling( &mat, 1.0f, 1.0f , 1.0f );
	
	ID3DXSprite* pID3DXSprite = CDXUTDialog::GetManager()->m_pSprite;
	pID3DXSprite->SetTransform(&mat);	
//	pID3DXSprite->Flush();

	sint x = rcDraw.left;
	sint y = rcDraw.top;

	sint nMaxHeight = 0;


	LineRecord* plr = glrf.Create();
	plr->iBegin = 0;
	plr->iEnd = 0;
	plr->codecount = 0;
	plr->width = 0;
	plr->height = 0;
	

	for (sint i = 0 ; ; ++i )
	{
		const char16& c = text[i];
		if(c != 0)					
		{
			
			if(c != L'\n' && plr->codecount < 1024)
			{
				uint icode = plr->codecount;
				plr->codes[icode] = &codes.Query(c);
				plr->width += plr->codes[icode]->rect.right - plr->codes[icode]->rect.left;
				if(plr->height < (uint)RectHeight(plr->codes[icode]->rect))
					plr->height = (uint)RectHeight(plr->codes[icode]->rect);
				++plr->codecount;
			}
			else
			{
				
				plr->iEnd = i ;	
				nMaxHeight += plr->height;
				lines.push_back(plr);
				

				//CreateObj(&plr);
				//plr = glrf.Create();
				plr = new LineRecord;
				plr->iBegin = i;
				plr->iEnd = i;
				plr->codecount = 0;
				plr->width = 0;
				plr->height = 0;
			}

			continue;
		}
		plr->iEnd = i ;	
		nMaxHeight += plr->height;
		lines.push_back(plr);
		break;	
	}

	
	SIZE szFont = _FontSize(codes.GetFont());
	sint linex = 0;
	sint liney = 0;	
	if(di.nFormat & DT_TOP)
	{
		liney = 0;
	}
	else if(di.nFormat & DT_VCENTER)
	{
		liney = (rcDraw.bottom - rcDraw.top) - nMaxHeight ;
		liney/= 2;
	}
	else if(di.nFormat & DT_BOTTOM)
	{
		liney = (rcDraw.bottom - rcDraw.top) - nMaxHeight ;
	}
	
	_FOREACH(LineRecordSet ,lines,it)
	{
		LineRecord* plr = *it;		
		sint nlimitwidth = (rcDraw.right - rcDraw.left);		
		if(di.nFormat & DT_LEFT)
		{
			linex = 0;
		}
		else if(di.nFormat & DT_RIGHT)
		{			
			linex = nlimitwidth - plr->width;
		}
		else if(di.nFormat & DT_CENTER)
		{
			linex =  nlimitwidth - plr->width;
			linex /= 2;
		}

		
		
		fnDrawLineText( di.nFormat ,linex , liney , plr , di , rcDraw);
		delete *it;
		//glrf.Destroy(*it);
		liney += szFont.cy;
		if(rcDraw.top + liney > rcDraw.bottom)
		{
			break;
		}
		
	}			
	lines.clear();
}
//////////////////////////////////////////////////////////////////////////
void WinFontSystem::RegisterFonts()
{
	fnRegisterFontFromPackage("font\\VTIMESB.ttf"); // VNI-Times-Bold(TrueType)
	fnRegisterFontFromPackage("font\\blei00d.ttf"); // 文鼎中粗隸
	fnRegisterFontFromPackage("font\\DFPPuDingW7-B5.ttf"); // 華康布丁體W7(P)
	fnRegisterFontFromPackage("font\\dfttww5.TTC"); // 華康娃娃體
	fnRegisterFontFromPackage("font\\pop1w5.TTC"); // 華康POP1體W5

}
