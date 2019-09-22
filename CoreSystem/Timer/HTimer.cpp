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
#pragma message("�޲z�Ҧ��Ұ�") 

#else
#pragma message("�@��Ҧ��Ұ�") 
#endif

////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
// Internal functions forward declarations
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
unsigned int GetTimerID( void* ptr )
{ // �b�o�̨M�wID���ͪ���k
    static unsigned int ID = 0;
    ++ID;
    return ID;

    // �ѫ��Ъ���m�ӨM�w(���Ъ���m�নINT)
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

    //bool operator == (unsigned int ID) { return (GetTimerID(this)==ID); } // ID��HTimer���O�����m�M�w
    unsigned int nID;
    bool operator == (unsigned int ID) { return (nID==ID); }
};
#pragma pack( pop )
typedef std::list< H_TIMER > TIMERS;
typedef TIMERS::iterator IT;
typedef TIMERS::const_iterator CIT; // ���ܨ䤺�e�����L��

//--------------------------------------------------------------------------------------
// �i�H������ TimerList �ŧi�̫e���Y�i
// �o�˰����n�B�O�G�ϥΨ�~�|���͡C
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
// �\�b�j��̤@����s
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
            { // �ɶ���IĲ�o�I
                LPCALLBACKTIMER pCallbackTimer = HTimer.pCallbackTimer;
                if( pCallbackTimer )
                    pCallbackTimer( HTimer.nID, -HTimer.fCountdown, HTimer.pCallbackUserContext ); // ������U�禡

                HTimer.fCountdown = HTimer.fTimeoutInSecs; // ���m�ɶ�
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
        if( HTimer.bEnabled ) // �p�G fTimeoutInSecs <= 0 �N��C�@����Ĳ�o
        {
            HTimer.fCountdown -= m_fElapsedTime;
            if( HTimer.fCountdown <= 0 )
            { // �ɶ���IĲ�o�I
                LPCALLBACKTIMER pCallbackTimer = HTimer.pCallbackTimer;
                if( pCallbackTimer ) // �o��i�H���ΧP�_
                    pCallbackTimer( HTimer.nID, -HTimer.fCountdown, HTimer.pUserContext ); // ������U�禡

                HTimer.fCountdown = HTimer.fTimeoutInSecs; // ���m�ɶ�
            }
        }
        return (HTimer.nID==0);
    }
};


//--------------------------------------------------------------------------------------
// �\�b�j��̤@����s
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
// �o�ܭ��n�A�]���i��|�o�ͦb�B�z�����R��Timer���ʧ@
// �ҥH�����b�B�z�e���R�����ʧ@
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
// 		//cosiann �����L�I �o�~�Oobserver
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
// �^�ǲ��ͪ�ID
//--------------------------------------------------------------------------------------
unsigned int SetTimer( LPCALLBACKTIMER pCallbackTimer, float fTimeoutInSecs, void* pUserContext )
{
    if( NULL == pCallbackTimer )
    { // TODO: ��^�I�禡���ū��Ъ��ɭԤ��W�[Timer(�W�[�S���N�q)
        return 0;
    }

    TIMERS& TimerList = GetTimers();
    TimerList.push_back( H_TIMER() ); // �W�[�@��
    // TimerList.resize( TimerList.size()+1 ); // �W�[�@��

    H_TIMER& HTimer = TimerList.back();
    HTimer.pCallbackTimer = pCallbackTimer;
    HTimer.pUserContext = pUserContext;
    HTimer.fTimeoutInSecs = fTimeoutInSecs;
    HTimer.fCountdown = fTimeoutInSecs;
    HTimer.bEnabled = true;
    HTimer.nID = GetTimerID( &HTimer );

    return HTimer.nID;
    //return GetTimerID( &HTimer ); // ID��HTimer���O�����m�M�w
}

//--------------------------------------------------------------------------------------
// �S���άO���ৡ�Ǧ^ false
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
// TODO: remove �L�@�ΡAwhy�H
// remove( GetTimers().begin(), GetTimers().end(), nIDEvent )

//--------------------------------------------------------------------------------------
// ���s�p��
//--------------------------------------------------------------------------------------
bool ResetTimer( unsigned int nIDEvent )
{
    bool ret = false;

    IT i( GetTimer( nIDEvent ) );
    if( i != GetTimers().end() )
    {
        H_TIMER &HTimer = (*i);
        HTimer.fCountdown = HTimer.fTimeoutInSecs; // ���m�ɶ�
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
        HTimer.fCountdown = HTimer.fTimeoutInSecs; // ���m�ɶ�
        HTimer.bEnabled = false;
        ret = true;
    }

    return ret;
}

//--------------------------------------------------------------------------------------
// �]�w�s���˼Ʈɶ�
//--------------------------------------------------------------------------------------
bool SetTimeoutInSecs( unsigned int nIDEvent, float fTimeoutInSecs )
{
    bool ret = false;

    IT i( GetTimer( nIDEvent ) );
    if( i != GetTimers().end() )
    {
        H_TIMER &HTimer = (*i);
        HTimer.fTimeoutInSecs = fTimeoutInSecs; // ���m�ɶ�
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
        HTimer.fCountdown = HTimer.fTimeoutInSecs; // ���m�ɶ�
        ret = true;
    }

    return ret;
} */

//--------------------------------------------------------------------------------------
// TODO: ����i�אּ�ϥ�Algorithm��find
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
// �P�_Timer�s���s�b
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
// �H�U�O�ǤJ���^�I�禡�A����^�I�禡�নID�Y�i
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
{ // ���m�p��ɶ�
    return ResetTimer( GetTimerID(pCallbackTimer) );
}
bool SandRTimer( LPCALLBACKTIMER pCallbackTimer, bool bEnabled )
{ // Start or Stop and Reset �}�l�ΰ���í��m�p��ɶ�
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
