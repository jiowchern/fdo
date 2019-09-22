#ifndef _XMLDOM_20100112pm0823_
#define _XMLDOM_20100112pm0823_

#include "FDOXML.h"
namespace FDO
{
	namespace XMLDOM
	{
		//////////////////////////////////////////////////////////////////////////
		// ©w¸q
		typedef const char16*	lpcs16;
		typedef const char8*	lpcs8;
		
		typedef	HXMLDOMNODE		XDElementHandle; 
		typedef	HXMLDOMDOCUMENT	XDDocumentHandle; 
		struct Block;
		typedef Block*			BlockPtr;
		typedef const Block*	BlockCPtr;
		//////////////////////////////////////////////////////////////////////////
		// «Å§i
		
		struct DOMDocument;
		
		//////////////////////////////////////////////////////////////////////////
		
		template<sint nType,typename TBlock,typename THelper>
		struct Chip
		{
			enum{	TYPE = nType	};			
			friend struct DOMDocument;
			friend typename THelper;
			friend typename TBlock;
			
			Chip() : mHnd(0),mDoc(0) {}
			~Chip() {}
		private:
								Chip	(XDElementHandle h ,XDDocumentHandle doc = 0) : mHnd(h) , mDoc(doc){}
			inline void				_Set			(XDElementHandle h , XDDocumentHandle doc) {mHnd = h;mDoc = doc;}
			inline XDElementHandle	_GetElement		(void) const {return mHnd;}
			inline XDDocumentHandle	_GetDocument	(void) const {return mDoc;}
			XDElementHandle 	mHnd;			
			XDDocumentHandle 	mDoc;			
		};

		struct ReadHelper;
		struct ReadBlock;
		typedef Chip<0,ReadBlock,ReadHelper>	ReadChip;
		struct WriteHelper;
		struct WriteBlock;
		typedef Chip<1,WriteBlock,WriteHelper> WriteChip;
		
		//////////////////////////////////////////////////////////////////////////
		struct ReadHelper
		{
			ReadHelper		(XDElementHandle&);
			~ReadHelper		();

			ReadChip		FindFirst		(lpcs16 name)		const;
			ReadChip		FindNext		(ReadChip)			const;
			ReadChip		GetParent		()					const;
			uint			GetAttrib_uint	(lpcs16 name)		const;
			sint			GetAttrib_sint	(lpcs16 name)		const;
			lpcs16			GetAttrib_str16	(lpcs16 name)		const;			
			lpcs16			GetText			(lpcs16 name)		const;
		private:
			XDElementHandle& 	mHnd;
		};
		

		//////////////////////////////////////////////////////////////////////////
		struct WriteHelper
		{
			WriteHelper	(XDElementHandle&,XDDocumentHandle&);
			~WriteHelper();

			
			WriteChip		Request			(lpcs16 name)	;				
			WriteChip		GetFirst		(lpcs16 name)	;				
			WriteChip		GetNext			(WriteChip chip)	;				
			WriteChip		Parent		()					;
			void			RemoveAllChilds	()					;
			void			SetAttrib		(lpcs16 name,uint	)		;
			void			SetAttrib		(lpcs16 name,sint	)		;
			void			SetAttrib		(lpcs16 name,lpcs16	)		;
			void			SetAttrib		(lpcs16 name,lpcs8	)		;
			void			SetText			(lpcs16 name,lpcs16	)		;
		private:
			XDDocumentHandle&	mDoc;
			XDElementHandle& 	mHnd;
		};
		//////////////////////////////////////////////////////////////////////////	
		struct Block 
		{			
			friend struct DOMDocument;
			// common
							Block		();
							
			virtual			~Block		();
			lpcs16			GetName		()	const;
			bool			IsValid		()  const;
			
		protected:
			inline	void				_Set		(XDElementHandle h) {mHnd = h;}
			inline	XDElementHandle& 	_Get		() { return mHnd ;}
			inline	XDElementHandle 	_Get		() const { return mHnd ;}
							Block		(XDElementHandle );
			XDElementHandle 	mHnd;
			
		};
		
		//////////////////////////////////////////////////////////////////////////		

		
					
		struct ReadBlock : public Block , public ReadHelper
		{
			
							ReadBlock  (const ReadBlock& rb);
							ReadBlock  (XDElementHandle );
							ReadBlock	(ReadChip);
			virtual			~ReadBlock	();		
			operator ReadChip			() const ;
			void			operator=	(ReadChip& rc) ;
		} ;
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		struct WriteBlock : public Block , public WriteHelper, public ReadHelper
		{
							WriteBlock	(WriteChip)	;
			virtual			~WriteBlock	();		
			operator WriteChip			() const ;
			void			operator=	(WriteChip& rc) ;
			inline void			Set(XDDocumentHandle doc ) {mDoc = doc;};
		protected:
			XDDocumentHandle	mDoc;
		};
		

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		namespace FileAccess
		{
			Block		Load	(lpcs16 path);
			void		Save	(lpcs16 path,const Block& wb);
		};

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct IDocumentAccess
		{
			virtual bool	Read	(const ReadBlock& rb, lpcs16 path) = 0;
			virtual bool	Write	(WriteBlock& wb, lpcs16 path) = 0;
		};
		typedef IDocumentAccess* IDocumentAccessPtr;
		//////////////////////////////////////////////////////////////////////////
		struct DOMDocument
		{
					DOMDocument		();
					~DOMDocument	();
			void	Initial			(lpcs16 path,lpcs16 defaultRoot = L"FDO");
			void	Finalize		(lpcs16 path);
			bool	RegisterAccess	(lpcs16 elementpath,IDocumentAccessPtr );
			bool	UnegisterAccess	(IDocumentAccessPtr );
			void	Load			();
			void	Save			();
		private:
			
			struct AccessBlock : public WriteBlock /*, public ReadHelper*/
			{						
											AccessBlock			(const AccessBlock& ab);
											AccessBlock			(XDElementHandle h,XDDocumentHandle doc);
											AccessBlock			();
											~AccessBlock		();

				void						operator=			(const ReadChip& chip);
				void						operator=			(const WriteChip& chip);
				void						operator=			(const AccessBlock& block);

				void						Set					(XDDocumentHandle doc);
				inline	XDElementHandle		Get					() const {return Block::mHnd ;}			
				AccessBlock					FindNext			(const AccessBlock& bk);
				AccessBlock					FindFirst			();
			
			
			};

			

			struct Accesser
			{			
				Accesser(IDocumentAccessPtr p,const std::wstring& pth) : ptr(p),path(pth) {}
				~Accesser(){}
				IDocumentAccessPtr	ptr;
				std::wstring		path;
			};
			IDocumentAccessPtr		_FindAccesser	(lpcs16 path) const;
			void					_Load			(AccessBlock& bk, lpcs16 parentfullpath,Accesser& accer);
			void					_Save			(AccessBlock& bk, lpcs16 parentfullpath);
			void					_Save			(const std::wstring& prePath , Accesser& access , AccessBlock& bk);

			typedef std::list<Accesser> AccesserSet;	
			AccesserSet		mAccessers;

			AccessBlock			mRoot;
			XDDocumentHandle	mDoc;
		};
	};

};
#endif