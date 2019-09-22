/**
 * @file FdoAltitudeMap.h
 * @author Yuming Ho
 */

#ifndef FdoAltitudeMapH
#define FdoAltitudeMapH

#include "SCN_Struct.h"

namespace FDO
{

class CAltitudeData
{
private:
    unsigned char mValue;

public:
    CAltitudeData();
    CAltitudeData(unsigned char value);
    CAltitudeData(int value);
    ~CAltitudeData();
    bool operator==(unsigned char rhs) const;
    bool operator==(int rhs) const;
    operator unsigned char () const;
    operator unsigned int () const;
    unsigned char GetValue() const;
};

typedef std::vector<CAltitudeData> AltitudeDataList;

class CAltitudeIndex
{
private:
    int mIndex;
    UINT mID;
    fstring mName;
    float mHeight;
    D3DXCOLOR mColor;

public:
    CAltitudeIndex();
    CAltitudeIndex(const SCN_MapAltitudeIdx& rhs);
    CAltitudeIndex(const SCN_MapAltitudeIdx1& rhs);
    ~CAltitudeIndex();

    int GetIndex() const;
    UINT GetID() const;
    const fstring& GetName() const;
    float GetHeight() const;
    const D3DXCOLOR& GetColor() const;
};

typedef std::vector<CAltitudeIndex> AltitudeIndexList;

class CAltitudeMap
{
private:
    AltitudeDataList    mAltitudeDatas;
    AltitudeIndexList   mAltitudeIndices;

public:
    CAltitudeMap(void);
    ~CAltitudeMap(void);

    void Initialize(const SCN_Header& header, const SCN_MapNfo& mapNfo, const BYTE* data);
    const AltitudeDataList& GetDataList() const;
    const AltitudeIndexList& GetIndexList() const;
    const CAltitudeData& GetDataAt(size_t index) const;
    const CAltitudeIndex& GetIndexAt(size_t index) const;
    bool GetHeightAt(float *value, size_t index) const;
    bool GetColorAt(DWORD* value, size_t index) const;
};

} // namespace FDO

#endif // FdoAltitudeMapH