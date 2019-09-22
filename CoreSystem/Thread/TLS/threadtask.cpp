#include "stdafx.h"
#include "threadtask.h"
#include <list>
namespace NSTask
{

	ThreadTask::ThreadTask() : mbExit(true)
	{
		for (int i = 0 ; i <CommandType::COMMAND_COUNT ; ++i)
		{
			mCommandPorts[i] = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		}
		
		mhFinishPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

		
	}
	//////////////////////////////////////////////////////////////////////////

	ThreadTask::~ThreadTask()
	{
		mbExit = true;
		if(mhFinishPort)
			::CloseHandle(mhFinishPort);

		for (int i = 0 ; i <CommandType::COMMAND_COUNT ; ++i)
		{
			::CloseHandle(mCommandPorts[i]);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::Initial(unsigned int nUsingCPUFlag /*= -1*/)
	{
		mbExit = false;
		JGE::Win32Thread::Create(0, 0);
		JGE::Win32Thread::SetUsingCPU(nUsingCPUFlag);
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::Finalize()
	{
		mbExit = true;
		Terminate(1000);
	}
	//////////////////////////////////////////////////////////////////////////

	void ThreadTask::PushTask(Task* pTask)
	{
		_PushCommand(CommandType::PUSH_TASK,(void*)pTask);
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::EraseTask(TaskID nID)
	{
		_PushCommand(CommandType::ERASE_TASK,(void*)nID);
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::PauseTask(TaskID nID)
	{
		_PushCommand(CommandType::PAUSE_TASK,(void*)nID);
	}
	//////////////////////////////////////////////////////////////////////////
	uint ThreadTask::Run() 
	{
		
		TaskSet tasks;
		
		while(mbExit == false)
		{

			void* pTmp;
			if(_PopCommand(CommandType::PUSH_TASK,pTmp))
			{
				tasks.push_back((Task*)pTmp);
			}

			if(_PopCommand(CommandType::PAUSE_TASK,pTmp))
			{
				
			}

			if(_PopCommand(CommandType::ERASE_TASK,pTmp))
			{
				TaskID nId = (TaskID)pTmp;
				OutputDebugStringA("delete task...\n");
				TaskSet::iterator it = tasks.begin();
				for( ; it != tasks.end();++it)
				{
					Task* pTask = *it;
					if(pTask->GetID() != nId)
						continue;
					tasks.erase(it);
					delete pTask;
					OutputDebugStringA("delete task compile\n");
					break;
				}
			}

			// run task
			{
				TaskSet::iterator it = tasks.begin();
				for( ; it != tasks.end();++it)
				{
					Task* pTask = *it;
					pTask->Run();					
				}
			}

			// process end task
			{
				TaskSet::iterator it = tasks.begin();
				for( ; it != tasks.end();)
				{
					Task* pTask = *it;
					if(pTask->IsEnd())
					{
						tasks.erase(it);
						_PushFinish(pTask->GetID());
						delete pTask;						
						it = tasks.begin();
						continue;
					}
					
					++it;
					
				}
			
			}
		}
		// erase all
		{
			TaskSet::iterator it = tasks.begin();
			for( ; it != tasks.end();++it)
			{
				Task* pTask = *it;				
				delete pTask;										
			}

		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_PushFinish(TaskID nId)
	{
		::PostQueuedCompletionStatus(mhFinishPort, 0,ULONG_PTR(nId),0 );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_PushCommand(int nCmdId,void* pParam)
	{
		if(nCmdId < CommandType::COMMAND_COUNT)
			::PostQueuedCompletionStatus(mCommandPorts[nCmdId], 0,ULONG_PTR(pParam),0 );
	}
	//////////////////////////////////////////////////////////////////////////
	TaskID ThreadTask::FinishTask()
	{
		DWORD nBytes;
		TaskID nId;
		LPOVERLAPPED lpOverlapped;
		if(::GetQueuedCompletionStatus(
			mhFinishPort,
			&nBytes,
			(PULONG_PTR)&nId,
			&lpOverlapped, 0))
		{				
			return nId;
		}
		return DEF_INVALID_TASK_ID;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::_PopCommand(int nCmdId,void*& pParam)
	{
		if(nCmdId < CommandType::COMMAND_COUNT)
		{
			DWORD nBytes;
			
			LPOVERLAPPED lpOverlapped;
			if(::GetQueuedCompletionStatus(
				mCommandPorts[nCmdId],
				&nBytes,
				(PULONG_PTR)&pParam,
				&lpOverlapped, 0))
			{				
				return true;
			}
			
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	DebugTask::DebugTask()
	{
		instances.push_back(this);
	}
	//////////////////////////////////////////////////////////////////////////
	DebugTask::~DebugTask()
	{
		{
			std::list<DebugTask*>::iterator it =  instances.begin();
			for ( ; it != instances.end() ;++it)
			{
				if(*it == this)
				{
					instances.erase(it);
					break;
				}
			}
		}

		{
			TaskSet::iterator it = mTaskSet.begin();
			for( ; it != mTaskSet.end();++it)
			{
				Task* pTask = *it;
				delete pTask;			
			}		
		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	void DebugTask::Initial(unsigned int nUsingCPUFlag /*= -1*/)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void DebugTask::Finalize()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void DebugTask::PushTask(Task* pTask)
	{
		mTaskSet.push_back(pTask);
	}
	//////////////////////////////////////////////////////////////////////////
	void DebugTask::EraseTask(TaskID nId)
	{		
		TaskSet::iterator it = mTaskSet.begin();
		for( ; it != mTaskSet.end();++it)
		{
			Task* pTask = *it;
			if(pTask->GetID() != nId)
				continue;
			mTaskSet.erase(it);
			delete pTask;
			break;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	TaskID DebugTask::FinishTask()
	{
		TaskSet::iterator it = mTaskSet.begin();
		if(it != mTaskSet.end())
		{
			Task* pTask = *it;
			if(pTask->IsEnd())
			{
				TaskID nId = pTask->GetID();
				delete pTask;
				mTaskSet.erase(it);
				return nId;
			}
		}
		return DEF_INVALID_TASK_ID;
	}
	//////////////////////////////////////////////////////////////////////////
	void DebugTask::Run() 
	{
		// run task
		std::list<DebugTask*>::iterator it = instances.begin();
		for ( ; it != instances.end() ; ++it) 
		{
			DebugTask* pDebugTask = *it;
			TaskSet::iterator it2 = pDebugTask->mTaskSet.begin();
			for( ; it2 != pDebugTask->mTaskSet.end();++it2)
			{
				Task* pTask = *it2;
				if(pTask->IsEnd() == false)
					pTask->Run();					
			}
		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	std::list<DebugTask*> DebugTask::instances;
}