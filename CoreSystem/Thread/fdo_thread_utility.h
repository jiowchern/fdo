#ifndef _fdo_thread_utility_20080711pm0105_
#define _fdo_thread_utility_20080711pm0105_
#include "TLS\task_manager.h"
#include "TLS\threadtask.h"
#include "TLS\Task_Listener.h"
#include "singleton.h"


//#define _USING_THREAD_TASK 

namespace FDO
{
	// Thread ID定義
	enum ThreadID
	{
		ThreadID_INVALID,
		ThreadID_BEGIN,
		// 在此添加新的類型 ...
		//FINDPATH1 .
		//FINDPATH2 ,
		ThreadID_TEST = ThreadID_BEGIN ,
		// ...
		// 以下不能更動.
		ThreadID_END,
		ThreadID_COUNT = ThreadID_END - ThreadID_BEGIN
	};
	//////////////////////////////////////////////////////////////////////////
#ifdef _USING_THREAD_TASK 
	typedef NSTask::TaskManager<NSTask::ThreadTask> 
#else
	typedef NSTask::TaskManager<NSTask::DebugTask> 
#endif
		Win32TaskManager;

	typedef NSTask::TaskID			TaskID;
		
	//////////////////////////////////////////////////////////////////////////

	class TaskManager : public Win32TaskManager , public Singleton<TaskManager>
	{
	public:
			TaskManager		();
			~TaskManager	();
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	typedef NSTask::TaskInquisitor	TaskInquisitor;

	class TaskListener : public NSTask::TaskListener , public Singleton<TaskListener>
	{
	public:

	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void fnInitialTaskManager(int nThreadAmount,unsigned int* vUsingCPUFlag = 0);
	//////////////////////////////////////////////////////////////////////////
	void fnFinalizeTaskManager();
	//////////////////////////////////////////////////////////////////////////
	
};
#define FDO_CREATE_TASK_MANAGER		FDO::fnInitialTaskManager
#define FDO_DESTORY_TASK_MANAGER	FDO::fnFinalizeTaskManager
#define FDO_ADD_TASK				FDO::TaskManager::GetSingletonPtr()->AddTask

#define FDO_ADD_TASK_S(thread_id,run_func_ptr,run_param,finish_func_ptr)				\
{	\
	TaskID nId = FDO::TaskManager::GetSingletonPtr()->AddTask(thread_id,run_func_ptr,run_param);\
	if(finish_func_ptr) \
		FDO::TaskListener::GetSingletonPtr()->SetRequest(nId,finish_func_ptr); \
} 
#define FDO_ADD_TASK_M(thread_id,run_obj_ptr,run_func_ptr,run_param,finish_func_ptr)				\
{	\
	TaskID nId = FDO::TaskManager::GetSingletonPtr()->AddTask(thread_id,run_obj_ptr,run_func_ptr,run_param);\
	if(finish_func_ptr) \
		FDO::TaskListener::GetSingletonPtr()->SetRequest(nId,finish_func_ptr); \
} 

#define FDO_REMOVE_TASK				FDO::TaskManager::GetSingletonPtr()->RemoveTask	
#define FDO_LISTEN_FINISH_TASK		FDO::TaskManager::GetSingletonPtr()->ListenFinishTask

#endif