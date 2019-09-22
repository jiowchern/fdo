#ifndef _FDOSchedule_20100424pm0415_
#define _FDOSchedule_20100424pm0415_
#include "singleton.h"
#include "GameDatabase.h"
namespace FDO
{
	struct Schedule : public FDO::Singleton<Schedule>
	{	
		typedef TdgHandle TaskHandle;		
		struct NewTask 
		{
			TdgHandle	hProcess;
			float		fTime;		
		};

		static void			InitialSchedule	();
		static void			FinalizeSchedule();

		static TaskHandle	AddTask			(const NewTask& newSchedule , bool bStop );
		static void			Run				(TaskHandle hnd);
		static void			Pause			(TaskHandle hnd);
		static void			RemoveTask		(TaskHandle hnd);


		static void			HandleTask		(float fElapsedTime);

	private:
		Schedule();
		~Schedule();

		TaskHandle	_AddTask		(const NewTask& newSchedule);
		void		_RemoveTask		(TaskHandle hnd);
		void		_Run			(TaskHandle hnd);
		void		_Pause			(TaskHandle hnd);

		void		_HandleTask		(float fElapsedTime);
		
		
		struct Task : private NewTask	
		{
					Task		(TaskHandle hnd,const NewTask& newTask);
					~Task		();
			void	Enable		();
			void	Disable		();		
			void	Run			(float fElapsedTime);
		private:
			bool	mbEnable;
			float	fTimeCount;
		};		
		typedef ResoueceSystem::TResource<Task, ObjFactory>	TaskResource;
		TaskResource	mTaskRes;

		typedef std::list<TaskHandle>	TaskSet;
		TaskSet			mTasks;
	};

}

#endif