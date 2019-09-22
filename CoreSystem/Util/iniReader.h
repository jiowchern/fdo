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
    // ���� *** modify
    iniReader(const fstring& strFileName, bool packeted = false);
    iniReader(const unsigned char* pData, UINT uiDataSize);
    ~iniReader();

    void Clear();

    // ���� *** modify
    bool LoadFromFile(const fstring& strFileName,  bool packeted = false);
    bool LoadFromFileInMemory(const unsigned char* pData, unsigned int uiDataSize );

    int GetInteger(const fstring& key) const;
	double GetDoubler(const fstring &key) const ; 
    const fstring& GetString(const FDOCh* fmt, ...) const;
};

//-----------------------------------------------------------------------
// ���X�]�w�Ѽ�
//-----------------------------------------------------------------------
const fstring& GetConfig(const fstring& strFileName);

//���� *** modify
const fstring& GetSoundMap(const fstring& strTag);

} // namepsace FDO

#endif // iniReaderH
