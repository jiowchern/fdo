#ifndef _GameDatabase_20081210pm0623_
#define _GameDatabase_20081210pm0623_
#include "Common.h"
#include "singleton.h"
/************************************************************************/
/* 
	
*/
/************************************************************************/
#define DECLARE_GD_VALUE_MAP \
	static ValueEntry value_entrys[]; \
	static ValueMap			value_map; \
	virtual const ValueMap* GetValueMap() const ;	

#define BEGIN_GD_VALUE_MAP(theClass, baseClass) \
	const theClass::ValueMap* theClass::GetValueMap() const \
	{ return &theClass::value_map; } \
	theClass::ValueMap theClass::value_map = \
	{ &(baseClass::value_map), \
	(theClass::ValueEntry*) &(theClass::value_entrys) }; \
	theClass::ValueEntry theClass::value_entrys[] = \
{

#define END_GD_VALUE_MAP \
{ 0,0,0,0,0 } \
};

#define DECLARE_GD_EVENTLISTENER(this_class,listener_name) \
struct listener_name : IGameData::IEventListener\
	{ \
		virtual void OnGameDataEvent	(uint nEventId,const this_class& dat,uint param) = 0;\
	}
#define GD_HOLD_SET_COMMAND(command , call_back , param_type) { IGameData::SET ,command,static_cast<IGameData::PFN_VALUE_ASSIGN>(call_back),0,sizeof(param_type) },
#define GD_HOLD_GET_COMMAND(command , call_back , param_type) { IGameData::GET ,command,0,static_cast<IGameData::PFN_VALUE_GET>(call_back),sizeof(param_type) },
namespace FDO
{
	

	struct IGameData
	{
	protected:
		IGameData*	Get() {	return this;}
	public:
		enum
		{
			INVALUD,GET,SET
		};
		typedef bool (IGameData::*PFN_VALUE_ASSIGN)(void* pTarget,bool bSet,uint nObjSize,uint nObjAmount );		
		typedef bool (IGameData::*PFN_VALUE_GET)(void* pTarget,bool bSet,uint nObjSize,uint nObjAmount ) const;		
				IGameData	();
		virtual ~IGameData	();
		struct ValueEntry
		{	
			uint	nType;
			uint	nCmd;
			
			PFN_VALUE_ASSIGN	pfnAssignValue;						
			PFN_VALUE_GET		pfnValueGet;
			uint	nParamSize;
		};
		//////////////////////////////////////////////////////////////////////////
		struct ValueMap
		{
			ValueMap*	pBase;
			ValueEntry*	pEntries;
		};
		//////////////////////////////////////////////////////////////////////////		
		DECLARE_GD_VALUE_MAP;

		template<typename TData>
		bool Get(uint nCmd,TData* pData,uint amount = 1) const 
		{
			const ValueMap* pMap = GetValueMap() ;
			for (; pMap != 0 ; pMap = pMap->pBase)
			{
				int idx = 0;
				uint param_size = sizeof(TData);
				while (pMap->pEntries[idx].nType != IGameData::INVALUD)
				{
					
					if(pMap->pEntries[idx].nCmd != nCmd || pMap->pEntries[idx].nType != IGameData::GET || pMap->pEntries[idx].nParamSize != param_size)
					{
						assert( !((pMap->pEntries[idx].nCmd == nCmd && pMap->pEntries[idx].nType == IGameData::GET ) && pMap->pEntries[idx].nParamSize != param_size));
						++idx;
						continue;
					}

					return (this->*pMap->pEntries[idx].pfnValueGet)(pData,false,param_size,amount);
				}			
			}
			return false;
		}

		template<typename TData>
		bool Set(uint nCmd,TData* pData,uint amount = 1) 
		{
			const ValueMap* pMap = GetValueMap() ;
			for (; pMap != 0 ; pMap = pMap->pBase)
			{
				int idx = 0;
				uint param_size = sizeof(TData);
				while (pMap->pEntries[idx].nType != IGameData::INVALUD)
				{
					if(pMap->pEntries[idx].nCmd != nCmd || pMap->pEntries[idx].nType != IGameData::SET || pMap->pEntries[idx].nParamSize != param_size)
					{
						assert( !((pMap->pEntries[idx].nCmd == nCmd && pMap->pEntries[idx].nType == IGameData::SET ) && pMap->pEntries[idx].nParamSize != param_size));
						++idx;
						continue;
					}

					return (this->*pMap->pEntries[idx].pfnAssignValue)(pData,true,param_size,amount);
				}			
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		struct IEventListener	{	};
		virtual void	AddListener		(IEventListener* pObj) = 0;
		virtual void	RemoveListener	(IEventListener* pObj) = 0;
		virtual	void	Start			() {};
		virtual	void	ShutDown		() {};
	};
	typedef IGameData* IGameDataPtr;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	template<typename GameData,typename EventListener,typename DataType = GameData>
	class TGameData : public GameData
	{
	public:				
		
	private:
		
	protected:
		typedef std::list<EventListener*> EventListenerSet;
		EventListenerSet	mEventListeners;		

		void	_SendToListener	(uint nEvent,uint param)
		{
			EventListenerSet::iterator	it = mEventListeners.begin();
			for (;it != mEventListeners.end();++it )
			{
				(*it)->OnGameDataEvent(nEvent,*((DataType*)IGameData::Get()),param);
			}
		}
	public:
		//////////////////////////////////////////////////////////////////////////
		TGameData	()  {	}
		//////////////////////////////////////////////////////////////////////////
		virtual ~TGameData	()	{}
		//////////////////////////////////////////////////////////////////////////
		const DataType&	Get	()	const
		{	
			return *((DataType*)IGameData::Get()); 
		};		
		//////////////////////////////////////////////////////////////////////////
		void	AddListener		(EventListener* pObj)
		{
			mEventListeners.push_back(pObj);
		}
		//////////////////////////////////////////////////////////////////////////
		void	RemoveListener	(EventListener* pObj)
		{
			mEventListeners.remove(pObj);
		}
		//////////////////////////////////////////////////////////////////////////		
		virtual void AddListener(IGameData::IEventListener* pObj) 
		{
			AddListener((EventListener*)pObj);
		}
		//////////////////////////////////////////////////////////////////////////
		virtual void RemoveListener	(IGameData::IEventListener* pObj) 
		{
			RemoveListener((EventListener*)pObj);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////	

	typedef	std::map<string , IGameDataPtr> GameDataMap;
	class GameDataManager : public GameDataMap , public Singleton<GameDataManager>
	{
	public:
		bool			Register	(const char8* name,IGameDataPtr pgd);
		void			Unregister	(const char8* name);
		IGameDataPtr	Query		(const char8* name);
	};
	

	IGameDataPtr	fnQueryGameData(const char8* name);
};
#define GD_REGISTER(name , pgd) { 	if(FDO::GameDataManager::GetSingletonPtr()) FDO::GameDataManager::GetSingletonPtr()->Register(name,pgd); }
#define GD_UNREGISTER(name)  {if(FDO::GameDataManager::GetSingletonPtr()) FDO::GameDataManager::GetSingletonPtr()->Unregister(name);}
#define GD_QUERY(name) FDO::fnQueryGameData(name)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "boost\shared_ptr.hpp"
#include <utility>
#include <map>

#define RS_HANDLEVALID(hnd)	ResoueceSystem::fnValid(hnd)

namespace ResoueceSystem
{
	typedef uint64 uint64;
	typedef uint uint;
	struct IResourceManager;
	typedef IResourceManager* IResourceManagerPtr;
	//////////////////////////////////////////////////////////////////////////
	struct HANDLEIMPL
	{
		HANDLEIMPL(void* pObj,IResourceManagerPtr pOwner);
		HANDLEIMPL();
		~HANDLEIMPL();		
		inline bool operator==(HANDLEIMPL const &b) const
		{
			return mnKey == b.mnKey;
		}
		inline bool operator<(HANDLEIMPL const &b) const
		{
			return mnKey < b.mnKey;
		}
		inline bool operator>(HANDLEIMPL const &b) const
		{
			return mnKey > b.mnKey;
		}
		inline bool operator!=( HANDLEIMPL const &b) const
		{
			return mnKey != b.mnKey;
		}		
		

		inline uint64 GetKey() const
		{
			return mnKey;
		}

		static bool fnValid(HANDLEIMPL* pHnd);		
	private:
		
		uint64				mnKey;
		
	};

	typedef boost::shared_ptr<HANDLEIMPL> HANDLE;

	inline bool operator == (HANDLE const & a, HANDLE const & b) 
	{
		if(a.get() && b.get())
			return *a.get() == *b.get();
		return false;
	}

	inline bool operator < (HANDLE const & a, HANDLE const & b) 
	{
		if(a.get() && b.get())
			return *a.get() < *b.get();
		return false;
	}

	inline bool operator > (HANDLE const & a, HANDLE const & b) 
	{
		if(a.get() && b.get())
			return *a.get() > *b.get();
		return false;
	}

	inline bool operator != (HANDLE const & a, HANDLE const & b) 
	{
		if(a.get() && b.get())
			return *a.get() != *b.get();
		return true;
	}

	bool fnValid	(HANDLE hnd);


	struct IResourceManager
	{
		virtual void*	Find	(HANDLE hnd) = 0;
	};
	template<typename TOBJ,template <class,uint> class TFactory>
	class TResource : public IResourceManager //: protected TFactory<TOBJ,100> 
	{	
	public:		
	private:	
		typedef TResource<TOBJ,TFactory> THIS_CLASS;
		typedef TFactory<TOBJ,100> Factory;
		typedef TOBJ	ResourceData;
		typedef ResourceData*						ResourceDataPtr;
		typedef std::map<HANDLE,ResourceDataPtr>	RDPSet;
		//////////////////////////////////////////////////////////////////////////
		RDPSet	mRess;
		Factory	mFactory;
		//////////////////////////////////////////////////////////////////////////
		static HANDLE	_fnMakeHandle		()
		{		
			HANDLE hnd(new HANDLEIMPL(0,0));
			return hnd;
		}

		HANDLE	_Create(ResourceDataPtr prd,HANDLE hnd)
		{			
			mRess.insert( std::make_pair(hnd,prd) );
			return hnd;
		}
	public:

		//////////////////////////////////////////////////////////////////////////
		TResource()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		~TResource()
		{

		}

		
		//////////////////////////////////////////////////////////////////////////
		template<typename TReturn ,typename TFunction>
		TReturn Execute	(HANDLE hnd,TFunction fnc)
		{
			TOBJ* pObj = Query(hnd);			
			if(pObj)
			{				
				return (pObj->*fnc)();
			}
			throw 0;
		}			
		//////////////////////////////////////////////////////////////////////////
		template<typename TReturn ,typename P1 ,typename TFunction>
		TReturn Execute	(HANDLE hnd,TFunction fnc,P1 p1)
		{
			TOBJ* pObj = Query(hnd);			
			if(pObj)
			{				
				return (pObj->*fnc)(p1);
			}
			throw 0;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename TReturn ,typename P1,typename P2 ,typename TFunction>
		TReturn Execute	(HANDLE hnd,TFunction fnc,P1 p1,P2 p2)
		{
			TOBJ* pObj = Query(hnd);			
			if(pObj)
			{				
				return (pObj->*fnc)(p1,p2);
			}
			throw 0;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename TReturn ,typename P1,typename P2 ,typename P3,typename TFunction>
		TReturn Execute	(HANDLE hnd,TFunction fnc,P1 p1,P2 p2,P3 p3)
		{
			TOBJ* pObj = Query(hnd);			
			if(pObj)
			{				
				return (pObj->*fnc)(p1,p2,p3);
			}
			throw 0;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename TReturn ,typename P1,typename P2 ,typename P3,typename P4,typename TFunction>
		TReturn Execute	(HANDLE hnd,TFunction fnc,P1 p1,P2 p2,P3 p3,P4 p4)
		{
			TOBJ* pObj = Query(hnd);			
			if(pObj)
			{				
				return (pObj->*fnc)(p1,p2,p3,p4);
			}
			throw 0;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename TReturn ,typename P1,typename P2 ,typename P3,typename P4,typename P5,typename TFunction>
		TReturn Execute	(HANDLE hnd,TFunction fnc,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
		{
			TOBJ* pObj = Query(hnd);			
			if(pObj)
			{				
				return (pObj->*fnc)(p1,p2,p3,p4,p5);
			}
			throw 0;
		}

		//////////////////////////////////////////////////////////////////////////
		HANDLE	Create	()
		{
			HANDLE hnd = _fnMakeHandle();
			return _Create(new(mFactory.Create()) ResourceData(hnd),hnd);		
		}			
		//////////////////////////////////////////////////////////////////////////
		template<typename TCP>
		HANDLE	Create	(TCP param)
		{
			HANDLE hnd = _fnMakeHandle();
			return _Create(new(mFactory.Create()) ResourceData(hnd,param),hnd);		
		}	
		//////////////////////////////////////////////////////////////////////////
		template<typename TCP1,typename TCP2>
		HANDLE	Create	(TCP1 param1,TCP2 param2)
		{
			HANDLE hnd = _fnMakeHandle();
			return _Create(new(mFactory.Create()) ResourceData(hnd,param1,param2),hnd);		
		}	
		//////////////////////////////////////////////////////////////////////////		
		template<typename TCP1,typename TCP2,typename TCP3>
		HANDLE	Create	(TCP1 param1,TCP2 param2,TCP3 param3)
		{
			HANDLE hnd = _fnMakeHandle();
			return _Create(new(mFactory.Create()) ResourceData(hnd,param1,param2,param3),hnd);		
		}	
		//////////////////////////////////////////////////////////////////////////
		template<typename TCP1,typename TCP2,typename TCP3,typename TCP4>
		HANDLE	Create	(TCP1 param1,TCP2 param2,TCP3 param3,TCP4 param4)
		{
			HANDLE hnd = _fnMakeHandle();
			return _Create(new(mFactory.Create()) ResourceData(hnd,param1,param2,param3,param4),hnd);		
		}	
		//////////////////////////////////////////////////////////////////////////
		template<typename TCP1,typename TCP2,typename TCP3,typename TCP4,typename TCP5>
		HANDLE	Create	(TCP1 param1,TCP2 param2,TCP3 param3,TCP4 param4,TCP5 param5)
		{
			HANDLE hnd = _fnMakeHandle();
			return _Create(new(mFactory.Create()) ResourceData(hnd,param1,param2,param3,param4,param5),hnd);		
		}	
		// 沒有更多建立參數了 如果需要6種以上請麻煩用一個結構包賦吧
		//////////////////////////////////////////////////////////////////////////
		void	Destory	(HANDLE hnd)
		{
			if(RS_HANDLEVALID(hnd))
			{
				RDPSet::iterator it = mRess.find(hnd);	
				if(it != mRess.end())
				{
					it->second->~TOBJ();
					mFactory.Destroy(it->second);				
					mRess.erase(it);
				}
			}
			
		}
		//////////////////////////////////////////////////////////////////////////
		TOBJ*	Query	(HANDLE hnd)
		{
			if(RS_HANDLEVALID(hnd))
			{
				RDPSet::iterator it = mRess.find(hnd);
				if(it != mRess.end())
				{
					return it->second;
				}
			}
			
			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		virtual void*	Find	(HANDLE hnd) 
		{
			return Query(hnd);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
}


typedef ResoueceSystem::HANDLE TdgHandle;
typedef ResoueceSystem::HANDLE DialogHandle;

template<typename TObj>
struct SimpleHandleObject
{
protected:	
	typedef SimpleHandleObject<typename TObj>	THIS_CLASS;
	typedef typename TObj*						TObjPtr;
	typedef std::map<TdgHandle,TObjPtr>			ObjectSet;


	static std::map<TdgHandle,typename THIS_CLASS::TObjPtr>& GetSource()
	{
		static ObjectSet mObjs;
		return mObjs;
	};


	static bool Register(TdgHandle key,typename THIS_CLASS::TObjPtr pObj)
	{

		ObjectSet& mObjs = GetSource();

		ObjectSet::iterator it = mObjs.find(key);
		if(it == mObjs.end())
		{

			mObjs.insert( make_pair(key,pObj) );
			return true;
		}
		assert(0);
		return false;
	};

	static bool Unregister(TdgHandle key)
	{
		ObjectSet& mObjs = GetSource();
		ObjectSet::iterator it = mObjs.find(key);
		if(it != mObjs.end())
		{
			mObjs.erase(it);
			return true;
		}
		assert(0);
		return false;
	};

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
public:
	SimpleHandleObject	(TObjPtr pThis) :mHnd(new ResoueceSystem::HANDLEIMPL(0,0))
	{					
		Register(mHnd,pThis);
	};
	virtual ~SimpleHandleObject	() 
	{
		Unregister(mHnd);
	};

	static TObjPtr Query(TdgHandle key)
	{

		ObjectSet& mObjs = GetSource();
		ObjectSet::iterator it = mObjs.find(key);
		if(it != mObjs.end())
		{
			return it->second;

		}			
		return 0;
	}

	TdgHandle	GetHandle() const {return mHnd;}
private:
	TdgHandle	mHnd;
};
#endif