
#include "stdafx.h"
#include "XmlLoader.h"

namespace FDO
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	
	XmlLoader::XmlLoader() : 
		mpfnErr(&XmlLoader::fnXmlOnError),
		mpfnStartElement(&XmlLoader::fnXmlOnStartElement),
		mpfnEndElement(&XmlLoader::fnXmlOnEndElement),
		mpfnAttrib(&XmlLoader::fnXmlOnAttribute),
		mpfnCharacter(&XmlLoader::fnXmlCalcTextExtent),
		mpfnDocument(&XmlLoader::fnXmlOnEndDocument),
		mpCurDegree(0) , nInUnknownDegree(0) ,mpRootDegree(0) ,mpListener(0)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	XmlLoader::~XmlLoader()
	{
		for(size_t i =0 ; i< mDegrees.size() ; ++i)
		{
			delete mDegrees[i];
		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlLoader::AddDegree(IXmlDegree* pDeg)
	{		
		mDegrees.push_back(pDeg);
	}
	//////////////////////////////////////////////////////////////////////////
	IXmlDegree*	XmlLoader::GetDegree(uint idx)
	{
		if(mDegrees.size() > idx)
		{
			return mDegrees[idx];
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlLoader::ReadyParse(IXmlDegree* pRoot)
	{
		_ReadyParse(pRoot);
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlLoader::LoadXML(const LoadInfo& loadInfo,UICreateListener* pListener /*= 0*/)
	{
		
		_LoadXML(loadInfo,pListener);
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlLoader::_LoadXML(const LoadInfo& loadInfo,UICreateListener* pListener /*= 0*/)
	{
		//解析xml檔案...
		HXMLPARSER hParser;
		hParser = FDOXML_CreateParser();
		if (hParser)
		{
			mpListener = pListener;
			FDOXML_SetStartElementCallback(hParser, mpfnStartElement);
			FDOXML_SetEndElementCallback(hParser, mpfnEndElement);
			FDOXML_SetAttributeCallback(hParser, mpfnAttrib);
			FDOXML_SetCharactersCallback(hParser, mpfnCharacter);
			//FDOXML_SetEndDocumentCallback(hParser,mpfnDocument);
			FDOXML_SetErrorCallback(hParser, mpfnErr);

			FDOXML_Parse(hParser, loadInfo.xmlText, this/*loadInfo.pData*/);

			FDOXML_DestroyParser(hParser); 
			nInUnknownDegree = 0;
			
		}
		else
		{
			
		}

	}	
	//////////////////////////////////////////////////////////////////////////
	void XmlLoader::_ReadyParse(IXmlDegree* pRoot)
	{
		mpRootDegree = pRoot;
	}
	//////////////////////////////////////////////////////////////////////////
	void CALLBACK XmlLoader::fnXmlOnError(void* pvUserData, LPCWSTR szError)
	{
		XmlLoader* pReader =(XmlLoader*) pvUserData;
		if(pReader->mpListener)
			pReader->mpListener->Send(UICL::XMLLog,L"<ERROR>%s",szError);

	}
	//////////////////////////////////////////////////////////////////////////
	void CALLBACK XmlLoader::fnXmlOnStartElement(void* pvUserData, LPCWSTR szElement)
	{
		XmlLoader* pReader =(XmlLoader*) pvUserData;
		IXmlDegree *pDegree = 0;
		if(pReader->mpCurDegree)
		{
			pDegree = pReader->mpCurDegree->GetChild(szElement);
		}
		else
		{
			assert(pReader->mpRootDegree && "_ReadyParse");
			pDegree = pReader->mpRootDegree;
			pReader->mpCurDegree = pDegree;
		}
		

		if(pDegree)
		{
			pDegree->OnStart(szElement);
			pReader->mpCurDegree = pDegree;
		}	
		else
		{
			XmlLoader* pReader =(XmlLoader*) pvUserData;
			pReader->nInUnknownDegree++;
			if(pReader->mpListener)
				pReader->mpListener->Send(UICL::XMLLog,L"<Unknown Start>%d,%s",pReader->nInUnknownDegree,szElement);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CALLBACK XmlLoader::fnXmlOnEndElement(void* pvUserData, LPCWSTR szElement)
	{
		XmlLoader* pReader =(XmlLoader*) pvUserData;
		if(pReader->nInUnknownDegree == 0)
		{
			pReader->mpCurDegree->OnEnd(szElement);

			IXmlDegree *pDegree = pReader->mpCurDegree->GetParent();
			if(pDegree)
				pReader->mpCurDegree = pDegree;			
		}
		else
		{
			if(pReader->mpListener)
			pReader->mpListener->Send(UICL::XMLLog,L"<Unknown End>%d,%s",pReader->nInUnknownDegree,szElement);
			pReader->nInUnknownDegree--;
			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CALLBACK XmlLoader::fnXmlOnAttribute(void* pvUserData, LPCWSTR szName, LPCWSTR szValue)
	{		
		XmlLoader* pReader =(XmlLoader*) pvUserData;
		if(pReader->nInUnknownDegree == 0)
			pReader->mpCurDegree->OnAttrib(szName,wcslen(szName),szValue,wcslen(szValue));
		else
		{
			if(pReader->mpListener)
				pReader->mpListener->Send(UICL::XMLLog,L"<Unknown Attribute>%d,%s,%s",pReader->nInUnknownDegree,szName,szValue);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CALLBACK XmlLoader::fnXmlOnEndDocument(void* pvUserData)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void CALLBACK XmlLoader::fnXmlCalcTextExtent(void* pvUserData, LPCWSTR szCharacters)
	{
		XmlLoader* pReader =(XmlLoader*) pvUserData;
		if(pReader->nInUnknownDegree == 0)
			pReader->mpCurDegree->OnText(szCharacters,wcslen(szCharacters));
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	XmlLoopDegree::XmlLoopDegree(IXmlDegree* pParent,const wchar_t* element,IXmlElementEventBase* eleEvent)
		: XmlDegree(pParent,element,eleEvent,true)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	XmlLoopDegree::~XmlLoopDegree()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	IXmlDegree::IXmlDegree()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	IXmlDegree::~IXmlDegree()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	XmlDegree::XmlDegree(IXmlDegree* pParent,const wchar_t* element,IXmlElementEventBase* eleEvent,bool bLoop /*= false*/) 
		: mParent(pParent) ,mpEvent(eleEvent) , mbLoop(bLoop),mnLoopCount(0)
	{
		if(mParent)
		{
			assert(this != mParent);
			mParent->AddChild(element,this);		
		}
		mName = element;
	}
	//////////////////////////////////////////////////////////////////////////
	XmlDegree::~XmlDegree()
	{
		delete mpEvent;
		mpEvent = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlDegree::AddChild(const wchar_t* element,IXmlDegree* pChild)
	{
		
		wstring name ;name = element;
		mChilds[name] = pChild;			 
	}
	//////////////////////////////////////////////////////////////////////////
	// 存取
	IXmlDegree* XmlDegree::GetChild(const wchar_t* element)
	{
		wstring name ;name = element;
		std::map<wstring,IXmlDegree*>::iterator it =  mChilds.find(name);
		if(it != mChilds.end())
			return mChilds[name];

		if(mbLoop && mName == name)
		{
			++mnLoopCount;
			return this;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	IXmlDegree* XmlDegree::GetParent()
	{
		if(mnLoopCount == 0)
			return mParent;
		--mnLoopCount;
		return this;
	}
	//////////////////////////////////////////////////////////////////////////
	// 事件
	void XmlDegree::OnStart(const wchar_t* element)
	{
		if(mpEvent)
			mpEvent->OnStart(element,0);
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlDegree::OnEnd(const wchar_t* element)
	{
		if(mpEvent)
			mpEvent->OnEnd(element,0);
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlDegree::OnText(const wchar_t* text,int nLen)
	{
		if(mpEvent)
			mpEvent->OnText(text,nLen);
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlDegree::OnAttrib(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		if(mpEvent)
			mpEvent->OnAttrib(attr,nAttrLen,value,nValLen);
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	CirclesXmlDegree::CirclesXmlDegree(IXmlDegree* pParent,const wchar_t* element,IXmlElementEventBase* eleEvent )
		: XmlDegree(pParent,element,eleEvent,false)//,mpCurDegree(this)
	{
		mpCurDegree = this;
		mStackDegree.push_back(pParent);
	}
	//////////////////////////////////////////////////////////////////////////
	CirclesXmlDegree::~CirclesXmlDegree()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	// 存取
	IXmlDegree*	CirclesXmlDegree::GetChild(const wchar_t* element) 
	{
		std::wstring name(element);
		std::map<wstring,IXmlDegree*>::iterator it =  mChilds.find(name);
		if(it != mChilds.end())
		{
			mStackDegree.push_back(mpCurDegree);
			mpCurDegree = it->second;
			return this;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	IXmlDegree*	CirclesXmlDegree::GetParent() 
	{
		if(mStackDegree.size())
		{
			mpCurDegree = mStackDegree.back();
			mStackDegree.pop_back();
		}
		else
			mpCurDegree = 0;
		return mpCurDegree;
	}

	//////////////////////////////////////////////////////////////////////////
	// 事件
	void CirclesXmlDegree::OnStart(const wchar_t* element) 
	{
		if(mpCurDegree != this)
			mpCurDegree->OnStart(element);
		else
		{
			if(mpEvent)
			{
				mpEvent->OnStart(element,0);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CirclesXmlDegree::OnEnd(const wchar_t* element) 
	{
		if(mpCurDegree != this)
			mpCurDegree->OnEnd(element);
		else
		{
			if(mpEvent)
			{
				mpEvent->OnEnd(element,0);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CirclesXmlDegree::OnText(const wchar_t* text,int nLen) 
	{
		if(mpCurDegree != this)
			mpCurDegree->OnText(text,nLen);
		else
		{
			if(mpEvent)
			{
				mpEvent->OnText(text,nLen);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CirclesXmlDegree::OnAttrib(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) 
	{
		if(mpCurDegree != this)
			mpCurDegree->OnAttrib(attr,nAttrLen,value,nValLen);
		else
		{
			if(mpEvent)
			{
				mpEvent->OnAttrib(attr,nAttrLen,value,nValLen);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	DirectXmlDegree::DirectXmlDegree(IXmlDegree* pParent,const wchar_t* element,IXmlElementEventBase* eleEvent )
		: XmlDegree(pParent,element,eleEvent,false)//,mpCurDegree(this)
	{
		mpCurDegree = this;
		mStackDegree.push_back(pParent);
	}
	//////////////////////////////////////////////////////////////////////////
	DirectXmlDegree::~DirectXmlDegree()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	// 存取
	IXmlDegree*	DirectXmlDegree::GetChild(const wchar_t* element) 
	{
		mStackDegree.push_back(mpCurDegree);
		mpCurDegree = this;
		return mpCurDegree;
	}
	//////////////////////////////////////////////////////////////////////////
	IXmlDegree*	DirectXmlDegree::GetParent() 
	{
		if(mStackDegree.size())
		{
			mpCurDegree = mStackDegree.back();
			mStackDegree.pop_back();
		}
		else
			mpCurDegree = 0;
		return mpCurDegree;
	}

	//////////////////////////////////////////////////////////////////////////
	// 事件
	void DirectXmlDegree::OnStart(const wchar_t* element) 
	{
		if(mpCurDegree != 0 && mpEvent)
		{
			mpEvent->OnStart(element,0);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DirectXmlDegree::OnEnd(const wchar_t* element) 
	{
		if(mpCurDegree != 0 && mpEvent)
		{
			mpEvent->OnEnd(element,0);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DirectXmlDegree::OnText(const wchar_t* text,int nLen) 
	{
		if(mpCurDegree != 0 && mpEvent)
		{
			mpEvent->OnText(text,nLen);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DirectXmlDegree::OnAttrib(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) 
	{
		if(mpCurDegree != 0 && mpEvent)
		{
			mpEvent->OnAttrib(attr,nAttrLen,value,nValLen);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//TXmlElementEvent<RootXmlDegree> gempty(0,0,0,0,0);
	RootXmlDegree::RootXmlDegree() : XmlDegree(0,L"root",new TXmlElementEvent<RootXmlDegree>(0,(void (RootXmlDegree::*) (void))0,(void (RootXmlDegree::*) (void))0,0,0))
	{
		
	}
	//////////////////////////////////////////////////////////////////////////

	RootXmlDegree::~RootXmlDegree()
	{

	}
	
}