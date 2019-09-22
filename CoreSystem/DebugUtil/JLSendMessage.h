//---------------------------------------------------------------------------
#ifndef _JLSendMessage_H_
#define _JLSendMessage_H_

#include <windows.h>
#include <string>

#define WM_INTMESSAGE (WM_APP+500)

#if defined FDO_DEBUG_MESSAGE
    #define SendEventMessage2( p1, p2 ) { SendDebugMessage( p1, p2 ); }
    #define SendEventMessage4( p1, p2, p3, p4 ) { SendDebugMessage( p1, p2, p3, p4 ); }
#else
    #define SendEventMessage2( p1, p2 ) {}
    #define SendEventMessage4( p1, p2, p3, p4 ) {}
#endif

//---------------------------------------------------------------------------

namespace FDO
{

bool FindWindowHandle( const TCHAR* szWinName );
void SendMessageToWindow( const std::string& strMessage );
void SendMessageToWindow( const int& iMsg1, const int& iMsg2 );
void SendPacketMessage( const std::string& strMessage );
void SendPacketMessageHV( const std::wstring& strMessage );
void SendDebugMessage( const char* pPara1, const char* pPara2 = NULL, const char* pPara3 = NULL, const char* pPara4 = NULL );
void SendDebugMessage( const char* pPara1, int iPara2, const char* pPara3 = NULL, int iPara4 = 0 );
void SendDebugMessage( const char* pPara1, const char* pPara2, const char* pPara3, int iPara4 );
void SendDebugMessageVN( const WCHAR* pPara1, const WCHAR* pPara2 = NULL, const WCHAR* pPara3 = NULL, const WCHAR* pPara4 = NULL );
void SendDebugMessageVN( const WCHAR* pPara1, const int iPara2 = NULL, const WCHAR* pPara3 = NULL, const int iPara4 = NULL );

} // namespace FDO

//---------------------------------------------------------------------------
#endif