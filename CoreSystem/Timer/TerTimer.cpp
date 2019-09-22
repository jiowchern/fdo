//--------------------------------------------------------------------------------------
// File: TerTimer.cpp
//
// Desc:
//
// Copyright (c) Dynasty Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "stdafx.h"

#include "HTimer.h"
#include <time.h> // for time();
#include <memory>
//#include <list>
#include <vector>
#include <algorithm>
// #include <cassert>
// #include <assert.h>
using namespace std;

#pragma hdrstop
#include "TerTimer.h"

// int g_iServerTime = static_cast< int >( time(NULL) ); // 初始設定為系統時間
int g_iServerTime = 0;
// int g_iDiffTime = 0;


//--------------------------------------------------------------------------------------
// Internal functions forward declarations
//--------------------------------------------------------------------------------------
// void CALLBACK UpdateServerTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext );
void __stdcall UpdateServerTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext );
void __stdcall OnTerminalTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext );
//--------------------------------------------------------------------------------------
void SetServerTime( int iServerTime )
{ // 設定Server的時間，可以更新
    if( g_iServerTime == 0 )
        SetTimer( UpdateServerTime ); // 理論上只會執行一次

    g_iServerTime = iServerTime;

    /* static bool bSetTimer = false;
    if( bSetTimer == false )
    {
        SetTimer( UpdateServerTime ); // 理論上只會執行一次
        bSetTimer = true;
    } */

    // 計算本機時間和Server時間的差距
    // 可能產生的問題：設定了時間差距，再修改時間系統時間。
    // g_iDiffTime = static_cast< int >( difftime( iServerTime, time(NULL) ) );
}

//--------------------------------------------------------------------------------------
int GetServerTime()
{
    // 如果尚未設定Server時間，則先設定本地時間
    if( g_iServerTime == 0 )
        SetServerTime( time(NULL) );

    return g_iServerTime;
    // or...
    // 此方法不用每次更新時間
    // 但是可能產生的問題：設定了時間差距，再修改時間系統時間。
    // return time(NULL) + g_iDiffTime;
}

// void CALLBACK UpdateServerTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
void __stdcall UpdateServerTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{
    ++g_iServerTime; // 一秒更新一次
    g_iServerTime += static_cast< int >( fOveredTime );
    // or...
    // g_iServerTime += static_cast< int >( floor(fOveredTime) );

    // 方法1：每秒判斷一次
    /* IT pos = find( TerminalList.begin(), TerminalList.end(), g_iServerTime );
    if( IT != Terminal.end() )
    {
        Terminal.erase( pos ); // 只執行一次，所以刪除
        (*pos).pFunction(); // 執行
    } */
    // 方法2：自行設定Timer(目前使用這種方法)
}

//--------------------------------------------------------------------------------------
#pragma pack( push )
#pragma pack( 1 )
struct TerminalTimer
{
    TerminalTimer( LPCALLBACKTERTIMER pCallback, void* pUser )
    : pCallbackTerTimer( pCallback ), pUserContext( pUser ) {}

    // unsigned int nID;
    LPCALLBACKTERTIMER pCallbackTerTimer;
    void* pUserContext;

    // bool operator == (unsigned int ID) { return (nID==ID); }
    bool operator == ( void* ptr ) { return (this==ptr); }
};
#pragma pack( pop )

vector< TerminalTimer > TTimers;
typedef vector< TerminalTimer >::iterator IT;
// typedef list< TerminalTimer >::const_iterator CIT; // 改變其內容視為無效

//--------------------------------------------------------------------------------------
bool SetTerminalTimer( LPCALLBACKTERTIMER pCallbackTerTimer,int iTerminalTimeInSecs, void* pUserContext )
{
    if( NULL == pCallbackTerTimer )
        return false;

    // 重點是這行！要計算差值。但是依據的是Server的時間，不是本機的時間！
    int iTimeoutInSecs = iTerminalTimeInSecs - GetServerTime();
    if( iTimeoutInSecs <= 0 )
        return false; // 設定的終值時間不得小於Server時間

    TTimers.push_back( TerminalTimer(pCallbackTerTimer,pUserContext) ); // 增加一個
    TerminalTimer& TTimer = TTimers.back(); // 取得最後的
    SetTimer( OnTerminalTime, (float)iTimeoutInSecs, &TTimer ); // 設定回呼函式(internal)
    return true;
}

//--------------------------------------------------------------------------------------
void __stdcall OnTerminalTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{ // 執行觸發函式！
    KillTimer( nIDEvent ); // 這行是重點，因為只執行一次，所以在內部刪除此計時器

    TerminalTimer& TTimer = *static_cast< TerminalTimer* >( pUserContext );
    if( TTimer.pCallbackTerTimer )
        TTimer.pCallbackTerTimer( TTimer.pUserContext );
       
    // remove( TTimers.begin(), TTimers.end(), &TTimer ); // 不可行
    IT pos( find( TTimers.begin(), TTimers.end(), &TTimer ) );
    TTimers.erase( pos );

    /* 使用指標的方式
    TerminalTimer* TTimer = static_cast< TerminalTimer* >( pUserContext );
    if( TTimer && TTimer->pCallbackTerTimer )
        TTimer->pCallbackTerTimer( TTimer->pUserContext );
    IT pos( find( TTimers.begin(), TTimers.end(), TTimer ) );
    // if( pos != TTimers.end() ) 不需要做判斷
        TTimers.erase( pos );
    */
}

/*
void SetTerminalTimer( LPCALLBACKTERTIMER pCallbackTerTimer,int iTerminalTimeInSecs, void* pUserContext )
{
    // 如果尚未設定Server時間，則先設定本地時間
    if( GetServerTime() == 0 )
        SetServerTime( time(NULL) );

    // 重點是這行！要計算差值。但是依據的是Server的時間，不是本機的時間！
    float fTimeoutInSecs = static_cast< float >( iTerminalTimeInSecs - GetServerTime() );

    // 假如resize後比原本大，則以預設建構子增加
    // 增加的方式是使用insert到最後面，所以前面的值並不會改變
    // 以往認為是全部摧毀，然後全部重新建構是錯誤的觀念
    // 假如resize後比原本小，則從最後面開始剃除
    TTimers.resize( TTimers.size()+1 ); // 增加一個
    TerminalTimer& TTimer = TTimers.back(); // 取得最後的
    SetTimer( OnTerminalTime, fTimeoutInSecs, &TTimer );
    TTimer.nID = SetTimer( OnTerminalTime, fTimeoutInSecs, &TTimer.nID );
    TTimer.pCallbackTerTimer = pCallbackTerTimer;
    TTimer.pUserContext = pUserContext;
}

void __stdcall OnTerminalTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{ // 執行觸發函式！
    KillTimer( nIDEvent ); // 這行是重點，因為只執行一次，所以在內部刪除此計時器

    // nIDEvent 會等於 ID
    int ID = *static_cast< int* >( pUserContext );

    IT pos( find( TTimers.begin(), TTimers.end(), ID ) );
    if( pos != TTimers.end() )
    { // 找到！
        TerminalTimer& TTimer = (*pos);
        if( TTimer.pCallbackTerTimer )
            TTimer.pCallbackTerTimer( TTimer.pUserContext ); // 執行觸發函式！

        TTimers.erase( pos ); // 刪除
    }
}
*/

//--------------------------------------------------------------------------------------
// 以下是使用new的方法，不過會造成記憶體遺失
//--------------------------------------------------------------------------------------
/* void SetTerminalTimer( LPCALLBACKTERTIMER pCallbackTerTimer,int iTerminalTimeInSecs, void* pUserContext )
{
    // 如果尚未設定Server時間，則先設定本地時間
    if( GetServerTime() == 0 )
        SetServerTime( time(NULL) );

    TerminalTimer* pTTimer = new TerminalTimer(); // 在此處new出來的，但是不在此處delete！
    pTTimer->pCallbackTerTimer = pCallbackTerTimer;
    pTTimer->pUserContext = pUserContext;

    // 重點是這行！要計算差值。但是依據的是Server的時間，不是本機的時間！
    float fTimeoutInSecs = static_cast< float >( iTerminalTimeInSecs - GetServerTime() );

    // SetTimer( OnTerminalTime, fTimeoutInSecs, pTTimer );
    reinterpret_cast< void* >( &ID )
    SetTimer( OnTerminalTime, fTimeoutInSecs, ID );
}

void __stdcall OnTerminalTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{ // 執行觸發函式！
    KillTimer( nIDEvent ); // 這行是重點，因為只執行一次，所以在內部刪除此計時器

    TerminalTimer* ptr = static_cast< TerminalTimer* >( pUserContext );
    if( ptr )
    {
        auto_ptr< TerminalTimer > pTTimer( ptr ); // 重要！使用自動指標來接手
        if( pTTimer->pCallbackTerTimer )
            pTTimer->pCallbackTerTimer( pTTimer->pUserContext ); // 執行觸發函式！
    }
} */
