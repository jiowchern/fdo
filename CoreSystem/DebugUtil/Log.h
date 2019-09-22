#pragma once

//////////////////////////////////////////////////////////////////
#ifndef NO_LOG
class Log 
{
public:
	static void Init(int min_priority, const wchar_t *pFileName, bool outputDebugString);
	static void Shutdown();
	inline static void WriteLine(int priority, const wchar_t *sFormat, ...)
	{
		if(priority>=mMinPriority)
		{
			va_list vl;
			wchar_t sTraceString[1024];

			va_start(vl, sFormat);
			vswprintf(sTraceString, sizeof(sTraceString)/sizeof(wchar_t), sFormat, vl);
			va_end(vl);

			HandleLog(priority,sTraceString);
		}
	}
	inline static void WriteLineA(int priority, const char *sFormat, ...)
	{
		if(priority>=mMinPriority)
		{
			va_list vl;
			char sTraceString[1024];

			va_start(vl, sFormat);
			vsnprintf(sTraceString, sizeof(sTraceString)/sizeof(char), sFormat, vl);
			va_end(vl);

			HandleLog(priority, sTraceString, sizeof(sTraceString)/sizeof(char));
		}
	}

private:
	static void HandleLog(int priority, const wchar_t *sTraceString);
	static void HandleLog(int priority, char *sTraceString, int buffer_size);
	static int mMinPriority;
};

#define _LOG Log::WriteLine
#define _LOGA Log::WriteLineA

#else

#define _LOG 
#define _LOGA 

#endif