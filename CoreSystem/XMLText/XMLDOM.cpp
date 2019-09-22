#include "StdAfx.h"
#include "XMLDOM.h"

namespace FDO
{
	namespace XMLDOM
	{
	

	
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// 
		ReadHelper::ReadHelper(XDElementHandle& h) : mHnd(h)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		ReadHelper::~ReadHelper()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		ReadChip ReadHelper::GetParent()					const
		{			
			return ReadChip(FDOXML_DOMGetParentNode(mHnd));
		}
		//////////////////////////////////////////////////////////////////////////
		ReadChip ReadHelper::FindFirst(lpcs16 name)		const
		{			
			XDElementHandle hnd = FDOXML_DOMFindFirstChildNode(mHnd);
			while(hnd)
			{
				std::wstring nodeName(FDOXML_DOMGetNodeName(hnd));
				if(nodeName != name)
				{
					hnd = FDOXML_DOMFindNextChildNode(mHnd , hnd);
					continue;
				}
				return ReadChip(hnd);				
			}
			return ReadChip(0);
		}
		
		//////////////////////////////////////////////////////////////////////////
		ReadChip ReadHelper::FindNext(ReadChip rc)			const
		{
			XDElementHandle hnd = FDOXML_DOMFindFirstChildNode(mHnd);
			XDElementHandle hndsrc = rc._GetElement();
			lpcs16 name = FDOXML_DOMGetNodeName(hndsrc);
			bool bFind = false;
			while(hnd)
			{
				std::wstring nodeName(FDOXML_DOMGetNodeName(hnd));
				
				if(nodeName != name)
				{
					
					hnd = FDOXML_DOMFindNextChildNode(mHnd , hnd);
					continue;
				}

				if(bFind)
				{
					return ReadChip(hnd);
				}
				
				if(hnd == hndsrc)
				{
					bFind = true;				
					hnd = FDOXML_DOMFindNextChildNode(mHnd , hnd);
					continue;
				}
				hnd = FDOXML_DOMFindNextChildNode(mHnd , hnd);
			}
			return ReadChip(0);
		}
		//////////////////////////////////////////////////////////////////////////
		uint ReadHelper::GetAttrib_uint(lpcs16 name)		const
		{
			return FDOXML_DOMNodeGetAttrib_uint32(mHnd , name);
		}
		//////////////////////////////////////////////////////////////////////////
		sint ReadHelper::GetAttrib_sint(lpcs16 name)		const
		{
			return FDOXML_DOMNodeGetAttrib_sint32(mHnd , name);
		}
		//////////////////////////////////////////////////////////////////////////
		lpcs16 ReadHelper::GetAttrib_str16(lpcs16 name)		const
		{
			return FDOXML_DOMNodeGetAttrib_str16(mHnd , name);
		}		
		//////////////////////////////////////////////////////////////////////////
		lpcs16 ReadHelper::GetText(lpcs16 name)		const
		{
			return FDOXML_DOMNodeGetText(mHnd);
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		WriteHelper::WriteHelper(XDElementHandle& h,XDDocumentHandle& doc): mHnd(h) , mDoc(doc)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		WriteHelper::~WriteHelper()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		void WriteHelper::RemoveAllChilds()					
		{
			typedef std::list<XDDocumentHandle> ChildSet;
			ChildSet childs;
			XDDocumentHandle hChild = FDOXML_DOMFindFirstChildNode(mHnd);
			while (hChild)
			{
				childs.push_back(hChild);
				hChild = FDOXML_DOMFindNextChildNode(mHnd , hChild);
			}

			_FOREACH(ChildSet, childs , it)
			{
				XDDocumentHandle hnd = *it;
				FDOXML_DOMDestroyNode(mHnd , hnd);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		WriteChip WriteHelper::Parent()					
		{
			return WriteChip(FDOXML_DOMGetParentNode(mHnd) ,mDoc);
		}
		//////////////////////////////////////////////////////////////////////////
		WriteChip WriteHelper::Request(lpcs16 name)	
		{
			return WriteChip(FDOXML_DOMCreateNode(mDoc,mHnd,name),mDoc);
		}
		//////////////////////////////////////////////////////////////////////////
		WriteChip WriteHelper::GetFirst(lpcs16 name)	
		{
			XDElementHandle hnd = FDOXML_DOMFindFirstChildNode(mHnd);
			while(hnd)
			{
				std::wstring nodeName(FDOXML_DOMGetNodeName(hnd));
				if(nodeName != name)
				{
					hnd = FDOXML_DOMFindNextChildNode(mHnd , hnd);
					continue;
				}
				return WriteChip(hnd,mDoc);				
			}
			return WriteChip(0);
		}
		//////////////////////////////////////////////////////////////////////////
		WriteChip WriteHelper::GetNext(WriteChip chip)	
		{
			XDElementHandle hnd = FDOXML_DOMFindFirstChildNode(mHnd);
			XDElementHandle hndsrc = chip._GetElement();
			lpcs16 name = FDOXML_DOMGetNodeName(hndsrc);
			bool bFind = false;
			while(hnd)
			{
				std::wstring nodeName(FDOXML_DOMGetNodeName(hnd));

				if(nodeName != name)
				{

					hnd = FDOXML_DOMFindNextChildNode(mHnd , hnd);
					continue;
				}

				if(bFind)
				{
					return WriteChip(hnd,mDoc);
				}

				if(hnd == hndsrc)
				{
					bFind = true;				
					hnd = FDOXML_DOMFindNextChildNode(mHnd , hnd);
					continue;
				}
				hnd = FDOXML_DOMFindNextChildNode(mHnd , hnd);
			}
			return WriteChip(0);
		}
		//////////////////////////////////////////////////////////////////////////
		void WriteHelper::SetAttrib	(lpcs16 name,uint val	)		
		{
			FDOXML_DOMNodeSetAttrib_uint32(mHnd,name,val	);	
		}
		//////////////////////////////////////////////////////////////////////////
		void WriteHelper::SetAttrib	(lpcs16 name,sint val)		
		{
			FDOXML_DOMNodeSetAttrib_sint32(mHnd,name,val);	
		}
		//////////////////////////////////////////////////////////////////////////
		void WriteHelper::SetAttrib	(lpcs16 name,lpcs16	val)		
		{
			FDOXML_DOMNodeSetAttrib_str16(mHnd,name,val	);	
		}
		//////////////////////////////////////////////////////////////////////////
		void WriteHelper::SetAttrib	(lpcs16 name,lpcs8	val)		
		{
			FDOXML_DOMNodeSetAttrib_str8(mHnd,name,val	);	
		}
		//////////////////////////////////////////////////////////////////////////
		void WriteHelper::SetText(lpcs16 name,lpcs16 val)		
		{
			FDOXML_DOMNodeSetText(mHnd,val	);	
		}
		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		Block::Block() : mHnd(0) 
		{

		}
		//////////////////////////////////////////////////////////////////////////
		Block::Block(XDElementHandle hnd) : mHnd(hnd) 
		{

		}
		//////////////////////////////////////////////////////////////////////////
		Block::~Block()
		{

		}
		
		//////////////////////////////////////////////////////////////////////////
		lpcs16 Block::GetName()	const
		{			
			return FDOXML_DOMGetNodeName(mHnd);
		}
		//////////////////////////////////////////////////////////////////////////
		bool Block::IsValid() const
		{
			return mHnd != 0;	
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ReadBlock::ReadBlock (const ReadBlock& rb): Block(rb._Get()) ,ReadHelper(Block::mHnd)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		ReadBlock::ReadBlock(XDElementHandle h) : Block(h) ,ReadHelper(Block::mHnd)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		ReadBlock::ReadBlock(ReadChip rchip) : Block(rchip._GetElement()) ,ReadHelper(Block::mHnd)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		ReadBlock::~ReadBlock()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		ReadBlock::operator ReadChip() const
		{
			return ReadChip(Block::mHnd);
		}
		//////////////////////////////////////////////////////////////////////////
		void ReadBlock::operator= (ReadChip& rc)
		{
			Block::mHnd = rc.mHnd;
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		WriteBlock::WriteBlock(WriteChip wchip) 
			:mDoc(wchip._GetDocument())
			,Block(wchip._GetElement()) 
			, WriteHelper(Block::mHnd,mDoc)
			, ReadHelper(Block::mHnd)
			 
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		WriteBlock::~WriteBlock	()
		{

		}

		//////////////////////////////////////////////////////////////////////////
		WriteBlock::operator WriteChip() const
		{			
			return WriteChip(Block::mHnd,mDoc);
		}
		//////////////////////////////////////////////////////////////////////////
		void WriteBlock::operator= (WriteChip& rc)
		{
			Block::mHnd = rc.mHnd;			
			mDoc = rc.mDoc;
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		DOMDocument::AccessBlock::AccessBlock(const AccessBlock& ab)
			:/*ReadHelper(Block::mHnd) ,*/WriteBlock(WriteChip(ab._Get(),ab.mDoc)) 
		{

		}
		//////////////////////////////////////////////////////////////////////////
		DOMDocument::AccessBlock::AccessBlock(XDElementHandle h,XDDocumentHandle doc) 
			:/*ReadHelper(Block::mHnd) ,*/WriteBlock(WriteChip(h,doc)) 
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		DOMDocument::AccessBlock::AccessBlock() 
			: WriteBlock(WriteChip(0,0)) /*, ReadHelper(Block::mHnd)*/
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		DOMDocument::AccessBlock::~AccessBlock()
		{
			
		}
		void DOMDocument::AccessBlock::operator=(const AccessBlock& block)
		{
			Block::mHnd = block.Get();
			mDoc = block.mDoc;
		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::AccessBlock::operator=(const ReadChip& chip)
		{
			Block::mHnd = chip._GetElement();
			mDoc = 0;
		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::AccessBlock::operator=(const WriteChip& chip)
		{
			Block::mHnd = chip._GetElement();
			mDoc = chip.mDoc;
		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::AccessBlock::Set(XDDocumentHandle doc)
		{
			mDoc = doc;
			Block::mHnd = FDOXML_DOMGetRootNode(mDoc);
		}
		//////////////////////////////////////////////////////////////////////////
		DOMDocument::AccessBlock DOMDocument::AccessBlock::FindNext(const AccessBlock& bk)
		{									

			return AccessBlock(FDOXML_DOMFindNextChildNode(Block::_Get(),bk._Get()),mDoc);
		}
		//////////////////////////////////////////////////////////////////////////
		DOMDocument::AccessBlock DOMDocument::AccessBlock::FindFirst()
		{
			return AccessBlock(FDOXML_DOMFindFirstChildNode(Block::mHnd),mDoc);
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		DOMDocument::DOMDocument() : mDoc(0)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		DOMDocument::~DOMDocument()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::Initial(lpcs16 path,lpcs16 defaultRoot /*= L"FDO"*/)
		{
			// make root
			mDoc = FDOXML_DOMCreateFromFile(path);			
			if(mDoc == 0)
			{
				mDoc = FDOXML_DOMCreate(defaultRoot);
			}
			
			mRoot._Set(FDOXML_DOMGetRootNode(mDoc));
			mRoot.Set(mDoc);
			
		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::Finalize(lpcs16 path)
		{
			FDOXML_DOMSaveToFile(mDoc,path);
			FDOXML_DOMDestroy(mDoc);			
		}
		//////////////////////////////////////////////////////////////////////////
		bool DOMDocument::UnegisterAccess(IDocumentAccessPtr ptr)
		{
			_FOREACH( AccesserSet , mAccessers , it)
			{
				Accesser& accer = *it;
				if(accer.ptr != ptr)
				{
					continue;
				}

				mAccessers.erase(it);		
				return true;
			}	
			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		bool DOMDocument::RegisterAccess(lpcs16 elementpath,IDocumentAccessPtr ptr)
		{
			mAccessers.push_back(Accesser( ptr,elementpath));
			return true;
// 			AccesserSet::_Pairib ret = mAccessers.insert( make_pair(elementpath , Accesser(ptr)));
// 			if( !ret.second) 
// 			{
// 				assert(0);
// 			}
// 			return ret.second;
			
		}
		//////////////////////////////////////////////////////////////////////////
		IDocumentAccessPtr DOMDocument::_FindAccesser(lpcs16 path) const
		{
// 			AccesserSet::const_iterator it = mAccessers.find(path);
// 			if(it != mAccessers.end())
// 			{
// 				return it->second.ptr;
// 			}
			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::_Load(AccessBlock& obk , lpcs16 parentfullpath,Accesser& accer)
		{
			
			std::wstring currentfullpath;
			StringFormat16(currentfullpath , L"%s/%s" , parentfullpath , obk.GetName());	

			IDocumentAccessPtr ptr = accer.ptr;
			if(ptr)
			{					
				if(accer.path == currentfullpath )
				{
					ptr->Read(ReadChip(obk.Get()),currentfullpath.c_str());
					return;	
				}

				AccessBlock bk(obk.FindFirst()) ;
				while(bk.IsValid())
				{				
					_Load(bk , currentfullpath.c_str(),accer);
					bk = obk.FindNext(bk);
				}
			}		
							
		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::_Save(AccessBlock& obk, lpcs16 parentfullpath)
		{

// 			std::wstring currentfullpath;
// 			StringFormat16(currentfullpath , L"%s/%s" , parentfullpath , obk.GetName());			
// 			IDocumentAccessPtr ptr = _FindAccesser(currentfullpath.c_str());			
// 			if(ptr)
// 			{					
// 				ptr->Write(WriteBlock(WriteChip(obk.Get())),currentfullpath.c_str());
// 			}		
// 
// 			AccessBlock bk(obk.FindFirst()) ;
// 			while(bk.IsValid())
// 			{				
// 				_Save(bk, currentfullpath.c_str());
// 				bk = obk.FindNext(bk);
// 			}
		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::Load()
		{	
			AccesserSet::iterator it(mAccessers.begin());
			for( ; it != mAccessers.end() ; ++it)
			{
				_Load(mRoot,L"" , *it);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::_Save(const std::wstring& prePath , Accesser& access , AccessBlock& bk)
		{			
			std::wstring path;
			StringFormat16(path , L"%s/%s", prePath.c_str() , bk.GetName());

			if(access.path == path)
			{
				WriteBlock wb(WriteChip(bk._Get()));
				wb.Set(mDoc);
				access.ptr->Write(wb,access.path.c_str()) ;	
				return ;
			}

			AccessBlock child = bk.FindFirst();
			while(child.IsValid())
			{							
				_Save(path , access , child);
				child = bk.FindNext(child);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void DOMDocument::Save()
		{	
			_FOREACH(AccesserSet , mAccessers , it)
			{
				Accesser& access = *it;
				std::wstring path(access.path);
				if(access.ptr)
				{					
					AccessBlock bk(mRoot);
					std::wstring path;
					_Save(path , access , bk);			
				}
			}			
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		namespace Test
		{
			struct TestAccess : public IDocumentAccess
			{
				virtual bool	Read	(const ReadBlock& rb,lpcs16 path)
				{
					
					ReadBlock rb1 = rb.FindFirst(L"012");					

					sint hp = rb1.GetAttrib_sint(L"SP");
					sint sp = rb1.GetAttrib_sint(L"HP");
					
					rb1.FindFirst(L"111");
					return true;
				}
				virtual bool	Write	(WriteBlock& wb,lpcs16 path) 
				{
					wb.SetAttrib(L"HP" ,1000);
					return true;
				}
			};


			void fnTest()
			{
				DOMDocument domdoc;
				TestAccess	ta;
				domdoc.Initial(L"xml/aaa.xml");

				domdoc.RegisterAccess(L"dddd/aaaa1" , &ta);
				domdoc.RegisterAccess(L"dddd/aaaa1" , &ta);
				domdoc.RegisterAccess(L"dddd/aaaa2" , &ta);
				domdoc.RegisterAccess(L"dddd/aaaa3" , &ta);
				domdoc.RegisterAccess(L"dddd/aaaa4" , &ta);
				domdoc.RegisterAccess(L"dddd/aaaa5" , &ta);
				domdoc.RegisterAccess(L"dddd/aaaa6/aaaa8" , &ta);

				domdoc.Load();
				
				
				

				domdoc.Save();

				domdoc.Finalize(L"xml/aaa.xml");
			}
		};
		
	}

	

}