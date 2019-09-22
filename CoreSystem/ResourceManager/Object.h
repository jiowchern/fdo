#ifndef ObjectH
#define ObjectH
//---------------------------------------------------------------------------
#include "FdoString.h"
//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
class Object
{
protected:
    fstring m_FileName; // 物件檔名

    FDOCh m_strPath [_MAX_PATH ];
    FDOCh m_strDrive[_MAX_DRIVE];
    FDOCh m_strDir  [_MAX_DIR  ];
    FDOCh m_strFName[_MAX_FNAME];
    FDOCh m_strExt  [_MAX_EXT  ];

public:
    Object(const fstring& fileName)
        : m_FileName(fileName)
    {
        _splitpath(fileName.c_str(), m_strDrive, m_strDir, m_strFName, m_strExt );
    }
    virtual ~Object() {}

    //每個物件都有名字當作索引值
    const fstring& GetFileName() const { return m_FileName; }
    const FDOCh*   GetPath()     const { return m_strPath;  }
    const FDOCh*   GetDrive()    const { return m_strDrive; }
    const FDOCh*   GetDir()      const { return m_strDir;   }
    const FDOCh*   GetFName()    const { return m_strFName; }
    const FDOCh*   GetExt()      const { return m_strExt;   }
};
//---------------------------------------------------------------------------
} // namespace FDO
//---------------------------------------------------------------------------
#endif // ObjectH
