#ifndef _task_20080808pm0328_
#define _task_20080808pm0328_
#include <list>
namespace NSTask
{
	//////////////////////////////////////////////////////////////////////////
	typedef unsigned int TaskID;
#define	DEF_INVALID_TASK_ID 0
	//////////////////////////////////////////////////////////////////////////

	class FunctionSlot
	{
	public:
		virtual int operator() (void* param) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	class MemeberFunctionSlot : public FunctionSlot
	{		
		typedef int(T::*PFN_)(void* param);

		PFN_	mpFn;
		T*		mpObj;
	public:
		MemeberFunctionSlot(T* obj,PFN_ func ) : mpFn(func), mpObj(obj)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		~MemeberFunctionSlot()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		int operator() (void* param) 
		{
			return (mpObj->*mpFn)(param);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	class StaticFunctionSlot : public FunctionSlot
	{
	private:
		T* mpFn;
	public:
		StaticFunctionSlot(T* functor) :
		mpFn(functor)
		{}
		//////////////////////////////////////////////////////////////////////////
		~StaticFunctionSlot()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		int operator() (void* param) 
		{
			return (*mpFn)(param);
		}

	
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	class Task
	{
	public:
		enum
		{
			BEGIN,RUNNING,END
		};
	private:
		TaskID			mnTaskId;
		int				mnStatus;
		FunctionSlot*	mpSlot;
		void*			mParam;		

		TaskID			_CreateID	();
	public:
		
		
		//////////////////////////////////////////////////////////////////////////
		Task	(int (*function)(void*),void* param) ;
		//////////////////////////////////////////////////////////////////////////
		template<typename T>		
		Task	(T* pObj,int (T::*function)(void* param),void* param) : 
			mpSlot(new MemeberFunctionSlot<T>(pObj,function))	
			, mParam(param) ,mnStatus(BEGIN) , mnTaskId(_CreateID())
		{

		}
		//////////////////////////////////////////////////////////////////////////
				~Task	();
		
		//////////////////////////////////////////////////////////////////////////
		void	Run		();
		
		//////////////////////////////////////////////////////////////////////////
		TaskID	GetID	() const ;
		
		//////////////////////////////////////////////////////////////////////////
		bool	IsEnd	() const;
		
	};

	typedef std::list<Task*> TaskSet;
	
};


#endif