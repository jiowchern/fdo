//---------------------------------------------------------------------------
#ifndef iniReaderH
#define iniReaderH
//---------------------------------------------------------------------------
#include <map>
#include "FdoString.h"
//---------------------------------------------------------------------------
namespace FDO
{

class iniReader
{
    typedef std::map<fstring, fstring> DataMap;
    typedef DataMap::iterator DataMapIterator;
    typedef DataMap::const_iterator ConstDataMapIterator;

private:
    DataMap mDataMap;
    static const fstring sEmpty;

public:
    iniReader();
    // 音效 *** modify
    iniReader(const fstring& strFileName, bool packeted = false);
    iniReader(const unsigned char* pData, UINT uiDataSize);
    ~iniReader();

    void Clear();

    // 音效 *** modify
    bool LoadFromFile(const fstring& strFileName,  bool packeted = false);
    bool LoadFromFileInMemory(const unsigned char* pData, unsigned int uiDataSize );

    int GetInteger(const fstring& key) const;
	double GetDoubler(const fstring &key) const ; 
    const fstring& GetString(const FDOCh* fmt, ...) const;
};

//-----------------------------------------------------------------------
// 取出設定參數
//-----------------------------------------------------------------------
const fstring& GetConfig(const fstring& strFileName);

//音效 *** modify
const fstring& GetSoundMap(const fstring& strTag);

} // namepsace FDO

#endif // iniReaderH
