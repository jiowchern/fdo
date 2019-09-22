//
// Debug.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------------------------
#include "Common.h"
#include "Debug.h"
#include <stdarg.h>
#include <stdio.h>
#include "iniReader.h"
#include <assert.h>
#include <dbghelp.h>
#include "..\version.h"
#include "Log.h"
#pragma hdrstop
#define _SUPPORT_MINIDUMP_
using namespace FDO ;

static bool g_bDebug = true;
static bool g_bInitRead = false;

//<gino>
static _ShowGameMessageHandler g_showGameMessageFunc=NULL;
// 
void SetDebugGameMessageHandler(_ShowGameMessageHandler func)
{
	g_showGameMessageFunc = func;
}

//</gino>

static _ShowHVGameMessageHandler g_showHVGameMessageFunc=NULL;

void SetDebugHVGameMessageHandler(_ShowHVGameMessageHandler func)
{
	g_showHVGameMessageFunc = func;
}

class CDebugLogFile
{
private:
    FILE* m_pFile;
public:
    CDebugLogFile() { m_pFile = NULL; }
    ~CDebugLogFile() { Close(); }
    FILE* Open() 
	{ 
		if (!m_pFile) 
			m_pFile = fopen("DebugMsg.txt", "w+t"); 
		return m_pFile; 
	}
    void Close() 
	{ 
		if (m_pFile) 
			fclose(m_pFile); 
		m_pFile = NULL; 
	}
    FILE* File() { return m_pFile; }
    void WriteLine(const char* szLine) 
	{
		try
		{
			if (!m_pFile || !szLine) 
				return;
			if(szLine[0] == 0)
				return ;

			fwrite(szLine, 1, strlen(szLine), m_pFile);
			fwrite("\n", 1, 1, m_pFile);

		}
		catch (std::exception& e)
		{						
			Assert(0 , e.what());
		}
		catch (...)
		{
		}
    }
    void WriteLine(const std::string& str) 
	{ 
		WriteLine( str.c_str() ); 
	}

	void WriteLineW(const WCHAR* szLine) 
	{
        
		try
		{
			if (!m_pFile || !szLine ) 
				return;

			if(szLine[0] == 0)
				return ;
			fwrite( szLine, 1, wcslen(szLine), m_pFile );
			fwrite( L"\n", 1, 1, m_pFile);
		}			
		catch (std::exception& e)
		{
			Assert(0 , e.what());
		}
		catch (...)
		{
		}
        
    }
    void WriteLineW(const std::wstring& str) 
	{ 
		WriteLineW( str.c_str() ); 
	}
};

//static CDebugLogFile g_DebugLogFile;

void SetDebug(bool bDebug, bool bWriteFile)
{
    g_bDebug = bDebug;
    
    if (bDebug) {
      //  if (bWriteFile) {
     //       g_DebugLogFile.Open();
       // } // end if
    }
    else {
     //   g_DebugLogFile.Close();
    } // end else
}

template<>
void DebugMsg<const char*>(const char* fmt, ...)
{	

//     if (false == g_bInitRead) {
//         if (GetConfig("DebugMsg") == "Yes")
//             g_bDebug = true;
//         else g_bDebug = false;
//     }
//     g_bInitRead = true;
//     
//     if (false == g_bDebug)
//         return;

	std::string str;
	try
	{
		va_list   argptr;   
		va_start(argptr,   fmt);   
		int   bufsize   =   vsnprintf(NULL,   0,   fmt,   argptr)   ;   

		str.resize(bufsize);
		if(bufsize)
		{
			char8*   buf = &*str.begin();		
			vsnprintf(buf, bufsize, fmt, argptr);    		
		}		
		va_end(argptr);     
	}
	catch (std::exception& e)
	{
		Assert(0 , e.what());
	}
	catch (...)
	{
	}

    _LOGA( 0 , str.c_str() );

	
}

template<>
void DebugMsg<const WCHAR*>(const WCHAR* fmt, ...)
{
	// 	assert(g_showGameMessageFunc);
	// 
	//     if (false == g_bInitRead) {
	//         if (GetConfig("DebugMsg") == "Yes")
	//             g_bDebug = true;
	//         else g_bDebug = false;
	//     }
	//     g_bInitRead = true;
	//     
	//     if (false == g_bDebug)
	//         return;

	std::wstring str;
	try
	{

		va_list   argptr;   
		va_start(argptr,   fmt);   
		int   bufsize   =   _vsnwprintf(NULL,   0,   fmt,   argptr)   ;   

		str.resize(bufsize);
		if(bufsize)
		{
			char16*   buf = &*str.begin();		
			_vsnwprintf(buf,   bufsize,   fmt,   argptr);    		
		}		
		va_end(argptr);   
	}
	catch (std::exception& e)
	{
		Assert(0 , e.what());
	}
	catch (...)
	{
	}
	_LOG( 0 , str.c_str() );
}



//========================================================================================
// Memory Dump 
//========================================================================================
#ifndef _DEBUG
typedef 	BOOL  (*MiniDumpWriteDumpFunc)( HANDLE ,
  DWORD ,
  HANDLE ,
  MINIDUMP_TYPE ,
  PMINIDUMP_EXCEPTION_INFORMATION ,
  PMINIDUMP_USER_STREAM_INFORMATION ,
  PMINIDUMP_CALLBACK_INFORMATION 
);

LONG __stdcall TheCrashHandlerFunction ( EXCEPTION_POINTERS * pExPtrs )
{

#ifdef _SUPPORT_MINIDUMP_
	HMODULE hDLL = ::LoadLibrary(L"dbghelp.dll");


	MiniDumpWriteDumpFunc dumpFunc = NULL;
	if(hDLL!=NULL){
		dumpFunc = (MiniDumpWriteDumpFunc)::GetProcAddress(hDLL, "MiniDumpWriteDump");
		if(dumpFunc!=NULL){
			TCHAR DumpPath[_MAX_PATH];

			GetCurrentDirectory(_MAX_PATH, DumpPath);
			TCHAR timer[50];
			time_t t;

			time(&t);
			struct tm *ltime = localtime(&t);
			_tcsftime(timer, sizeof timer, _T("%Y%m%d-%H%M%S"), ltime);
			_tcscat(DumpPath, _T("\\"));
			_tcscat(DumpPath, _T(VERSION_PRODUCTNAME_VALUE));			
			_tcscat(DumpPath, _T("_V"));
			_tcscat(DumpPath, _T(VERSION_PRODUCTVERSION_VALUE));
			_tcscat(DumpPath, _T("@"));
			_tcscat(DumpPath,timer);
			_tcscat(DumpPath,_T("crash.dmp"));

			HANDLE hMiniDumpFile = CreateFile( DumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
			MINIDUMP_EXCEPTION_INFORMATION eInfo;
			eInfo.ThreadId = GetCurrentThreadId();
			eInfo.ExceptionPointers = pExPtrs;
			eInfo.ClientPointers = TRUE;

			dumpFunc(GetCurrentProcess(), GetCurrentProcessId(), hMiniDumpFile, MiniDumpNormal, &eInfo, NULL, NULL);
			
			CloseHandle(hMiniDumpFile);
		}
	}

#endif

	HWND hWnd = CreateWindow(L"DumpMsg", L"DumpMsgWin", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	::MessageBox(hWnd, _T("sorry for crash!"), _T("ERROR"), MB_OK);

	return EXCEPTION_EXECUTE_HANDLER;
}

void EnableCrashMemoryDump()
{
	SetUnhandledExceptionFilter(TheCrashHandlerFunction);
}
#endif
//========================================================================================
// Testing Routines
//========================================================================================

#include "CFDOXML.h"
#include "TextObject.h"

void Test_MakeTextObject()
{
    DebugMsg("Beginning Test_MakeTextObject().");
    
    D3DXCOLOR FontColor(0x11223344);
    LPCWSTR szText = L"asdfghjklqwertyuiopzxcvbnm";
    for (int i = 0; i < 300; i++) {
        CTextObject* pTextObject;// = new CTextObject();
        //pTextObject = g_FDOXML.Test_MakeTextObject(
        //    g_pD3DDevice, NULL, FontColor, szText, -1, 256);
        pTextObject = g_FDOXML.MakeTextObject(
            g_pD3DDevice, NULL, FontColor, szText, -1, 256);
        if (pTextObject) {
            delete pTextObject;
            pTextObject = NULL;
        } // end if
    } // end for
    
    DebugMsg("Test_MakeTextObject() completed.");
}

//========================================================================================
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
PerformanceMonitor::PerformanceMonitor(PFN_OUT pfnOut) : mpfnOut(pfnOut)
{

}

PerformanceMonitor::~PerformanceMonitor()
{

}

void PerformanceMonitor::Begin(const char* sCaption)
{
	mCaption = sCaption;
	QueryPerformanceFrequency(&mLintTC);
	QueryPerformanceCounter(&mLintBegin);	
}

void PerformanceMonitor::End()
{	
	QueryPerformanceCounter(&mLintEnd);
	float fTime = (float)(mLintEnd.QuadPart - mLintBegin.QuadPart) / mLintTC.QuadPart;
	if(mpfnOut)
	{
		mpfnOut(mCaption.c_str(),fTime);
	}
}

void PerformanceMonitor::fnDefaultOut(const char* caption , float fTime)
{
	char inf[256];
	sprintf(inf,"%s:%f\n",caption,fTime);
	::OutputDebugStringA(inf);
}