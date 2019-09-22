#ifndef _threadtask_20080808pm0600_
#define _threadtask_20080808pm0600_
#include "task.h"
#include <windows.h>
#include "win32_thread.h"
namespace NSTask
{

	namespace CommandType
	{
		enum
		{
			PUSH_TASK,
			ERASE_TASK,
			PAUSE_TASK,
			COMMAND_COUNT
		};
	}

	class ThreadTask : public JGE::Win32Thread
	{

		//////////////////////////////////////////////////////////////////////////
		HANDLE	mCommandPorts[CommandType::COMMAND_COUNT];		
		HANDLE	mhFinishPort;
		bool	mbExit;
		
		//////////////////////////////////////////////////////////////////////////
		void	_PushCommand	(int nCmdId,void* pParam);
		bool	_PopCommand		(int nCmdId,void*& pParam);
		void	_PushFinish		(TaskID nId);
	public:
				ThreadTask	();
				~ThreadTask	();

		void	Initial		(unsigned int nUsingCPUFlag = -1);
		void	Finalize	();
		void	PushTask	(Task* pTask);
		void	EraseTask	(TaskID nID);
		void	PauseTask	(TaskID nID);
		TaskID 	FinishTask	();
		//////////////////////////////////////////////////////////////////////////
		// Win32Thread
		uint	Run			() ;
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class DebugTask
	{
		TaskSet	mTaskSet;
		static std::list<DebugTask*> instances;
	public:

					DebugTask	();
					~DebugTask	();

		void		Initial		(unsigned int nUsingCPUFlag = -1);
		void		Finalize	();
		void		PushTask	(Task* pTask);
		void		EraseTask	(TaskID nID);
		void		PauseTask	(TaskID nID);
		TaskID 		FinishTask	();
		static void	Run			();
	};
}

#endif