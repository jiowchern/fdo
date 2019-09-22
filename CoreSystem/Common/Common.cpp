//
// Common.cpp
//
#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "Common.h"
//#include "HUITalkBar.h"

namespace FDO
{

const int BIT_NUM[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
                        8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576,
                        2097152, 4194304, 8388608, 16777216, 33554432, 67108864,
                        134217728, 268435456, 536870912, 1073741824, 2147483648 };
                        
//---------------------------------------------------------------------------
// class BufMgr
//---------------------------------------------------------------------------

class BufMgr
{
private:
    BYTE m_StaticBuf[2048];
    void* m_pvBuf;
    int m_cbStaticBuf;
public:
    BufMgr();
    ~BufMgr();
    void* GetBuf(int cbReq);
};

BufMgr::BufMgr()
{
    m_pvBuf = NULL;
    m_cbStaticBuf = sizeof(m_StaticBuf) / sizeof(m_StaticBuf[0]);
}

BufMgr::~BufMgr()
{
    if (m_pvBuf) {
        free(m_pvBuf);
        m_pvBuf = NULL;
    }
}

void* BufMgr::GetBuf(int cbReq)
{
    // Free additional allocated buffer.
    if (m_pvBuf) {
        free(m_pvBuf);
        m_pvBuf = NULL;
    }
    // Check if default fits the need.
    if (cbReq < m_cbStaticBuf) {
        return m_StaticBuf;
    }
    // Else, allocate a new buffer.
    m_pvBuf = malloc(cbReq);
    return m_pvBuf;
}

static BufMgr _BufMgr;
static BufMgr _BufMgr2;
static BufMgr _BufMgr3;
static BufMgr _BufMgr4;

//---------------------------------------------------------------------------

const WCHAR* AnsiToUnicodeThread1(const char* szSrc)
{
    int cchBuf = MultiByteToWideChar(CP_ACP, 0, szSrc, -1, NULL, 0);
    WCHAR* pwchBuf = (WCHAR*)_BufMgr3.GetBuf(cchBuf * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, szSrc, -1, pwchBuf, cchBuf);
    return pwchBuf;
}


const WCHAR* AnsiToUnicodeThread2(const char* szSrc)
{
    int cchBuf = MultiByteToWideChar(CP_ACP, 0, szSrc, -1, NULL, 0);
    WCHAR* pwchBuf = (WCHAR*)_BufMgr4.GetBuf(cchBuf * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, szSrc, -1, pwchBuf, cchBuf);
    return pwchBuf;
}

//WCHAR <-> ASNI (Multi-Byte) (傳回內部buffer指標!!!小心使用)
static bool bA2UKey = false;
const WCHAR* AnsiToUnicode(const char* szSrc)
{
	Assert(bA2UKey == false , "AnsiToUnicode Buffer 重複利用!");
	bA2UKey = true;
    int cchBuf = MultiByteToWideChar(CP_ACP, 0, szSrc, -1, NULL, 0);
    WCHAR* pwchBuf = (WCHAR*)_BufMgr.GetBuf(cchBuf * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, szSrc, -1, pwchBuf, cchBuf);
	bA2UKey = false;
    return pwchBuf;
}

static bool bU2AKey = false;
const char* UnicodeToAnsi(const WCHAR* wszSrc)
{
	Assert(bU2AKey == false , "UnicodeToAnsi Buffer 重複利用!");
	bU2AKey = true;
    int cbBuf = WideCharToMultiByte(CP_ACP, 0, wszSrc, -1, NULL, 0, NULL, NULL);
    char* pchBuf = (char*)_BufMgr2.GetBuf(cbBuf);
    WideCharToMultiByte(CP_ACP, 0, wszSrc, -1, pchBuf, cbBuf, NULL, NULL);
	bU2AKey = false;
    return pchBuf;
}

// WCHAR <-> UTF8
int WCHARToUTF8(char *utf8,const WCHAR *unicode, int  maxoutput)
{
   return WideCharToMultiByte(CP_UTF8,0,unicode,-1,utf8,maxoutput,NULL,NULL);
}

int UTF8ToWCHAR(WCHAR *unicode, const char *utf8, int maxoutput)
{
   return MultiByteToWideChar(CP_UTF8,0,utf8,-1,unicode,maxoutput);
}

//----------------------------------------------------------------------------------------------------
// LangConv
//----------------------------------------------------------------------------------------------------

#define _CODEPAGE_BIG5   950
#define _CODEPAGE_GB     936

static UINT _uCurrentCodePage = 0;

static UINT _GetCurrentCodePage()
{
    if (0 == _uCurrentCodePage)
        _uCurrentCodePage = GetOEMCP();
    return _uCurrentCodePage;
}

LPCWSTR GB2BIG5_W(LPCWSTR szGB)
{
    if (!szGB)
        return NULL;

    if (_CODEPAGE_GB == _GetCurrentCodePage())
        return szGB;
        
    if (_CODEPAGE_BIG5 != _GetCurrentCodePage())
        return szGB;

    int cchReq = 0;

    const DWORD dwLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
    int cchGB = lstrlenW(szGB) + 1;
    
    cchReq = LCMapString(dwLCID, LCMAP_TRADITIONAL_CHINESE, szGB, cchGB, NULL, 0);
    if (0 == cchReq)
        return NULL;
    WCHAR* pwchBuf = (WCHAR*)_BufMgr.GetBuf(cchReq * sizeof(WCHAR));
    int cchMapped = LCMapString(dwLCID, LCMAP_TRADITIONAL_CHINESE, szGB, cchGB, pwchBuf, cchReq);
    if (0 == cchMapped)
        return NULL;
    
    cchReq = WideCharToMultiByte(_CODEPAGE_BIG5, 0, pwchBuf, cchMapped, NULL, 0, NULL, NULL);
    if (0 == cchReq)
        return NULL;
    char* pchBuf = (char*)_BufMgr2.GetBuf(cchReq * sizeof(char));
    int cchMultiByte = WideCharToMultiByte(_CODEPAGE_BIG5, 0, pwchBuf, cchMapped, pchBuf, cchReq, "?", NULL);
    if (0 == cchMultiByte)
        return NULL;
        
    cchReq = MultiByteToWideChar(_CODEPAGE_BIG5, 0, pchBuf, cchMultiByte, NULL, 0);
    pwchBuf = (WCHAR*)_BufMgr.GetBuf(cchReq * sizeof(WCHAR));
    int cchWide = MultiByteToWideChar(_CODEPAGE_BIG5, 0, pchBuf, cchMultiByte, pwchBuf, cchReq);
    if (!cchWide)
        return NULL;

    return pwchBuf;
}

LPCWSTR LangConv(LPCWSTR wszSrc, WORD wSrcLangID)
{
    if (!wszSrc)
        return NULL;
        
    if (0 == wSrcLangID)
        return wszSrc;
    if (L'\0' == wszSrc[0])
        return wszSrc;
    
    static std::wstring _wstrSrc;
    _wstrSrc = wszSrc;
    LPCWSTR wszResult = _wstrSrc.c_str();
    
    const WORD wPrimaryLangID = PRIMARYLANGID(wSrcLangID);
    
    if (LANG_CHINESE == wPrimaryLangID) {
        const WORD wSubLangID = SUBLANGID(wSrcLangID);
        switch (wSubLangID) {
            case SUBLANG_CHINESE_SIMPLIFIED:
                if (_CODEPAGE_BIG5 == _GetCurrentCodePage()) {
                    wszResult = GB2BIG5_W(wszSrc);
                } // end if
                break;
            case SUBLANG_CHINESE_TRADITIONAL:
                break;
        } // end switch
    } // end if
    
    return wszResult;
}

bool IsBit(void* pBuffer, sint offsetBit)
{
	uint nBlock = sizeof(uint8) ;
	uint8* pAddr = ((uint8*)pBuffer + (offsetBit / 8) * nBlock );
	uint nRet = (*pAddr) & (1 << (offsetBit % 8));
	return   nRet != 0;

}

}

