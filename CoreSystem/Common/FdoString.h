#ifndef FdoStringH
#define FdoStringH

#include <string>
#include <vector>

namespace FDO
{

//typedef _TCHAR FDOCh;
typedef char FDOCh;
//typedef wchar_t FDOCh;
typedef std::basic_string<FDOCh> fstring;
typedef std::vector<fstring> FStringList;

class CStringConvertor
{
private:
    mutable std::vector<char> mStringA;
    mutable std::vector<wchar_t> mStringW;

public:
    CStringConvertor(const std::string& source);
    CStringConvertor(const std::wstring& source);
    CStringConvertor(const char* source);
    CStringConvertor(const wchar_t* source);
    ~CStringConvertor();

    // casting
    operator const char* () const;
    operator const wchar_t* () const;
    operator std::string () const;
    operator std::wstring () const;
    operator int () const;
    operator float () const;
    operator long () const;
    operator double () const;

public:
    const char* ToAnsi() const;
    const wchar_t* ToUni() const;
    const FDOCh* ToFdo() const;
};

} // namesapce FDO

#define _F(str) FDO::CStringConvertor(str)
#define _D(str) FDO::CStringConvertor(str)

#endif // FdoStringH