//--------------------------------------------------------------------------------------
// File: TerTimer.h
//
// Desc: �]�w�׭Ȯɶ��|Ĳ�o���禡�A�u�|Ĳ�o�@��
//
// Copyright (c) Dynasty Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef TER_TIMER_H
#define TER_TIMER_H
//--------------------------------------------------------------------------------------
typedef void (__stdcall *LPCALLBACKTERTIMER)( void* pUserContext );
// typedef void (CALLBACK *LPCALLBACKTERTIMER)( void* pUserContext ); // �ϥ�CALLBACK
//void __stdcall OnTerminal( void* pUserContext ); // �ŧi�d��
//void CALLBACK OnTerminal( void* pUserContext ); // �ŧi�d��

void SetServerTime( int iServerTime ); // �]�wServer�ɶ�
int  GetServerTime(); // ���oServer�ɶ�

// �]�w�׭Ȯɶ��H�η|Ĳ�o���禡
bool SetTerminalTimer( LPCALLBACKTERTIMER pCallbackTerTimer,int iTerminalTimeInSecs, void* pUserContext=0 );
//--------------------------------------------------------------------------------------
#endif // end of TER_TIMER_H