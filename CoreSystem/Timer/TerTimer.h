//--------------------------------------------------------------------------------------
// File: TerTimer.h
//
// Desc: 設定終值時間會觸發的函式，只會觸發一次
//
// Copyright (c) Dynasty Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef TER_TIMER_H
#define TER_TIMER_H
//--------------------------------------------------------------------------------------
typedef void (__stdcall *LPCALLBACKTERTIMER)( void* pUserContext );
// typedef void (CALLBACK *LPCALLBACKTERTIMER)( void* pUserContext ); // 使用CALLBACK
//void __stdcall OnTerminal( void* pUserContext ); // 宣告範例
//void CALLBACK OnTerminal( void* pUserContext ); // 宣告範例

void SetServerTime( int iServerTime ); // 設定Server時間
int  GetServerTime(); // 取得Server時間

// 設定終值時間以及會觸發的函式
bool SetTerminalTimer( LPCALLBACKTERTIMER pCallbackTerTimer,int iTerminalTimeInSecs, void* pUserContext=0 );
//--------------------------------------------------------------------------------------
#endif // end of TER_TIMER_H