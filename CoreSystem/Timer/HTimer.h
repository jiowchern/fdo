//--------------------------------------------------------------------------------------
// File: Timer.h
//
// Desc:
//
// Copyright (c) Dynasty Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
//#pragma once
#ifndef HTIMER_H
#define HTIMER_H

//--------------------------------------------------------------------------------------
typedef void (__stdcall *LPCALLBACKTIMER)( unsigned int nIDEvent, float fOveredTime, void* pUserContext );
//void __stdcall OnTimer( unsigned int nIDEvent, float fOveredTime, void* pUserContext ); // 宣告範例

void HandleTimers( float fElapsedTime ); // 擺在迴圈裡一直更新

// 設定一組新的計時器，傳回的數值為註冊函式的記憶體位置，所以一定要使用unsigned int來接
unsigned int SetTimer( LPCALLBACKTIMER pCallbackTimer, float fTimeoutInSecs=1.0f, void* pUserContext=0 );

//--------------------------------------------------------------------------------------
// 以下是傳入的參數為ID
//--------------------------------------------------------------------------------------
bool KillTimer( unsigned int nIDEvent ); // 刪除計時器
bool SetTimeoutInSecs( unsigned int nIDEvent, float fTimeoutInSecs ); // 設定新的TimeOut時間

bool SetTimerEnabled( unsigned int nIDEvent, bool bEnabled ); // 設定計時器智能、失能
bool GetTimerEnabled( unsigned int nIDEvent );

static bool StartTimer( unsigned int nIDEvent ) { return SetTimerEnabled( nIDEvent, true ); } // 啟動計時器(預設)
static bool PauseTimer( unsigned int nIDEvent ) { return SetTimerEnabled( nIDEvent, false ); } // 暫停計時器
bool ResetTimer( unsigned int nIDEvent ); // 重置計算時間
bool StopTimer( unsigned int nIDEvent ); // 重置並停止計時器
// Start or Stop and Reset 開始或停止並重置計算時間
// bool SandRTimer( unsigned int nIDEvent, bool bEnabled );
// bool StartRTimer( unsigned int nIDEvent ) { return SandRTimer( nIDEvent, true ); }
// bool StopRTimer( unsigned int nIDEvent ) { return SandRTimer( nIDEvent, false ); }

//--------------------------------------------------------------------------------------
// 以下是傳入的參數為CALLBACK
//--------------------------------------------------------------------------------------
/*
unsigned int GetTimerID( LPCALLBACKTIMER pCallbackTimer ); // 取得註冊回呼函式的ID
void KillTimer( LPCALLBACKTIMER pCallbackTimer ); // 刪除計時器
bool SetTimeoutInSecs( LPCALLBACKTIMER pCallbackTimer, float fTimeoutInSecs ); // 設定新的TimeOut時間

bool SetTimerEnabled( LPCALLBACKTIMER pCallbackTimer, bool bEnabled );
bool GetTimerEnabled( LPCALLBACKTIMER pCallbackTimer );

bool StartTimer( LPCALLBACKTIMER pCallbackTimer );
bool StopTimer( LPCALLBACKTIMER pCallbackTimer );
bool ResetTimer( LPCALLBACKTIMER pCallbackTimer ); // 重置計算時間
// Start or Stop and Reset 開始或停止並重置計算時間
bool SandRTimer( LPCALLBACKTIMER pCallbackTimer, bool bEnabled );
bool StartRTimer( LPCALLBACKTIMER pCallbackTimer );
bool StopRTimer( LPCALLBACKTIMER pCallbackTimer );
*/
//--------------------------------------------------------------------------------------
#endif

/*
EXAMPLE:
//--------------------------------------------------------------------------------------
// File: TestClass.h
class TestClass
{
public:
    int ID;
    static void CALLBACK StaticOnTimer( unsigned int nIDEvent, float fOveredTime, void* pUserContext );
    void OnTimer( unsigned int nIDEvent, float fOveredTime );
};
//--------------------------------------------------------------------------------------
// File: TestClass.cpp
// 時間到會自動執行這個
void CALLBACK TestClass::StaticOnTimer( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{
    // TestClass *pTest = (TestClass*)pUserContext;
    TestClass *pTest = static_cast< TestClass* >(pUserContext);
    if( pTest )
        pTest->OnTimer( nIDEvent, fOveredTime );
}

void TestClass::OnTimer( unsigned int nIDEvent, float fOveredTime )
{
    // 你要做的事…
}

int main()
{
    TestClass MyClass;
    // MyClass.ID = SetTimer( MyClass.StaticOnTimer, 1.0f, (void*)&MyClass );
    MyClass.ID = SetTimer( MyClass.StaticOnTimer, 1.0f, static_cast< void* >(&MyClass) );

    return 0;
}
*/