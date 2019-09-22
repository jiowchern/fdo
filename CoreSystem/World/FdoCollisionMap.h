/**
 * @file CollisionMap.h
 * @author Yuming Ho
 */
#ifndef FdoCollisionMapH
#define FdoCollisionMapH

#include "SCN_Struct.h"
#include "FdoString.h"
#include <d3dx9math.h>

namespace FDO
{

class CCollisionData
{
private:
    unsigned char mValue;

public:
    CCollisionData();
    CCollisionData(unsigned char value);
    CCollisionData(int value);
    ~CCollisionData();
    unsigned char GetValue() const;
    bool IsValid() const;
    bool operator==(unsigned char rhs) const;
    bool operator==(int rhs) const;
    operator unsigned char () const;
    operator unsigned int () const;
};

typedef std::vector<CCollisionData> CollisionDataList;

class CCollisionIndex
{
private:
    /// ¸I¼²¯Á¤Þ
    int mIndex;
    /// ¸I¼²ID
    UINT mID;
    /// ¸I¼²¼h¦WºÙ
    fstring mName;
    /// ¸I¼²¼hÃþ§O
    BYTE mClass;
    /// ¸I¼²¼hÃC¦â
    D3DXCOLOR mColor;

public:
    CCollisionIndex();
    CCollisionIndex(const SCN_MapCollideIdx& rhs);
    CCollisionIndex(const SCN_MapCollideIdx1& rhs);
    CCollisionIndex(const SCN_MapCollideIdx2& rhs);
    ~CCollisionIndex();

    UINT GetID() const;
    int GetIndex() const;
    const fstring& GetName() const;
    BYTE GetClass() const;
    const D3DXCOLOR& GetColor() const;
};

typedef std::vector<CCollisionIndex> CollisionIndexList;

class CCollisionMap
{
private:
    CollisionDataList   mCollisionDatas;
    CollisionIndexList  mCollisionIndices;

public:
    CCollisionMap(void);
    ~CCollisionMap(void);

    void Initialize(const SCN_Header& header, const SCN_MapNfo& mapNfo, const BYTE* data);
    bool GetCollisionValue(size_t index, WORD *ID, WORD *Class) const;
    bool GetColorAt(DWORD* value, size_t index) const;

    const CollisionDataList& GetDataList() const;
    const CollisionIndexList& GetIndexList() const;
    const CCollisionData& GetDataAt(size_t index) const;
    const CCollisionIndex& GetIndexAt(size_t index) const;
};

} // namespace FDO

#endif // CollisionMapH
