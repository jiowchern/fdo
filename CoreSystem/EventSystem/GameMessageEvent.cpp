#include "stdafx.h"
#include "GameMessageEvent.h"
#include "GameMessageEventBase.h"
#include "Debug.h"
#include "Timer.h"
namespace GameEvent
{
	Target::EventMap Target::eventMap =
	{
		0,
		&Target::vEvent[0]
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Target::EventEntry Target::vEvent[] =
	{
		{ 0, 0 }
	};
	//////////////////////////////////////////////////////////////////////////
	Target::Target()  : mSimpleHandleObject(this)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Target::~Target()
	{
// 		TDGGameEventManager* pMgr = TDGGameEventManager::GetSingletonPtr();
// 		if(pMgr)
// 		{
// 			assert(pMgr->Check(this) == 0 && "不正確的事件監聽元件釋放");
// 			pMgr->Unregister(this);
// 		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	Result Target::OnGameEvent(EID nId,EPARAM param)
	{
		EventMap* pMap = GetEventMap();

		for (; pMap != 0 ; pMap = pMap->pBase)
		{		
			int idx = 0;
			for ( ; pMap->pEntries[idx].pfn ; ++idx)
			{
				EventEntry* pEntries = &pMap->pEntries[idx];
				if(pEntries->nId == nId && pEntries->pfn != 0)
				{					
					
					Result r = (this->*pEntries->pfn)(nId,param);   					
					if(r == Result::Finish)
						return r;
					
				}
			}		
		}
		return Result::Unfinished;
	}
	//////////////////////////////////////////////////////////////////////////
	Target::EventMap* Target::GetEventMap() const 
	{ 
		return &Target::eventMap; 
	} 
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	Dispatcher::Dispatcher()  :  mpEvents(&mEvents1)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Dispatcher::~Dispatcher	()
	{

	}
	
	
	//////////////////////////////////////////////////////////////////////////
	void Dispatcher::Send(EID nId,EPARAM param)
	{		
		_Dispatch(nId , param);
	}
	//////////////////////////////////////////////////////////////////////////
	void Dispatcher::Post(EID nId,EPARAM param, Priority priority /*= Lowest_Priority*/)
	{
		TempData td;
		td.nId = nId;
		td.param = param;		
		mpEvents->insert( make_pair(priority ,td) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Dispatcher::Register(TargetPtr tp,Priority priority /*= Lowest_Priority*/)
	{						
		LoginAccount pa;
		pa.hAccount	= tp->GetHandle();
		pa.priority	= priority;
		mLoginAccounts.push_back(pa);
	}
	//////////////////////////////////////////////////////////////////////////
	void Dispatcher::_Register(TargetSet* pTargets,TargetPtr tp,Priority priority)
	{
		pTargets->insert( make_pair(priority ,tp->GetHandle()));				
	}
	
	//////////////////////////////////////////////////////////////////////////
	void Dispatcher::Unregister(TargetPtr tp)
	{		
		LogoutAccount pa;
		pa.hAccount	= tp->GetHandle();		
		mLogoutAccounts.push_back(pa);
	}
	//////////////////////////////////////////////////////////////////////////	
	void Dispatcher::_Unregister(TargetSet*	pTargets,TargetPtr tp)
	{
		// 找ptr
		bool bFind = false;
		TargetSet::iterator tarIt(pTargets->begin());
		for ( ; tarIt != pTargets->end() ; ++tarIt)
		{
			TargetPtr pTar = GETarget::Query(tarIt->second);
			if(pTar != tp)
				continue;


			bFind = true;

			break;
		}

		if(bFind == true)
		{
			pTargets->erase(tarIt);
		}
		else
		{
			//assert(0);
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void DispatcherProcessDispatcherProcess(const char* caption , float fTime)
	{
		static uint passTime = 0;
		static float fTotalTime = 0.0f;
		static uint64 nCount = 0;

		nCount ++ ;
		fTotalTime += fTime;
		

		if((fTotalTime < 60 * 60 * 24 && nCount) == false)			
		{
			nCount = 1;
			fTotalTime = 0.0f;
		}

		
		passTime += g_Timer.GetTimeDelta();
		
		if(passTime > 5 * 1000)
		{
			std::string out;
			StringFormat8(out,"%s %f\n",caption,fTotalTime / nCount);
			::OutputDebugStringA(out.c_str());

			passTime = 0;
		}
	}
	PerformanceMonitor DispatcherProcessDispatcherProcesspm(DispatcherProcessDispatcherProcess);
	//////////////////////////////////////////////////////////////////////////
	void Dispatcher::Process()
	{

		
		_FOREACH(LoginAccountSet , mLoginAccounts ,it)
		{
			LoginAccount& la =  *it;
			GETarget* pTar = GETarget::Query(la.hAccount);
			if(pTar)
				_Register(&mTargets,pTar,la.priority);
		}
		mLoginAccounts.resize(0);

		_FOREACH(LogoutAccountSet , mLogoutAccounts ,it)
		{
			LogoutAccount& la =  *it;
			GETarget* pTar = GETarget::Query(la.hAccount);
			if(pTar)
				_Unregister(&mTargets,pTar);
		}
		mLogoutAccounts.resize(0);
		
		// 處理
		{

			EvenrSet& cures = *mpEvents;
			mpEvents = (mpEvents == &mEvents1 )? &mEvents2 : &mEvents1;
			//mpEvents->clear();
						
			EvenrSet::iterator it(cures.begin());
			
			for ( ; it != cures.end() ; ++it)
			{

				const TempData td(it->second);
				
				_Dispatch(td.nId,td.param);				
				
			}	
			cures.clear();
			
		}

	}
	
	//////////////////////////////////////////////////////////////////////////
	void DispatcherProcessPFN_OUT(const char* caption , float fTime)
	{
		if(fTime < 0.0001f)
			return ;
		static std::map<string , float > view;


		std::map<string , float >::iterator it = view.find(caption);
		if (it != view.end())
		{
			if(it->second< fTime)
			{
				it->second = fTime;

				std::string out;
				StringFormat8(out,"Dispatcher %s %f\n",caption,fTime);
				::OutputDebugStringA(out.c_str());
			}
		}
		else
		{
			view.insert(make_pair(caption, fTime));
			std::string out;
			StringFormat8(out,"Dispatcher %s %f\n",caption,fTime);
			::OutputDebugStringA(out.c_str());
		}
		
	}
	PerformanceMonitor gpm(DispatcherProcessPFN_OUT);
	Result Dispatcher::_Dispatch(EID id,EPARAM param)
	{

		{
			
			TargetSet& curTar = mTargets;			
			TargetSet::iterator it = curTar.begin();		
			for ( ; it!=curTar.end(); ++it)
			{
				TargetPtr tp = GETarget::Query(it->second);
				if(tp == 0)
				{				
					continue;
				}
				GameEvent::Result res;
				try
				{
					res = tp->OnGameEvent(id,param);			
				}				
				catch (...)
				{
					assert(0);
				}
				
				if(res.mres != GameEvent::Result::Finish)
				{					
					continue;
				}			
				return Result::Finish;	

			}
		}
		
		return Result::Unfinished;
	}
	//////////////////////////////////////////////////////////////////////////	

	void Dispatcher::_PendingAccount()
	{
		
	}
}


namespace FDO
{
	void	fnInitialTDGGameEvent	()
	{

		assert(TEventObjectFactorySet::GetSingletonPtr() == 0);
		new TEventObjectFactorySet;

		assert(TDGGameEventManager::GetSingletonPtr() == 0);
		new TDGGameEventManager;


	}
	//////////////////////////////////////////////////////////////////////////
	void	fnFinalTDGGameEvent		()
	{
		assert(TDGGameEventManager::GetSingletonPtr());
		delete TDGGameEventManager::GetSingletonPtr();

		assert(TEventObjectFactorySet::GetSingletonPtr());
		delete TEventObjectFactorySet::GetSingletonPtr();

	}
	//////////////////////////////////////////////////////////////////////////
	void	fnRegisterTDGGameEvent	(GETargetPtr  tp,GameEvent::Priority priority /*= Lowest_Priority*/)
	{
		assert(TDGGameEventManager::GetSingletonPtr());
		if(TDGGameEventManager::GetSingletonPtr())
			TDGGameEventManager::GetSingletonPtr()->Register(tp,priority);
	}
	//////////////////////////////////////////////////////////////////////////
	void	fnUnregisterTDGGameEvent(GETargetPtr  tp)
	{
		assert(TDGGameEventManager::GetSingletonPtr());
		if(TDGGameEventManager::GetSingletonPtr())
			TDGGameEventManager::GetSingletonPtr()->Unregister(tp);
	}
	//////////////////////////////////////////////////////////////////////////
	// 	void	fnSendTDGGameEvent		(GameEvent::EID nId,GEParam param)
	// 	{
	// 		assert(TDGGameEventManager::GetSingletonPtr());		
	// 		TDGGameEventManager::GetSingletonPtr()->Send(nId,param);
	// 	}
	//////////////////////////////////////////////////////////////////////////
	// 	void	fnPostTDGGameEvent		(GameEvent::EID nId,GEParam param, GameEvent::Priority priority /*= Lowest_Priority*/)
	// 	{
	// 		assert(TDGGameEventManager::GetSingletonPtr());
	// 		TDGGameEventManager::GetSingletonPtr()->Post(nId,param,priority);
	// 	}
	//////////////////////////////////////////////////////////////////////////
	void	fnProcessTDGGameEvent	()
	{
		assert(TDGGameEventManager::GetSingletonPtr());
		TDGGameEventManager::GetSingletonPtr()->Process();
	}
}
