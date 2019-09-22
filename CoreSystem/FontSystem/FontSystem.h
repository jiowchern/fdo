#ifndef _FontSystem_20090617pm0202_
#define _FontSystem_20090617pm0202_

#include "ElementDef.h"
#include "FS_Font.h"
#include "FS_Texture.h"
namespace FontSystem
{	
	//////////////////////////////////////////////////////////////////////////
	template<typename TFont,typename TTxr,typename TDrawInfo> 
	struct Core : public TextureManager<TTxr>
	{
		typedef Core<TFont,TTxr,TDrawInfo>					ThisType;
		typedef TDrawInfo									DrawInfo;
		typedef TTxr										Texture;
		typedef TFont										Font;
		typedef typename TFont::Style						FontStyle;		
		typedef ResoueceSystem::TResource<Font,ObjFactory>	FontResouece;
		typedef TextureManager<TTxr>						TextureManager;
		typedef typename TextureManager::TextureCode		TextureCode;

		struct CodeQueryer
		{
							CodeQueryer(FontHandle hFont ,ThisType& TxrMgr) 
								: mhFont(hFont), mMgr(TxrMgr)
							{

							}

							~CodeQueryer() {}
			TextureCode&	Query(Code code) 
			{
				return mMgr.Query(mhFont , code);
			}
			
			FontHandle		GetFont()
			{
				return		mhFont;
			}
		private:
			ThisType&		mMgr;
			FontHandle		mhFont;
		};
		//////////////////////////////////////////////////////////////////////////
					Core		();					
		virtual		~Core		();
					
		void		Initial		();
		void		Release		();

		FontHandle	QueryFont	(const FontStyle& data);	

		template<typename TChar>
		void		Draw		(FontHandle hnd,const TChar* text,const Rect32& rcDraw,const DrawInfo& di);


	protected:		
		FontHandle		_CreateFont	(const FontStyle& data);

		// Ä~©Ó¦Û TextureManager
		virtual void	CreateFontTexture	(FontHandle hFont,Texture& txr,Rect32& rc,Code code)
		{
			Font* font = mFontres.Query(hFont);
			if(font)
			{
				_MakeFontTexture(font,txr,rc,code);
			}
		}		
		//////////////////////////////////////////////////////////////////////////
		virtual	SIZE	_FontSize	(FontHandle hnd) 
		{
			SIZE sz;
			const FontStyle& style = mFontres.Execute<const FontStyle&>(hnd,&Font::GetStyle);
			sz.cx = style.size ;
			sz.cy = style.size ;
			return sz;
		}
		//////////////////////////////////////////////////////////////////////////
		virtual Texture	CreateTexture	(uint w,uint h) 
		{
			return _CreateTexture(w,h);
		}
		//////////////////////////////////////////////////////////////////////////
		virtual void	DestoryTexture	(Texture& txr) 
		{
			return _DestoryTexture(txr);
		}

		virtual Texture	_CreateTexture	(uint w,uint h) = 0;
		virtual void	_DestoryTexture	(Texture& txr) = 0;
		virtual void	_MakeFontTexture(Font* font, Texture& txr,Rect32& rc,Code code) = 0;		
		virtual	void	_Draw			(const Rect32& rc,const char16* text,CodeQueryer& codes,const DrawInfo& di) = 0;


		typedef std::list<FontHandle>	FontSet;
		FontResouece		mFontres;
		FontSet				mFonts;
	};
#define FS_TEMPLATE_DIM_CORE(ret_type)		template<typename TFont,typename TTxr,typename TDrawInfo> ret_type Core<TFont,TTxr>

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	

	template<typename TFont,typename TTxr,typename TDrawInfo> 
	Core<TFont,TTxr,TDrawInfo>::Core()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////	
	template<typename TFont,typename TTxr,typename TDrawInfo> 
	Core<TFont,TTxr,TDrawInfo>::~Core()
	{
		
		
	}
	//////////////////////////////////////////////////////////////////////////
	
	template<typename TFont,typename TTxr,typename TDrawInfo> 
	void Core<TFont,TTxr,TDrawInfo>::Initial()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename TFont,typename TTxr,typename TDrawInfo> 
	void Core<TFont,TTxr,TDrawInfo>::Release()
	{
		TextureManager::Release();
		_FOREACH(FontSet , mFonts , it)
		{
			FontHandle hnd = *it;
			mFontres.Execute<void>(hnd,&Font::Release);
			mFontres.Destory(hnd);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename TFont,typename TTxr,typename TDrawInfo> 
	FontHandle Core<TFont,TTxr,TDrawInfo>::QueryFont(const FontStyle& data)
	{		
		_FOREACH(FontSet , mFonts , it)
		{
			FontHandle hnd = *it;
			if(mFontres.Execute<bool>(hnd,&Font::Equal,data) == false)
			{
				continue;
			}
			return hnd;
			break;
		}				
		// create font
		return _CreateFont(data);		
	}
	//////////////////////////////////////////////////////////////////////////
		
	template<typename TFont,typename TTxr,typename TDrawInfo>
		template<typename TChar>
	void Core<TFont,TTxr,TDrawInfo>::Draw(FontHandle hnd,const TChar* text,const Rect32& rcDraw,const DrawInfo& di)
	{ 				
		if(RS_HANDLEVALID(hnd))
		{
			CodeQueryer cq(hnd,*this);
			_Draw(rcDraw , text , cq ,di);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename TFont,typename TTxr,typename TDrawInfo>
	FontHandle Core<TFont,TTxr,TDrawInfo>::_CreateFont(const FontStyle& data)
	{
		FontHandle hnd = mFontres.Create(data);
		mFontres.Execute<void>(hnd,&Font::Initial);
		mFonts.push_back(hnd);
		return hnd;
	}
}
#endif