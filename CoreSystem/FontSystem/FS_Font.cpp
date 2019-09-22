#include "StdAfx.h"
#include "FS_Font.H"

namespace FontSystem
{
	Font::Font(FontHandle hnd,const Font::Style& style) : mstyle(style)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Font::~Font()
	{

	}
	
	//////////////////////////////////////////////////////////////////////////
	bool Font::Equal(const Font::Style& fs) const
	{		
		return Font::fnEqual(mstyle,fs);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Font::fnEqual(const Style& fs1,const Style& fs2)
	{
		return fs1.face == fs2.face  &&
		fs1.italic == fs2.italic&&
		fs1.size == fs2.size&&
		fs1.strikeout == fs2.strikeout&&
		fs1.underline == fs2.underline&&
		fs1.weight == fs2.weight;
	}
}