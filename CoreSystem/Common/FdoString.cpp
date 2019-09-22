#include "StdAfx.h"
#include "FdoString.h"

namespace FDO
{

CStringConvertor::CStringConvertor(const char* source)
{
    size_t len = strlen(source);
    mStringA.resize(len + 1);
    mStringA[len] = '\0';
    strncpy(&mStringA.front(), source, len);
}
//---------------------------------------------------------------------------
CStringConvertor::CStringConvertor(const wchar_t* source)
{
    size_t len = wcslen(source);
    mStringW.resize(len + 1);
    mStringW[len] = L'\0';
    wcsncpy(&mStringW.front(), source, len);
}
//---------------------------------------------------------------------------
CStringConvertor::CStringConvertor(const std::string& source)
{
    size_t len = source.length();
    mStringA.resize(len + 1);
    mStringA[len] = '\0';
    strncpy(&mStringA.front(), source.c_str(), len);
}
//---------------------------------------------------------------------------
CStringConvertor::CStringConvertor(const std::wstring& source)
{
    size_t len = source.length();
    mStringW.resize(len + 1);
    mStringW[len] = L'\0';
    wcsncpy(&mStringW.front(), source.c_str(), len);
}
//---------------------------------------------------------------------------
CStringConvertor::~CStringConvertor()
{
}
//---------------------------------------------------------------------------
CStringConvertor::operator int () const
{
    return _ttoi(*this);
}
//---------------------------------------------------------------------------
CStringConvertor::operator float () const
{
    return static_cast<float>(_tstof(*this));
}
//---------------------------------------------------------------------------
CStringConvertor::operator long () const
{
    return _ttol(*this);
}
//---------------------------------------------------------------------------
CStringConvertor::operator double () const
{
    return _tstof(*this);
}
//---------------------------------------------------------------------------
CStringConvertor::operator std::string () const
{
    return this->ToAnsi();
}
//---------------------------------------------------------------------------
CStringConvertor::operator std::wstring () const
{
    return this->ToUni();
}
//---------------------------------------------------------------------------
CStringConvertor::operator const wchar_t* () const
{
    if (mStringW.empty())
    {
        // 計算大小
        int size = ::MultiByteToWideChar(CP_ACP, 0, ToAnsi(), -1, NULL, 0);
        // 配置空間
        mStringW.resize(size);
        // 轉換
        ::MultiByteToWideChar(CP_ACP, 0, ToAnsi(), -1, &mStringW.front(), size);
    }
    return &mStringW.front();
}
//---------------------------------------------------------------------------
CStringConvertor::operator const char* () const
{
    if (mStringA.empty())
    {
        // 計算字串長度
        int size = ::WideCharToMultiByte(CP_ACP, 0, ToUni(), -1, NULL, 0, NULL, 0);
        // 配置空間
        mStringA.resize(size);
        // 轉換字碼
        ::WideCharToMultiByte(CP_ACP, 0, ToUni(), -1, &mStringA.front(), size, NULL, 0);
    }
    return &mStringA.front();
}
//---------------------------------------------------------------------------
const char* CStringConvertor::ToAnsi() const
{
    return *this;
}
//---------------------------------------------------------------------------
const wchar_t* CStringConvertor::ToUni() const
{
    return *this;
}
//---------------------------------------------------------------------------
const FDOCh* CStringConvertor::ToFdo() const
{
    return *this;
//#if defined(UNICODE) || defined(_UNICODE)
//    return ToUni();
//#else
//    return ToAnsi();
//#endif
}

} // namespace FDO