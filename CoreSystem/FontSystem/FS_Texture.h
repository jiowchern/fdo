#ifndef _FS_Texture_20090618pm0223_
#define _FS_Texture_20090618pm0223_
#include "FS_Type.h"
#include "ElementDef.h"
#include <hash_map>
namespace FontSystem
{	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	template<typename TTxr>
	struct TextureBook 
	{
		enum
		{
			COL_COUNT = 20,	
			ROW_COUNT = 20,	
		};
		typedef TTxr Texture;
		struct TextureCode
		{
			TextureCode	(Texture t,Rect32 r) : txr(t) ,rect(r)
			{
				
			}
			~TextureCode()
			{

			}
			Texture	txr;
			Rect32	rect;
		};		
		
		//////////////////////////////////////////////////////////////////////////
		struct Supplier
		{
			virtual Texture	CreateTexture		(uint w,uint h) = 0;
			virtual void	DestoryTexture		(Texture&) = 0;
			
			//////////////////////////////////////////////////////////////////////////
			virtual void	CreateFontTexture	(FontHandle hFont,Texture& txr,Rect32& rc,Code code) = 0;			
		};
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
						TextureBook		(FontHandle h,Supplier* pSupplier,const SIZE sz) 
							: mhFont(h) ,mpSupplier(pSupplier) , mCodeSize(sz)
						{
							mCodeSize.cx *= 2;
							mCodeSize.cy ;
						}
						~TextureBook	()
						{
							
						}
		
		TextureCode&	QueryCode		(Code code)
		{
			TextureCodeCache::iterator it = mCodeCache.find(code);
			if(it != mCodeCache.end())
			{
				return it->second;
			}
			else
			{
				TextureCode hnd = _CreateCode(code);

				mCodeCache.insert(make_pair(code,hnd));
				
				return QueryCode(code);
			}
			assert(0);
			static TextureCode badTexCode = TextureCode(TTxr(),Rect32());
			return badTexCode;
		}
		//////////////////////////////////////////////////////////////////////////
		void	Release ()
		{
			Book::iterator it = mBook.begin();
			for ( ; mBook.end() != it;++it)
			{
				mpSupplier->DestoryTexture(it->txr);								
			}
			mBook.clear();
		}
	private:
		struct Page
		{
			Texture		txr;
			uint		amount; // 已使用數量
		};
		
		TextureCode	_CreateCode		(Code code)
		{
			if(mBook.size() != 0)
			{
				Page& page = mBook.back();
				if(page.amount < COL_COUNT * ROW_COUNT)
				{
					return _AddCode(page,code);
				}
				else
				{
					mBook.push_back(_CreatePage());
					return _AddCode(mBook.back(),code);
				}
			}
			else
			{
				mBook.push_back(_CreatePage());
				return _AddCode(mBook.back(),code);
			}
			
		}
		//////////////////////////////////////////////////////////////////////////
		Page	_CreatePage		()
		{
			Texture txr = _CreateTexture(COL_COUNT * mCodeSize.cx , ROW_COUNT * mCodeSize.cy);
			Page page;
			page.txr = txr;
			page.amount = 0;
			return page;
		}
		//////////////////////////////////////////////////////////////////////////
		Texture	_CreateTexture		(uint w,uint h) 
		{
			return mpSupplier->CreateTexture(w,h);
		}
		//////////////////////////////////////////////////////////////////////////
		void	_CreateFontTexture	(FontHandle hFont,Page& page,Rect32& rc,Code code)
		{
			mpSupplier->CreateFontTexture(hFont,page.txr,rc,code);
		}
		//////////////////////////////////////////////////////////////////////////
		TextureCode _AddCode	(Page& page,Code code)
		{			
			Rect32 rc;
			rc.left = page.amount % COL_COUNT * mCodeSize.cx;
			rc.top	= (page.amount / ROW_COUNT) * mCodeSize.cy;
			rc.right = rc.left + mCodeSize.cx;
			rc.bottom = rc.top + mCodeSize.cy;
			page.amount++;			
			_CreateFontTexture( mhFont , page, rc, code );
			TextureCode tc(page.txr,rc);
			return tc;
		}
		//////////////////////////////////////////////////////////////////////////

		typedef std::list<Page> Book;
		typedef stdext::hash_map<Code,TextureCode>						TextureCodeCache;

		Book				mBook;
		
		TextureCodeCache	mCodeCache;
		SIZE				mCodeSize;
		FontHandle			mhFont;
		Supplier*			mpSupplier;
	};
	//////////////////////////////////////////////////////////////////////////	
	template<typename TTxr>
	struct TextureManager : public TextureBook<TTxr>::Supplier
	{
		typedef TextureBook<TTxr>				Book;
		typedef typename TextureBook<TTxr>::TextureCode			TextureCode;
				TextureManager()
				{

				}
				//////////////////////////////////////////////////////////////////////////
		virtual ~TextureManager()
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		TextureCode&	Query	(FontHandle hnd,Code nCode)
		{
			FontTextureCache::iterator it = mFontTextures.find(hnd);
			if(it != mFontTextures.end())
			{
				Book* pBook = it->second;	
				return pBook->QueryCode(nCode);
			}
			else
			{
				_CreateBook(mFontTextures,hnd);
				return Query(hnd,nCode);
			}
			assert(0);
			static TextureCode badTexCode = TextureCode(TTxr(),Rect32());
			return badTexCode;
		}
		//////////////////////////////////////////////////////////////////////////
		SIZE	GetCodeSize	(FontHandle hnd,Code nCode)
		{
			 TextureCode& code = Query(hnd,nCode);
			 SIZE szCode;
			 szCode.cx = RectWidth(code.rect);
			 szCode.cy = RectHeight(code.rect);
			return szCode;
		}
		//////////////////////////////////////////////////////////////////////////
		void	Release			()
		{
			_FOREACH(FontTextureCache,mFontTextures,it)
			{
				Book* pBok = it->second;
				pBok->Release();
				delete pBok;
			}
			mFontTextures.clear();
		}
	protected:	
		//////////////////////////////////////////////////////////////////////////
		typedef std::map<FontHandle , Book* >		FontTextureCache;

		void	_CreateBook	(FontTextureCache& cache,FontHandle hnd)
		{			
			cache.insert( make_pair(hnd , new Book(hnd,this,_FontSize(hnd))) );
		}

		virtual	SIZE	_FontSize	(FontHandle hnd) = 0;
		//////////////////////////////////////////////////////////////////////////
		
		FontTextureCache		mFontTextures;
		
			
	};
}
#endif