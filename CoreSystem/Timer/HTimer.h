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
//void __stdcall OnTimer( unsigned int nIDEvent, float fOveredTime, void* pUserContext ); // �ŧi�d��

void HandleTimers( float fElapsedTime ); // �\�b�j��̤@����s

// �]�w�@�շs���p�ɾ��A�Ǧ^���ƭȬ����U�禡���O�����m�A�ҥH�@�w�n�ϥ�unsigned int�ӱ�
unsigned int SetTimer( LPCALLBACKTIMER pCallbackTimer, float fTimeoutInSecs=1.0f, void* pUserContext=0 );

//--------------------------------------------------------------------------------------
// �H�U�O�ǤJ���ѼƬ�ID
//--------------------------------------------------------------------------------------
bool KillTimer( unsigned int nIDEvent ); // �R���p�ɾ�
bool SetTimeoutInSecs( unsigned int nIDEvent, float fTimeoutInSecs ); // �]�w�s��TimeOut�ɶ�

bool SetTimerEnabled( unsigned int nIDEvent, bool bEnabled ); // �]�w�p�ɾ�����B����
bool GetTimerEnabled( unsigned int nIDEvent );

static bool StartTimer( unsigned int nIDEvent ) { return SetTimerEnabled( nIDEvent, true ); } // �Ұʭp�ɾ�(�w�])
static bool PauseTimer( unsigned int nIDEvent ) { return SetTimerEnabled( nIDEvent, false ); } // �Ȱ��p�ɾ�
bool ResetTimer( unsigned int nIDEvent ); // ���m�p��ɶ�
bool StopTimer( unsigned int nIDEvent ); // ���m�ð���p�ɾ�
// Start or Stop and Reset �}�l�ΰ���í��m�p��ɶ�
// bool SandRTimer( unsigned int nIDEvent, bool bEnabled );
// bool StartRTimer( unsigned int nIDEvent ) { return SandRTimer( nIDEvent, true ); }
// bool StopRTimer( unsigned int nIDEvent ) { return SandRTimer( nIDEvent, false ); }

//--------------------------------------------------------------------------------------
// �H�U�O�ǤJ���ѼƬ�CALLBACK
//--------------------------------------------------------------------------------------
/*
unsigned int GetTimerID( LPCALLBACKTIMER pCallbackTimer ); // ���o���U�^�I�禡��ID
void KillTimer( LPCALLBACKTIMER pCallbackTimer ); // �R���p�ɾ�
bool SetTimeoutInSecs( LPCALLBACKTIMER pCallbackTimer, float fTimeoutInSecs ); // �]�w�s��TimeOut�ɶ�

bool SetTimerEnabled( LPCALLBACKTIMER pCallbackTimer, bool bEnabled );
bool GetTimerEnabled( LPCALLBACKTIMER pCallbackTimer );

bool StartTimer( LPCALLBACKTIMER pCallbackTimer );
bool StopTimer( LPCALLBACKTIMER pCallbackTimer );
bool ResetTimer( LPCALLBACKTIMER pCallbackTimer ); // ���m�p��ɶ�
// Start or Stop and Reset �}�l�ΰ���í��m�p��ɶ�
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
// �ɶ���|�۰ʰ���o��
void CALLBACK TestClass::StaticOnTimer( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{
    // TestClass *pTest = (TestClass*)pUserContext;
    TestClass *pTest = static_cast< TestClass* >(pUserContext);
    if( pTest )
        pTest->OnTimer( nIDEvent, fOveredTime );
}

void TestClass::OnTimer( unsigned int nIDEvent, float fOveredTime )
{
    // �A�n�����ơK
}

int main()
{
    TestClass MyClass;
    // MyClass.ID = SetTimer( MyClass.StaticOnTimer, 1.0f, (void*)&MyClass );
    MyClass.ID = SetTimer( MyClass.StaticOnTimer, 1.0f, static_cast< void* >(&MyClass) );

    return 0;
}
*/