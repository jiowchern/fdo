#include "StdAfx.h"
#include "KeywordSystem.h"

namespace Keyword
{

	Environment::Environment()
		: GUI::XmlElement(0,0,COUNT)
	{
		_SetPropertySet(NAME,L"NAME",CNV_WCHAR);

		_SetPropertySet(FONT_COLOR,L"F",CNV_ARGB);
		_SetPropertySet(FONT_NAME,L"FONT",CNV_WCHAR);
		_SetPropertySet(FONT_HEIGHT,L"FONT_HEIGHT",CNV_INT);
		_SetPropertySet(FONT_WEIGHT,L"FONT_WEIGHT",CNV_INT);
		_SetPropertySet(FONT_BLOD,L"FONT_BLOD",CNV_INT);
		_SetPropertySet(FONT_ITALIC,L"FONT_ITALIC",CNV_INT);
		_SetPropertySet(ID,L"ID",CNV_INT);
		_SetPropertySet(X,L"X",CNV_INT);
		_SetPropertySet(Y,L"Y",CNV_INT);
		_SetPropertySet(MAPID,L"MAPID",CNV_INT);
		_SetPropertySet(NPCID,L"NPCID",CNV_INT);
		_SetPropertySet(HINTID,L"HINTID",CNV_INT);
		_SetPropertySet(ITEMID,L"ITEMID",CNV_INT);
		_SetPropertySet(MONID,L"MONID",CNV_INT);
		
	}
	//////////////////////////////////////////////////////////////////////////
	Environment::~Environment()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Environment::Clone(Environment& ele)
	{
		for (uint i = 0 ; i < COUNT ; ++i)
		{
			
			if(ele._Get(i))
			{
				GUI::XmlElement::Set(i,ele._Get(i));				
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	EnvironmentManager::EnvironmentManager() : mpEnvCur(0) ,mpEnvRoot(0)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	EnvironmentManager::~EnvironmentManager()
	{
		if(mpEnvRoot)
		{
			Tree::TreeFactory::fnDestoryTree(mpEnvRoot);
		}
		mpEnvRoot = 0;	
	}
	//////////////////////////////////////////////////////////////////////////
	void EnvironmentManager::Start()
	{
		if(mpEnvRoot)
		{
			Tree::TreeFactory::fnDestoryTree(mpEnvRoot);
		}
		mpEnvRoot = 0;	
	}
	//////////////////////////////////////////////////////////////////////////
	void EnvironmentManager::Done()
	{		
		if(mpEnvRoot)
		{
			Tree::TreeFactory::fnDestoryTree(mpEnvRoot);
		}
		mpEnvRoot = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void EnvironmentManager::Begin(const char16* name)
	{		
		if(mpEnvCur == 0)
		{
			mpEnvCur = Tree::TreeFactory::fnCreate<EnvironmentNode>(0);						
			mpEnvRoot = mpEnvCur;
		}
		else
		{
			EnvironmentNode* pNode = Tree::TreeFactory::fnCreate<EnvironmentNode>(mpEnvCur);			
// 			Environment& ele = pNode->GetObjectRef();
// 			ele.Clone(mpEnvCur->GetObjectRef());
			mpEnvCur = pNode;
		}
		mpEnvCur->GetObjectRef().SetAttrib(L"NAME",0,name,0);
	}
	//////////////////////////////////////////////////////////////////////////
	void EnvironmentManager::End()
	{
		if(mpEnvCur)
			mpEnvCur = mpEnvCur->GetParent();
		
	}

	Environment* EnvironmentManager::GetCurrentEnvironment()
	{
		if(mpEnvCur)
			return mpEnvCur->GetObjectPoint();
		return 0;
	}
	EnvironmentNode* EnvironmentManager::GetCurrentEnvironmentNode()
	{
		return mpEnvCur;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	static bool	gbException = false;
	void	EnableException	()
	{
		gbException = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void	DisableException	()
	{
		gbException = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Run(const char16* keyword,IEnvironmentListener* pListener )
	{
		XmlParser parser;
		parser.Initial();
		
		parser.Run(keyword,pListener);
		
		parser.Release();		
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	XmlParser::XmlParser()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	XmlParser::~XmlParser()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void XmlParser::Initial()
	{
		FDO::IXmlElementEventBase* pEve;
		// «Ø¥ß¶¥¼h
		
		pEve = new FDO::TXmlElementEvent<XmlParser>(
			this,
			&XmlParser::XmlStartRoot,
			&XmlParser::XmlEndRoot,
			&XmlParser::XmlAttribRoot,
			&XmlParser::XmlTextRoot	);

		// 0 // ROOT
		mLoader.AddDegree(new FDO::DirectXmlDegree(0,L"ROOT",pEve));
				
		mLoader.ReadyParse(mLoader.GetDegree(0));
		
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlParser::Release()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlParser::XmlStartRoot(const char16* name,int nLen)
	{
		mEnvMgr.Begin(name);
		EnvironmentNode* pEnv = mEnvMgr.GetCurrentEnvironmentNode();
		if(pEnv)
		{			
			mElements.push_back(new Element( L"" , pEnv));
		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlParser::XmlEndRoot(const char16* name,int nLen)
	{
		mEnvMgr.End();
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlParser::XmlAttribRoot(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen)
	{
		Environment* pEnv = mEnvMgr.GetCurrentEnvironment();
		if(pEnv)
		{
			pEnv->SetAttrib(attr,nAttrLen,value,nValLen);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlParser::XmlTextRoot(const wchar_t* text,int nLen)
	{		
		EnvironmentNode* pEnv = mEnvMgr.GetCurrentEnvironmentNode();
		if(pEnv)
		{			
			mElements.push_back(new Element( text , pEnv));
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlParser::Run(const char16* text,IEnvironmentListener* pListener)
	{
		XmlLoader::LoadInfo inf;
		inf.xmlText = text;

		mEnvMgr.Start();

		bool bSuccess = false;	
		ParseException pe;
		try 
		{
			mLoader.LoadXML(inf);
			bSuccess = true;
		}
		catch (const FDOParseException& toCatch) 
		{			
			pe.fdoParseException = toCatch;
			pe.parseText = text;						
		}
		_FOREACH(ElementSet , mElements , it)
		{
			IEnvironmentListener::EventData e(**it);
			pListener->OnEnvironment(e);
			
		}
		_FOREACH(ElementSet , mElements , it)
		{
			delete *it;
		}
		mElements.clear();
		mEnvMgr.Done();
		if(bSuccess == false)
		{
			if(gbException == true)
				throw pe;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	
}