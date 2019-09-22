#ifndef _20080814pm0346_task_listener_
#define _20080814pm0346_task_listener_
#include "Task.h"
#include <map>
namespace NSTask
{
	class TaskInquisitor
	{
	public:

						TaskInquisitor	();
		virtual			~TaskInquisitor	();
		virtual	void	TaskFinish		(int nThreadId,TaskID nId) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class TaskListener
	{		
		std::map<TaskID,TaskInquisitor*> mInquisitors;
	public:
				TaskListener	();
		virtual	~TaskListener	();

		void	SetRequest		(TaskID nId,TaskInquisitor* pInquisitor);
		void	operator()		(int nThreadId,TaskID nTaskId);
	};
};
#endif