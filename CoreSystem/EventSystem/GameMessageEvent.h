#ifndef _GameMessageEvent_20090612am1158_
#define _GameMessageEvent_20090612am1158_
#include "boost/shared_ptr.hpp"
#include "GameDatabase.h"
/*
簡單的遊戲事件驅動幫浦
將事件指派到指定的目標
*/

#define DECLARE_GAMEEVENT_MAP \
	static EventEntry	vEvent[]; \
	static EventMap			eventMap; \
	virtual EventMap* GetEventMap() const;

#define BEGIN_GAMEEVENT_MAP(theClass, baseClass) \
	theClass::EventMap* theClass::GetEventMap() const \
	{ return &theClass::eventMap; } \
	theClass::EventMap theClass::eventMap = \
	{ &(baseClass::eventMap), \
	(theClass::EventEntry*) &(theClass::vEvent) }; \
	theClass::EventEntry theClass::vEvent[] = \
{
#define _HOOK_GAMEEVENT(gmid,call_back) \
	{gmid, static_cast<GameEvent::Target::PFN_EVENT>(call_back)} , 
#define END_GAMEEVENT_MAP \
{ 0, (GameEvent::Target::PFN_EVENT)0 } \
};


namespace GameEvent
{
	struct Result
	{
		enum{ Unfinished , Finish };
		Result(uint nResult):mres(nResult) {}
		Result() :mres(Unfinished) {}
		Result(const Result& nResult) : mres(nResult.mres){}

		uint mres;

		inline bool operator ==(const Result& nResult) const
		{
			return mres == nResult.mres;
		}
	};
	//typedef   Result;		
	enum
	{
		Lowest_Priority = 0
	};
	typedef int	EID;

	struct ParamObj
	{
		virtual ~ParamObj()	{}
	};
	typedef ParamObj* ParamObjPtr;
	typedef boost::shared_ptr<ParamObj>	EPARAM;

	typedef int Priority;

	//////////////////////////////////////////////////////////////////////////
	struct Target
	{
		//////////////////////////////////////////////////////////////////////////
		typedef Result (Target::*PFN_EVENT)(EID	nId,EPARAM param);	
		struct EventEntry
		{							
			EID	nId;
			PFN_EVENT pfn;
		};
		//////////////////////////////////////////////////////////////////////////
		struct EventMap
		{
			EventMap*	pBase;
			EventEntry*	pEntries;
		};
		//////////////////////////////////////////////////////////////////////////
		//
		Target		();
		virtual			~Target		();
		Result			OnGameEvent	(EID nId,EPARAM param);
		DECLARE_GAMEEVENT_MAP;

		static Target* Query(TdgHandle key)
		{
			return SimpleHandleObject<Target>::Query(key);			
		}

		inline TdgHandle	GetHandle() const {return mSimpleHandleObject.GetHandle();}
	private:
		SimpleHandleObject<Target>	mSimpleHandleObject;
	};
	typedef Target* TargetPtr;
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	struct Dispatcher
	{		
		Dispatcher	();
		~Dispatcher	();

		void	Send		(EID nId,EPARAM param);
		void	Post		(EID nId,EPARAM param, Priority priority = Lowest_Priority);
		void	Register	(TargetPtr tp,Priority priority = Lowest_Priority);
		void	Unregister	(TargetPtr tp);
		void	Process		();
		
	private:
		//////////////////////////////////////////////////////////////////////////
		struct TempData
		{
			EID		nId;
			EPARAM	param;			
		};
		typedef std::multimap<Priority,TempData,std::greater<Priority>> EvenrSet;		
		EvenrSet*		mpEvents;			
		EvenrSet		mEvents1;
		EvenrSet		mEvents2;


		//////////////////////////////////////////////////////////////////////////
		typedef std::multimap<Priority,TdgHandle,std::greater<Priority>> TargetSet;
		TargetSet			mTargets;

		struct LoginAccount
		{
			Priority	priority;
			TdgHandle	hAccount;
		};
		struct LogoutAccount
		{			
			TdgHandle	hAccount;
		};
		typedef std::list<LoginAccount>		LoginAccountSet;
		typedef std::list<LogoutAccount>	LogoutAccountSet;
		LoginAccountSet		mLoginAccounts;
		LogoutAccountSet	mLogoutAccounts;

		void	_PendingAccount	();
		//////////////////////////////////////////////////////////////////////////
		Result	_Dispatch		(EID id,EPARAM param);			
		
		void	_Register		(TargetSet*	pTargets,TargetPtr tp,Priority priority = Lowest_Priority);
		void	_Unregister		(TargetSet*	pTargets,TargetPtr tp);
		
	};
};

namespace FDO
{
	typedef GameEvent::Target		GETarget;
	typedef GameEvent::TargetPtr	GETargetPtr;
	typedef GameEvent::Result		GEResult;
	typedef GameEvent::EPARAM		GEParam;
	typedef GameEvent::EID			GEID;
	typedef GameEvent::ParamObj		GEParamObj;
	typedef std::list<DialogHandle> HandleSet;
};

namespace FDO
{

	struct TDGGameEventManager : public Singleton<TDGGameEventManager> , public GameEvent::Dispatcher
	{

	};
	//////////////////////////////////////////////////////////////////////////
	// api
	void	fnInitialTDGGameEvent	();
	void	fnFinalTDGGameEvent		();	
	void	fnRegisterTDGGameEvent	(GETargetPtr tp,GameEvent::Priority priority = GameEvent::Lowest_Priority);
	void	fnUnregisterTDGGameEvent(GETargetPtr  tp);

	template<typename TParam>
	void	fnSendTDGGameEvent		(GameEvent::EID nId,TParam param)
	{
		assert(TDGGameEventManager::GetSingletonPtr());		
		TDGGameEventManager::GetSingletonPtr()->Send(nId,param.GetParam());
	}

	template<typename TParam>
	void	fnPostTDGGameEvent		(GameEvent::EID nId,TParam param, GameEvent::Priority priority = GameEvent::Lowest_Priority)
	{
		assert(TDGGameEventManager::GetSingletonPtr());
		TDGGameEventManager::GetSingletonPtr()->Post(nId,param.GetParam(),priority);
	}

	void	fnProcessTDGGameEvent	();
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

}

//////////////////////////////////////////////////////////////////////////
// 初始化

#define TDG_GAMEEVENTSYSTEM_CREATE		FDO::fnInitialTDGGameEvent
#define TDG_GAMEEVENTSYSTEM_DESTORY		FDO::fnFinalTDGGameEvent
//////////////////////////////////////////////////////////////////////////
// 監聽元件註冊

#define TDG_GAMEEVENTSYSTEM_REG			FDO::fnRegisterTDGGameEvent
#define TDG_GAMEEVENTSYSTEM_UNREG		FDO::fnUnregisterTDGGameEvent
//////////////////////////////////////////////////////////////////////////
// 事件傳送

#define TDG_GAMEEVENTSYSTEM_SEND		FDO::fnSendTDGGameEvent
#define TDG_GAMEEVENTSYSTEM_POST		FDO::fnPostTDGGameEvent
//////////////////////////////////////////////////////////////////////////
#define TDG_GAMEEVENTSYSTEM_PROCESS		FDO::fnProcessTDGGameEvent
#define TDG_GAMEEVENTSYSTEM_GETPARAM(var_name,param_name,param_type) param_type::ObjPtr var_name = static_cast<param_type::ObjPtr>(param_name.get())
#endif


