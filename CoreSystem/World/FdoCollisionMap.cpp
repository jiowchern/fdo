#include "StdAfx.h"
#include "FdoCollisionMap.h"
#include <cassert>

namespace FDO
{

CCollisionData::CCollisionData()
: mValue(0)
{
}

CCollisionData::CCollisionData(unsigned char value)
: mValue(value)
{
}

CCollisionData::CCollisionData(int value)
: mValue(static_cast<unsigned char>(value))
{
}

CCollisionData::~CCollisionData()
{
}

bool CCollisionData::operator==(unsigned char rhs) const
{
    return (mValue == rhs);
}

bool CCollisionData::operator==(int rhs) const
{
    return (mValue == rhs);
}

bool CCollisionData::IsValid() const
{
    return (mValue != 1);
}

unsigned char CCollisionData::GetValue() const
{
    return mValue;
}

CCollisionData::operator unsigned char () const
{
    return mValue;
}

CCollisionData::operator unsigned int () const
{
    return mValue;
}

CCollisionIndex::CCollisionIndex()
: mIndex(0)
, mID(0)
, mClass(0)
, mColor(BLACK)
{
}

CCollisionIndex::CCollisionIndex(const SCN_MapCollideIdx& rhs)
: mIndex(rhs.CollideIdx)
, mID(rhs.CollideID)
, mClass(rhs.CollideClass)
, mColor(rhs.Color)
, mName(_F(rhs.CollideName))
{
}

CCollisionIndex::CCollisionIndex(const SCN_MapCollideIdx1& rhs)
: mIndex(0)
, mID(rhs.CollideID)
, mClass(0)
, mColor(rhs.Color)
, mName(_F(rhs.CollideName))
{
}

CCollisionIndex::CCollisionIndex(const SCN_MapCollideIdx2& rhs)
: mIndex(rhs.CollideIdx)
, mID(rhs.CollideID)
, mClass(0)
, mColor(rhs.Color)
, mName(_F(rhs.CollideName))
{
}

CCollisionIndex::~CCollisionIndex()
{
}

CCollisionMap::CCollisionMap(void)
{
}

CCollisionMap::~CCollisionMap(void)
{
}

void CCollisionMap::Initialize(const SCN_Header& header, const SCN_MapNfo& mapNfo, const BYTE* data)
{
    switch (header.Version)
    {
        case 1:
        {
            const SCN_MapCollideIdx1Ptr p = (SCN_MapCollideIdx1Ptr)(data + mapNfo.CollideIdxOffset);
            mCollisionIndices.assign(p, p + mapNfo.CollideIdxCount);
            break;
        }
        case 2:
        case 3:
        {
            const SCN_MapCollideIdx2Ptr p = (SCN_MapCollideIdx2Ptr)(data + mapNfo.CollideIdxOffset);
            mCollisionIndices.assign(p, p + mapNfo.CollideIdxCount);
            break;
        }
        default:
        {
            const SCN_MapCollideIdxPtr p = (SCN_MapCollideIdxPtr)(data + mapNfo.CollideIdxOffset);
            mCollisionIndices.assign(p, p + mapNfo.CollideIdxCount);
        }
    }

    int nfoCount = (mapNfo.GridWCount - 1) * (mapNfo.GridHCount - 1);
    switch (header.Version)
    {
        case 1:
        {
            const int* p = (int*)(data + mapNfo.CollideInforOffset);
            mCollisionDatas.assign(p, p + nfoCount);
            break;
        }
        default:
        {
            const BYTE* p = (BYTE*)(data + mapNfo.CollideInforOffset);
            mCollisionDatas.assign(p, p + nfoCount);
        }
    }
}

UINT CCollisionIndex::GetID() const
{
    return mID;
}

int CCollisionIndex::GetIndex() const
{
    return mIndex;
}

const fstring& CCollisionIndex::GetName() const
{
    return mName;
}

BYTE CCollisionIndex::GetClass() const
{
    return mClass;
}

const D3DXCOLOR& CCollisionIndex::GetColor() const
{
    return mColor;
}

const CollisionDataList& CCollisionMap::GetDataList() const
{
    return mCollisionDatas;
}

const CollisionIndexList& CCollisionMap::GetIndexList() const
{
    return mCollisionIndices;
}

const CCollisionData& CCollisionMap::GetDataAt(size_t index) const
{
    assert(index <= mCollisionDatas.size());
    return mCollisionDatas[index];
}

const CCollisionIndex& CCollisionMap::GetIndexAt(size_t index) const
{
    assert(index <= mCollisionIndices.size());
    return mCollisionIndices[index];
}

bool CCollisionMap::GetCollisionValue(size_t index, WORD *ID, WORD *Class) const
{
    F_RETURN(ID != NULL);
    (*ID) = 0;

    F_RETURN(Class != NULL);
    (*Class) = 0;

    F_RETURN(index < mCollisionDatas.size());
    index = mCollisionDatas[index];

    F_RETURN(index < mCollisionIndices.size());
    (*ID)    = mCollisionIndices[index].GetID();
    (*Class) = mCollisionIndices[index].GetClass();

    // 10：原野
    // 11：城鎮
    // 12：天空
    // 13：地空
    // 14：室內
    // 15：迷宮
    // 16：工會
    // 17：鬥技場
    F_RETURN((*Class) >= 10);
    F_RETURN((*Class) <= 16);

    return true;
}

bool CCollisionMap::GetColorAt(DWORD* value, size_t index) const
{
    F_RETURN(value != NULL);
    (*value) = 0;

    F_RETURN(index < mCollisionDatas.size());

    _FOREACH_CONST(CollisionIndexList, mCollisionIndices, pos)
    {
        if (mCollisionDatas[index].GetValue() == (*pos).GetID())
        {
            (*value) = (*pos).GetColor();
            return true;
        }
    }

    //F_RETURN(index < mCollisionIndices.size())
    //(*value) = mCollisionIndices[index].GetColor();

    return false;
}

} // namespace FDO