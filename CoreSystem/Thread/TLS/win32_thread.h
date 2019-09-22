#ifndef _win32_thread_20080710pm0153_
#define _win32_thread_20080710pm0153_
#include "jge_thread.h"


namespace JGE
{
	#define THREAD_HANDLE		WThread
	struct WTHREAD__;
	typedef struct WTHREAD__*	WThread;
	//////////////////////////////////////////////////////////////////////////
	struct EnableCPU
	{
		union
		{
			uint nCPUs;
			struct  
			{
				bool bCPU0:1;bool bCPU1:1;bool bCPU2:1;bool bCPU3:1;bool bCPU4:1;
				bool bCPU5:1;bool bCPU6:1;bool bCPU7:1;bool bCPU8:1;bool bCPU9:1;
				bool bCPU10:1;bool bCPU11:1;bool bCPU12:1;bool bCPU13:1;bool bCPU14:1;
				bool bCPU15:1;bool bCPU16:1;bool bCPU17:1;bool bCPU18:1;bool bCPU19:1;
				bool bCPU20:1;bool bCPU21:1;bool bCPU22:1;bool bCPU23:1;bool bCPU24:1;
				bool bCPU25:1;bool bCPU26:1;bool bCPU27:1;bool bCPU28:1;bool bCPU29:1;
				bool bCPU30:1;bool bCPU31:1;
			};
		};
		//////////////////////////////////////////////////////////////////////////
		EnableCPU() : nCPUs((uint)-1)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		EnableCPU(uint nEnable) : nCPUs(nEnable)
		{

		}
	};
	//////////////////////////////////////////////////////////////////////////
	class Win32Thread : public Thread<WThread>
	{
		friend uint __stdcall fnWin32Proc(void*);

		void	_Terminate		(uint nWaitMillisecond);
	public:
				Win32Thread		();
		//////////////////////////////////////////////////////////////////////////
		virtual	~Win32Thread	();
		//////////////////////////////////////////////////////////////////////////
		void	Create			(uint uFlags = 0, uint nStack = 0) ;		
		//////////////////////////////////////////////////////////////////////////
		void	Terminate		(uint nWaitMillisecond) ;
		//////////////////////////////////////////////////////////////////////////
		void	SetUsingCPU		(uint nEnableFlags);
	};
};
#endif