#include "StdAfx.h"
#include "Log.h"

#ifndef NO_LOG

class LogImpl 
{
	HANDLE mhFile;
	bool mbOuptutDebugString;
public:	
	LogImpl():mhFile(NULL), mbOuptutDebugString(false){}; 
	~LogImpl()
	{
		Clear();
	}
	void HandleLog(int priority, const wchar_t *sTraceString)
	{
		if(mhFile!=NULL)
		{		
			char buff[1024];
			int size = WCHARToUTF8(buff, sTraceString, 1023);
			size--; //size also contains a null terminator so we step back
			buff[size++] = '\r';
			buff[size++] = '\n';
			buff[size] = '\0';
			DWORD writedSize;
			WriteFile(mhFile, buff, size, &writedSize, NULL);
		}
		if(mbOuptutDebugString)
			OutputDebugString(sTraceString);
	}

	void HandleLog(int priority, char *sTraceString, int buffer_size)
	{
		if(mhFile!=NULL)
		{		
			int pos = std::min((int)strlen(sTraceString), buffer_size-3);
			sTraceString[pos++] = '\r';
			sTraceString[pos++] = '\n';
			sTraceString[pos] = '\0';
			DWORD writedSize;
			WriteFile(mhFile, sTraceString, pos, &writedSize, NULL);
		}
		if(mbOuptutDebugString)
			OutputDebugStringA(sTraceString);
	}

	void Clear()
	{
		if(mhFile!=NULL)
		{
			::CloseHandle(mhFile);
			mhFile = NULL;
		}
	}

	void OpenLogFile(const wchar_t *pFilename)
	{
		DWORD access = GENERIC_WRITE;
		DWORD openmode = CREATE_ALWAYS;
		DWORD flags = FILE_ATTRIBUTE_NORMAL;
		DWORD sharing = FILE_SHARE_READ | FILE_SHARE_WRITE;
		mhFile = ::CreateFile(pFilename,access,sharing,NULL,openmode,flags,NULL);//_wfopen(pFilename, L"wb");
	}
	void EnableOutputDebugString(bool value)
	{
		mbOuptutDebugString = value;
	}
};


static LogImpl g_Log;

int Log::mMinPriority = 0;
void Log::Init(int min_priority, const wchar_t *pFileName, bool outputDebugString)
{
	Log::mMinPriority = min_priority;
	if(pFileName)
	{
		g_Log.OpenLogFile(pFileName);
	}
	g_Log.EnableOutputDebugString(outputDebugString);
}

void Log::Shutdown()
{
	g_Log.Clear();
}

void Log::HandleLog(int priority, const wchar_t *sTraceString)
{
	g_Log.HandleLog(priority, sTraceString);
}

void Log::HandleLog(int priority, char *sTraceString, int buffer_size)
{
	g_Log.HandleLog(priority, sTraceString, buffer_size);
}

#endif