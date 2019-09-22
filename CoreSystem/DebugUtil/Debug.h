//
// Debug.h
//
#ifndef __Debug_h__
#define __Debug_h__

//void SetDebug(bool bDebug, bool bWriteFile);

template<typename TFormat>
void DebugMsg(TFormat fmt,...);

template<>
void DebugMsg<const WCHAR*>(const WCHAR* fmt, ...);
template<>
void DebugMsg<const char*>(const char* fmt, ...);
#define DebugLog DebugMsg
//void DebugMsg(const char* format, ...);

//void DebugLog(const char* format, ...);

//<daisy>
//void DebugMsg(const WCHAR* format, ...);
//void DebugLog(const WCHAR* format, ...);
//</daisy>

//<gino>
typedef void (*_ShowGameMessageHandler)(const char *msg);
void SetDebugGameMessageHandler(_ShowGameMessageHandler func);
//</gino>

//<daisy>
typedef void (*_ShowHVGameMessageHandler)(const WCHAR *msg);
void SetDebugHVGameMessageHandler(_ShowHVGameMessageHandler func);
//</daisy>

#ifdef _DEBUG
#define EnableCrashMemoryDump() 
#else
void EnableCrashMemoryDump();
#endif
// Testing Routines
void Test_MakeTextObject();

class PerformanceMonitor
{
	typedef void (*PFN_OUT)(const char* caption , float fTime);
	std::string			mCaption;
	LARGE_INTEGER mLintTC , mLintBegin ,mLintEnd;
	PFN_OUT	mpfnOut;
	static void fnDefaultOut(const char* caption , float fTime);
public:
	
	PerformanceMonitor(PFN_OUT pfnOut = fnDefaultOut);
	~PerformanceMonitor();

	void	Begin	(const char* sCaption);
	void	End		();
};
#endif // __Debug_h__

