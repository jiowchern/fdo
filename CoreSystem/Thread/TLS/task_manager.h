#ifndef _20080814pm0157_task_manager_
#define _20080814pm0157_task_manager_
#include "task.h"
#include <map>

namespace NSTask
{
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

	template<class ThreadType>
	class TaskManager
	{
	protected:
		typedef std::map<int,ThreadType*> ThreadSet;

		ThreadSet		mThreads;

		ThreadType*		_Find	(int Id)
		{
			ThreadSet::iterator it = mThreads.find(Id);
			if(it != mThreads.end())
				return it->second;
			return 0;
		}
	public:
		TaskManager		()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		virtual ~TaskManager	()
		{
			ThreadSet::iterator it = mThreads.begin();
			for ( ; it != mThreads.end() ; ++it)
			{				
				ThreadType* pT = it->second;
				pT->Finalize();
				delete pT;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		bool	CreateThread			(int nId,unsigned int nUsingCPUFlag = -1)
		{
			if(_Find(nId) == 0)
			{
				mThreads[nId] = new ThreadType;
				mThreads[nId]->Initial(nUsingCPUFlag);
				return true;
			}
			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		void	DestoryThread			(int nId)
		{
			ThreadSet::iterator it = mThreads.begin();
			for ( ; it != mThreads.end() ; ++it)
			{
				if(it->first != nId)
					continue;

				ThreadType* pT = it->second;
				pT->Finalize();
				delete pT;
				mThreads.erase(it);
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TaskID	AddTask				(int nThreadId,int (*function)(void* param),void* param)
		{
			ThreadType* pT;
			if(pT = _Find(nThreadId))
			{
				Task* pTask = new Task(function,param);			
				pT->PushTask(pTask);
				return pTask->GetID();
			}
			return DEF_INVALID_TASK_ID;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename T>		
		TaskID	AddTask				(int nThreadId,T* pObj,int (T::*function)(void* param),void* param)
		{
			ThreadType* pT;
			if(pT = _Find(nThreadId))
			{
				Task* pTask = new Task(pObj,function,param);
				pT->PushTask(pTask);
				return pTask->GetID();
			}
			return DEF_INVALID_TASK_ID;
		}			
		//////////////////////////////////////////////////////////////////////////
		bool	RemoveTask				(int nThreadId,TaskID taskID)
		{
			ThreadType* pT;
			if(pT = _Find(nThreadId))
			{
				pT->EraseTask(taskID);
				return true;
			}
			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		bool	PauseTask				(int nThreadId,TaskID taskID)
		{
			ThreadType* pT;
			if(pT = _Find(nThreadId))
			{
				pT->PauseTask(taskID);
				return true;
			}
			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename T>
		void	ListenFinishTask	(T* pListener)
		{
			struct FinishInfo
			{
				FinishInfo(uint nT,uint nt) : nThread(nT) , nTask(nt)	{}
				uint nThread;
				uint nTask;
			};
			std::list<FinishInfo> finishInfos;
			ThreadSet::iterator it = mThreads.begin();
			for ( ; it != mThreads.end() ; ++it)
			{				
				TaskID nId;
				while( (nId = it->second->FinishTask()) != DEF_INVALID_TASK_ID)
				{
					
					finishInfos.push_back( FinishInfo(it->first,nId) );
				}
			}

			for(std::list<FinishInfo>::iterator it = finishInfos.begin() ; it != finishInfos.end() ; ++it)
			{
				(*pListener)(it->nThread,it->nTask);
			}			
		}
	};
}
#endif