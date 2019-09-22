//
// CDXUTIMEEditBox.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
#include <strsafe.h>
#include "CDXUTTimer.h"
#include "FDOUT.h"
#include "PacketReader.h"
#include "JLAnimation2DManager.h"
//#include "HUISystemSet.h"
#include <cmath>
#include "J_SoundManager.h"
#include <stdio.h>
#include "iniReader.h"
#include "CFDOXML.h"
#include "DialogType.h"
#include "Common.h"
#pragma hdrstop
#include "CDXUTIMEEditBox.h"
#include "CDXUTDialog.h"

//==========================================================================================
// Definitions
//==========================================================================================

#define UNISCRIBE_DLLNAME L"\\usp10.dll"

#define GETPROCADDRESS( Module, APIName, Temp ) \
    Temp = GetProcAddress( Module, #APIName ); \
    if( Temp ) \
        *(FARPROC*)&_##APIName = Temp

#define PLACEHOLDERPROC( APIName ) \
    _##APIName = Dummy_##APIName

#define IMM32_DLLNAME L"\\imm32.dll"
#define VER_DLLNAME L"\\version.dll"

//--------------------------------------------------------------------------------------
// CDXUTIMEEditBox class
//--------------------------------------------------------------------------------------
// DXUT_MAX_EDITBOXLENGTH is the maximum string length allowed in edit boxes,
// including the NULL terminator.
// 
// Uniscribe does not support strings having bigger-than-16-bits length.
// This means that the string must be less than 65536 characters long,
// including the NULL terminator.
#define DXUT_MAX_EDITBOXLENGTH 0x00c8                    

// IME constants
#define CHT_IMEFILENAME1    "TINTLGNT.IME" // New Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // New Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

//--------------------------------------------------------------------------------------
// Input Method Versions
//--------------------------------------------------------------------------------------
#define MAKEIMEVERSION( major, minor )      ( (DWORD)( ( (BYTE)( major ) << 24 ) | ( (BYTE)( minor ) << 16 ) ) )
#define IMEID_CHT_VER42 ( LANG_CHT | MAKEIMEVERSION( 4, 2 ) )   // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 ( LANG_CHT | MAKEIMEVERSION( 4, 3 ) )   // New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 ( LANG_CHT | MAKEIMEVERSION( 4, 4 ) )   // New ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 ( LANG_CHT | MAKEIMEVERSION( 5, 0 ) )   // New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 ( LANG_CHT | MAKEIMEVERSION( 5, 1 ) )   // New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 ( LANG_CHT | MAKEIMEVERSION( 5, 2 ) )   // New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 ( LANG_CHT | MAKEIMEVERSION( 6, 0 ) )   // New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)
#define IMEID_CHS_VER41 ( LANG_CHS | MAKEIMEVERSION( 4, 1 ) )   // MSPY1.5  // SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42 ( LANG_CHS | MAKEIMEVERSION( 4, 2 ) )   // MSPY2    // Win2k/WinME
#define IMEID_CHS_VER53 ( LANG_CHS | MAKEIMEVERSION( 5, 3 ) )   // MSPY3    // WinXP

// Function pointers
INPUTCONTEXT* (WINAPI * CDXUTIMEEditBox::_ImmLockIMC)( HIMC ) = CDXUTIMEEditBox::Dummy_ImmLockIMC;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmUnlockIMC)( HIMC ) = CDXUTIMEEditBox::Dummy_ImmUnlockIMC;
LPVOID (WINAPI * CDXUTIMEEditBox::_ImmLockIMCC)( HIMCC ) = CDXUTIMEEditBox::Dummy_ImmLockIMCC;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmUnlockIMCC)( HIMCC ) = CDXUTIMEEditBox::Dummy_ImmUnlockIMCC;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmDisableTextFrameService)( DWORD ) = CDXUTIMEEditBox::Dummy_ImmDisableTextFrameService;
LONG (WINAPI * CDXUTIMEEditBox::_ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD ) = CDXUTIMEEditBox::Dummy_ImmGetCompositionStringW;
DWORD (WINAPI * CDXUTIMEEditBox::_ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD ) = CDXUTIMEEditBox::Dummy_ImmGetCandidateListW;
HIMC (WINAPI * CDXUTIMEEditBox::_ImmGetContext)( HWND ) = CDXUTIMEEditBox::Dummy_ImmGetContext;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmReleaseContext)( HWND, HIMC ) = CDXUTIMEEditBox::Dummy_ImmReleaseContext;
HIMC (WINAPI * CDXUTIMEEditBox::_ImmAssociateContext)( HWND, HIMC ) = CDXUTIMEEditBox::Dummy_ImmAssociateContext;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmGetOpenStatus)( HIMC ) = CDXUTIMEEditBox::Dummy_ImmGetOpenStatus;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmSetOpenStatus)( HIMC, BOOL ) = CDXUTIMEEditBox::Dummy_ImmSetOpenStatus;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD ) = CDXUTIMEEditBox::Dummy_ImmGetConversionStatus;
HWND (WINAPI * CDXUTIMEEditBox::_ImmGetDefaultIMEWnd)( HWND ) = CDXUTIMEEditBox::Dummy_ImmGetDefaultIMEWnd;
UINT (WINAPI * CDXUTIMEEditBox::_ImmGetIMEFileNameA)( HKL, LPSTR, UINT ) = CDXUTIMEEditBox::Dummy_ImmGetIMEFileNameA;
UINT (WINAPI * CDXUTIMEEditBox::_ImmGetVirtualKey)( HWND ) = CDXUTIMEEditBox::Dummy_ImmGetVirtualKey;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD ) = CDXUTIMEEditBox::Dummy_ImmNotifyIME;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmSetConversionStatus)( HIMC, DWORD, DWORD ) = CDXUTIMEEditBox::Dummy_ImmSetConversionStatus;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmSimulateHotKey)( HWND, DWORD ) = CDXUTIMEEditBox::Dummy_ImmSimulateHotKey;
BOOL (WINAPI * CDXUTIMEEditBox::_ImmIsIME)( HKL ) = CDXUTIMEEditBox::Dummy_ImmIsIME;
UINT (WINAPI * CDXUTIMEEditBox::_GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT ) = CDXUTIMEEditBox::Dummy_GetReadingString; // Traditional Chinese IME
BOOL (WINAPI * CDXUTIMEEditBox::_ShowReadingWindow)( HIMC, BOOL ) = CDXUTIMEEditBox::Dummy_ShowReadingWindow; // Traditional Chinese IME
BOOL (APIENTRY * CDXUTIMEEditBox::_VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT ) = CDXUTIMEEditBox::Dummy_VerQueryValueA;
BOOL (APIENTRY * CDXUTIMEEditBox::_GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID )= CDXUTIMEEditBox::Dummy_GetFileVersionInfoA;
DWORD (APIENTRY * CDXUTIMEEditBox::_GetFileVersionInfoSizeA)( LPSTR, LPDWORD ) = CDXUTIMEEditBox::Dummy_GetFileVersionInfoSizeA;
UINT (APIENTRY * CDXUTIMEEditBox::_ImmGetDescription)( HKL, LPTSTR, UINT ) = CDXUTIMEEditBox::Dummy_ImmGetDescription;

HINSTANCE CDXUTIMEEditBox::s_hDllImm32;      // IMM32 DLL handle
HINSTANCE CDXUTIMEEditBox::s_hDllVer;        // Version DLL handle
HKL       CDXUTIMEEditBox::s_hklCurrent;     // Current keyboard layout of the process
bool      CDXUTIMEEditBox::s_bVerticalCand;  // Indicates that the candidates are listed vertically
WCHAR     CDXUTIMEEditBox::s_aszIndicator[5][3] = // String to draw to indicate current input locale
            {
                L"En",
                L"\x7B80",
                L"\x7E41",
                L"\xAC00",
                L"\x3042",
            };
LPWSTR    CDXUTIMEEditBox::s_wszCurrIndicator = CDXUTIMEEditBox::s_aszIndicator[0];  // Points to an indicator string that corresponds to current input locale
bool      CDXUTIMEEditBox::s_bInsertOnType;     // Insert the character as soon as a key is pressed (Korean behavior)
HINSTANCE CDXUTIMEEditBox::s_hDllIme;           // Instance handle of the current IME module
HIMC      CDXUTIMEEditBox::s_hImcDef;           // Default input context
CDXUTIMEEditBox::IMESTATE  CDXUTIMEEditBox::s_ImeState = IMEUI_STATE_OFF;
bool      CDXUTIMEEditBox::s_bEnableImeSystem;  // Whether the IME system is active
POINT     CDXUTIMEEditBox::s_ptCompString;      // Composition string position. Updated every frame.
int       CDXUTIMEEditBox::s_nCompCaret;
int       CDXUTIMEEditBox::s_nFirstTargetConv;  // Index of the first target converted char in comp string.  If none, -1.
CUniBuffer CDXUTIMEEditBox::s_CompString = CUniBuffer( 0 );
BYTE      CDXUTIMEEditBox::s_abCompStringAttr[MAX_COMPSTRING_SIZE];
DWORD     CDXUTIMEEditBox::s_adwCompStringClause[MAX_COMPSTRING_SIZE];
WCHAR     CDXUTIMEEditBox::s_wszReadingString[32];
CDXUTIMEEditBox::CCandList CDXUTIMEEditBox::s_CandList;       // Data relevant to the candidate list
bool      CDXUTIMEEditBox::s_bShowReadingWindow; // Indicates whether reading window is visible
bool      CDXUTIMEEditBox::s_bHorizontalReading; // Indicates whether the reading window is vertical or horizontal
bool      CDXUTIMEEditBox::s_bChineseIME;
CGrowableArray< CDXUTIMEEditBox::CInputLocale > CDXUTIMEEditBox::s_Locale; // Array of loaded keyboard layout on system
#if defined(DEBUG) | defined(_DEBUG)
bool      CDXUTIMEEditBox::m_bIMEStaticMsgProcCalled = false;
#endif

//--------------------------------------------------------------------------------------
// Input Language Identifiers
//--------------------------------------------------------------------------------------
#define LANG_CHT      MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL) // 繁
#define LANG_CHS      MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)  // 簡

//--------------------------------------------------------------------------------------
// Input Locale Identifiers (HKL)
//--------------------------------------------------------------------------------------
// Input Locale Identifiers (HKL) - Traditional Chinese (CHT)
#define _HKL_CHT_PHONETIC      ((HKL)(INT_PTR)0xE0010404) // 注音﹝ㄅ﹞    - Phonetic
#define _HKL_CHT_CHANGJIE      ((HKL)(INT_PTR)0xE0020404) // 倉頡﹝倉﹞    - ChangJie
#define _HKL_CHT_QUICK         ((HKL)(INT_PTR)0xE0030404) // 速﹝速﹞      - Quick
#define _HKL_CHT_BIG5CODE      ((HKL)(INT_PTR)0xE0040404) // 內碼﹝內﹞    - Big5Code
#define _HKL_CHT_ARRAY         ((HKL)(INT_PTR)0xE0050404) // 行列﹝行﹞    - Array
#define _HKL_CHT_DAYI          ((HKL)(INT_PTR)0xE0060404) // 大易﹝易﹞    - DaYi
#define _HKL_CHT_UNICODE       ((HKL)(INT_PTR)0xE0070404) // Unicode﹝單﹞ - Unicode
#define _HKL_CHT_NEW_PHONETIC  ((HKL)(INT_PTR)0xE0080404) // 新注音﹝注﹞  - New Phonetic
#define _HKL_CHT_NEW_CHANGJIE  ((HKL)(INT_PTR)0xE0090404) // 新倉頡﹝倉﹞  - New ChangJie
#define _HKL_CHT_CHEWING       ((HKL)(INT_PTR)0xE0200404) // 新酷音﹝酷﹞  - Chewing
// Input Locale Identifiers (HKL) - Simplified Chinese (CHS)
#define _HKL_CHS_PINYIN        ((HKL)(INT_PTR)0xE00E0804) // Microsoft Pinyin IME (MSPY)﹝中﹞
#define _HKL_CHS_ZHENGMA       ((HKL)(INT_PTR)0xE0030804) // 鄭碼﹝鄭﹞ - ZhengMa
// Input Locale Identifiers (HKL) - Japanese (JPN)
#define _HKL_JPN_IMJP81        ((HKL)(INT_PTR)0xE0010411) // Japanese Input System (MS-IME2002) (imjp81.ime)
// Input Locale Identifiers (HKL) - Korean (KOR)
#define _HKL_KOR_IMEKR61       ((HKL)(INT_PTR)0xE0010412) // Korean Input System (IME 2000) (imekr61.ime)

//--------------------------------------------------------------------------------------
// Input Method Indicators
//--------------------------------------------------------------------------------------
struct HKL_Indicator {
    HKL m_HKL;
    LPCWSTR m_szIndicator;
    D3DCOLOR m_Color;
};

static HKL_Indicator _HKL_Indicator[] = {
    // Traditional Chinese
    { _HKL_CHT_PHONETIC,     L"ㄅ", FDO::MAZARINE }, // 注音﹝ㄅ﹞    - Phonetic
    { _HKL_CHT_CHANGJIE,     L"倉", FDO::MAZARINE }, // 倉頡﹝倉﹞    - ChangJie
    { _HKL_CHT_QUICK,        L"速", FDO::MAZARINE }, // 速﹝速﹞      - Quick
    { _HKL_CHT_BIG5CODE,     L"內", FDO::MAZARINE }, // 內碼﹝內﹞    - Big5Code
    { _HKL_CHT_ARRAY,        L"行", FDO::MAZARINE }, // 行列﹝行﹞    - Array
    { _HKL_CHT_DAYI,         L"易", FDO::MAZARINE }, // 大易﹝易﹞    - DaYi
    { _HKL_CHT_UNICODE,      L"單", FDO::MAZARINE }, // Unicode﹝單﹞ - Unicode
    { _HKL_CHT_NEW_PHONETIC, L"注", FDO::GREEN },    // 新注音﹝注﹞  - New Phonetic
    { _HKL_CHT_NEW_CHANGJIE, L"倉", FDO::MAZARINE }, // 新倉頡﹝倉﹞  - New ChangJie
    { _HKL_CHT_CHEWING,      L"酷", FDO::MAZARINE }, // 新酷音﹝酷﹞  - Chewing
    // Simplified Chinese
    { _HKL_CHS_PINYIN,       L"笢", FDO::MAZARINE }, // Microsoft Pinyin IME (MSPY)﹝中﹞
    { _HKL_CHS_ZHENGMA,      L"痑", FDO::MAZARINE }, // 鄭碼﹝鄭﹞ - ZhengMa
    // Japanese
    { _HKL_JPN_IMJP81,       L"日", FDO::MAZARINE }, // Japanese Input System (MS-IME2002) (imjp81.ime)
    // Korean
    { _HKL_KOR_IMEKR61,      L"韓", FDO::MAZARINE }, // Korean Input System (IME 2000) (imekr61.ime)
};

static bool GetInputMethodIndicator(HKL hkl, LPWSTR& pszOut)
{
    int len = sizeof(_HKL_Indicator) / sizeof(HKL_Indicator);
    for (int i = 0; i < len; i++) {
        if (_HKL_Indicator[i].m_HKL == hkl) {
            pszOut = (LPWSTR)_HKL_Indicator[i].m_szIndicator;
            return true;
        } // end if
    } // end for
    return false;
}

//--------------------------------------------------------------------------------------
CDXUTIMEEditBox::CDXUTIMEEditBox( CDXUTDialog *pDialog )
{
    CDXUTIMEEditBox::Initialize(); // ensure static vars are properly init'ed first

    m_Type = DXUT_CONTROL_IMEEDITBOX;
    m_pDialog = pDialog;

    s_bEnableImeSystem = true;
    m_nIndicatorWidth = 0;
    m_ReadingColor = D3DCOLOR_ARGB( 188, 255, 255, 255 );
    m_ReadingWinColor = D3DCOLOR_ARGB( 128, 0, 0, 0 );
    m_ReadingSelColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
    m_ReadingSelBkColor = D3DCOLOR_ARGB( 128, 80, 80, 80 );
    m_CandidateColor = D3DCOLOR_ARGB( 255, 200, 200, 200 );
    m_CandidateWinColor = D3DCOLOR_ARGB( 128, 0, 0, 0 );
    m_CandidateSelColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_CandidateSelBkColor = D3DCOLOR_ARGB( 128, 158, 158, 158 );
    m_CompColor = D3DCOLOR_ARGB( 255, 200, 200, 255 );
    m_CompWinColor = D3DCOLOR_ARGB( 198, 0, 0, 0 );
    m_CompCaretColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_CompTargetColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_CompTargetBkColor = D3DCOLOR_ARGB( 255, 150, 150, 150 );
    m_CompTargetNonColor = D3DCOLOR_ARGB( 255, 255, 255, 0 );
    m_CompTargetNonBkColor = D3DCOLOR_ARGB( 255, 150, 150, 150 );
    m_IndicatorImeColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_IndicatorEngColor = D3DCOLOR_ARGB( 255, 120, 120, 120 );
    m_IndicatorBkColor = D3DCOLOR_ARGB( 255, 180, 180, 180 );

    // ming add >> Indicator 的位移
    m_OffsetX = 0;
    m_OffsetY = 0;
    m_bIndicatorVisible = true;
    m_bShowCompUp = true;
    // ming add <<
}


//--------------------------------------------------------------------------------------
CDXUTIMEEditBox::~CDXUTIMEEditBox()
{
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::SendKey( BYTE nVirtKey )
{
    keybd_event( nVirtKey, 0, 0,               0 );
    keybd_event( nVirtKey, 0, KEYEVENTF_KEYUP, 0 );
}


//--------------------------------------------------------------------------------------
// Called by CDXUTResourceCache's OnCreateDevice.  This gives the class a
// chance to initialize its default input context associated with the app window.
HRESULT CDXUTIMEEditBox::StaticOnCreateDevice()
{
    // Save the default input context
    s_hImcDef = _ImmGetContext( FDO::DXUTGetHWND() );
    _ImmReleaseContext( FDO::DXUTGetHWND(), s_hImcDef );

    return S_OK;
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::UpdateRects()
{
    // Temporary adjust m_width so that CDXUTEditBox can compute
    // the correct rects for its rendering since we need to make space
    // for the indicator button
    int nWidth = m_width;
    m_width -= m_nIndicatorWidth + m_nBorder * 2; // Make room for the indicator button
    CDXUTEditBox::UpdateRects();

    // ming edit >>
    //m_width = nWidth;  // Restore
    swap( m_width, nWidth );
    // ming edit <<

    // Compute the indicator button rectangle
    SetRect( &m_rcIndicator, m_rcBoundingBox.right, m_rcBoundingBox.top, m_x + m_width, m_rcBoundingBox.bottom );
//    InflateRect( &m_rcIndicator, -m_nBorder, -m_nBorder );

    // ming edit >>
    //m_rcBoundingBox.right = m_rcBoundingBox.left + m_width;
    m_rcBoundingBox.right = m_rcBoundingBox.left + nWidth;
    // ming edit <<
}


//--------------------------------------------------------------------------------------
//  GetImeId( UINT uIndex )
//      returns 
//  returned value:
//  0: In the following cases
//      - Non Chinese IME input locale
//      - Older Chinese IME
//      - Other error cases
//
//  Othewise:
//      When uIndex is 0 (default)
//          bit 31-24:  Major version
//          bit 23-16:  Minor version
//          bit 15-0:   Language ID
//      When uIndex is 1
//          pVerFixedInfo->dwFileVersionLS
//
//  Use IMEID_VER and IMEID_LANG macro to extract version and language information.
//  

// We define the locale-invariant ID ourselves since it doesn't exist prior to WinXP
// For more information, see the CompareString() reference.
#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

DWORD CDXUTIMEEditBox::GetImeId( UINT uIndex )
{
    static HKL hklPrev = 0;
    static DWORD dwID[2] = { 0, 0 };  // Cache the result
    
    DWORD   dwVerSize;
    DWORD   dwVerHandle;
    LPVOID  lpVerBuffer;
    LPVOID  lpVerData;
    UINT    cbVerData;
    char    szTmp[1024];

    if( uIndex >= sizeof( dwID ) / sizeof( dwID[0] ) )
        return 0;

    if( hklPrev == s_hklCurrent )
        return dwID[uIndex];

    hklPrev = s_hklCurrent;  // Save for the next invocation

    // Check if we are using an older Chinese IME
    if( !( ( s_hklCurrent == _HKL_CHT_NEW_PHONETIC ) || 
           ( s_hklCurrent == _HKL_CHT_NEW_CHANGJIE ) || 
           ( s_hklCurrent == _HKL_CHS_PINYIN ) ) )
    {
        dwID[0] = dwID[1] = 0;
        return dwID[uIndex];
    }

    // Obtain the IME file name
    if ( !_ImmGetIMEFileNameA( s_hklCurrent, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
    {
        dwID[0] = dwID[1] = 0;
        return dwID[uIndex];
    }

    // Check for IME that doesn't implement reading string API
    if ( !_GetReadingString )
    {
        if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) )
        {
            dwID[0] = dwID[1] = 0;
            return dwID[uIndex];
        }
    }

    dwVerSize = _GetFileVersionInfoSizeA( szTmp, &dwVerHandle );
    if( dwVerSize )
    {
        lpVerBuffer = HeapAlloc( GetProcessHeap(), 0, dwVerSize );
        if( lpVerBuffer )
        {
            if( _GetFileVersionInfoA( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) )
            {
                if( _VerQueryValueA( lpVerBuffer, "\\", &lpVerData, &cbVerData ) )
                {
                    DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
                    dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
                    if( _GetReadingString
                        ||
                        ( GetLanguage() == LANG_CHT &&
                          ( dwVer == MAKEIMEVERSION(4, 2) || 
                            dwVer == MAKEIMEVERSION(4, 3) || 
                            dwVer == MAKEIMEVERSION(4, 4) || 
                            dwVer == MAKEIMEVERSION(5, 0) ||
                            dwVer == MAKEIMEVERSION(5, 1) ||
                            dwVer == MAKEIMEVERSION(5, 2) ||
                            dwVer == MAKEIMEVERSION(6, 0) ) )
                        ||
                        ( GetLanguage() == LANG_CHS &&
                          ( dwVer == MAKEIMEVERSION(4, 1) ||
                            dwVer == MAKEIMEVERSION(4, 2) ||
                            dwVer == MAKEIMEVERSION(5, 3) ) )
                      )
                    {
                        dwID[0] = dwVer | GetLanguage();
                        dwID[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
                    }
                }
            }
            HeapFree( GetProcessHeap(), 0, lpVerBuffer );
        }
    }

    return dwID[uIndex];
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::CheckInputLocale()
{
    static HKL hklPrev = 0;
    s_hklCurrent = GetKeyboardLayout( 0 );    
    
    if ( hklPrev == s_hklCurrent )
        return;

    hklPrev = s_hklCurrent;
    switch ( GetPrimaryLanguage() )
    {
        // Simplified Chinese
        case LANG_CHINESE:
            s_bVerticalCand = true;
            switch ( GetSubLanguage() )
            {
                case SUBLANG_CHINESE_SIMPLIFIED:
                    s_wszCurrIndicator = s_aszIndicator[INDICATOR_CHS];
                    s_bVerticalCand = GetImeId() == 0;
                    break;
                case SUBLANG_CHINESE_TRADITIONAL:
                    s_wszCurrIndicator = s_aszIndicator[INDICATOR_CHT];
                    break;
                default:    // unsupported sub-language
                    s_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
                    break;
            }
            break;
        // Korean
        case LANG_KOREAN:
            s_wszCurrIndicator = s_aszIndicator[INDICATOR_KOREAN];
            s_bVerticalCand = false;
            break;
        // Japanese
        case LANG_JAPANESE:
            s_wszCurrIndicator = s_aszIndicator[INDICATOR_JAPANESE];
            s_bVerticalCand = true;
            break;
        default:
            // A non-IME language.  Obtain the language abbreviation
            // and store it for rendering the indicator later.
            s_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
    }

    // If non-IME, use the language abbreviation.
    if( s_wszCurrIndicator == s_aszIndicator[INDICATOR_NON_IME] )
    {
        WCHAR wszLang[5];
        GetLocaleInfoW( MAKELCID( LOWORD( s_hklCurrent ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszLang, 5 );
        s_wszCurrIndicator[0] = wszLang[0];
        s_wszCurrIndicator[1] = towlower( wszLang[1] );
    }
    
    // 顯示輸入法名稱。
    GetInputMethodIndicator(s_hklCurrent, s_wszCurrIndicator);
}

//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::CheckToggleState()
{
    CheckInputLocale();
    bool bIme = _ImmIsIME( s_hklCurrent ) != 0;
    //s_bChineseIME = ( GetPrimaryLanguage() == LANG_CHINESE ) && bIme;
    //s_bChineseIME = bIme;

    HIMC hImc;
    if( NULL != ( hImc = _ImmGetContext( FDO::DXUTGetHWND() ) ) )
    {
        if( bIme )
        //if( s_bChineseIME )
        {
            DWORD dwConvMode, dwSentMode;
            _ImmGetConversionStatus( hImc, &dwConvMode, &dwSentMode );
            s_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
        }
        else
        {
            s_ImeState = ( bIme && _ImmGetOpenStatus( hImc ) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
        }
        _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );
    }
    else {
        s_ImeState = IMEUI_STATE_OFF;
    }
}


//--------------------------------------------------------------------------------------
// Enable/disable the entire IME system.  When disabled, the default IME handling
// kicks in.
void CDXUTIMEEditBox::EnableImeSystem( bool bEnable )
{
    s_bEnableImeSystem = bEnable;
}


//--------------------------------------------------------------------------------------
// Sets up IME-specific APIs for the IME edit controls.  This is called every time
// the input locale changes.
void CDXUTIMEEditBox::SetupImeApi()
{
    char szImeFile[MAX_PATH + 1];

    _GetReadingString = NULL;
    _ShowReadingWindow = NULL;
    if( _ImmGetIMEFileNameA( s_hklCurrent, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) == 0 )
        return;

    if( s_hDllIme ) FreeLibrary( s_hDllIme );
    s_hDllIme = LoadLibraryA( szImeFile );
    if ( !s_hDllIme )
        return;
    _GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
        ( GetProcAddress( s_hDllIme, "GetReadingString" ) );
    _ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
        ( GetProcAddress( s_hDllIme, "ShowReadingWindow" ) );
}


//--------------------------------------------------------------------------------------
// Resets the composition string.
void CDXUTIMEEditBox::ResetCompositionString()
{
    s_nCompCaret = 0;
    s_CompString.SetText( L"", 0 );
    ZeroMemory( s_abCompStringAttr, sizeof(s_abCompStringAttr) );
}


//--------------------------------------------------------------------------------------
// Truncate composition string by sending keystrokes to the window.
void CDXUTIMEEditBox::TruncateCompString( bool bUseBackSpace, int iNewStrLen )
{
    if( !s_bInsertOnType )
        return;

    int cc = (int) wcslen( s_CompString.GetBuffer() );
    assert( iNewStrLen == 0 || iNewStrLen >= cc );

    // Send right arrow keystrokes to move the caret
    //   to the end of the composition string.
    for (int i = 0; i < cc - s_nCompCaret; ++i )
        SendMessage( FDO::DXUTGetHWND(), WM_KEYDOWN, VK_RIGHT, 0 );
    SendMessage( FDO::DXUTGetHWND(), WM_KEYUP, VK_RIGHT, 0 );

    if( bUseBackSpace || m_bInsertMode )
        iNewStrLen = 0;

    // The caller sets bUseBackSpace to false if there's possibility of sending
    // new composition string to the app right after this function call.
    // 
    // If the app is in overwriting mode and new comp string is 
    // shorter than current one, delete previous comp string 
    // till it's same long as the new one. Then move caret to the beginning of comp string.
    // New comp string will overwrite old one.
    if( iNewStrLen < cc )
    {
        for( int i = 0; i < cc - iNewStrLen; ++i )
        {
            SendMessage( FDO::DXUTGetHWND(), WM_KEYDOWN, VK_BACK, 0 );  // Backspace character
            SendMessageW( FDO::DXUTGetHWND(), WM_CHAR, VK_BACK, 0 );
        }
        SendMessage( FDO::DXUTGetHWND(), WM_KEYUP, VK_BACK, 0 );
    }
    else
        iNewStrLen = cc;

    // Move the caret to the beginning by sending left keystrokes
    for (int i = 0; i < iNewStrLen; ++i )
        SendMessage( FDO::DXUTGetHWND(), WM_KEYDOWN, VK_LEFT, 0 );
    SendMessage( FDO::DXUTGetHWND(), WM_KEYUP, VK_LEFT, 0 );
}


//--------------------------------------------------------------------------------------
// Sends the current composition string to the application by sending keystroke
// messages.
void CDXUTIMEEditBox::SendCompString()
{
    for( int i = 0; i < lstrlen( s_CompString.GetBuffer() ); ++i )
        MsgProc( WM_CHAR, (WPARAM)s_CompString[i], 0 );
}


//--------------------------------------------------------------------------------------
// Outputs current composition string then cleans up the composition task.
void CDXUTIMEEditBox::FinalizeString( bool bSend )
{
    HIMC hImc;
    if( NULL == ( hImc = _ImmGetContext( FDO::DXUTGetHWND() ) ) )
        return;

    static bool bProcessing = false;
    if( bProcessing )    // avoid infinite recursion
    {
        DXUTTRACE( L"CDXUTIMEEditBox::FinalizeString: Reentrant detected!\n" );
        _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );
        return;
    }
    bProcessing = true;

    if( !s_bInsertOnType && bSend )
    {
        // Send composition string to app.
        LONG lLength = lstrlen( s_CompString.GetBuffer() );
        // In case of CHT IME, don't send the trailing double byte space, if it exists.
        if( GetLanguage() == LANG_CHT
            && s_CompString[lLength - 1] == 0x3000 )
        {
            s_CompString[lLength - 1] = 0;
        }
        SendCompString();
    }

    ResetCompositionString();
    // Clear composition string in IME
    _ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
    // the following line is necessary as Korean IME doesn't close cand list
    // when comp string is cancelled.
    _ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 ); 
    _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );
    bProcessing = false;
}


//--------------------------------------------------------------------------------------
// Determine whether the reading window should be vertical or horizontal.
void CDXUTIMEEditBox::GetReadingWindowOrientation( DWORD dwId )
{
    s_bHorizontalReading = ( s_hklCurrent == _HKL_CHS_PINYIN ) || ( s_hklCurrent == _HKL_CHT_NEW_CHANGJIE ) || ( dwId == 0 );
    if( !s_bHorizontalReading && ( dwId & 0x0000FFFF ) == LANG_CHT )
    {
        WCHAR wszRegPath[MAX_PATH];
        HKEY hKey;
        DWORD dwVer = dwId & 0xFFFF0000;
        StringCchCopy( wszRegPath, MAX_PATH, L"software\\microsoft\\windows\\currentversion\\" );
        StringCchCat( wszRegPath, MAX_PATH, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? L"MSTCIPH" : L"TINTLGNT" );
        LONG lRc = RegOpenKeyExW( HKEY_CURRENT_USER, wszRegPath, 0, KEY_READ, &hKey );
        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = RegQueryValueExW( hKey, L"Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
            if (lRc == ERROR_SUCCESS)
            {
                if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
                       ( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
                     ||
                     ( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
                       (BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
                   )
                {
                    s_bHorizontalReading = true;
                }
            }
            RegCloseKey( hKey );
        }
    }
}


//--------------------------------------------------------------------------------------
// Obtain the reading string upon WM_IME_NOTIFY/INM_PRIVATE notification.
void CDXUTIMEEditBox::GetPrivateReadingString()
{
    DWORD dwId = GetImeId();
    if( !dwId )
    {
        s_bShowReadingWindow = false;
        return;
    }

    HIMC hImc;
    hImc = _ImmGetContext( FDO::DXUTGetHWND() );
    if( !hImc )
    {
        s_bShowReadingWindow = false;
        return;
    }

    DWORD dwReadingStrLen = 0;
    DWORD dwErr = 0;
    WCHAR *pwszReadingStringBuffer = NULL;  // Buffer for when the IME supports GetReadingString()
    WCHAR *wstr = 0;
    bool bUnicodeIme = false;  // Whether the IME context component is Unicode.
    INPUTCONTEXT *lpIC = NULL;

    if( _GetReadingString )
    {
        UINT uMaxUiLen;
        BOOL bVertical;
        // Obtain the reading string size
        dwReadingStrLen = _GetReadingString( hImc, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen );
        if( dwReadingStrLen )
        {
            wstr = pwszReadingStringBuffer = (LPWSTR)HeapAlloc( GetProcessHeap(), 0, sizeof(WCHAR) * dwReadingStrLen );
            if( !pwszReadingStringBuffer )
            {
                // Out of memory. Exit.
                _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );
                return;
            }

            // Obtain the reading string
            dwReadingStrLen = _GetReadingString( hImc, dwReadingStrLen, wstr, (PINT)&dwErr, &bVertical, &uMaxUiLen );
        }

        s_bHorizontalReading = !bVertical;
        bUnicodeIme = true;
    }
    else
    {
        // IMEs that doesn't implement Reading String API

        lpIC = _ImmLockIMC( hImc );
        
        LPBYTE p = 0;
        switch( dwId )
        {
            case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
            case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
            case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 24 );
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
                dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
                wstr = (WCHAR *)( p + 56 );
                bUnicodeIme = true;
                break;

            case IMEID_CHT_VER50: // 5.0.x.x // WinME
                p = *(LPBYTE *)( (LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 3 * 4 );
                if( !p ) break;
                p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
                dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
                wstr = (WCHAR *)(p + 1*4 + (16*2+2*4) + 5*4);
                bUnicodeIme = false;
                break;

            case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
            case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
            case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 4);
                if( !p ) break;
                p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
                dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
                wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
                bUnicodeIme = true;
                break;

            // the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
            case IMEID_CHS_VER41:
            {
                int nOffset;
                nOffset = ( GetImeId( 1 ) >= 0x00000002 ) ? 8 : 7;

                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + nOffset * 4);
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 7*4 + 16*2*4);
                dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
                dwErr = __min( dwErr, dwReadingStrLen );
                wstr = (WCHAR *)(p + 6*4 + 16*2*1);
                bUnicodeIme = true;
                break;
            }

            case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
            {
                OSVERSIONINFOW osi;
                osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
                GetVersionExW( &osi );

                int nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(WCHAR) : sizeof(char);
                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
                dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
                wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
                bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
            }
        }   // switch
    }

    // Copy the reading string to the candidate list first
    s_CandList.awszCandidate[0][0] = 0;
    s_CandList.awszCandidate[1][0] = 0;
    s_CandList.awszCandidate[2][0] = 0;
    s_CandList.awszCandidate[3][0] = 0;
    s_CandList.dwCount = dwReadingStrLen;
    s_CandList.dwSelection = (DWORD)-1; // do not select any char
    if( bUnicodeIme )
    {
        UINT i;
        for( i = 0; i < dwReadingStrLen; ++i ) // dwlen > 0, if known IME
        {
            if( dwErr <= i && s_CandList.dwSelection == (DWORD)-1 )
            {
                // select error char
                s_CandList.dwSelection = i;
            }

            s_CandList.awszCandidate[i][0] = wstr[i];
            s_CandList.awszCandidate[i][1] = 0;
        }
        s_CandList.awszCandidate[i][0] = 0;
    }
    else
    {
        char *p = (char *)wstr;
        DWORD i, j;
        for( i = 0, j = 0; i < dwReadingStrLen; ++i, ++j ) // dwlen > 0, if known IME
        {
            if( dwErr <= i && s_CandList.dwSelection == (DWORD)-1 )
            {
                s_CandList.dwSelection = j;
            }
            // Obtain the current code page
            WCHAR wszCodePage[8];
            UINT uCodePage = CP_ACP;  // Default code page
            if( GetLocaleInfoW( MAKELCID( GetLanguage(), SORT_DEFAULT ),
                                LOCALE_IDEFAULTANSICODEPAGE,
                                wszCodePage,
                                sizeof(wszCodePage)/sizeof(wszCodePage[0]) ) )
            {
                uCodePage = wcstoul( wszCodePage, NULL, 0 );
            }
            MultiByteToWideChar( uCodePage, 0, p + i, IsDBCSLeadByteEx( uCodePage, p[i] ) ? 2 : 1,
                                 s_CandList.awszCandidate[j], 1 );
            if( IsDBCSLeadByteEx( uCodePage, p[i] ) )
                ++i;
        }
        s_CandList.awszCandidate[j][0] = 0;
        s_CandList.dwCount = j;
    }
    if( !_GetReadingString )
    {
        _ImmUnlockIMCC( lpIC->hPrivate );
        _ImmUnlockIMC( hImc );
        GetReadingWindowOrientation( dwId );
    }
    _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );

    if( pwszReadingStringBuffer )
        HeapFree( GetProcessHeap(), 0, pwszReadingStringBuffer );

    // Copy the string to the reading string buffer
    if( s_CandList.dwCount > 0 )
        s_bShowReadingWindow = true;
    else
        s_bShowReadingWindow = false;
    if( s_bHorizontalReading )
    {
        s_CandList.nReadingError = -1;
        s_wszReadingString[0] = 0;
        for( UINT i = 0; i < s_CandList.dwCount; ++i )
        {
            if( s_CandList.dwSelection == i )
                s_CandList.nReadingError = lstrlen( s_wszReadingString );
            StringCchCat( s_wszReadingString, 32, s_CandList.awszCandidate[i] );
        }
    }

    s_CandList.dwPageSize = MAX_CANDLIST;
}


//--------------------------------------------------------------------------------------
// This function is used only briefly in CHT IME handling,
// so accelerator isn't processed.
void CDXUTIMEEditBox::PumpMessage()
{
	Assert(0, "Not implement");
/*    MSG msg;

    while( PeekMessageW( &msg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        if( !GetMessageW( &msg, NULL, 0, 0 ) )
        {
            //PostQuitMessage( (int)msg.wParam );
            ExitGame((int)msg.wParam);
            return;
        }
        TranslateMessage( &msg );
        DispatchMessageA( &msg );
    }*/
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::OnFocusIn()
{
    CDXUTEditBox::OnFocusIn();
    
    if( s_bEnableImeSystem )
    {
        _ImmAssociateContext( FDO::DXUTGetHWND(), s_hImcDef );
        CheckToggleState();
    } else
        _ImmAssociateContext( FDO::DXUTGetHWND(), NULL );

    //
    // Set up the IME global state according to the current instance state
    //
    HIMC hImc;
    if( NULL != ( hImc = _ImmGetContext( FDO::DXUTGetHWND() ) ) ) 
    {
        if( !s_bEnableImeSystem )
            s_ImeState = IMEUI_STATE_OFF;

        _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );
        CheckToggleState();
    }
}

//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::OnFocusOut()
{
    CDXUTEditBox::OnFocusOut();
    FinalizeString( false );  // Don't send the comp string as to match RichEdit behavior
    _ImmAssociateContext( FDO::DXUTGetHWND(), NULL );
}

//--------------------------------------------------------------------------------------
bool CDXUTIMEEditBox::StaticMsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    HIMC hImc;

    if( !s_bEnableImeSystem )
        return false;

#if defined(DEBUG) | defined(_DEBUG)
    m_bIMEStaticMsgProcCalled = true;
#endif

    switch( uMsg )
    {
        case WM_ACTIVATEAPP:
            if( wParam )
            {
                // Populate s_Locale with the list of keyboard layouts.
                UINT cKL = GetKeyboardLayoutList( 0, NULL );
                s_Locale.RemoveAll();
                HKL *phKL = new HKL[cKL];
                if( phKL )
                {
                    GetKeyboardLayoutList( cKL, phKL );
                    for( UINT i = 0; i < cKL; ++i )
                    {
                        CInputLocale Locale;

                        // Filter out East Asian languages that are not IME.
                        if( ( PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_CHINESE ||
                              PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_JAPANESE ||
                              PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_KOREAN ) &&
                              !_ImmIsIME( phKL[i] ) )
                              continue;

                        // If this language is already in the list, don't add it again.
                        bool bBreak = false;
                        for( int e = 0; e < s_Locale.GetSize(); ++e )
                            if( LOWORD( s_Locale.GetAt( e ).m_hKL ) ==
                                LOWORD( phKL[i] ) )
                            {
                                bBreak = true;
                                break;
                            }
                        if( bBreak )
                            break;

                        Locale.m_hKL = phKL[i];
                        WCHAR wszDesc[128] = L"";
                        switch( PRIMARYLANGID( LOWORD( phKL[i] ) ) )
                        {
                            // Simplified Chinese
                            case LANG_CHINESE:
                                switch( SUBLANGID( LOWORD( phKL[i] ) ) )
                                {
                                    case SUBLANG_CHINESE_SIMPLIFIED:
                                        StringCchCopy( Locale.m_wszLangAbb, 3, s_aszIndicator[INDICATOR_CHS] );
                                        break;
                                    case SUBLANG_CHINESE_TRADITIONAL:
                                        StringCchCopy( Locale.m_wszLangAbb, 3, s_aszIndicator[INDICATOR_CHT] );
                                        break;
                                    default:    // unsupported sub-language
                                        GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
                                        Locale.m_wszLangAbb[0] = wszDesc[0];
                                        Locale.m_wszLangAbb[1] = towlower( wszDesc[1] );
                                        Locale.m_wszLangAbb[2] = L'\0';
                                        break;
                                }
                                break;
                            // Korean
                            case LANG_KOREAN:
                                StringCchCopy( Locale.m_wszLangAbb, 3, s_aszIndicator[INDICATOR_KOREAN] );
                                break;
                            // Japanese
                            case LANG_JAPANESE:
                                StringCchCopy( Locale.m_wszLangAbb, 3, s_aszIndicator[INDICATOR_JAPANESE] );
                                break;         
                            default:
                                // A non-IME language.  Obtain the language abbreviation
                                // and store it for rendering the indicator later.
                                GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
                                Locale.m_wszLangAbb[0] = wszDesc[0];
                                Locale.m_wszLangAbb[1] = towlower( wszDesc[1] );
                                Locale.m_wszLangAbb[2] = L'\0';
                                break;
                        }

                        GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SLANGUAGE, wszDesc, 128 );
                        StringCchCopy( Locale.m_wszLang, 64, wszDesc );

                        s_Locale.Add( Locale );
                    }
                    delete[] phKL;
                }
            }
            break;

        case WM_INPUTLANGCHANGE:
            DXUTTRACE( L"WM_INPUTLANGCHANGE\n" );
            {
                UINT uLang = GetPrimaryLanguage();
                CheckToggleState();
                if ( (int)uLang != GetPrimaryLanguage() )
                {
                    // Korean IME always inserts on keystroke.  Other IMEs do not.
                    s_bInsertOnType = ( GetPrimaryLanguage() == LANG_KOREAN );
                }

                // IME changed.  Setup the new IME.
                SetupImeApi();
                if( _ShowReadingWindow )
                {
                    if ( NULL != ( hImc = _ImmGetContext( FDO::DXUTGetHWND() ) ) )
                    {
                        _ShowReadingWindow( hImc, false );
                        _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );
                    }
                }
            }
            return true;

        case WM_IME_SETCONTEXT:
            DXUTTRACE( L"WM_IME_SETCONTEXT\n" );
            //
            // We don't want anything to display, so we have to clear this
            //
            lParam = 0;
            return false;

        // Handle WM_IME_STARTCOMPOSITION here since
        // we do not want the default IME handler to see
        // this when our fullscreen app is running.
        case WM_IME_STARTCOMPOSITION:
            DXUTTRACE( L"WM_IME_STARTCOMPOSITION\n" );
            ResetCompositionString();
            // Since the composition string has its own caret, we don't render
            // the edit control's own caret to avoid double carets on screen.
            s_bHideCaret = true;
            return true;

        case WM_IME_COMPOSITION:
            DXUTTRACE( L"WM_IME_COMPOSITION\n" );
            return true;
    }

    return false;
}


//--------------------------------------------------------------------------------------
bool CDXUTIMEEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
			DXUTFontNode* pFont = m_pDialog->GetFont( GetElement( 9 )->iFont );

            // Check if this click is on top of the composition string
            int nCompStrWidth;
            s_CompString.CPtoX( s_CompString.GetTextSize(), FALSE, &nCompStrWidth );

            if( s_ptCompString.x <= pt.x &&
                s_ptCompString.y <= pt.y &&
                s_ptCompString.x + nCompStrWidth > pt.x &&
                s_ptCompString.y + pFont->nHeight > pt.y )
            {
                int nCharBodyHit, nCharHit;
                int nTrail;

                //音效 *** modify
                PlaySoundWAV();  //under control class


                // Determine the character clicked on.
                s_CompString.XtoCP( pt.x - s_ptCompString.x, &nCharBodyHit, &nTrail );
                if( nTrail && nCharBodyHit < s_CompString.GetTextSize() )
                    nCharHit = nCharBodyHit + 1;
                else
                    nCharHit = nCharBodyHit;

                // Now generate keypress events to move the comp string cursor
                // to the click point.  First, if the candidate window is displayed,
                // send Esc to close it.
                HIMC hImc = _ImmGetContext( FDO::DXUTGetHWND() );
                if( !hImc )
                    return true;

                _ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 );
                _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );

                switch( GetPrimaryLanguage() )
                {
                    case LANG_JAPANESE:
                        // For Japanese, there are two cases.  If s_nFirstTargetConv is
                        // -1, the comp string hasn't been converted yet, and we use
                        // s_nCompCaret.  For any other value of s_nFirstTargetConv,
                        // the string has been converted, so we use clause information.

                        if( s_nFirstTargetConv != -1 )
                        {
                            int nClauseClicked = 0;
                            while( (int)s_adwCompStringClause[nClauseClicked + 1] <= nCharBodyHit )
                                ++nClauseClicked;

                            int nClauseSelected = 0;
                            while( (int)s_adwCompStringClause[nClauseSelected + 1] <= s_nFirstTargetConv )
                                ++nClauseSelected;

                            BYTE nVirtKey = nClauseClicked > nClauseSelected ? VK_RIGHT : VK_LEFT;
                            int nSendCount = abs( nClauseClicked - nClauseSelected );
                            while( nSendCount-- > 0 )
                                SendKey( nVirtKey );

                            return true;
                        }

                        // Not converted case. Fall thru to Chinese case.

                    case LANG_CHINESE:
                    {
                        // For Chinese, use s_nCompCaret.
                        BYTE nVirtKey = nCharHit > s_nCompCaret ? VK_RIGHT : VK_LEFT;
                        int nSendCount = abs( nCharHit - s_nCompCaret );
                        while( nSendCount-- > 0 )
                            SendKey( nVirtKey );
                        break;
                    }
                }

                return true;
            }

            // Check if the click is on top of the candidate window
            if( s_CandList.bShowWindow && PtInRect( &s_CandList.rcCandidate, pt ) )
            {
                if( s_bVerticalCand )
                {
                    // Vertical candidate window

                    // Compute the row the click is on
                    int nRow = ( pt.y - s_CandList.rcCandidate.top ) / pFont->nHeight;

                    if( nRow < (int)s_CandList.dwCount )
                    {
                        // nRow is a valid entry.
                        // Now emulate keystrokes to select the candidate at this row.
                        switch( GetPrimaryLanguage() )
                        {
                            case LANG_CHINESE:
                            case LANG_KOREAN:
                                // For Chinese and Korean, simply send the number keystroke.
                                SendKey( (BYTE) ('0' + nRow + 1) );
                                break;

                            case LANG_JAPANESE:
                                // For Japanese, move the selection to the target row,
                                // then send Right, then send Left.

                                BYTE nVirtKey;
                                if( nRow > (int)s_CandList.dwSelection )
                                    nVirtKey = VK_DOWN;
                                else
                                    nVirtKey = VK_UP;
                                int nNumToHit = abs( int( nRow - s_CandList.dwSelection ) );
                                for( int nStrike = 0; nStrike < nNumToHit; ++nStrike )
                                    SendKey( nVirtKey );

                                // Do this to close the candidate window without ending composition.
                                SendKey( VK_RIGHT );
                                SendKey( VK_LEFT );

                                break;
                        }
                    }
                } else
                {
                    // Horizontal candidate window

                    // Determine which the character the click has hit.
                    int nCharHit;
                    int nTrail;
                    s_CandList.HoriCand.XtoCP( pt.x - s_CandList.rcCandidate.left, &nCharHit, &nTrail );

                    // Determine which candidate string the character belongs to.
                    int nCandidate = s_CandList.dwCount - 1;

                    int nEntryStart = 0;
                    for( UINT i = 0; i < s_CandList.dwCount; ++i )
                    {
                        if( nCharHit >= nEntryStart )
                        {
                            // Haven't found it.
                            nEntryStart += lstrlenW( s_CandList.awszCandidate[i] ) + 1;  // plus space separator
                        } else
                        {
                            // Found it.  This entry starts at the right side of the click point,
                            // so the char belongs to the previous entry.
                            nCandidate = i - 1;
                            break;
                        }
                    }

                    // Now emulate keystrokes to select the candidate entry.
                    switch( GetPrimaryLanguage() )
                    {
                        case LANG_CHINESE:
                        case LANG_KOREAN:
                            // For Chinese and Korean, simply send the number keystroke.
                            SendKey( (BYTE) ('0' + nCandidate + 1) );
                            break;
                    }
                }

                return true;
            }
        }
    }

    // If we didn't care for the msg, let the parent process it.
    return CDXUTEditBox::HandleMouse( uMsg, pt, wParam, lParam );
}


//--------------------------------------------------------------------------------------
bool CDXUTIMEEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

#if defined(DEBUG) | defined(_DEBUG)
    // DXUT.cpp used to call CDXUTIMEEditBox::StaticMsgProc() so that, but now
    // this is the application's responsiblity.  To do this, call 
    // CDXUTDialogResourceManager::MsgProc() before calling this function.
    assert( m_bIMEStaticMsgProcCalled && L"To fix, call CDXUTDialogResourceManager::MsgProc() first" );
#endif

    bool trappedData;
    bool *trapped = &trappedData;

    HIMC hImc;
    static LPARAM lAlt = 0x80000000, lCtrl = 0x80000000, lShift = 0x80000000;

    *trapped = false;
    if( !s_bEnableImeSystem )
        return CDXUTEditBox::MsgProc( uMsg, wParam, lParam );

    switch( uMsg )
    {
        //
        //  IME Handling
        //
        case WM_IME_COMPOSITION:
            DXUTTRACE( L"WM_IME_COMPOSITION\n" );
            {
                LONG lRet;  // Returned count in CHARACTERS
                WCHAR wszCompStr[MAX_COMPSTRING_SIZE];

                *trapped = true;
                if( NULL == ( hImc = _ImmGetContext( FDO::DXUTGetHWND() ) ) )
                {
                    break;
                }

                // Get the caret position in composition string
                if ( lParam & GCS_CURSORPOS )
                {
                    s_nCompCaret = _ImmGetCompositionStringW( hImc, GCS_CURSORPOS, NULL, 0 );
                    if( s_nCompCaret < 0 )
                        s_nCompCaret = 0; // On error, set caret to pos 0.
                }

                // ResultStr must be processed before composition string.
                //
                // This is because for some IMEs, such as CHT, pressing Enter
                // to complete the composition sends WM_IME_COMPOSITION with both
                // GCS_RESULTSTR and GCS_COMPSTR.  Retrieving the result string
                // gives the correct string, while retrieving the comp string
                // (GCS_COMPSTR) gives empty string.  GCS_RESULTSTR should be
                // handled first so that the application receives the string.  Then
                // GCS_COMPSTR can be handled to clear the comp string buffer.

                if ( lParam & GCS_RESULTSTR )
                {
                    DXUTTRACE( L"  GCS_RESULTSTR\n" );
                    lRet = _ImmGetCompositionStringW( hImc, GCS_RESULTSTR, wszCompStr, sizeof( wszCompStr ) );
                    if( lRet > 0 )
                    {
                        lRet /= sizeof(WCHAR);
                        wszCompStr[lRet] = 0;  // Force terminate
                        TruncateCompString( false, (int)wcslen( wszCompStr ) );
                        s_CompString.SetText( wszCompStr, GetLanguage() );
                        SendCompString();
                        ResetCompositionString();
                    }
                }

                //
                // Reads in the composition string.
                //
                if ( lParam & GCS_COMPSTR )
                {
                    DXUTTRACE( L"  GCS_COMPSTR\n" );
                    //////////////////////////////////////////////////////
                    // Retrieve the latest user-selected IME candidates
                    lRet = _ImmGetCompositionStringW( hImc, GCS_COMPSTR, wszCompStr, sizeof( wszCompStr ) );
                    if( lRet > 0 )
                    {
                        lRet /= sizeof(WCHAR);  // Convert size in byte to size in char
                        wszCompStr[lRet] = 0;  // Force terminate
                        //
                        // Remove the whole of the string
                        //
                        TruncateCompString( false, (int)wcslen( wszCompStr ) );

                        s_CompString.SetText( wszCompStr, GetLanguage() );

                        // Older CHT IME uses composition string for reading string
                        if ( GetLanguage() == LANG_CHT && !GetImeId() )
                        {
                            if( lstrlen( s_CompString.GetBuffer() ) )
                            {
                                s_CandList.dwCount = 4;             // Maximum possible length for reading string is 4
                                s_CandList.dwSelection = (DWORD)-1; // don't select any candidate

                                // Copy the reading string to the candidate list
                                for( int i = 3; i >= 0; --i )
                                {
                                    if( i > lstrlen( s_CompString.GetBuffer() ) - 1 )
                                        s_CandList.awszCandidate[i][0] = 0;  // Doesn't exist
                                    else
                                    {
                                        s_CandList.awszCandidate[i][0] = s_CompString[i];
                                        s_CandList.awszCandidate[i][1] = 0;
                                    }
                                }
                                s_CandList.dwPageSize = MAX_CANDLIST;
                                // Clear comp string after we are done copying
                                ZeroMemory( (LPVOID)s_CompString.GetBuffer(), 4 * sizeof(WCHAR) );
                                s_bShowReadingWindow = true;
                                GetReadingWindowOrientation( 0 );
                                if( s_bHorizontalReading )
                                {
                                    s_CandList.nReadingError = -1;  // Clear error

                                    // Create a string that consists of the current
                                    // reading string.  Since horizontal reading window
                                    // is used, we take advantage of this by rendering
                                    // one string instead of several.
                                    //
                                    // Copy the reading string from the candidate list
                                    // to the reading string buffer.
                                    s_wszReadingString[0] = 0;
                                    for( UINT i = 0; i < s_CandList.dwCount; ++i )
                                    {
                                        if( s_CandList.dwSelection == i )
                                            s_CandList.nReadingError = lstrlen( s_wszReadingString );
                                        StringCchCat( s_wszReadingString, 32, s_CandList.awszCandidate[i] );
                                    }
                                }
                            }
                            else
                            {
                                s_CandList.dwCount = 0;
                                s_bShowReadingWindow = false;
                            }
                        }

                        if( s_bInsertOnType )
                        {
                            // Send composition string to the edit control
                            SendCompString();
                            // Restore the caret to the correct location.
                            // It's at the end right now, so compute the number
                            // of times left arrow should be pressed to
                            // send it to the original position.
                            int nCount = lstrlen( s_CompString.GetBuffer() + s_nCompCaret );
                            // Send left keystrokes
                            for( int i = 0; i < nCount; ++i )
                                SendMessage( FDO::DXUTGetHWND(), WM_KEYDOWN, VK_LEFT, 0 );
                            SendMessage( FDO::DXUTGetHWND(), WM_KEYUP, VK_LEFT, 0 );
                        }
                    }

                    ResetCaretBlink();
                }

                // Retrieve comp string attributes
                if( lParam & GCS_COMPATTR )
                {
                    lRet = _ImmGetCompositionStringW( hImc, GCS_COMPATTR, s_abCompStringAttr, sizeof( s_abCompStringAttr ) );
                    if( lRet > 0 )
                        s_abCompStringAttr[lRet] = 0;  // ??? Is this needed for attributes?
                }

                // Retrieve clause information
                if( lParam & GCS_COMPCLAUSE )
                {
                    lRet = _ImmGetCompositionStringW(hImc, GCS_COMPCLAUSE, s_adwCompStringClause, sizeof( s_adwCompStringClause ) );
                    s_adwCompStringClause[lRet / sizeof(DWORD)] = 0;  // Terminate
                }

                _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );
            }
            break;

        case WM_IME_ENDCOMPOSITION:
            DXUTTRACE( L"WM_IME_ENDCOMPOSITION\n" );
            TruncateCompString();
            ResetCompositionString();
            // We can show the edit control's caret again.
            s_bHideCaret = false;
            // Hide reading window
            s_bShowReadingWindow = false;
            break;

        case WM_IME_NOTIFY:
            DXUTTRACE( L"WM_IME_NOTIFY %u\n", wParam );
            switch( wParam )
            {
                case IMN_SETCONVERSIONMODE:
                    DXUTTRACE( L"  IMN_SETCONVERSIONMODE\n" );
                    CheckToggleState();
                    break;
                case IMN_SETOPENSTATUS:
                    DXUTTRACE( L"  IMN_SETOPENSTATUS\n" );
                    CheckToggleState();
                    break;

                case IMN_OPENCANDIDATE:
                case IMN_CHANGECANDIDATE:
                {
                    //DXUTTRACE( wParam == IMN_CHANGECANDIDATE ? L"  IMN_CHANGECANDIDATE\n" : L"  IMN_OPENCANDIDATE\n" ); ming mark

                    s_CandList.bShowWindow = true;
                    *trapped = true;
                    if( NULL == ( hImc = _ImmGetContext( FDO::DXUTGetHWND() ) ) )
                        break;

                    LPCANDIDATELIST lpCandList = NULL;
                    DWORD dwLenRequired;

                    s_bShowReadingWindow = false;
                    // Retrieve the candidate list
                    dwLenRequired = _ImmGetCandidateListW( hImc, 0, NULL, 0 );
                    if( dwLenRequired )
                    {
                        lpCandList = (LPCANDIDATELIST)HeapAlloc( GetProcessHeap(), 0, dwLenRequired );
                        dwLenRequired = _ImmGetCandidateListW( hImc, 0, lpCandList, dwLenRequired );
                    }

                    if( lpCandList )
                    {
                        // Update candidate list data
                        s_CandList.dwSelection = lpCandList->dwSelection;
                        s_CandList.dwCount = lpCandList->dwCount;

                        int nPageTopIndex = 0;
                        s_CandList.dwPageSize = __min( lpCandList->dwPageSize, MAX_CANDLIST );
                        if( GetPrimaryLanguage() == LANG_JAPANESE )
                        {
                            // Japanese IME organizes its candidate list a little
                            // differently from the other IMEs.
                            nPageTopIndex = ( s_CandList.dwSelection / s_CandList.dwPageSize ) * s_CandList.dwPageSize;
                        }
                        else
                            nPageTopIndex = lpCandList->dwPageStart;

                        // Make selection index relative to first entry of page
                        s_CandList.dwSelection = ( GetLanguage() == LANG_CHS && !GetImeId() ) ? (DWORD)-1
                                                 : s_CandList.dwSelection - nPageTopIndex;

                        ZeroMemory( s_CandList.awszCandidate, sizeof(s_CandList.awszCandidate) );
                        for( UINT i = nPageTopIndex, j = 0;
                            (DWORD)i < lpCandList->dwCount && j < s_CandList.dwPageSize;
                            i++, j++ )
                        {
                            // Initialize the candidate list strings
                            LPWSTR pwsz = s_CandList.awszCandidate[j];
                            // For every candidate string entry,
                            // write [index] + Space + [String] if vertical,
                            // write [index] + [String] + Space if horizontal.
                            *pwsz++ = (WCHAR)( L'0' + ( (j + 1) % 10 ) );  // Index displayed is 1 based
                            if( s_bVerticalCand )
                                *pwsz++ = L' ';
                            WCHAR *pwszNewCand = (LPWSTR)( (LPBYTE)lpCandList + lpCandList->dwOffset[i] );
                            while ( *pwszNewCand )
                                *pwsz++ = *pwszNewCand++;
                            if( !s_bVerticalCand )
                                *pwsz++ = L' ';
                            *pwsz = 0;  // Terminate
                        }

                        // Make dwCount in s_CandList be number of valid entries in the page.
                        s_CandList.dwCount = lpCandList->dwCount - lpCandList->dwPageStart;
                        if( s_CandList.dwCount > lpCandList->dwPageSize )
                            s_CandList.dwCount = lpCandList->dwPageSize;

                        HeapFree( GetProcessHeap(), 0, lpCandList );
                        _ImmReleaseContext( FDO::DXUTGetHWND(), hImc );

                        // Korean and old Chinese IME can't have selection.
                        // User must use the number hotkey or Enter to select
                        // a candidate.
                        if( GetPrimaryLanguage() == LANG_KOREAN ||
                            GetLanguage() == LANG_CHT && !GetImeId() )
                        {
                            s_CandList.dwSelection = (DWORD)-1;
                        }

                        // Initialize s_CandList.HoriCand if we have a
                        // horizontal candidate window.
                        if( !s_bVerticalCand )
                        {
                            WCHAR wszCand[MAX_COMPSTRING_SIZE] = L"";

                            s_CandList.nFirstSelected = 0;
                            s_CandList.nHoriSelectedLen = 0;
                            for( UINT i = 0; i < MAX_CANDLIST; ++i )
                            {
                                if( s_CandList.awszCandidate[i][0] == L'\0' )
                                    break;

                                WCHAR wszEntry[32];
                                StringCchPrintf( wszEntry, 32, L"%s ", s_CandList.awszCandidate[i] );
                                // If this is the selected entry, mark its char position.
                                if( s_CandList.dwSelection == i )
                                {
                                    s_CandList.nFirstSelected = lstrlen( wszCand );
                                    s_CandList.nHoriSelectedLen = lstrlen( wszEntry ) - 1;  // Minus space
                                }
                                StringCchCat( wszCand, MAX_COMPSTRING_SIZE, wszEntry );
                            }
                            wszCand[lstrlen(wszCand) - 1] = L'\0';  // Remove the last space
                            s_CandList.HoriCand.SetText( wszCand, 0 );
                        }
                    }
                    break;
                }

                case IMN_CLOSECANDIDATE:
                {
                    DXUTTRACE( L"  IMN_CLOSECANDIDATE\n" );
                    s_CandList.bShowWindow = false;
                    if( !s_bShowReadingWindow )
                    {
                        s_CandList.dwCount = 0;
                        ZeroMemory( s_CandList.awszCandidate, sizeof(s_CandList.awszCandidate) );
                    }
                    *trapped = true;
                    break;
                }

                case IMN_PRIVATE:
                    DXUTTRACE( L"  IMN_PRIVATE\n" );
                    {
                        if( !s_CandList.bShowWindow )
                            GetPrivateReadingString();

                        // Trap some messages to hide reading window
                        DWORD dwId = GetImeId();
                        switch( dwId )
                        {
                            case IMEID_CHT_VER42:
                            case IMEID_CHT_VER43:
                            case IMEID_CHT_VER44:
                            case IMEID_CHS_VER41:
                            case IMEID_CHS_VER42:
                                if( ( lParam == 1 ) || ( lParam == 2 ) )
                                {
                                    *trapped = true;
                                }
                                break;

                            case IMEID_CHT_VER50:
                            case IMEID_CHT_VER51:
                            case IMEID_CHT_VER52:
                            case IMEID_CHT_VER60:
                            case IMEID_CHS_VER53:
                                if( (lParam == 16) || (lParam == 17) || (lParam == 26) || (lParam == 27) || (lParam == 28) )
                                {
                                    *trapped = true;
                                }
                                break;
                        }
                    }
                    break;

                default:
                    *trapped = true;
                    break;
            }
            break;

        // When Text Service Framework is installed in Win2K, Alt+Shift and Ctrl+Shift combination (to switch input
        // locale / keyboard layout) doesn't send WM_KEYUP message for the key that is released first. We need to check
        // if these keys are actually up whenever we receive key up message for other keys.
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if ( !( lAlt & 0x80000000 ) && wParam != VK_MENU && ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) == 0 )
            {
                PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_MENU, ( lAlt & 0x01ff0000 ) | 0xC0000001 );
            }   
            else if ( !( lCtrl & 0x80000000 ) && wParam != VK_CONTROL && ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) == 0 )
            {
                PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_CONTROL, ( lCtrl & 0x01ff0000 ) | 0xC0000001 );
            }
            else if ( !( lShift & 0x80000000 ) && wParam != VK_SHIFT && ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) == 0 )
            {
                PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_SHIFT, ( lShift & 0x01ff0000 ) | 0xC0000001 );
            }
        // fall through WM_KEYDOWN / WM_SYSKEYDOWN
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
                
            if (true == CDXUTEditBox::HandleKeyboard(uMsg, wParam, lParam))
                return true;

            switch ( wParam )
            {
                case VK_MENU:
                    lAlt = lParam;
                    break;
                case VK_SHIFT:
                    lShift = lParam;
                    break;
                case VK_CONTROL:
                    lCtrl = lParam;
                    break;
            }
            //break;
            // Fall through to default case
            // so we invoke the parent.            
        default:
            // Let the parent handle the message that we
            // don't handle.
            return CDXUTEditBox::MsgProc( uMsg, wParam, lParam );

    }  // switch

    return *trapped;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTIMEEditBox::DrawText(LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, bool bShadow, int nCount)
{
    HRESULT hr = m_pDialog->DrawPlainText(strText, pElement, prcDest, bShadow, nCount);
    return hr;
}

//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::RenderCandidateReadingWindow( IDirect3DDevice9* pd3dDevice, float fElapsedTime, bool bReading )
{
    RECT rc;
    UINT nNumEntries = bReading ? 4 : MAX_CANDLIST;
    D3DCOLOR TextColor, TextBkColor, SelTextColor, SelBkColor;
    int nX, nXFirst, nXComp;
    m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
    m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

    if( bReading )
    {
        TextColor = m_ReadingColor;
        TextBkColor = m_ReadingWinColor;
        SelTextColor = m_ReadingSelColor;
        SelBkColor = m_ReadingSelBkColor;
    } else
    {
        TextColor = m_CandidateColor;
        TextBkColor = m_CandidateWinColor;
        SelTextColor = m_CandidateSelColor;
        SelBkColor = m_CandidateSelBkColor;
    }

    // For Japanese IME, align the window with the first target converted character.
    // For all other IMEs, align with the caret.  This is because the caret
    // does not move for Japanese IME.
    if ( GetLanguage() == LANG_CHT && !GetImeId() )
        nXComp = 0;
    else
    if( GetPrimaryLanguage() == LANG_JAPANESE )
        s_CompString.CPtoX( s_nFirstTargetConv, FALSE, &nXComp );
    else
        s_CompString.CPtoX( s_nCompCaret, FALSE, &nXComp );

    // Compute the size of the candidate window
    int nWidthRequired = 0;
    int nHeightRequired = 0;
    int nSingleLineHeight = 0;

    if( ( s_bVerticalCand && !bReading ) ||
        ( !s_bHorizontalReading && bReading ) )
    {
        // Vertical window
        for( UINT i = 0; i < nNumEntries; ++i )
        {
            if( s_CandList.awszCandidate[i][0] == L'\0' )
                break;
            SetRect( &rc, 0, 0, 0, 0 );
			m_pDialog->CalcTextRect( s_CandList.awszCandidate[i], GetElement( 1 ) , &rc );
            nWidthRequired = __max( nWidthRequired, rc.right - rc.left );
            nSingleLineHeight = __max( nSingleLineHeight, rc.bottom - rc.top );
        }
        nHeightRequired = nSingleLineHeight * nNumEntries;
    } else
    {
        // Horizontal window
        SetRect( &rc, 0, 0, 0, 0 );
        if( bReading )
			m_pDialog->CalcTextRect( s_wszReadingString, GetElement( 1 ) , &rc );
        else 
			m_pDialog->CalcTextRect( s_CandList.HoriCand.GetBuffer(), GetElement( 1 ) , &rc );
        nWidthRequired = rc.right - rc.left;
        nSingleLineHeight = nHeightRequired = rc.bottom - rc.top;
    }

    // Now that we have the dimension, calculate the location for the candidate window.
    // We attempt to fit the window in this order:
    // bottom, top, right, left.

    bool bHasPosition = false;

    // Bottom
    SetRect( &rc, s_ptCompString.x + nXComp, s_ptCompString.y + m_rcText.bottom - m_rcText.top,
                  s_ptCompString.x + nXComp + nWidthRequired, s_ptCompString.y + m_rcText.bottom - m_rcText.top + nHeightRequired );
    // if the right edge is cut off, move it left.
    if( rc.right > m_pDialog->GetWidth() )
    {
        rc.left -= rc.right - m_pDialog->GetWidth();
        rc.right = m_pDialog->GetWidth();
    }
    if( rc.bottom <= m_pDialog->GetHeight() )
        bHasPosition = true;

    // Top
    if( !bHasPosition )
    {
        SetRect( &rc, s_ptCompString.x + nXComp, s_ptCompString.y - nHeightRequired,
                      s_ptCompString.x + nXComp + nWidthRequired, s_ptCompString.y );
        // if the right edge is cut off, move it left.
        if( rc.right > m_pDialog->GetWidth() )
        {
            rc.left -= rc.right - m_pDialog->GetWidth();
            rc.right = m_pDialog->GetWidth();
        }
        if( rc.top >= 0 )
            bHasPosition = true;
    }

    // Right
    if( !bHasPosition )
    {
        int nXCompTrail= 0;
        // 底下這行會導致行列與大易輸入法出錯。
        // s_CompString.CPtoX( s_nCompCaret, TRUE, &nXCompTrail );
        SetRect( &rc, s_ptCompString.x + nXCompTrail, 0,
                      s_ptCompString.x + nXCompTrail + nWidthRequired, nHeightRequired );
        if( rc.right <= m_pDialog->GetWidth() )
            bHasPosition = true;
    }

    // Left
    if( !bHasPosition )
    {
        SetRect( &rc, s_ptCompString.x + nXComp - nWidthRequired, 0,
                      s_ptCompString.x + nXComp, nHeightRequired );
        if( rc.right >= 0 )
            bHasPosition = true;
    }

    if( !bHasPosition )
    {
        // The dialog is too small for the candidate window.
        // Fall back to render at 0, 0.  Some part of the window
        // will be cut off.
        rc.left = 0;
        rc.right = nWidthRequired;
    }

    // ming add >> 使畫字不超過範圍
    if( rc.bottom > m_pDialog->GetHeight() )
        OffsetRect( &rc, 0, m_pDialog->GetHeight()-rc.bottom );
    // ming add <<

    // If we are rendering the candidate window, save the position
    // so that mouse clicks are checked properly.
    if( !bReading )
        s_CandList.rcCandidate = rc;

    // Render the elements
    m_pDialog->DrawRect( &rc, TextBkColor );
    if( ( s_bVerticalCand && !bReading ) ||
        ( !s_bHorizontalReading && bReading ) )
    {
        // Vertical candidate window
        for( UINT i = 0; i < nNumEntries; ++i )
        {
            // Here we are rendering one line at a time
            rc.bottom = rc.top + nSingleLineHeight;
            // Use a different color for the selected string
            if( s_CandList.dwSelection == i )
            {
                m_pDialog->DrawRect( &rc, SelBkColor );
				GetElement( 1 )->FontColor.Current = SelTextColor;
            } else
				GetElement( 1 )->FontColor.Current = TextColor;

			DrawText( s_CandList.awszCandidate[i], GetElement( 1 ), &rc );

            rc.top += nSingleLineHeight;
        }
    } else
    {
        // Horizontal candidate window
		GetElement( 1 )->FontColor.Current = TextColor;
        if( bReading )
			DrawText( s_wszReadingString, GetElement( 1 ) , &rc );
        else
			DrawText( s_CandList.HoriCand.GetBuffer(), GetElement( 1 ) , &rc );

        // Render the selected entry differently
        if( !bReading )
        {
            int nXLeft, nXRight;
            s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected, FALSE, &nXLeft );
            s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected + s_CandList.nHoriSelectedLen, FALSE, &nXRight );

            rc.right = rc.left + nXRight;
            rc.left += nXLeft;
            m_pDialog->DrawRect( &rc, SelBkColor );
			GetElement( 1 )->FontColor.Current = SelTextColor;
			DrawText( s_CandList.HoriCand.GetBuffer() + s_CandList.nFirstSelected,
                                GetElement( 1 ) , &rc, m_bShadow, s_CandList.nHoriSelectedLen );
        }
    }
}

//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::RenderComposition( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    RECT rcCaret = { 0, 0, 0, 0 };
    int nX, nXFirst;
    m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
    m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );
	CDXUTElement *pElement = GetElement( 1 ) ;

    // Get the required width
    RECT rc = { m_rcText.left + nX - nXFirst, m_rcText.top,
                m_rcText.left + nX - nXFirst, m_rcText.bottom };
    m_pDialog->CalcTextRect( s_CompString.GetBuffer(), pElement, &rc );

    // If the composition string is too long to fit within
    // the text area, move it to below the current line.
    // This matches the behavior of the default IME.
    if( rc.right > m_rcText.right )
        OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );

    // Save the rectangle position for processing highlighted text.
    RECT rcFirst = rc;

    // Update s_ptCompString for RenderCandidateReadingWindow().
    s_ptCompString.x = rc.left;

    // ming add >> 使畫字不超過範圍
    if( m_bShowCompUp )
        s_ptCompString.y = -rc.top;
    else
        s_ptCompString.y = rc.top;
    // ming add <<

    D3DCOLOR TextColor = m_CompColor;
    // Render the window and string.
    // If the string is too long, we must wrap the line.
    pElement->FontColor.Current = TextColor;
    const WCHAR *pwszComp = s_CompString.GetBuffer();
    int nCharLeft = s_CompString.GetTextSize();
    for( ; ; )
    {
        // Find the last character that can be drawn on the same line.
        int nLastInLine;
        int bTrail;
        s_CompString.XtoCP( m_rcText.right - rc.left, &nLastInLine, &bTrail );
        int nNumCharToDraw = __min( nCharLeft, nLastInLine );
        m_pDialog->CalcTextRect( pwszComp, pElement, &rc, nNumCharToDraw );

        // Draw the background
        // For Korean IME, blink the composition window background as if it
        // is a cursor.
        if( GetPrimaryLanguage() == LANG_KOREAN )
        {
            if( m_bCaretOn )
            {
                m_pDialog->DrawRect( &rc, m_CompWinColor );
            }
            else
            {
                // Not drawing composition string background. We
                // use the editbox's text color for composition
                // string text.
				TextColor = GetElement(0)->FontColor.States[DXUT_STATE_NORMAL];
            }
        } else
        {
            // Non-Korean IME. Always draw composition background.
            m_pDialog->DrawRect( &rc, m_CompWinColor );
        }

        // Draw the text
        pElement->FontColor.Current = TextColor;
        DrawText( pwszComp, pElement, &rc, m_bShadow, nNumCharToDraw );

        // Advance pointer and counter
        nCharLeft -= nNumCharToDraw;
        pwszComp += nNumCharToDraw;
        if( nCharLeft <= 0 )
            break;

        // Advance rectangle coordinates to beginning of next line
        OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );
    }

    // Load the rect for the first line again.
    rc = rcFirst;

    // Inspect each character in the comp string.
    // For target-converted and target-non-converted characters,
    // we display a different background color so they appear highlighted.
    int nCharFirst = 0;
    nXFirst = 0;
    s_nFirstTargetConv = -1;
    BYTE *pAttr;
    const WCHAR *pcComp;
    for( pcComp = s_CompString.GetBuffer(), pAttr = s_abCompStringAttr;
          *pcComp != L'\0'; ++pcComp, ++pAttr )
    {
        D3DCOLOR bkColor;

        // Render a different background for this character
        int nXLeft, nXRight;
        s_CompString.CPtoX( int(pcComp - s_CompString.GetBuffer()), FALSE, &nXLeft );
        s_CompString.CPtoX( int(pcComp - s_CompString.GetBuffer()), TRUE, &nXRight );

        // Check if this character is off the right edge and should
        // be wrapped to the next line.
        if( nXRight - nXFirst > m_rcText.right - rc.left )
        {
            // Advance rectangle coordinates to beginning of next line
            OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );

            // Update the line's first character information
            nCharFirst = int(pcComp - s_CompString.GetBuffer());
            s_CompString.CPtoX( nCharFirst, FALSE, &nXFirst );
        }

        // If the caret is on this character, save the coordinates
        // for drawing the caret later.
        if( s_nCompCaret == int(pcComp - s_CompString.GetBuffer()) )
        {
            rcCaret = rc;
            rcCaret.left += nXLeft - nXFirst - 1;
            rcCaret.right = rcCaret.left + 2;
        }

        // Set up color based on the character attribute
        if( *pAttr == ATTR_TARGET_CONVERTED )
        {
            pElement->FontColor.Current = m_CompTargetColor;
            bkColor = m_CompTargetBkColor;
        }
        else
        if( *pAttr == ATTR_TARGET_NOTCONVERTED )
        {
            pElement->FontColor.Current = m_CompTargetNonColor;
            bkColor = m_CompTargetNonBkColor;
        }
        else
        {
            continue;
        }

        RECT rcTarget = { rc.left + nXLeft - nXFirst, rc.top, rc.left + nXRight - nXFirst, rc.bottom };
        m_pDialog->DrawRect( &rcTarget, bkColor );
        DrawText( pcComp, pElement, &rcTarget, m_bShadow, 1 );

        // Record the first target converted character's index
        if( -1 == s_nFirstTargetConv )
            s_nFirstTargetConv = int(pAttr - s_abCompStringAttr);
    }

    // Render the composition caret
    if( m_bCaretOn )
    {
        // If the caret is at the very end, its position would not have
        // been computed in the above loop.  We compute it here.
        if( s_nCompCaret == s_CompString.GetTextSize() )
        {
            s_CompString.CPtoX( s_nCompCaret, FALSE, &nX );
            rcCaret = rc;
            rcCaret.left += nX - nXFirst - 1;
            rcCaret.right = rcCaret.left + 2;
        }

        m_pDialog->DrawRect( &rcCaret, m_CompCaretColor );
    }
}

//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::RenderIndicator( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    CDXUTElement *pElement = GetElement( 9 ) ;
	pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

    RECT rcIndicator = m_rcIndicator;
    OffsetRect( &rcIndicator, m_OffsetX, m_OffsetY ); // ming add

    //m_pDialog->DrawSprite( pElement, &m_rcIndicator );
	m_pDialog->DrawSprite( pElement, &rcIndicator );
    RECT rc = rcIndicator;
    InflateRect( &rc, -m_nSpacing, -m_nSpacing );
    pElement->FontColor.Current = s_ImeState == IMEUI_STATE_ON && s_bEnableImeSystem ? m_IndicatorImeColor : m_IndicatorEngColor;
    RECT rcCalc = { 0, 0, 0, 0 };
    
    // If IME system is off, draw English indicator.
    LPCWSTR pwszIndicator;
    if (s_bEnableImeSystem && _ImmIsIME(s_hklCurrent) && (s_ImeState == IMEUI_STATE_ON))
        pwszIndicator = s_wszCurrIndicator;
    else pwszIndicator = s_aszIndicator[0];

    m_pDialog->CalcTextRect( pwszIndicator, pElement, &rcCalc );
    DrawText( pwszIndicator, pElement, &rc );
}

//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if( m_bVisible == false )
        return;

    // If we have not computed the indicator symbol width,
    // do it.
    if( !m_nIndicatorWidth )
    {
        for( int i = 0; i < 5; ++i )
        {
            RECT rc = { 0, 0, 0, 0 };
            m_pDialog->CalcTextRect( s_aszIndicator[i], GetElement( 9 ) , &rc );
            m_nIndicatorWidth = __max( m_nIndicatorWidth, rc.right - rc.left );
        }
        // Update the rectangles now that we have the indicator's width
        UpdateRects();
    }

    // Let the parent render first (edit control)
    CDXUTEditBox::Render( pd3dDevice, fElapsedTime );

    CDXUTElement* pElement = GetElement( 1 );
    if( pElement )
    {
        s_CompString.SetFontNode( m_pDialog->GetFont( pElement->iFont ) );
        s_CandList.HoriCand.SetFontNode( m_pDialog->GetFont( pElement->iFont ) );
    }

    //
    // Now render the IME elements
    //
    if( m_bHasFocus )
    {
        if( m_bIndicatorVisible )
        {
            // Render the input locale indicator
            RenderIndicator( pd3dDevice, fElapsedTime );
        }

        // Display the composition string.
        // This method should also update s_ptCompString
        // for RenderCandidateReadingWindow.
        RenderComposition( pd3dDevice, fElapsedTime );

        // Display the reading/candidate window. RenderCandidateReadingWindow()
        // uses s_ptCompString to position itself.  s_ptCompString must have
        // been filled in by RenderComposition().
        if( s_bShowReadingWindow )
            // Reading window
            RenderCandidateReadingWindow( pd3dDevice, fElapsedTime, true );
        else
        if( s_CandList.bShowWindow )
            // Candidate list window
            RenderCandidateReadingWindow( pd3dDevice, fElapsedTime, false );
    }
}


//--------------------------------------------------------------------------------------
void CUniBuffer::Initialize()
{
    if( s_hDll ) // Only need to do once
        return;

    WCHAR wszPath[MAX_PATH+1];
    if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
        return;

    // Verify whether it is safe to concatenate these strings
    int len1 = lstrlen(wszPath);
    int len2 = lstrlen(UNISCRIBE_DLLNAME);
    if (len1 + len2 > MAX_PATH)
    {
        return;
    }

    // We have verified that the concatenated string will fit into wszPath,
    // so it is safe to concatenate them.
    StringCchCat( wszPath, MAX_PATH, UNISCRIBE_DLLNAME );

    s_hDll = LoadLibrary( wszPath );
    if( s_hDll )
    {
        FARPROC Temp;
        GETPROCADDRESS( s_hDll, ScriptApplyDigitSubstitution, Temp );
        GETPROCADDRESS( s_hDll, ScriptStringAnalyse, Temp );
        GETPROCADDRESS( s_hDll, ScriptStringCPtoX, Temp );
        GETPROCADDRESS( s_hDll, ScriptStringXtoCP, Temp );
        GETPROCADDRESS( s_hDll, ScriptStringFree, Temp );
        GETPROCADDRESS( s_hDll, ScriptString_pLogAttr, Temp );
        GETPROCADDRESS( s_hDll, ScriptString_pcOutChars, Temp );
    }
}


//--------------------------------------------------------------------------------------
void CUniBuffer::Uninitialize()
{
    if( s_hDll )
    {
        PLACEHOLDERPROC( ScriptApplyDigitSubstitution );
        PLACEHOLDERPROC( ScriptStringAnalyse );
        PLACEHOLDERPROC( ScriptStringCPtoX );
        PLACEHOLDERPROC( ScriptStringXtoCP );
        PLACEHOLDERPROC( ScriptStringFree );
        PLACEHOLDERPROC( ScriptString_pLogAttr );
        PLACEHOLDERPROC( ScriptString_pcOutChars );

        FreeLibrary( s_hDll );
        s_hDll = NULL;
    }
}


//--------------------------------------------------------------------------------------
bool CUniBuffer::SetBufferSize( int nNewSize )
{
    // If the current size is already the maximum allowed,
    // we can't possibly allocate more.
    if( m_nBufferSize == DXUT_MAX_EDITBOXLENGTH )
        return false;

    int nAllocateSize = ( nNewSize == -1 || nNewSize < m_nBufferSize * 2 ) ? ( m_nBufferSize ? m_nBufferSize * 2 : MAX_COMPSTRING_SIZE ) : nNewSize * 2;

    // Cap the buffer size at the maximum allowed.
    if( nAllocateSize > DXUT_MAX_EDITBOXLENGTH )
        nAllocateSize = DXUT_MAX_EDITBOXLENGTH;

    WCHAR *pTempBuffer = new WCHAR[nAllocateSize];
    if( !pTempBuffer )
	{
        return false;
	}

	//要先清空
	ZeroMemory( pTempBuffer, sizeof(WCHAR) * nAllocateSize );
    
	if( m_pwszBuffer )
    {
        CopyMemory( pTempBuffer, m_pwszBuffer, m_nBufferSize * sizeof(WCHAR) );
        delete[] m_pwszBuffer;
    }
    else
    {
        ZeroMemory( pTempBuffer, sizeof(WCHAR) * nAllocateSize );
    }

    m_pwszBuffer = pTempBuffer;
    m_nBufferSize = nAllocateSize;
    return true;
}


//--------------------------------------------------------------------------------------
// Uniscribe -- Analyse() analyses the string in the buffer
//--------------------------------------------------------------------------------------
HRESULT CUniBuffer::Analyse()
{
    if( m_Analysis )
        _ScriptStringFree( &m_Analysis );

    SCRIPT_CONTROL ScriptControl; // For uniscribe
    SCRIPT_STATE   ScriptState;   // For uniscribe
    ZeroMemory( &ScriptControl, sizeof(ScriptControl) );
    ZeroMemory( &ScriptState, sizeof(ScriptState) );
    _ScriptApplyDigitSubstitution ( NULL, &ScriptControl, &ScriptState );

    if( !m_pFontNode )
        return E_FAIL;

    HRESULT hr = _ScriptStringAnalyse( m_pFontNode->pFont ? m_pFontNode->pFont->GetDC() : NULL,
                                       m_pwszBuffer,
                                       lstrlenW( m_pwszBuffer ) + 1,  // NULL is also analyzed.
                                       lstrlenW( m_pwszBuffer ) * 3 / 2 + 16,
                                       -1,
                                       SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
                                       0,
                                       &ScriptControl,
                                       &ScriptState,
                                       NULL,
                                       NULL,
                                       NULL,
                                       &m_Analysis );
    if( SUCCEEDED( hr ) )
        m_bAnalyseRequired = false;  // Analysis is up-to-date
    return hr;
}


//--------------------------------------------------------------------------------------
CUniBuffer::CUniBuffer( int nInitialSize, EditBoxPlugin::CDXUTEditBoxInputPlugin* pInputPlugin)
: m_pInputFilterPlugin(pInputPlugin)
{
    CUniBuffer::Initialize();  // ensure static vars are properly init'ed first

    m_nBufferSize = 0;
    m_pwszBuffer = NULL;
    m_bAnalyseRequired = true;
    m_Analysis = NULL;
    m_pFontNode = NULL;
    
    if( nInitialSize > 0 )
        SetBufferSize( nInitialSize );

	m_nInputLimitCount = DXUT_MAX_EDITBOXLENGTH;
}


//--------------------------------------------------------------------------------------
CUniBuffer::~CUniBuffer()
{
    delete[] m_pwszBuffer;
    if( m_Analysis )
        _ScriptStringFree( &m_Analysis );
}


//--------------------------------------------------------------------------------------
WCHAR& CUniBuffer::operator[]( int n )  // No param checking
{
    // This version of operator[] is called only
    // if we are asking for write access, so
    // re-analysis is required.
    m_bAnalyseRequired = true;
    return m_pwszBuffer[n];
}


//--------------------------------------------------------------------------------------
void CUniBuffer::Clear()
{
    *m_pwszBuffer = L'\0';
    m_bAnalyseRequired = true;
}


//--------------------------------------------------------------------------------------
// Inserts the char at specified index.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
CUniBuffer::InsertResult CUniBuffer::InsertChar( int nIndex, const WCHAR& wChar, EditBoxPlugin::CDXUTEditBoxInputPlugin& filterPlugin )
{
    assert( nIndex >= 0 );

    if( nIndex < 0 || nIndex > lstrlenW( m_pwszBuffer ) )
		return InsertResult::RESULT_INVALIDINDEX;  // invalid index

    // Check for maximum length allowed
	if( !ChkInputLimit( 1 ))
		return InsertResult::RESULT_MAXIUMLENGTH;
//     if( GetTextSize() + 1 >= DXUT_MAX_EDITBOXLENGTH )
//         return false;

    if( lstrlenW( m_pwszBuffer ) + 1 >= m_nBufferSize )
    {
        if( !SetBufferSize( -1 ) )
			return InsertResult::RESULT_OUTOFMEMORY;  // out of memory
    }

    assert( m_nBufferSize >= 2 );

    // Shift the characters after the index, start by copying the null terminator
    WCHAR* dest = m_pwszBuffer + lstrlenW(m_pwszBuffer)+1;
    WCHAR* stop = m_pwszBuffer + nIndex;
    WCHAR* src = dest - 1; 

    // 雖然CodeGuard在這裡會出現錯誤，不過應該是沒有錯的
    while( dest > stop )
    {            
        *dest-- = *src--;
    }

    // Set new character
    m_pwszBuffer[ nIndex ] = wChar;
    m_bAnalyseRequired = true;


// 	wstring inputStr(m_pwszBuffer);
// 	uint nOldSize = inputStr.size();
// 	m_pInputFilterPlugin->RunInputPlugin( inputStr );
// 	uint nNewSize = inputStr.size();
// 	uint nDiffCount = nOldSize - nNewSize;
// 	
// 	if( nDiffCount >= 0 )
// 	{
// 		nIndex = nIndex - nDiffCount;
// 		wcsncpy(m_pwszBuffer , inputStr.c_str() , m_nBufferSize  );
// 		m_pwszBuffer[m_nBufferSize -1] = 0;
// 		return false;
// 	}
// 	else
// 	{
// 		int i=0;
// 	}
	
	return InsertResult::RESULT_OK;
}


//--------------------------------------------------------------------------------------
// Removes the char at specified index.
// If nIndex == -1, remove the last char.
//--------------------------------------------------------------------------------------
bool CUniBuffer::RemoveChar( int nIndex )
{
    if( !lstrlenW( m_pwszBuffer ) || nIndex < 0 || nIndex >= lstrlenW( m_pwszBuffer ) )
        return false;  // Invalid index

    MoveMemory( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof(WCHAR) * ( lstrlenW( m_pwszBuffer ) - nIndex ) );
    m_bAnalyseRequired = true;

// 	wstring inputStr(m_pwszBuffer);
// 	uint nOldSize = inputStr.size();
// 	m_pInputFilterPlugin->RunInputPlugin( inputStr );
// 	uint nNewSize = inputStr.size();
// 	uint nDiffCount = nOldSize - nNewSize;
// 
// 	if( nDiffCount > 0 )
// 	{
// 		nIndex = nIndex - nDiffCount;
// 		wcsncpy(m_pwszBuffer , inputStr.c_str() , m_nBufferSize  );
// 		m_pwszBuffer[m_nBufferSize -1] = 0;
// 	}

    return true;
}



//--------------------------------------------------------------------------------------
// Inserts the first nCount characters of the string pStr at specified index.
// If nCount == -1, the entire string is inserted.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
CUniBuffer::InsertResult CUniBuffer::InsertString( int nIndex, const WCHAR *pStr, int nCount )
{
    assert( nIndex >= 0 );

	int tem = lstrlenW( m_pwszBuffer );
    if( nIndex > tem )
	{
		return InsertResult::RESULT_INVALIDINDEX;  // invalid index
	}

	//抓出要貼上的字數
    if( -1 == nCount )
	{
		nCount = lstrlenW( pStr );
	}

    // Check for maximum length allowed
//     if( GetTextSize() + nCount >= DXUT_MAX_EDITBOXLENGTH )
// 	{
// 		return false;
// 	}

	// 已經超過限制字數
	if( !ChkInputLimit(nCount) )
	{		
		return InsertResult::RESULT_MAXIUMLENGTH;
	}

	//重設緩衝大小
    if( GetTextSize() + nCount >= ((m_nBufferSize / 2) - 1))
    {
		uint nBufferSize = (GetTextSize() + nCount + 1) * 2;
        if( !SetBufferSize( nBufferSize ) )
		{
			return InsertResult::RESULT_OUTOFMEMORY;  // out of memory
		}
    }

	std::wstring tmpstr(m_pwszBuffer);


	tmpstr.insert(nIndex , pStr , nCount);
	wcsncpy(m_pwszBuffer , tmpstr.c_str() , m_nBufferSize );
	m_pwszBuffer[m_nBufferSize - 1] = 0;
	//CopyMemory( m_pwszBuffer , tmpstr.c_str() , tmpstr.length() * 2 );
	m_bAnalyseRequired = true;

	//過濾
// 	wstring inputStr(m_pwszBuffer);
// 	m_pInputFilterPlugin->RunInputPlugin( inputStr );
// 	wcsncpy(m_pwszBuffer , inputStr.c_str() , m_nBufferSize);
// 	m_pwszBuffer[m_nBufferSize - 1] = 0;
	
// 	if( inputStr.size() > m_nBufferSize )
// 	{
// 		wstring str = "123";
// 		m_pwszBuffer[m_nBufferSize] = '\0';
// 	}
// 	else
// 	{
// 		m_pwszBuffer[inputStr.length()] = '\0';
// 	}
	
	return InsertResult::RESULT_OK;

	
}


//--------------------------------------------------------------------------------------
bool CUniBuffer::SetText( LPCWSTR wszText, WORD wLangID )
{
    assert( wszText != NULL );

    int nRequired = int(wcslen( wszText ) + 1);
	nRequired = nRequired < GetInputLimit() ? nRequired : GetInputLimit();
    // Check for maximum length allowed
	
//     if( nRequired >= DXUT_MAX_EDITBOXLENGTH )
//         return false;

//     while( GetBufferSize() < nRequired )
//         if( !SetBufferSize( -1 ) )
//             break;

	SetBufferSize( nRequired );
    // Check again in case out of memory occurred inside while loop.
    /*if( GetBufferSize() >= nRequired )*/
    {
		//CUniBuffer::Clear();		
		/*if(ChkInputLimit(nRequired))*/
		{
			//wszText = FDO::LangConv(wszText, wLangID);
			StringCchCopy( m_pwszBuffer, GetBufferSize(), wszText );
			m_bAnalyseRequired = true;
			return true;
		}        
    }
//     else
//         return false;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//檢查字數是否超過限制
////////////////////////////////////////////////////////////////////////////////
bool CUniBuffer::ChkInputLimit(int nCount)
{	
	int nLimit = GetTextSize() + nCount;	

	return nLimit < m_nInputLimitCount ;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CUniBuffer::SetInputLimit(int nCount)
{ 
	if(nCount <= 0)
	{
		m_nInputLimitCount = DXUT_MAX_EDITBOXLENGTH;	
	}
	else
	{
		m_nInputLimitCount = nCount; 
		if(m_nInputLimitCount > DXUT_MAX_EDITBOXLENGTH)
			m_nInputLimitCount = DXUT_MAX_EDITBOXLENGTH;
	}
}



//--------------------------------------------------------------------------------------
HRESULT CUniBuffer::CPtoX( int nCP, BOOL bTrail, int *pX )
{
    assert( pX );
    *pX = 0;  // Default

    HRESULT hr = S_OK;
    if( m_bAnalyseRequired )
        hr = Analyse();

    if( SUCCEEDED( hr ) )
        hr = _ScriptStringCPtoX( m_Analysis, nCP, bTrail, pX );

    return hr;
}


//--------------------------------------------------------------------------------------
HRESULT CUniBuffer::XtoCP( int nX, int *pCP, int *pnTrail )
{
    assert( pCP && pnTrail );
    *pCP = 0; *pnTrail = FALSE;  // Default

    HRESULT hr = S_OK;
    if( m_bAnalyseRequired )
        hr = Analyse();

    if( SUCCEEDED( hr ) )
        hr = _ScriptStringXtoCP( m_Analysis, nX, pCP, pnTrail );

    // If the coordinate falls outside the text region, we
    // can get character positions that don't exist.  We must
    // filter them here and convert them to those that do exist.
    if( *pCP == -1 && *pnTrail == TRUE )
    {
        *pCP = 0; *pnTrail = FALSE;
    } else
    if( *pCP > lstrlenW( m_pwszBuffer ) && *pnTrail == FALSE )
    {
        *pCP = lstrlenW( m_pwszBuffer ); *pnTrail = TRUE;
    }

    return hr;
}


//--------------------------------------------------------------------------------------
void CUniBuffer::GetPriorItemPos( int nCP, int *pPrior )
{
    *pPrior = nCP;  // Default is the char itself

    if( m_bAnalyseRequired )
        if( FAILED( Analyse() ) )
            return;

    const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr( m_Analysis );
    if( !pLogAttr )
        return;

    if( !_ScriptString_pcOutChars( m_Analysis ) )
        return;
    int nInitial = *_ScriptString_pcOutChars( m_Analysis );
    if( nCP - 1 < nInitial )
        nInitial = nCP - 1;
    for( int i = nInitial; i > 0; --i )
        if( pLogAttr[i].fWordStop ||       // Either the fWordStop flag is set
            ( !pLogAttr[i].fWhiteSpace &&  // Or the previous char is whitespace but this isn't.
                pLogAttr[i-1].fWhiteSpace ) )
        {
            *pPrior = i;
            return;
        }
    // We have reached index 0.  0 is always a break point, so simply return it.
    *pPrior = 0;
}
    

//--------------------------------------------------------------------------------------
void CUniBuffer::GetNextItemPos( int nCP, int *pPrior )
{
    *pPrior = nCP;  // Default is the char itself

    HRESULT hr = S_OK;
    if( m_bAnalyseRequired )
        hr = Analyse();
    if( FAILED( hr ) )
        return;

    const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr( m_Analysis );
    if( !pLogAttr )
        return;

    if( !_ScriptString_pcOutChars( m_Analysis ) )
        return;
    int nInitial = *_ScriptString_pcOutChars( m_Analysis );
    if( nCP + 1 < nInitial )
        nInitial = nCP + 1;
    for( int i = nInitial; i < *_ScriptString_pcOutChars( m_Analysis ) - 1; ++i )
    {
        if( pLogAttr[i].fWordStop )      // Either the fWordStop flag is set
        {
            *pPrior = i;
            return;
        }
        else
        if( pLogAttr[i].fWhiteSpace &&  // Or this whitespace but the next char isn't.
            !pLogAttr[i+1].fWhiteSpace )
        {
            *pPrior = i+1;  // The next char is a word stop
            return;
        }
    }
    // We have reached the end. It's always a word stop, so simply return it.
    *pPrior = *_ScriptString_pcOutChars( m_Analysis ) - 1;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::ResetCaretBlink()
{
    m_bCaretOn = true;
    m_dfLastBlink = DXUTGetGlobalTimer()->GetAbsoluteTime();
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::Initialize()
{
    if( s_hDllImm32 ) // Only need to do once
        return;

    FARPROC Temp;

    s_CompString.SetBufferSize( MAX_COMPSTRING_SIZE );

    WCHAR wszPath[MAX_PATH+1];
    if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
        return;
    StringCchCat( wszPath, MAX_PATH, IMM32_DLLNAME );
    s_hDllImm32 = LoadLibrary( wszPath );
    if( s_hDllImm32 )
    {
        GETPROCADDRESS( s_hDllImm32, ImmLockIMC, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmUnlockIMC, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmLockIMCC, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmUnlockIMCC, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmDisableTextFrameService, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetCompositionStringW, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetCandidateListW, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetContext, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmReleaseContext, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmAssociateContext, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetOpenStatus, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmSetOpenStatus, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetConversionStatus, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetDefaultIMEWnd, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetIMEFileNameA, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetVirtualKey, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmNotifyIME, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmSetConversionStatus, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmSimulateHotKey, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmIsIME, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetDescription, Temp );
    }

    if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
        return;
    StringCchCat( wszPath, MAX_PATH, VER_DLLNAME );
    s_hDllVer = LoadLibrary( wszPath );
    if( s_hDllVer )
    {
        GETPROCADDRESS( s_hDllVer, VerQueryValueA, Temp );
        GETPROCADDRESS( s_hDllVer, GetFileVersionInfoA, Temp );
        GETPROCADDRESS( s_hDllVer, GetFileVersionInfoSizeA, Temp );
    }
}


//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::Uninitialize()
{
    if( s_hDllImm32 )
    {
        PLACEHOLDERPROC( ImmLockIMC );
        PLACEHOLDERPROC( ImmUnlockIMC );
        PLACEHOLDERPROC( ImmLockIMCC );
        PLACEHOLDERPROC( ImmUnlockIMCC );
        PLACEHOLDERPROC( ImmDisableTextFrameService );
        PLACEHOLDERPROC( ImmGetCompositionStringW );
        PLACEHOLDERPROC( ImmGetCandidateListW );
        PLACEHOLDERPROC( ImmGetContext );
        PLACEHOLDERPROC( ImmReleaseContext );
        PLACEHOLDERPROC( ImmAssociateContext );
        PLACEHOLDERPROC( ImmGetOpenStatus );
        PLACEHOLDERPROC( ImmSetOpenStatus );
        PLACEHOLDERPROC( ImmGetConversionStatus );
        PLACEHOLDERPROC( ImmGetDefaultIMEWnd );
        PLACEHOLDERPROC( ImmGetIMEFileNameA );
        PLACEHOLDERPROC( ImmGetVirtualKey );
        PLACEHOLDERPROC( ImmNotifyIME );
        PLACEHOLDERPROC( ImmSetConversionStatus );
        PLACEHOLDERPROC( ImmSimulateHotKey );
        PLACEHOLDERPROC( ImmIsIME );
        PLACEHOLDERPROC( ImmGetDescription );

        FreeLibrary( s_hDllImm32 );
        s_hDllImm32 = NULL;
    }
    if( s_hDllIme )
    {
        PLACEHOLDERPROC( GetReadingString );
        PLACEHOLDERPROC( ShowReadingWindow );

        FreeLibrary( s_hDllIme );
        s_hDllIme = NULL;
    }
    if( s_hDllVer )
    {
        PLACEHOLDERPROC( VerQueryValueA );
        PLACEHOLDERPROC( GetFileVersionInfoA );
        PLACEHOLDERPROC( GetFileVersionInfoSizeA );

        FreeLibrary( s_hDllVer );
        s_hDllVer = NULL;
    }
}

//--------------------------------------------------------------------------------------
// ming add 清除
//--------------------------------------------------------------------------------------
void CDXUTIMEEditBox::ClearTexture()
{
    for( int i=0; i<10; ++i )
        SetTexture( "", 0, i );
}

//---------------------------------------------------------------------------
bool CDXUTIMEEditBox::fnInInputState() 
{
// 	HIMC hImc;
// 	if( NULL != ( hImc = _ImmGetContext( FDO::DXUTGetHWND() ) ) )
// 	{
// 		if( bIme )
// 			//if( s_bChineseIME )
// 		{
// 			DWORD dwConvMode, dwSentMode;
// 			_ImmGetConversionStatus( hImc, &dwConvMode, &dwSentMode );
// 			s_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
// 		}
// 		else
// 		{
// 			s_ImeState = ( bIme && _ImmGetOpenStatus( hImc ) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
// 		}
// 		_ImmReleaseContext( FDO::DXUTGetHWND(), hImc );
// 	}
// 	else {
// 		s_ImeState = IMEUI_STATE_OFF;
// 	}
// 
// WM_IME_COMPOSITION
	return s_CompString.GetTextSize() != 0;
	//return s_ImeState == IMEUI_STATE_ON;
}
