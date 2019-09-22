#include "stdafx.h"
#include "FDOSchedule.h"
#include "GameCommand.h"
namespace FDO
{
	static Schedule* gpSchedule = 0;
	void Schedule::InitialSchedule()
	{		
		gpSchedule = new Schedule;
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::FinalizeSchedule()
	{		

		delete gpSchedule;
		gpSchedule = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	Schedule::TaskHandle Schedule::AddTask(const NewTask& newSchedule , bool bStop)
	{
		if(gpSchedule)
		{
			Schedule::TaskHandle h = gpSchedule->_AddTask(newSchedule);
			if(bStop == false)
			{
				gpSchedule->_Run(h);
			}
			else
			{
				gpSchedule->_Pause(h);
			}
			return h;
		}
		return TdgHandle();
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::Run(TaskHandle hnd)
	{
		if(gpSchedule)
			return gpSchedule->_Run(hnd);
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::Pause(TaskHandle hnd)
	{
		if(gpSchedule)
			return gpSchedule->_Pause(hnd);
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::RemoveTask(TaskHandle hnd)
	{
		if(gpSchedule)
			return gpSchedule->_RemoveTask(hnd);
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::HandleTask(float fElapsedTime)
	{
		if(gpSchedule)
			return gpSchedule->_HandleTask(fElapsedTime);
	}
	//////////////////////////////////////////////////////////////////////////
	Schedule::Schedule()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Schedule::~Schedule()
	{
		TaskSet& tasks = mTasks;
		_FOREACH_CONST(TaskSet , tasks , it)
		{
			TaskHandle hTask = *it;
			mTaskRes.Destory(hTask);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Schedule::TaskHandle Schedule::_AddTask(const NewTask& newSchedule)
	{		
		TaskHandle hTask = mTaskRes.Create(newSchedule);
		mTasks.push_back(hTask);
		return hTask;
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::_RemoveTask(TaskHandle hnd)
	{
		mTasks.remove(hnd);
		mTaskRes.Destory(hnd);
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::_Run(TaskHandle hnd)
	{
		Task* pTask = mTaskRes.Query(hnd);
		if(pTask)
		{
			pTask->Enable();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::_Pause(TaskHandle hnd)
	{
		Task* pTask = mTaskRes.Query(hnd);
		if(pTask)
		{
			pTask->Disable();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::_HandleTask(float fElapsedTime)
	{
		TaskSet tasks = mTasks;
		_FOREACH_CONST(TaskSet , tasks , it)
		{
			TaskHandle hTask = *it;
			Task* pTask = mTaskRes.Query(hTask);
			if(pTask )
			{
				pTask->Run(fElapsedTime);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	
	Schedule::Task::Task(TaskHandle hnd,const NewTask& newTask)
	{
		NewTask::fTime = newTask.fTime;
		NewTask::hProcess = newTask.hProcess;
		fTimeCount = fTime;
	}
	//////////////////////////////////////////////////////////////////////////
	Schedule::Task::~Task()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::Task::Enable()
	{
		mbEnable = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Schedule::Task::Disable()
	{
		mbEnable = false;
	}	
	
	//////////////////////////////////////////////////////////////////////////
	void Schedule::Task::Run(float fElapsedTime)
	{
		if(mbEnable )
		{
			fTimeCount -= fElapsedTime;
			if(fTimeCount <= 0.0f)
			{
				GameCommand::Executer&	exr = CommandSystem::fnGetExecuter();
				exr.RunLight<float>(NewTask::hProcess , -fTimeCount);
				fTimeCount = fTime;
			}		
		}
		
	}
	
}