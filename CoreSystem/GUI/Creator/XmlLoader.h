#ifndef _XmlLoader_20080819pm0245_
#define _XmlLoader_20080819pm0245_
#include "FDOXML.h"
#include "Listener.h"
namespace FDO
{
	struct IXmlElementEventBase
	{
		virtual ~IXmlElementEventBase()
		{

		}
		virtual void	OnStart		(const wchar_t* name,int nLen) = 0;	
		virtual void	OnEnd		(const wchar_t* name,int nLen) = 0;	
		virtual void	OnAttrib	(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) = 0;	
		virtual void	OnText		(const wchar_t* text,int nLen) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	template<typename ClassName>
	class TXmlElementEvent : public IXmlElementEventBase
	{
		typedef void (ClassName::*PFN_XEL_ELEMENT_PARAM) (const wchar_t* name,int nLen);
		typedef void (ClassName::*PFN_XEL_ELEMENT) (void);
		typedef void (ClassName::*PFN_XEL_TEXT) (const wchar_t* text,int nLen);
		typedef void (ClassName::*PFN_XEL_ATTRIB) (const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);
		ClassName*				mpEventListener;
		PFN_XEL_ELEMENT			mpfnStart;
		PFN_XEL_ELEMENT_PARAM	mpfnStartName;
		
		PFN_XEL_ELEMENT			mpfnEnd;
		PFN_XEL_ELEMENT_PARAM	mpfnEndName;
		PFN_XEL_TEXT			mpfnText;
		PFN_XEL_ATTRIB			mpfnAttr;
	public:

		TXmlElementEvent(ClassName* pEL,PFN_XEL_ELEMENT pStart,PFN_XEL_ELEMENT pEnd,PFN_XEL_ATTRIB pAttr,PFN_XEL_TEXT pText) :
		  mpEventListener(pEL),mpfnStart(pStart),mpfnEnd(pEnd),mpfnText(pText),mpfnAttr(pAttr),mpfnStartName(0),mpfnEndName(0)
		  {

		  }
		  //////////////////////////////////////////////////////////////////////////
		  TXmlElementEvent(ClassName* pEL,PFN_XEL_ELEMENT_PARAM pStartName,PFN_XEL_ELEMENT_PARAM pEndName,PFN_XEL_ATTRIB pAttr,PFN_XEL_TEXT pText) :
		  mpEventListener(pEL),mpfnStart(0),mpfnEnd(0),mpfnText(pText),mpfnAttr(pAttr),mpfnStartName(pStartName),mpfnEndName(pEndName)
		  {

		  }
		  //////////////////////////////////////////////////////////////////////////
		  virtual ~TXmlElementEvent()
		  {

		  }
		  //////////////////////////////////////////////////////////////////////////
		  void	SetEvent	(PFN_XEL_ELEMENT pStart,PFN_XEL_ELEMENT pEnd,PFN_XEL_ATTRIB pAttr,PFN_XEL_TEXT pText)
		  {
			  mpfnStart = pStart;
			  mpfnEnd = pEnd;
			  mpfnText = pText;
			  mpfnAttr = pAttr;
		  }
		  //////////////////////////////////////////////////////////////////////////
		  void	OnStart		(const wchar_t* name,int nLen) 
		  {			  
			  if(mpfnStart)
				  (mpEventListener->*mpfnStart)();
			  if(mpfnStartName)
				  (mpEventListener->*mpfnStartName)(name,nLen);
		  }
		  //////////////////////////////////////////////////////////////////////////
		  void	OnEnd		(const wchar_t* name,int nLen) 
		  {			  
			  if(mpfnEnd)
				  (mpEventListener->*mpfnEnd)();
			  if(mpfnEndName)
				  (mpEventListener->*mpfnEndName)(name,nLen);
		  }
		  //////////////////////////////////////////////////////////////////////////
		  void	OnAttrib	(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) 
		  {			  
			  if(mpfnAttr)
				  (mpEventListener->*mpfnAttr)(attr,nAttrLen,value,nValLen);
		  }
		  //////////////////////////////////////////////////////////////////////////
		  void	OnText		(const wchar_t* text,int nLen) 
		  {			  
			  if(mpfnText)
				  (mpEventListener->*mpfnText)(text,nLen);
		  }
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	struct IXmlDegree
	{
								IXmlDegree		();
		virtual					~IXmlDegree		();		
		virtual		void		AddChild		(const wchar_t* element,IXmlDegree* pChild) = 0;
		//////////////////////////////////////////////////////////////////////////
		// 存取
		virtual		IXmlDegree*	GetChild		(const wchar_t* element) = 0;
		virtual		IXmlDegree*	GetParent		() = 0;
		
		//////////////////////////////////////////////////////////////////////////
		// 事件
		virtual		void		OnStart			(const wchar_t* element) = 0;		
		virtual		void		OnEnd			(const wchar_t* element) = 0;		
		virtual		void		OnText			(const wchar_t* text,int nLen) = 0;		
		virtual		void		OnAttrib		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) = 0;		
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class XmlDegree : public IXmlDegree
	{
	public:	

	protected:


		std::map<wstring,IXmlDegree*>	mChilds;		
		IXmlDegree*						mParent;		
		IXmlElementEventBase*			mpEvent;
		bool							mbLoop;
		int								mnLoopCount;
		wstring							mName;

	public:

		//////////////////////////////////////////////////////////////////////////
		// 初始化
		XmlDegree		(IXmlDegree* pParent,const wchar_t* element,IXmlElementEventBase* eleEvent ,bool bLoop = false);
		virtual		~XmlDegree		();		
		void		AddChild		(const wchar_t* element,IXmlDegree* pChild);
		//////////////////////////////////////////////////////////////////////////
		// 存取
		IXmlDegree*			GetChild		(const wchar_t* element);
		IXmlDegree*			GetParent		();
		//const ElementEvent*	GetEvent		() const;
		//////////////////////////////////////////////////////////////////////////
		// 事件
		void		OnStart			(const wchar_t* element);		
		void		OnEnd			(const wchar_t* element);		
		void		OnText			(const wchar_t* text,int nLen);		
		void		OnAttrib		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);		
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 循環式
	struct CirclesXmlDegree : public XmlDegree
	{
								CirclesXmlDegree(IXmlDegree* pParent,const wchar_t* element,IXmlElementEventBase* eleEvent );
		virtual					~CirclesXmlDegree();		
		//////////////////////////////////////////////////////////////////////////
		// 存取
		virtual		IXmlDegree*	GetChild		(const wchar_t* element) ;
		virtual		IXmlDegree*	GetParent		() ;

		//////////////////////////////////////////////////////////////////////////
		// 事件
		virtual		void		OnStart			(const wchar_t* element) ;		
		virtual		void		OnEnd			(const wchar_t* element) ;		
		virtual		void		OnText			(const wchar_t* text,int nLen) ;		
		virtual		void		OnAttrib		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) ;		
	private:
		std::list<IXmlDegree*>	mStackDegree;
		IXmlDegree*				mpCurDegree;
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 直接式 Child = this parent = stack
	struct DirectXmlDegree : public XmlDegree
	{
								DirectXmlDegree	(IXmlDegree* pParent,const wchar_t* element,IXmlElementEventBase* eleEvent );
		virtual					~DirectXmlDegree();		
		//////////////////////////////////////////////////////////////////////////
		// 存取
		virtual		IXmlDegree*	GetChild		(const wchar_t* element) ;
		virtual		IXmlDegree*	GetParent		() ;

		//////////////////////////////////////////////////////////////////////////
		// 事件
		virtual		void		OnStart			(const wchar_t* element) ;		
		virtual		void		OnEnd			(const wchar_t* element) ;		
		virtual		void		OnText			(const wchar_t* text,int nLen) ;		
		virtual		void		OnAttrib		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) ;		
	private:
		std::list<IXmlDegree*>	mStackDegree;
		IXmlDegree*				mpCurDegree;
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class XmlLoopDegree : public XmlDegree
	{
	public:
		XmlLoopDegree(IXmlDegree* pParent,const wchar_t* element,IXmlElementEventBase* eleEvent);
		~XmlLoopDegree();
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class XmlLoader;
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class XmlLoader
	{			
	
		OnErrorFunc							mpfnErr;
		OnStartElementFunc					mpfnStartElement;
		OnEndElementFunc					mpfnEndElement;
		OnAttributeFunc						mpfnAttrib;
		OnCharactersFunc					mpfnCharacter;
		OnEndDocumentFunc					mpfnDocument;
		IXmlDegree*							mpCurDegree;
		IXmlDegree*							mpRootDegree;
		int									nInUnknownDegree;
		//////////////////////////////////////////////////////////////////////////
		// xml callback
		static void CALLBACK fnXmlOnError(void* pvUserData, LPCWSTR szError);
		static void CALLBACK fnXmlOnStartElement(void* pvUserData, LPCWSTR szElement);
		static void CALLBACK fnXmlOnEndElement(void* pvUserData, LPCWSTR szElement);
		static void CALLBACK fnXmlOnAttribute(void* pvUserData, LPCWSTR szName, LPCWSTR szValue);
		static void CALLBACK fnXmlOnEndDocument(void* pvUserData);
		static void CALLBACK fnXmlCalcTextExtent(void* pvUserData, LPCWSTR szCharacters);
		//////////////////////////////////////////////////////////////////////////
	public:
		struct LoadInfo
		{
			const wchar_t*		xmlText;
			//void*				pData;
		};
	protected:
		std::vector<IXmlDegree*>				mDegrees;		
		UICreateListener*					mpListener;
		
		
		void			_LoadXML		(const LoadInfo& loadInfo,UICreateListener* pListener = 0);
		void			_ReadyParse		(IXmlDegree* pRoot);
	public:
						XmlLoader	();
			virtual		~XmlLoader	();
			void		AddDegree	(IXmlDegree* pDeg);
			IXmlDegree*	GetDegree	(uint nId);

			void		ReadyParse	(IXmlDegree* pRoot);
			void		LoadXML		(const LoadInfo& loadInfo,UICreateListener* pListener = 0);
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	class RootXmlDegree : public XmlDegree 
	{
	public:
			RootXmlDegree	(); 
			~RootXmlDegree	();		
	
	};

};


#endif