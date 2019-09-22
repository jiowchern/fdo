//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "FDO_GM.h"

namespace FDO
{

//-----------------------------------------------------------------------------
FDO_GM::FDO_GM()
{
    m_hDLL = NULL;
}

//-----------------------------------------------------------------------------
FDO_GM::~FDO_GM()
{
    if( m_hDLL != NULL )
    {
        ::FreeLibrary( m_hDLL ); //從記憶體中釋放DLL
        m_hDLL = NULL;
    }
}

//-----------------------------------------------------------------------------
bool FDO_GM::InitDLL()
{
    m_hDLL = ::LoadLibrary( L"GM.dll" ); // 載入DLL到行程的記憶體內

    if( m_hDLL == NULL )
    {
        //::MessageBox( NULL, L"Error! Could not found GM.dll", L"Error", NULL );
        return false;
    }

    // 取得所需函式的函式指標
    Create              = (__Create) ::GetProcAddress( m_hDLL, "Create" );
    Destroy             = (__Destroy) ::GetProcAddress( m_hDLL, "Destroy" );
    Login               = (__Login) ::GetProcAddress( m_hDLL, "Login" );
    RecvData            = (__RecvData) ::GetProcAddress( m_hDLL, "RecvData" );
    GetDataCount        = (__GetDataCount) ::GetProcAddress( m_hDLL, "GetDataCount" );
    GetData             = (__GetData) ::GetProcAddress( m_hDLL, "GetData" );
    FreeData            = (__FreeData) ::GetProcAddress( m_hDLL, "FreeData" );
    GetServerVersion    = (__GetServerVersion) ::GetProcAddress( m_hDLL, "GetServerVersion" );

    return true;
}
//----------------------------------------------------------------------------

} //namespace FDO





