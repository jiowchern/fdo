#include "stdafx.h"
#include "win32_thread.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <stdlib.h>

namespace JGE
{
	uint __stdcall fnWin32Proc(void* args)
	{
		Win32Thread* pThread = (Win32Thread*)args;
		uint retCode = pThread->Run();
		_endthreadex(retCode);
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	Win32Thread::Win32Thread()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32Thread::~Win32Thread()
	{
		_Terminate(1000);

	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Thread::Create(uint uFlags /*= 0*/, uint nStack /*= 0*/) 
	{
		mhThread = (THREAD_HANDLE)_beginthreadex(0, nStack, fnWin32Proc, (void*)this, 0, &miThread);
		::SetThreadPriority(mhThread , THREAD_PRIORITY_LOWEST );
		
		if(mhThread)
			return;
		int err = errno;

//		assert(0);
	}	
	//////////////////////////////////////////////////////////////////////////
	void Win32Thread::Terminate(uint nWaitMillisecond) 
	{
		_Terminate(nWaitMillisecond);
		mhThread = 0;
		miThread = 0;
		mpVoid = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Thread::_Terminate(uint nWaitMillisecond)
	{
		if(mhThread)
		{
			DWORD uExitCode;
			if(GetExitCodeThread((HANDLE)mhThread, &uExitCode) && uExitCode == STILL_ACTIVE)
			{
				uExitCode = WaitForSingleObject((HANDLE)mhThread, nWaitMillisecond);
				if(uExitCode == WAIT_TIMEOUT)
					TerminateThread((HANDLE)mhThread, 0);
			}
			CloseHandle((HANDLE)mhThread);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Thread::SetUsingCPU(uint nEnableFlags)
	{
		DWORD_PTR e = nEnableFlags;
		DWORD res;
		if(mhThread)
		{
			res = SetThreadAffinityMask(mhThread,e);			
		}
	}
};