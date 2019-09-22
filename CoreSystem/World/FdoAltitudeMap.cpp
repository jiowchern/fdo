#include "StdAfx.h"
#include "FdoAltitudeMap.h"
#include <cassert>

namespace FDO
{

CAltitudeMap::CAltitudeMap(void)
{
}

CAltitudeMap::~CAltitudeMap(void)
{
}

int CAltitudeIndex::GetIndex() const
{
    return mIndex;
}

UINT CAltitudeIndex::GetID() const
{
    return mID;
}

const fstring& CAltitudeIndex::GetName() const
{
    return mName;
}

float CAltitudeIndex::GetHeight() const
{
    return mHeight;
}

const D3DXCOLOR& CAltitudeIndex::GetColor() const
{
    return mColor;
}

CAltitudeIndex::CAltitudeIndex()
: mIndex(0)
, mID(0)
, mHeight(0.f)
, mColor(BLACK)
{
}

CAltitudeIndex::CAltitudeIndex(const SCN_MapAltitudeIdx& rhs)
: mIndex(rhs.AltitudeIdx)
, mID(rhs.AltitudeID)
, mName(_F(rhs.AltitudeName))
, mHeight(rhs.AltitudeHigh)
, mColor(rhs.Color)
{
}

CAltitudeIndex::CAltitudeIndex(const SCN_MapAltitudeIdx1& rhs)
: mID(rhs.AltitudeID)
, mName(_F(rhs.AltitudeName))
, mHeight(rhs.AltitudeHigh)
, mColor(rhs.Color)
{
}

CAltitudeIndex::~CAltitudeIndex()
{
}

CAltitudeData::CAltitudeData()
: mValue(0)
{
}

CAltitudeData::CAltitudeData(unsigned char value)
: mValue(value)
{
}

CAltitudeData::CAltitudeData(int value)
: mValue(static_cast<unsigned char>(value))
{
}

CAltitudeData::~CAltitudeData()
{
}

void CAltitudeMap::Initialize(const SCN_Header& header, const SCN_MapNfo& mapNfo, const BYTE* data)
{
    // index
    switch (header.Version)
    {
        case 1:
        {
            const SCN_MapAltitudeIdx1Ptr p = (SCN_MapAltitudeIdx1Ptr)(data + mapNfo.AltitudeIdxOffset);
            mAltitudeIndices.assign(p, p + mapNfo.AltitudeIdxCount);
            break;
        }
        default:
        {
            const SCN_MapAltitudeIdxPtr p = (SCN_MapAltitudeIdxPtr)(data + mapNfo.AltitudeIdxOffset);
            mAltitudeIndices.assign(p, p + mapNfo.AltitudeIdxCount);
        }
    }
    // data
    size_t nfoCount = (mapNfo.GridHCount - 1) * (mapNfo.GridWCount - 1);
    switch (header.Version)
    {
        case 1:
        {
            const int* p = (int*)(data + mapNfo.AltitudeInforOffset);
            mAltitudeDatas.assign(p, p + nfoCount);
            break;
        }
        default:
        {
            const BYTE* p = (BYTE*)(data + mapNfo.AltitudeInforOffset);
            mAltitudeDatas.assign(p, p + nfoCount);
        }
    }
}

const AltitudeDataList& CAltitudeMap::GetDataList() const
{
    return mAltitudeDatas;
}

const AltitudeIndexList& CAltitudeMap::GetIndexList() const
{
    return mAltitudeIndices;
}

const CAltitudeData& CAltitudeMap::GetDataAt(size_t index) const
{
    assert(index < mAltitudeDatas.size());
    return mAltitudeDatas[index];
}

const CAltitudeIndex& CAltitudeMap::GetIndexAt(size_t index) const
{
    assert(index < mAltitudeIndices.size());
    return mAltitudeIndices[index];
}

bool CAltitudeMap::GetColorAt(DWORD* value, size_t index) const
{
    F_RETURN(value != NULL);
    (*value) = WHITE;

    F_RETURN(index < mAltitudeDatas.size());
    index = mAltitudeDatas[index];

    F_RETURN(index < mAltitudeIndices.size());
    (*value) = mAltitudeIndices[index].GetColor();

    return true;
}

bool CAltitudeMap::GetHeightAt(float *value, size_t index) const
{
    F_RETURN(value != NULL);
    (*value) = 0.f;

    F_RETURN(index < mAltitudeDatas.size());
    index = mAltitudeDatas[index];

    F_RETURN(index < mAltitudeIndices.size());
    (*value) = mAltitudeIndices[index].GetHeight();

    return true;
}

CAltitudeData::operator unsigned char () const
{
    return mValue;
}

CAltitudeData::operator unsigned int () const
{
    return mValue;
}

bool CAltitudeData::operator==(unsigned char rhs) const
{
    return (mValue == rhs);
}

bool CAltitudeData::operator==(int rhs) const
{
    return (mValue == rhs);
}

unsigned char CAltitudeData::GetValue() const
{
    return mValue;
}

} // namespace FDO