#ifndef _win_fontsystem_20090622pm0203_
#define _win_fontsystem_20090622pm0203_
#include "FontSystem.h"

struct WinFont : public FontSystem::Font
{
	WinFont	(FontSystem::FontHandle hnd,const WinFont::Style&);
	virtual			~WinFont();
	virtual void	Initial	() ;
	virtual void	Release	() ;		

	inline uint		GetRenderMode() const { return mrenderMode; } 
	inline uint		GetAscent() const { return mnAscent; } 
	inline HDC		GetHDC() const { return mhDC; } 
private:
	HFONT	mhFont;
	HDC		mhDC;
	uint	mrenderMode;
	uint	mnAscent;
};
//////////////////////////////////////////////////////////////////////////
struct WinTexture 
{
	typedef IDirect3DTexture9*		Texture;
	Texture			txr;
	D3DXIMAGE_INFO	info;
	sint			offsetx;
	sint			offsety;
	sint			nWidth;
	sint			nHeight;
};

//////////////////////////////////////////////////////////////////////////

struct WinTextDrawInfo
{
	WinTextDrawInfo() :nColor(0),nFormat(0),nStyle(0)
	{};
	~WinTextDrawInfo(){};
	uint	nColor;
	uint	nFormat;
	enum
	{
		STYLE_NORMAL	= 0,
		STYLE_SHADOW	= 1,
		STYLE_OUTLINE	= 2
	};
	uint	nStyle;
};

struct WinFontSystem : public FontSystem::Core<WinFont,WinTexture,WinTextDrawInfo>
{
	typedef FontSystem::Code Code;
						WinFontSystem	();
						~WinFontSystem	();
	void				RegisterFonts	();
protected:
	virtual WinTexture	_CreateTexture	(uint w,uint h) ;
	virtual void		_DestoryTexture	(Texture& txr) ;
	virtual void		_MakeFontTexture(WinFont* font, WinTexture& txr,Rect32& rc,Code code) ;
	virtual	void		_Draw			(const Rect32& rc,const char16* text,CodeQueryer& codes,const WinTextDrawInfo& di) ;	
};

typedef FontSystem::FontHandle			FontHandle;
typedef FontSystem::Font::Style			FontStyle;

template<typename TChar>
void	fnRegisterFontFromPackage		(const TChar* path)
{
	memPack* pmem = LoadFromPacket(path);
	if(pmem && pmem->DataSize)
	{
		DWORD nFonts;
		AddFontMemResourceEx(pmem->Data,pmem->DataSize,0,&nFonts);	
	}	
}


#endif