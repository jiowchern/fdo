//--------------------------------------------------------------------------------------
// File: HTimer.cpp
//
// Desc:
//
// Copyright (c) Dynasty Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "stdafx.h"

#include <vector>
#include <algorithm>
using namespace std;

#pragma hdrstop
#include "HTimer.h"

//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("管理模式啟動") 

#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
// Internal functions forward declarations
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
unsigned int GetTimerID( void* ptr )
{ // 在這裡決定ID產生的方法
    static unsigned int ID = 0;
    ++ID;
    return ID;

    // 由指標的位置來決定(指標的位置轉成INT)
    //return reinterpret_cast< unsigned int >( ptr );
}

//--------------------------------------------------------------------------------------
// Stores timer callback info
//--------------------------------------------------------------------------------------
#pragma pack( push )
#pragma pack( 1 )
struct H_TIMER
{
    LPCALLBACKTIMER pCallbackTimer;
    void* pUserContext;
    float fTimeoutInSecs;
    float fCountdown;
    bool  bEnabled;

    //bool operator == (unsigned int ID) { return (GetTimerID(this)==ID); } // ID由HTimer的記憶體位置決定
    unsigned int nID;
    bool operator == (unsigned int ID) { return (nID==ID); }
};
#pragma pack( pop )
typedef std::list< H_TIMER > TIMERS;
typedef TIMERS::iterator IT;
typedef TIMERS::const_iterator CIT; // 改變其內容視為無效

//--------------------------------------------------------------------------------------
// 可以直接把 TimerList 宣告最前面即可
// 這樣做的好處是：使用到才會產生。
//--------------------------------------------------------------------------------------
TIMERS& GetTimers()
{
    static TIMERS TimerList;
    return TimerList;
}
TIMERS* GetTimersPtr() { return &GetTimers(); }

IT GetTimer( unsigned int nIDEvent ) 
{ 
	TIMERS& timers = GetTimers();
	_FOREACH(TIMERS , timers, it)
	{
		if (it->nID == nIDEvent)
		{
			return it;
		}
	}
	return timers.end();
		//find( GetTimers().begin(), GetTimers().end(), nIDEvent ); 
}
bool FindTimer( CIT pos) { return ( pos != GetTimers().end() ); }
bool FindTimer( unsigned int nIDEvent ) { return FindTimer( GetTimer( nIDEvent ) ); }
void RemoveTimer( unsigned int nIDEvent ) { GetTimers().erase( GetTimer( nIDEvent ) ); }

//--------------------------------------------------------------------------------------
// 擺在迴圈裡一直更新
//--------------------------------------------------------------------------------------
/* void HandleTimers( float fElapsedTime )
{
    TIMERS &TimerList = GetTimers();

    CIT end( TimerList.end() );
    for( IT i(TimerList.begin()); i!=end; ++i )
    {
        H_TIMER &HTimer = (*i);
        if( HTimer.bEnabled && HTimer.fTimeoutInSecs>0 )
        {
            HTimer.fCountdown -= fElapsedTime;
            if( HTimer.fCountdown <= 0 )
            { // 時間到！觸發！
                LPCALLBACKTIMER pCallbackTimer = HTimer.pCallbackTimer;
                if( pCallbackTimer )
                    pCallbackTimer( HTimer.nID, -HTimer.fCountdown, HTimer.pCallbackUserContext ); // 執行註冊函式

                HTimer.fCountdown = HTimer.fTimeoutInSecs; // 重置時間
            }
        }
    }
} */

//--------------------------------------------------------------------------------------
// template<class Type>
class ProcessTimer
{
private:
   // Type Factor; // The value to Handle
   float m_fElapsedTime;

public:
    // ProcessTimer( const Type& Val ) : Factor ( Val ) {}
    ProcessTimer( float fElapsedTime ) : m_fElapsedTime ( fElapsedTime ) { }
    // The function call for...
    // void operator ( ) ( Type& elem ) const
    bool operator ( ) ( H_TIMER& HTimer ) const
    //void operator ( ) ( H_TIMER& HTimer ) const
    {
        // if( HTimer.bEnabled && HTimer.fTimeoutInSecs>0 )
        if( HTimer.bEnabled ) // 如果 fTimeoutInSecs <= 0 代表每一次都觸發
        {
            HTimer.fCountdown -= m_fElapsedTime;
            if( HTimer.fCountdown <= 0 )
            { // 時間到！觸發！
                LPCALLBACKTIMER pCallbackTimer = HTimer.pCallbackTimer;
                if( pCallbackTimer ) // 這行可以不用判斷
                    pCallbackTimer( HTimer.nID, -HTimer.fCountdown, HTimer.pUserContext ); // 執行註冊函式

                HTimer.fCountdown = HTimer.fTimeoutInSecs; // 重置時間
            }
        }
        return (HTimer.nID==0);
    }
};


//--------------------------------------------------------------------------------------
// 擺在迴圈裡一直更新
//--------------------------------------------------------------------------------------
void HandleTimers( float fElapsedTime )
{ 

	TIMERS& timers =  GetTimers();

	TIMERS::iterator it(timers.begin());
	while(it != timers.end())
	{
		ProcessTimer pt(fElapsedTime);
		if(pt(*it))
		{
			it = timers.erase(it);
			if(it == timers.end())
				break;

			continue;
		}

		++it;
	}
	//remove_if( GetTimers().begin(), GetTimers().end(), ProcessTimer( fEla1psedTime ) ); 
}

// wstring& ret()
// {
// 	...
// 	static wstring empty();
// 	return empty;
// }
//--------------------------------------------------------------------------------------
// 這很重要，因為可能會發生在處理中做刪除Timer的動作
// 所以集中在處理前做刪除的動作
//--------------------------------------------------------------------------------------
bool KillTimer( unsigned int nIDEvent )
{
    bool ret = false;

	TIMERS& timers = GetTimers();
	_FOREACH(TIMERS , timers, it)
	{
		if (it->nID == nIDEvent)
		{
			H_TIMER* HTimer = &*it;
			HTimer->nID = 0;
			HTimer->pCallbackTimer = 0;
			HTimer->pUserContext =(void*) 0x12345678;
			ret = true;
		}
	}
	return ret;

//     IT i( GetTimer( nIDEvent ) );
//     if( i != GetTimers().end() )
//     {
//         H_TIMER* HTimer = &*i;
//         HTimer->nID = 0;
// 		HTimer->pCallbackTimer = 0;
//         ret = true;
// 
// #ifdef RESMGR
// 
// 		//cosiann 移除他！ 這才是observer
// 		//GetTimers().erase(i) ;
// 
// #else
// 
// #endif

    //}

    return ret;
}

//--------------------------------------------------------------------------------------
// Starts a user defined timer callback
// 回傳產生的ID
//--------------------------------------------------------------------------------------
unsigned int SetTimer( LPCALLBACKTIMER pCallbackTimer, float fTimeoutInSecs, void* pUserContext )
{
    if( NULL == pCallbackTimer )
    { // TODO: 當回呼函式為空指標的時候不增加Timer(增加沒有意義)
        return 0;
    }

    TIMERS& TimerList = GetTimers();
    TimerList.push_back( H_TIMER() ); // 增加一個
    // TimerList.resize( TimerList.size()+1 ); // 增加一個

    H_TIMER& HTimer = TimerList.back();
    HTimer.pCallbackTimer = pCallbackTimer;
    HTimer.pUserContext = pUserContext;
    HTimer.fTimeoutInSecs = fTimeoutInSecs;
    HTimer.fCountdown = fTimeoutInSecs;
    HTimer.bEnabled = true;
    HTimer.nID = GetTimerID( &HTimer );

    return HTimer.nID;
    //return GetTimerID( &HTimer ); // ID由HTimer的記憶體位置決定
}

//--------------------------------------------------------------------------------------
// 沒找到或是失能均傳回 false
//--------------------------------------------------------------------------------------
bool GetTimerEnabled( unsigned int nIDEvent )
{
    bool ret = false;

    IT i( GetTimer( nIDEvent ) );
    if( i != GetTimers().end() )
        ret = (*i).bEnabled;

    return ret;
}

//--------------------------------------------------------------------------------------
// OK
//--------------------------------------------------------------------------------------
bool SetTimerEnabled( unsigned int nIDEvent, bool bEnabled )
{
    bool ret = false;

    IT i( GetTimer( nIDEvent ) );
    if( i != GetTimers().end() )
    {
        // H_TIMER &HTimer = (*i);
        // HTimer.bEnabled = bEnabled;
        (*i).bEnabled = bEnabled;

        ret = true;
    }

    return ret;
}

//--------------------------------------------------------------------------------------
/* bool KillTimer( unsigned int nIDEvent )
{
    return ( remove( GetTimers().begin(), GetTimers().end(), nIDEvent ) != GetTimers().end() );
} */
/* bool KillTimer( unsigned int nIDEvent )
{
    bool ret = false;

    IT i( GetTimer( nIDEvent ) );
    if( i != GetTimers().end() )
    {
        GetTimers().erase( i );
        ret = true;
    }

    return ret;
} */
// TODO: remove 無作用，why？
// remove( GetTimers().begin(), GetTimers().end(), nIDEvent )

//--------------------------------------------------------------------------------------
// 重新計時
//--------------------------------------------------------------------------------------
bool ResetTimer( unsigned int nIDEvent )
{
    bool ret = false;

    IT i( GetTimer( nIDEvent ) );
    if( i != GetTimers().end() )
    {
        H_TIMER &HTimer = (*i);
        HTimer.fCountdown = HTimer.fTimeoutInSecs; // 重置時間
        ret = true;
    }

    return ret;
}

//--------------------------------------------------------------------------------------
bool StopTimer( unsigned int nIDEvent )
{
    bool ret = false;

    IT i( GetTimer( nIDEvent ) );
    if( i != GetTimers().end() )
    {
        H_TIMER &HTimer = (*i);
        HTimer.fCountdown = HTimer.fTimeoutInSecs; // 重置時間
        HTimer.bEnabled = false;
        ret = true;
    }

    return ret;
}

//--------------------------------------------------------------------------------------
// 設定新的倒數時間
//--------------------------------------------------------------------------------------
bool SetTimeoutInSecs( unsigned int nIDEvent, float fTimeoutInSecs )
{
    bool ret = false;

    IT i( GetTimer( nIDEvent ) );
    if( i != GetTimers().end() )
    {
        H_TIMER &HTimer = (*i);
        HTimer.fTimeoutInSecs = fTimeoutInSecs; // 重置時間
        ret = true;
    }

    return ret;
}

//--------------------------------------------------------------------------------------
/* bool SandRTimer( unsigned int nIDEvent, bool bEnabled )
{
    bool ret = false;

    IT i( GetTimer( nIDEvent ) );
    if( i != GetTimers().end() )
    {
        H_TIMER &HTimer = (*i);
        HTimer.bEnabled = bEnabled;
        HTimer.fCountdown = HTimer.fTimeoutInSecs; // 重置時間
        ret = true;
    }

    return ret;
} */

//--------------------------------------------------------------------------------------
// TODO: 之後可改為使用Algorithm的find
// return find( TimerList.begin(), TimerList.end(), nIDEvent );
//--------------------------------------------------------------------------------------
/* IT GetTimer( unsigned int nIDEvent )
{
    TIMERS &TimerList = GetTimers();

    IT ret( TimerList.end() );

    CIT end( TimerList.end() );
    for( IT i(TimerList.begin()); i!=end; ++i )
    {
        H_TIMER &HTimer = (*i);
        if( HTimer.nID == nIDEvent )
        {
            ret = i;
            break;
        }
    }

    return ret;
} */

//--------------------------------------------------------------------------------------
// 判斷Timer存不存在
//--------------------------------------------------------------------------------------
/* bool FindTimer( unsigned int nIDEvent )
{
    bool ret = false;

    IT i( GetTimer(nIDEvent) );
    if( FindTimer( i ) )
        ret = true;

    return ret;
} */

//--------------------------------------------------------------------------------------
// 以下是傳入的回呼函式，先把回呼函式轉成ID即可
//--------------------------------------------------------------------------------------
/* void KillTimer( LPCALLBACKTIMER pCallbackTimer )
{
    KillTimer( GetTimerID(pCallbackTimer) );
}
bool SetTimerEnabled( LPCALLBACKTIMER pCallbackTimer, bool bEnabled )
{
    return SetTimerEnabled( GetTimerID(pCallbackTimer), bEnabled );
}
bool GetTimerEnabled( LPCALLBACKTIMER pCallbackTimer )
{
    return GetTimerEnabled( GetTimerID(pCallbackTimer) );
}
bool SetTimeoutInSecs( LPCALLBACKTIMER pCallbackTimer, float fTimeoutInSecs )
{
    return SetTimeoutInSecs( GetTimerID(pCallbackTimer), fTimeoutInSecs );
}
bool StartTimer( LPCALLBACKTIMER pCallbackTimer )
{
    return StartTimer( GetTimerID(pCallbackTimer) );
}
bool StopTimer( LPCALLBACKTIMER pCallbackTimer )
{
    return StopTimer( GetTimerID(pCallbackTimer) );
}
bool ResetTimer( LPCALLBACKTIMER pCallbackTimer )
{ // 重置計算時間
    return ResetTimer( GetTimerID(pCallbackTimer) );
}
bool SandRTimer( LPCALLBACKTIMER pCallbackTimer, bool bEnabled )
{ // Start or Stop and Reset 開始或停止並重置計算時間
    return SandRTimer( GetTimerID(pCallbackTimer), bEnabled );
}
bool StartRTimer( LPCALLBACKTIMER pCallbackTimer )
{
    return StartRTimer( GetTimerID(pCallbackTimer) );
}
bool StopRTimer( LPCALLBACKTIMER pCallbackTimer )
{
    return StopRTimer( GetTimerID(pCallbackTimer) );
} */
