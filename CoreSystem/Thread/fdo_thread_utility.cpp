#include "stdafx.h"
#include "fdo_thread_utility.h"

namespace FDO
{
	//////////////////////////////////////////////////////////////////////////
	TaskManager::TaskManager()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	TaskManager::~TaskManager()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	void fnInitialTaskManager(int nThreadAmount,unsigned int* vUsingCPUFlag)
	{
		bool bCreateFlags = vUsingCPUFlag == 0;
		if(bCreateFlags)
		{
			vUsingCPUFlag = new unsigned int[nThreadAmount];
			memset(vUsingCPUFlag,-1,sizeof(unsigned int) * nThreadAmount);
		}

		if(TaskManager::GetSingletonPtr() == 0)
		{
			TaskManager* pMgr = new TaskManager;
			for (int i = 0 ; i < nThreadAmount ; ++i)
			{
				pMgr->CreateThread(i+1,vUsingCPUFlag[i]);
			}
			
		}

		if(bCreateFlags)
		{
			delete vUsingCPUFlag;
		}


		new TaskListener;
	}
	//////////////////////////////////////////////////////////////////////////
	void fnFinalizeTaskManager()
	{
		TaskManager* pMgr = TaskManager::GetSingletonPtr();
		if(pMgr)
		{			
			delete pMgr;
		}

		if(TaskListener::GetSingletonPtr())
			delete TaskListener::GetSingletonPtr();
	}
}