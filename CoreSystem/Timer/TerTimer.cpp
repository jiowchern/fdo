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

// int g_iServerTime = static_cast< int >( time(NULL) ); // ��l�]�w���t�ήɶ�
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
{ // �]�wServer���ɶ��A�i�H��s
    if( g_iServerTime == 0 )
        SetTimer( UpdateServerTime ); // �z�פW�u�|����@��

    g_iServerTime = iServerTime;

    /* static bool bSetTimer = false;
    if( bSetTimer == false )
    {
        SetTimer( UpdateServerTime ); // �z�פW�u�|����@��
        bSetTimer = true;
    } */

    // �p�⥻���ɶ��MServer�ɶ����t�Z
    // �i�ಣ�ͪ����D�G�]�w�F�ɶ��t�Z�A�A�ק�ɶ��t�ήɶ��C
    // g_iDiffTime = static_cast< int >( difftime( iServerTime, time(NULL) ) );
}

//--------------------------------------------------------------------------------------
int GetServerTime()
{
    // �p�G�|���]�wServer�ɶ��A�h���]�w���a�ɶ�
    if( g_iServerTime == 0 )
        SetServerTime( time(NULL) );

    return g_iServerTime;
    // or...
    // ����k���ΨC����s�ɶ�
    // ���O�i�ಣ�ͪ����D�G�]�w�F�ɶ��t�Z�A�A�ק�ɶ��t�ήɶ��C
    // return time(NULL) + g_iDiffTime;
}

// void CALLBACK UpdateServerTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
void __stdcall UpdateServerTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{
    ++g_iServerTime; // �@���s�@��
    g_iServerTime += static_cast< int >( fOveredTime );
    // or...
    // g_iServerTime += static_cast< int >( floor(fOveredTime) );

    // ��k1�G�C��P�_�@��
    /* IT pos = find( TerminalList.begin(), TerminalList.end(), g_iServerTime );
    if( IT != Terminal.end() )
    {
        Terminal.erase( pos ); // �u����@���A�ҥH�R��
        (*pos).pFunction(); // ����
    } */
    // ��k2�G�ۦ�]�wTimer(�ثe�ϥγo�ؤ�k)
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
// typedef list< TerminalTimer >::const_iterator CIT; // ���ܨ䤺�e�����L��

//--------------------------------------------------------------------------------------
bool SetTerminalTimer( LPCALLBACKTERTIMER pCallbackTerTimer,int iTerminalTimeInSecs, void* pUserContext )
{
    if( NULL == pCallbackTerTimer )
        return false;

    // ���I�O�o��I�n�p��t�ȡC���O�̾ڪ��OServer���ɶ��A���O�������ɶ��I
    int iTimeoutInSecs = iTerminalTimeInSecs - GetServerTime();
    if( iTimeoutInSecs <= 0 )
        return false; // �]�w���׭Ȯɶ����o�p��Server�ɶ�

    TTimers.push_back( TerminalTimer(pCallbackTerTimer,pUserContext) ); // �W�[�@��
    TerminalTimer& TTimer = TTimers.back(); // ���o�̫᪺
    SetTimer( OnTerminalTime, (float)iTimeoutInSecs, &TTimer ); // �]�w�^�I�禡(internal)
    return true;
}

//--------------------------------------------------------------------------------------
void __stdcall OnTerminalTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{ // ����Ĳ�o�禡�I
    KillTimer( nIDEvent ); // �o��O���I�A�]���u����@���A�ҥH�b�����R�����p�ɾ�

    TerminalTimer& TTimer = *static_cast< TerminalTimer* >( pUserContext );
    if( TTimer.pCallbackTerTimer )
        TTimer.pCallbackTerTimer( TTimer.pUserContext );
       
    // remove( TTimers.begin(), TTimers.end(), &TTimer ); // ���i��
    IT pos( find( TTimers.begin(), TTimers.end(), &TTimer ) );
    TTimers.erase( pos );

    /* �ϥΫ��Ъ��覡
    TerminalTimer* TTimer = static_cast< TerminalTimer* >( pUserContext );
    if( TTimer && TTimer->pCallbackTerTimer )
        TTimer->pCallbackTerTimer( TTimer->pUserContext );
    IT pos( find( TTimers.begin(), TTimers.end(), TTimer ) );
    // if( pos != TTimers.end() ) ���ݭn���P�_
        TTimers.erase( pos );
    */
}

/*
void SetTerminalTimer( LPCALLBACKTERTIMER pCallbackTerTimer,int iTerminalTimeInSecs, void* pUserContext )
{
    // �p�G�|���]�wServer�ɶ��A�h���]�w���a�ɶ�
    if( GetServerTime() == 0 )
        SetServerTime( time(NULL) );

    // ���I�O�o��I�n�p��t�ȡC���O�̾ڪ��OServer���ɶ��A���O�������ɶ��I
    float fTimeoutInSecs = static_cast< float >( iTerminalTimeInSecs - GetServerTime() );

    // ���presize���쥻�j�A�h�H�w�]�غc�l�W�[
    // �W�[���覡�O�ϥ�insert��̫᭱�A�ҥH�e�����Ȩä��|����
    // �H���{���O�����R���A�M��������s�غc�O���~���[��
    // ���presize���쥻�p�A�h�q�̫᭱�}�l�c��
    TTimers.resize( TTimers.size()+1 ); // �W�[�@��
    TerminalTimer& TTimer = TTimers.back(); // ���o�̫᪺
    SetTimer( OnTerminalTime, fTimeoutInSecs, &TTimer );
    TTimer.nID = SetTimer( OnTerminalTime, fTimeoutInSecs, &TTimer.nID );
    TTimer.pCallbackTerTimer = pCallbackTerTimer;
    TTimer.pUserContext = pUserContext;
}

void __stdcall OnTerminalTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{ // ����Ĳ�o�禡�I
    KillTimer( nIDEvent ); // �o��O���I�A�]���u����@���A�ҥH�b�����R�����p�ɾ�

    // nIDEvent �|���� ID
    int ID = *static_cast< int* >( pUserContext );

    IT pos( find( TTimers.begin(), TTimers.end(), ID ) );
    if( pos != TTimers.end() )
    { // ���I
        TerminalTimer& TTimer = (*pos);
        if( TTimer.pCallbackTerTimer )
            TTimer.pCallbackTerTimer( TTimer.pUserContext ); // ����Ĳ�o�禡�I

        TTimers.erase( pos ); // �R��
    }
}
*/

//--------------------------------------------------------------------------------------
// �H�U�O�ϥ�new����k�A���L�|�y���O�����
//--------------------------------------------------------------------------------------
/* void SetTerminalTimer( LPCALLBACKTERTIMER pCallbackTerTimer,int iTerminalTimeInSecs, void* pUserContext )
{
    // �p�G�|���]�wServer�ɶ��A�h���]�w���a�ɶ�
    if( GetServerTime() == 0 )
        SetServerTime( time(NULL) );

    TerminalTimer* pTTimer = new TerminalTimer(); // �b���Bnew�X�Ӫ��A���O���b���Bdelete�I
    pTTimer->pCallbackTerTimer = pCallbackTerTimer;
    pTTimer->pUserContext = pUserContext;

    // ���I�O�o��I�n�p��t�ȡC���O�̾ڪ��OServer���ɶ��A���O�������ɶ��I
    float fTimeoutInSecs = static_cast< float >( iTerminalTimeInSecs - GetServerTime() );

    // SetTimer( OnTerminalTime, fTimeoutInSecs, pTTimer );
    reinterpret_cast< void* >( &ID )
    SetTimer( OnTerminalTime, fTimeoutInSecs, ID );
}

void __stdcall OnTerminalTime( unsigned int nIDEvent, float fOveredTime, void* pUserContext )
{ // ����Ĳ�o�禡�I
    KillTimer( nIDEvent ); // �o��O���I�A�]���u����@���A�ҥH�b�����R�����p�ɾ�

    TerminalTimer* ptr = static_cast< TerminalTimer* >( pUserContext );
    if( ptr )
    {
        auto_ptr< TerminalTimer > pTTimer( ptr ); // ���n�I�ϥΦ۰ʫ��Шӱ���
        if( pTTimer->pCallbackTerTimer )
            pTTimer->pCallbackTerTimer( pTTimer->pUserContext ); // ����Ĳ�o�禡�I
    }
} */
