#ifndef _FS_Font_20090617pm0510_
#define _FS_Font_20090617pm0510_

#include "FS_Type.h"
namespace FontSystem
{
	
	struct Font 
	{		
		struct Style
		{
			std::wstring	face;
			uint		size;
			uint		weight;
			bool		italic;
			bool		underline;
			bool		strikeout;
		} ;	
		//////////////////////////////////////////////////////////////////////////
						Font	(FontHandle hnd,const Style& style);
		virtual			~Font	();
		virtual void	Initial	() = 0;
		virtual void	Release	() = 0;		
		bool			Equal	(const Style& fs) const;
		const Style&	GetStyle() const { return mstyle;}
	protected:

		static	bool	fnEqual	(const Style& fs1,const Style& fs2);
		Style	mstyle;
	};
	
}


#endif