#include "stdafx.h"
#include "task.h"

namespace NSTask
{
	class TaskIDFactory
	{
		unsigned int mnSerialNumber;
	public:
		TaskIDFactory	() :mnSerialNumber(0)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		~TaskIDFactory	()
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		TaskID Get()
		{
			return  ++mnSerialNumber;
		}
		
	};

	TaskIDFactory gTaskIDFactory;
	//////////////////////////////////////////////////////////////////////////
	Task::Task(int (*function)(void*),void* param) : 
		mpSlot(new StaticFunctionSlot<int (void* param)>(function))	
		, mParam(param) ,mnStatus(BEGIN) , mnTaskId(_CreateID())
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Task::~Task ()
	{
		delete mpSlot;
	}
	//////////////////////////////////////////////////////////////////////////
	void Task::Run	()
	{						
		if(mnStatus != END)
		{				
			mnStatus = (*mpSlot)(mParam);			
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	TaskID Task::GetID() const 
	{
		return mnTaskId;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Task::IsEnd() const
	{
		return mnStatus == END;
	}
	//////////////////////////////////////////////////////////////////////////
	TaskID Task::_CreateID()
	{
		return gTaskIDFactory.Get();		
	}
};