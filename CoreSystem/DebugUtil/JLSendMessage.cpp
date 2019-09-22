#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "JLSendMessage.h"

HWND g_hReceiveWin = NULL;

namespace FDO
{

//---------------------------------------------------------------------------
bool FindWindowHandle( const TCHAR* szWinName )
{
    g_hReceiveWin = FindWindow( NULL, szWinName );

    if( g_hReceiveWin )
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
void SendMessageToWindow( const std::string& strMessage )
{
    if( g_hReceiveWin )
    {
        COPYDATASTRUCT cpData;
        cpData.dwData = NULL;
        cpData.cbData = (DWORD)strMessage.size();
        cpData.lpData = (PVOID)strMessage.c_str();
        SendMessage( g_hReceiveWin, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cpData );
    }
}
//---------------------------------------------------------------------------
void SendMessageToWindow( const int& iMsg1, const int& iMsg2 )
{
    if( g_hReceiveWin )
    {
        SendMessage( g_hReceiveWin, WM_INTMESSAGE, (WPARAM)iMsg1, (LPARAM)iMsg2 );
    }
}
//---------------------------------------------------------------------------
void SendPacketMessage( const std::string& strMessage )
{
    if( g_hReceiveWin )
    {
        COPYDATASTRUCT cpData;
        cpData.dwData = NULL;
        cpData.cbData = (DWORD)strMessage.size();
        cpData.lpData = (PVOID)strMessage.c_str();
        SendMessage( g_hReceiveWin, WM_COPYDATA, (WPARAM)1, (LPARAM)&cpData );
    }
}

//---------------------------------------------------------------------------
void SendPacketMessageHV( const std::wstring& strMessage )
{
    if( g_hReceiveWin )
    {
        COPYDATASTRUCT cpData;
        cpData.dwData = NULL;
        cpData.cbData = (DWORD)300;
        cpData.lpData = (PVOID)strMessage.c_str();
        SendMessage( g_hReceiveWin, WM_COPYDATA, (WPARAM)3, (LPARAM)&cpData );
    }
}
//---------------------------------------------------------------------------
void SendDebugMessage( const char* pPara1, const char* pPara2, const char* pPara3, const char* pPara4 )
{
    if( g_hReceiveWin )
    {
        std::string strMessage( pPara1 );
        strMessage += " : ";

        if( pPara2 )
            strMessage += pPara2;

        if( pPara3 )
        {
            strMessage += ",   ";
            strMessage += pPara3;
            strMessage += " : ";
        }

        if( pPara4 )
            strMessage += pPara4;

        COPYDATASTRUCT cpData;
        cpData.dwData = NULL;
        cpData.cbData = (DWORD)strMessage.size();
        cpData.lpData = (PVOID)strMessage.c_str();
        SendMessage( g_hReceiveWin, WM_COPYDATA, (WPARAM)2, (LPARAM)&cpData );
    }
}
//---------------------------------------------------------------------------
void SendDebugMessage( const char* pPara1, int iPara2, const char* pPara3, int iPara4 )
{
    if( g_hReceiveWin )
    {
        char szNum[12];
        std::string strMessage( pPara1 );
        strMessage += " : ";

        _itoa( iPara2, szNum, 10 );
        strMessage += szNum;

        if( pPara3 )
        {
            strMessage += ",   ";
            strMessage += pPara3;
            strMessage += " : ";
        }

        if( iPara4 )
        {
            _itoa( iPara4, szNum, 10 );
            strMessage += szNum;
        }

        COPYDATASTRUCT cpData;
        cpData.dwData = NULL;
        cpData.cbData = (DWORD)strMessage.size();
        cpData.lpData = (PVOID)strMessage.c_str();
        SendMessage( g_hReceiveWin, WM_COPYDATA, (WPARAM)2, (LPARAM)&cpData );
    }
}
//---------------------------------------------------------------------------
void SendDebugMessage( const char* pPara1, const char* pPara2, const char* pPara3, int iPara4 )
{
    if( g_hReceiveWin )
    {
        char szNum[12];
        std::string strMessage( pPara1 );
        strMessage += " : ";

        strMessage += pPara2;

        if( pPara3 )
        {
            strMessage += ",   ";
            strMessage += pPara3;
            strMessage += " : ";
        }

        if( iPara4 )
        {
            _itoa( iPara4, szNum, 10 );
            strMessage += szNum;
        }

        COPYDATASTRUCT cpData;
        cpData.dwData = NULL;
        cpData.cbData = (DWORD)strMessage.size();
        cpData.lpData = (PVOID)strMessage.c_str();
        SendMessage( g_hReceiveWin, WM_COPYDATA, (WPARAM)2, (LPARAM)&cpData );
    }
}
//---------------------------------------------------------------------------
void SendDebugMessageVN( const WCHAR* pPara1, const WCHAR* pPara2, const WCHAR* pPara3, const WCHAR* pPara4 )
{
    if( g_hReceiveWin )
    {
        std::wstring strMessage( pPara1 );
        strMessage += L" : ";

        if( pPara2 )
            strMessage += pPara2;

        if( pPara3 )
        {
            strMessage += L",   ";
            strMessage += pPara3;
            strMessage += L" : ";
        }

        if( pPara4 )
            strMessage += pPara4;

        COPYDATASTRUCT cpData;
        cpData.dwData = NULL;
        cpData.cbData = (DWORD)300;
        cpData.lpData = (PVOID)strMessage.c_str();
        SendMessage( g_hReceiveWin, WM_COPYDATA, (WPARAM)4, (LPARAM)&cpData );
    }
}
//---------------------------------------------------------------------------
void SendDebugMessageVN( const WCHAR* pPara1, int iPara2, const WCHAR* pPara3, int iPara4 )
{
    if( g_hReceiveWin )
    {
        char szNum[12];
        std::wstring strMessage( pPara1 );
        strMessage += L" : ";

        _itoa( iPara2, szNum, 10 );
        strMessage += ANSItoUNICODE( szNum );

        if( pPara3 )
        {
            strMessage += L",   ";
            strMessage += pPara3;
            strMessage += L" : ";
        }

        if( iPara4 )
        {
            _itoa( iPara4, szNum, 10 );
            strMessage += ANSItoUNICODE( szNum );
        }

        COPYDATASTRUCT cpData;
        cpData.dwData = NULL;
        cpData.cbData = (DWORD)300;
        cpData.lpData = (PVOID)strMessage.c_str();
        SendMessage( g_hReceiveWin, WM_COPYDATA, (WPARAM)4, (LPARAM)&cpData );
    }
}
//---------------------------------------------------------------------------

} // namespace FDO






