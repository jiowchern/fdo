#ifndef _thread_20080710pm0139_
#define _thread_20080710pm0139_

namespace JGE
{
	
	template<class THREAD_HANDLE_TYPE>
	class Thread
	{
	protected:
		THREAD_HANDLE_TYPE	mhThread;	
		uint				miThread;
		void*				mpVoid;
		
		//////////////////////////////////////////////////////////////////////////
		virtual uint	Run	() = 0;
	public:
						Thread		() 
		: mhThread(0),miThread(0),mpVoid(0)
		{
		}
		//////////////////////////////////////////////////////////////////////////
		virtual			~Thread		()
		{
			mhThread = 0;
			miThread = 0;
			mpVoid = 0;
		}
		//////////////////////////////////////////////////////////////////////////
		virtual void	Create		(uint uFlags = 0, uint nStack = 0) = 0;
		
		//////////////////////////////////////////////////////////////////////////
		virtual void	Terminate	(uint nWaitMillisecond) = 0;			
	};
}


#endif