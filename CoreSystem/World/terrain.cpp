/**
 * @file terrain.cpp
 * @author Frank Luna (C) All Rights Reserved
 * @modifier Yuming Ho
 * @brief Represents a 3D terrain.
 */
#include "stdafx.h"
#include "terrain.h"
#include "FdoCollisionMap.h"
#include "FdoAltitudeMap.h"
#include "SCN.h"
#include "FdoUpdateArgs.h"

#include <fstream>
#include <cmath>
#include "PhenixD3D9RenderSystem.h"
#include "Common.h"
//---------------------------------------------------------------------------
namespace FDO
{

const unsigned int GRID_RATIO = 4;

Terrain::Terrain(IDirect3DDevice9* d3dDevice)
: _device(d3dDevice)
{
}

Terrain::~Terrain()
{
}

bool Terrain::GetGameCell(HeightMapCellPtr heightMapCell, unsigned int CellX, 
                          unsigned int CellZ) const
{
    F_RETURN(heightMapCell != NULL);
    _LOOP(heightMapCell->count, pos)
    {
        heightMapCell->lines[pos] = NULL;
    }

    F_RETURN(CellX < GetGameCellWidth());
    F_RETURN(CellZ < GetGameCellDepth());

    unsigned int index = GetGameCellIndex(CellX, CellZ);

    unsigned int l[heightMapCell->count];
    l[0] = index * 2 + CellZ;
    l[1] = l[0] + 1;

    if (CellX + 1 == GetGameCellWidth())
    {
        l[2] = l[1] + 1;
    }
    else
    {
        index = GetGameCellIndex(CellX + 1, CellZ);
        l[2] = index * 2 + CellZ + 1;
    }

    if (CellZ + 1 == GetGameCellDepth())
    {
        if (CellX + 1 == GetGameCellWidth())
        {
            l[3] = l[2] + 1;
        }
        else
        {
            l[3] = l[1] + 1;
        }
    }
    else
    {
        index = GetGameCellIndex(CellX, CellZ + 1);
        l[3] = index * 2 + CellZ + 1;
    }

    F_RETURN(l[0] < mHeightMapLineList.size());
    F_RETURN(l[1] < mHeightMapLineList.size());
    F_RETURN(l[2] < mHeightMapLineList.size());
    F_RETURN(l[3] < mHeightMapLineList.size());

    _LOOP(heightMapCell->count, i)
    {
        heightMapCell->lines[i] = &mHeightMapLineList[l[i]];
    }

    return true;
}

void Terrain::CleanUp()
{
    base::CleanUp();

    //mCollisionMap = NULL;
    //mAltitudeMap = NULL;
    mHeightMap.clear();

    mCellWidth = 0;
    mCellDepth = 0;
    mGameVertexCount = 0;
    mGameTriangleCount = 0;
    mCellSpacing = 1.f;
    mHeightSpacing = 1.f;
}

void Terrain::SetCellSpacing(float value)
{
    mCellSpacing = value;
}

void Terrain::SetHeightSpacing(float value)
{
    mHeightSpacing = value;
}

bool Terrain::LoadRawFileInMemory(const BYTE* data, unsigned int size)
{
    // bmp file header information
    BITMAPINFOHEADER nfoHeader;
    memcpy(&nfoHeader, data + sizeof_BITMAPFILEHEADER, sizeof_BITMAPINFOHEADER);

    const BYTE* bmpData = data + sizeof_BITMAPFILEHEADER + sizeof_BITMAPINFOHEADER;

    SetCellWidth(nfoHeader.biWidth);
    SetCellDepth(nfoHeader.biHeight);
    
    // 一個Pixel的Byte數
    //unsigned int step = nfoHeader->biBitCount / 8;
    //unsigned int pos = 0;

    //for (unsigned int y = 0; y <= mCellDepth; ++y)
    //{
    //    for (unsigned int x = 0; x <= mCellWidth, ++x)
    //    {
    //        unsigned int index = y * mCellWidth + x;
    //        
    //        [index] = static_cast<float>(bmpData[pos]) * mHeightSpacing;
    //        
    //        pos += step;
    //    }
    //}
    return true;
}

//---------------------------------------------------------------------------
// OK
//---------------------------------------------------------------------------
bool Terrain::getGameCellVertices(int CellX, int CellZ,
      D3DXVECTOR3 *v0, D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR3 *v3)
{
    F_RETURN(v0 != NULL);
    F_RETURN(v1 != NULL);
    F_RETURN(v2 != NULL);
    F_RETURN(v3 != NULL);

    // v2   v3
    //  *---*
    //  | / |
    //  *---*
    // v0   v1
    F_RETURN(CellX >= 0);
    F_RETURN(CellZ >= 0);

    GameCell gameCell;
    F_RETURN(GetGameCell(&gameCell, CellX, CellZ));

	*v0 = *(gameCell.v0);
    *v1 = *(gameCell.v1);
    *v2 = *(gameCell.v2);
    *v3 = *(gameCell.v3);

    return true;
}

//---------------------------------------------------------------------------
// 判斷是否碰撞層 OK
//---------------------------------------------------------------------------
bool Terrain::IsValidGameCell(int CellX, int CellZ) const
{
    size_t index = GetGameCellIndex(CellX, CellZ);

    F_RETURN(mCollisionMap != NULL);
    return mCollisionMap->GetDataAt(index).IsValid();
}
//---------------------------------------------------------------------------
bool SGameCell::Hit(const RAY& ray)
{
    T_RETURN(Triangle(*v0, *v1, *v2).IsRayIntersect(ray));
    T_RETURN(Triangle(*v1, *v2, *v3).IsRayIntersect(ray));
    return false;
}
//---------------------------------------------------------------------------
void Terrain::SetCollisionMap(CollisionMapPtr value)
{
    mCollisionMap = value;
}

CollisionMapPtr Terrain::GetCollisionMap() const
{
    return mCollisionMap;
}

void Terrain::SetAltitudeMap(AltitudeMapPtr value)
{
    mAltitudeMap = value;
    UpdateHeightMap();
}

AltitudeMapPtr Terrain::GetAltitudeMap() const
{
    return mAltitudeMap;
}

void Terrain::UpdateHeightMap()
{
    mHeightMap.resize(GetGameVertexCount(), VEC3_ZERO);

    // coordinates to start generating vertices at
    int endX = GetWidth() / 2; // 128
    int endZ = GetDepth() / 2; // 128

    // coordinates to end generating vertices at
    int startX = -endX; // -128
    int startZ = -endZ; // -128

    int spacingH = GetWidth() / GetGameCellWidth(); // 2
    int spacingV = GetDepth() / GetGameCellDepth(); // 2

    for (int z=startZ, i=0; z<=endZ; z+=spacingV, ++i)
    {
        for (int x = startX, j=0; x<=endX; x+=spacingH, ++j)
        {
            int index = i * GetGameVertexWidth() + j;

            mHeightMap[index].x = static_cast<float>(x);
            mHeightMap[index].y = GetGameVertexHeight(index);
            mHeightMap[index].z = static_cast<float>(z);
        }
    }

    mHeightMapVertexListOutOfData = true;
}

const ImmediateVertexList& Terrain::GetHeightMapVertexList() const
{
    if (mHeightMapVertexListOutOfData)
    {
        UpdateHeightMapVertexList();
    }
    return mHeightMapVertexList;
}

void Terrain::UpdateHeightMapVertexList() const
{
    mHeightMapLineList.clear();
    mHeightMapVertexList.clear();

    _ULOOP(GetGameCellDepth(), y)
    {
        _ULOOP(GetGameCellWidth(), x)
        {
            GameCell gameCell;
            this->GetGameCell(&gameCell, x, y);
            this->AddLine(Line(*gameCell.v0, *gameCell.v1), WHITE);
            this->AddLine(Line(*gameCell.v0, *gameCell.v2), WHITE);

            if (x + 1 == GetGameCellWidth())
            {
                this->AddLine(Line(*gameCell.v1, *gameCell.v3), WHITE);
            }

            if (y + 1 == GetGameCellDepth())
            {
                this->AddLine(Line(*gameCell.v2, *gameCell.v3), WHITE);
            }
        }
    }

    _ULOOP(GetGameCellDepth(), y)
    {
        _ULOOP(GetGameCellWidth(), x)
        {
            unsigned int index = GetGameCellIndex(x, y);
            D3DCOLOR color;
            F_CONTINUE(mCollisionMap->GetColorAt(&color, index));

            HeightMapCell hmCell;
            F_CONTINUE(GetGameCell(&hmCell, x, y));
            SetHeightMapCellColor(hmCell, color);
        }
    }

    mHeightMapVertexListOutOfData = false;
}

void Terrain::SetHeightMapCellColor(const HeightMapCell& hmCell, D3DCOLOR color) const
{
    _LOOP(hmCell.count, i)
    {
        _LOOP(hmCell.lines[i]->count, j)
        {
            size_t index = hmCell.lines[i]->vertexIndex[j];
            assert(index < mHeightMapVertexList.size());
            mHeightMapVertexList[index].color = color;
        }
    }
}

void Terrain::AddLine(const Line& line, D3DCOLOR color) const
{
    HeightMapLine l;

    ImmediateVertex v;
    // v0
    v.position = line.pts[0];
    v.color = color;
    v.uv.x = 0.f;
    v.uv.y = 0.f;
    mHeightMapVertexList.push_back(v);

    l.vertexIndex[0] = mHeightMapVertexList.size() - 1;

    // v1
    v.position = line.pts[1];
    v.color = color;
    v.uv.x = 1.f;
    v.uv.y = 1.f;
    mHeightMapVertexList.push_back(v);
    l.vertexIndex[1] = mHeightMapVertexList.size() - 1;

    mHeightMapLineList.push_back(l);
}
/**
 * get the heights of the quad we're in:
 *
 * *---*---*
 * | 1 | 0 |
 * *---*---*
 * | 3 | 2 |
 * *---*---*
 */
float Terrain::GetGameVertexHeight(int index) const
{
    int x = index % GetGameVertexWidth();
    int y = index / GetGameVertexWidth();

    int c0 = GetGameCellIndex(x, y);
    int c1 = c0 - 1;
    int c2 = GetGameCellIndex(x, y-1);
    int c3 = c2 - 1;

    int count = 0;
    float totalHeight = 0.f;
    float Result = 0.f;
    float height = 0.f;

    if (GetGameCellHeight(&height, c0, y))
    {
        if (Result < height)
        {
            Result = height;
        }
        //++count;
        //totalHeight += height;
    }

    if (GetGameCellHeight(&height, c1, y))
    {
        if (Result < height)
        {
            Result = height;
        }
        //++count;
        //totalHeight += height;
    }

    if (GetGameCellHeight(&height, c2, y-1))
    {
        if (Result < height)
        {
            Result = height;
        }
        //++count;
        //totalHeight += height;
    }

    if (GetGameCellHeight(&height, c3, y-1))
    {
        if (Result < height)
        {
            Result = height;
        }
        //++count;
        //totalHeight += height;
    }

    //assert(count > 0);
    //Result = totalHeight / static_cast<float>(count);

    return Result;
}

bool Terrain::GetGameCellHeight(float* height, int index, unsigned int CellZ) const
{
    F_RETURN(height != NULL);
    (*height) = 0.f;

    F_RETURN(IsCalcableGameCell(index, CellZ));

    mAltitudeMap->GetHeightAt(height, index);
    return true;
}

bool Terrain::IsCalcableGameCell(int index, unsigned int CellZ) const
{
    F_RETURN(index >= 0);

    unsigned int newIndex = static_cast<unsigned int>(index);
    F_RETURN(mAltitudeMap != NULL);
    F_RETURN(newIndex < mAltitudeMap->GetDataList().size());

    F_RETURN((newIndex / GetGameCellWidth()) == CellZ);

    return true;
}

unsigned int Terrain::GetCellWidth() const
{
    return mCellWidth;
}

unsigned int Terrain::GetCellDepth() const
{
    return mCellDepth;
}

unsigned int Terrain::GetVertexWidth() const
{
    return GetCellWidth() + 1;
}

unsigned int Terrain::GetVertexDepth() const
{
    return GetCellDepth() + 1;
}

unsigned int Terrain::GetGameCellWidth() const
{
    return GetCellWidth() * GRID_RATIO;
}

unsigned int Terrain::GetGameCellDepth() const
{
    return GetCellDepth() * GRID_RATIO;
}

unsigned int Terrain::GetGameVertexWidth() const
{
    return GetGameCellWidth() + 1;
}

unsigned int Terrain::GetGameVertexDepth() const
{
    return GetGameCellDepth() + 1;
}

unsigned int Terrain::GetWidth() const
{
    return GetCellWidth() * static_cast<unsigned int>(mCellSpacing);
}

unsigned int Terrain::GetDepth() const
{
    return GetCellDepth() * static_cast<unsigned int>(mCellSpacing);
}

unsigned int Terrain::GetCellCount() const
{
    return GetCellWidth() * GetCellDepth();
}

unsigned int Terrain::GetVertexCount() const
{
    return GetVertexWidth() * GetVertexDepth();
}

unsigned int Terrain::GetGameCellCount() const
{
    return GetGameCellWidth() * GetGameCellDepth();
}

unsigned int Terrain::GetGameVertexCount() const
{
    //return GetGameVertexWidth() * GetGameVertexDepth();
    return mGameVertexCount;
}

unsigned int Terrain::GetGameCellIndex(unsigned int x, unsigned int y) const
{
    return GetGameCellWidth() * y + x;
}

unsigned int Terrain::GetGameVertexIndex(unsigned int x, unsigned int y) const
{
    return GetGameVertexWidth() * y + x;
}

unsigned int Terrain::GetCellIndex(unsigned int x, unsigned int y) const
{
    return GetCellWidth() * y + x;
}

unsigned int Terrain::GetVertexIndex(unsigned int x, unsigned int y) const
{
    return GetVertexWidth() * y + x;
}

void Terrain::SetCellWidth(unsigned int value)
{
    mCellWidth = value;
    mGameVertexCount = GetGameVertexWidth() * GetGameVertexDepth();
    mGameTriangleCount = GetGameCellWidth() * GetGameCellDepth() * 2;
}

void Terrain::SetCellDepth(unsigned int value)
{
    mCellDepth = value;
    mGameVertexCount = GetGameVertexWidth() * GetGameVertexDepth();
    mGameTriangleCount = GetGameCellWidth() * GetGameCellDepth() * 2;
}

unsigned int Terrain::GetGameIndexCount() const
{
    return mGameTriangleCount * 3;
}

//bool Terrain::UpdateHeightMapIB() const
//{
//    SafeRelease(mHeightMapIB);
//
//    F_RETURN(_device != NULL);
//
//    HRESULT hr;
//    hr = _device->CreateIndexBuffer
//    (
//        GetGameIndexCount() * sizeof_WORD,
//		D3DUSAGE_WRITEONLY,
//        D3DFMT_INDEX16,
//        D3DPOOL_MANAGED,
//        &mHeightMapIB,
//        0
//    );
//
//    F_RETURN(hr == S_OK);
//
//    WORD* indices = NULL;
//
//	hr = mHeightMapIB->Lock(0, 0, (void**)&indices, 0);
//    F_RETURN(hr == S_OK);
//
//	// index to start of a group of 6 indices that describe the
//	// two triangles that make up a quad
//	unsigned int baseIndex = 0;
//
//	// loop through and compute the triangles of each quad
//	_ULOOP(GetGameCellDepth(), y)
//	{
//		_ULOOP(GetGameCellWidth(), x)
//		{
//		    // 第一個三角形
//			indices[baseIndex    ] = (y+1) * GetGameVertexWidth() + x;
//			indices[baseIndex + 1] =  y    * GetGameVertexWidth() + x + 1;
//			indices[baseIndex + 2] =  y    * GetGameVertexWidth() + x;
//
//            // 第二個三角形
//			indices[baseIndex + 3] = (y+1) * GetGameVertexWidth() + x + 1;
//			indices[baseIndex + 4] =  y    * GetGameVertexWidth() + x + 1;
//			indices[baseIndex + 5] = (y+1) * GetGameVertexWidth() + x;
//
//			// next quad
//			baseIndex += 6;
//		}
//	}
//
//    hr = mHeightMapIB->Unlock();
//    F_RETURN(hr == S_OK);
//
//    mHeightMapIBOutOfData = false;
//    return true;
//}

//bool Terrain::UpdateHeightMapVB() const
//{
//    F_RETURN(_device != NULL);
//
//    SafeRelease(mHeightMapVB);
//
//    HRESULT hr = S_OK;
//    hr = _device->CreateVertexBuffer
//    (
//        GetGameVertexCount() * HeightMapVertex::sizeof_HeightMapVertex,
//		D3DUSAGE_WRITEONLY,
//        HeightMapVertex::FVF,
//        D3DPOOL_MANAGED,
//        &mHeightMapVB,
//        0
//    );
//
//    F_RETURN(hr == S_OK);
//
//	HeightMapVertexPtr v = NULL;
//	hr = mHeightMapVB->Lock(0, 0, (void**)&v, 0); // 取得緩衝器的記憶體
//    F_RETURN(hr == S_OK);
//
//	// coordinates to start generating vertices at
//	int endX = GetGameCellWidth() / 2;
//	int endZ = GetGameCellDepth() / 2;
//	// coordinates to end generating vertices at
//	int startX = -endX;
//	int startZ = -endZ;
//	// compute the increment size of the texture coordinates
//	// from one vertex to the next.
//	float uCoordIncrementSize = 1.f / (float)GetGameCellWidth();
//	float vCoordIncrementSize = 1.f / (float)GetGameCellDepth();
//
//    for (int z=startZ, i=0; z <= endZ; ++z, ++i)
//    {
//	    for (int x=startX, j=0; x <= endX; ++x, ++j)
//	    {
//		    // compute the correct index into the vertex buffer and heightmap
//		    // based on where we are in the nested loop.
//		    int index = i * GetGameVertexWidth() + j;
//
//            //v[index].Position.x = static_cast<float>(x);
//            //v[index].Position.y = GetGameVertexHeight(index);
//            //v[index].Position.z = static_cast<float>(z);
//            v[index].Position   = mHeightMap[index];
//            v[index].Normal     = VEC3_UNIT_Y;
//            v[index].UV.x       = (float)j * uCoordIncrementSize;
//            v[index].UV.y       = 1.f - (float)i * vCoordIncrementSize;
//            v[index].Color      = GetGameVertexColor(index);
//	    }
//    }
//
//    hr = mHeightMapVB->Unlock();
//    F_RETURN(hr == S_OK);
//
//    mHeightMapVBOutOfData = false;
//    return true;
//}

DWORD Terrain::GetGameVertexColor(int index) const
{
    int x = index % GetGameVertexWidth();
    int y = index / GetGameVertexWidth();

    int c0 = GetGameCellIndex(x, y);
    int c1 = c0 - 1;
    int c2 = GetGameCellIndex(x, y-1);
    int c3 = c2 - 1;

    DWORD Result = WHITE;

    if (GetGameCellColor(&Result, c0, y  )) return Result;
    if (GetGameCellColor(&Result, c1, y  )) return Result;
    if (GetGameCellColor(&Result, c2, y-1)) return Result;
    if (GetGameCellColor(&Result, c3, y-1)) return Result;

    return Result;
}

bool Terrain::GetGameCellColor(DWORD* color, int index, unsigned int CellZ) const
{
    F_RETURN(color != NULL);
    (*color) = WHITE;

    F_RETURN(IsCalcableGameCell(index, CellZ));

    mAltitudeMap->GetColorAt(color, index);
    return true;
}

bool Terrain::GetGameCell(GameCellPtr gameCell, unsigned int CellX, unsigned int CellZ) const
{
    F_RETURN(gameCell != NULL);
    gameCell->v0 = NULL;
    gameCell->v1 = NULL;
    gameCell->v2 = NULL;
    gameCell->v3 = NULL;
    gameCell->index = 0;

    F_RETURN(CellX < GetGameCellWidth());
    F_RETURN(CellZ < GetGameCellDepth());

    // v2   v3
    //  *---*
    //  | / |
    //  *---*
    // v0   v1
    unsigned int v0 = GetGameVertexIndex(CellX, CellZ);
    unsigned int v1 = v0 + 1;
    unsigned int v2 = GetGameVertexIndex(CellX, CellZ + 1);
    unsigned int v3 = v2 + 1;

    F_RETURN(v0 < mHeightMap.size());
    F_RETURN(v1 < mHeightMap.size());
    F_RETURN(v2 < mHeightMap.size());
    F_RETURN(v3 < mHeightMap.size());

    gameCell->v0 = const_cast<D3DXVECTOR3*>(&mHeightMap[v0]);
    gameCell->v1 = const_cast<D3DXVECTOR3*>(&mHeightMap[v1]);
    gameCell->v2 = const_cast<D3DXVECTOR3*>(&mHeightMap[v2]);
    gameCell->v3 = const_cast<D3DXVECTOR3*>(&mHeightMap[v3]);
    gameCell->index = GetGameCellIndex(CellX, CellZ);

    return true;
}

float Terrain::GetGameHeight(unsigned int CellX, unsigned int CellZ) const
{
    float Result = 0.f;

    if (mAltitudeMap == NULL)
    {
        return Result;
    }

    mAltitudeMap->GetHeightAt(&Result, GetGameCellIndex(CellX, CellZ));

    return Result;
}

float Terrain::GetGameHeight(float PosX, float PosZ) const
{
    float Result = 0.f;

	PosX *= 0.5f;
	PosZ *= 0.5f;

    // 小於x的最大整數
	int CellX = static_cast<int>(::floor(PosX));
    // 小於z的最大整數
	int CellZ = static_cast<int>(::floor(PosZ));

    if (CellX < 0) return Result;
    if (CellZ < 0) return Result;

    GameCell gameCell;
    if (GetGameCell(&gameCell, CellX, CellZ) == false)
    {
        return Result;
    }

    //mAltitudeMap->GetHeightAt(&Result, gameCell.index);
    //return Result;

	// get the heights of the quad we're in:
	//
    // C     D
    //  *---*
    //  | / |
    //  *---*
    // A     B
    float A = gameCell.v0->y;
	float B = gameCell.v1->y;
	float C = gameCell.v2->y;
	float D = gameCell.v3->y;

	//
	// Find the triangle we are in:
	//

	// Translate by the transformation that takes the upper-left
	// corner of the cell we are in to the origin.  Recall that our 
	// cellspacing was nomalized to 1.  Thus we have a unit square
	// at the origin of our +x -> 'right' and +z -> 'down' system.
	float dx = PosX - static_cast<float>(CellX);
	float dz = PosZ - static_cast<float>(CellZ);

	// Note the below compuations of u and v are unneccessary, we really
	// only need the height, but we compute the entire vector to emphasis
	// the books discussion.

	if(dz < 1.0f - dx)  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {A}, plus the heights
		// found by interpolating on each vector u and v.
		Result = A + D3DLerp(0.f, uy, dx) + D3DLerp(0.f, vy, dz);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {D}, plus the heights
		// found by interpolating on each vector u and v.
		Result = D + D3DLerp(0.f, uy, 1.f - dx) + D3DLerp(0.f, vy, 1.f - dz);
	}

    return Result;
}

bool Terrain::GetGameVertexXY(unsigned int* x, unsigned int* y, unsigned int index) const
{
    F_RETURN(x != NULL);
    F_RETURN(y != NULL);
    F_RETURN(index < GetGameVertexCount());
    *x = index % GetGameVertexWidth();
    *y = index / GetGameVertexWidth();
    return true;
}

bool Terrain::GetGameCellXY(unsigned int* x, unsigned int* y, unsigned int index) const
{
    F_RETURN(x != NULL);
    F_RETURN(y != NULL);
    F_RETURN(index < GetGameCellCount());
    *x = index % GetGameCellWidth();
    *y = index / GetGameCellWidth();
    return true;
}

bool Terrain::Update(const UpdateArgsPtr &args)
{
    return true;
}

bool Terrain::Draw()
{
    //_device->SetTransform(D3DTS_WORLD, &(this->GetWorldMatrix()));
    //_device->SetStreamSource(0, this->GetHeightMapVB(), 0, HeightMapVertex::sizeof_HeightMapVertex);
    //_device->SetFVF(HeightMapVertex::FVF);
    //_device->SetIndices(this->GetHeightMapIB());
    //_device->SetMaterial(&(this->GetMaterial())); // 設定質料
    //_device->SetTexture(0, mTextures[mActiveTexture]); // 設定材質
    //_device->DrawIndexedPrimitive
    //(
    //    D3DPT_TRIANGLELIST,
    //    0,
    //    0,
    //    mGameVertexCount,
    //    0,
    //    mGameTriangleCount
    //);
    return false;
}

void Terrain::RegisterForDrawing(SCNPtr scn)
{
    if (this->IsVisible())
    {
        scn->RegisterForDrawing(DRAWKIND::TERRAIN, this);
    }
}

bool Terrain::GetCell(const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir,
    int* pCellX, int* pCellY , bool Xdir , bool Zdir)
{
    // 限定搜尋範圍
    const float RANGE = 40.0f;
    // 紀錄原始位置
    D3DXVECTOR2 vSrc(static_cast<float>(*pCellX), static_cast<float>(*pCellY));
    // 紀錄目的格與來源格距離
    float fCurrDist = 0.0f;
    // 找到目的格旗標
    bool bHit = false;

	int X = 0 ; 
	int EndX = 0 ; 
	int Y = 0 ; 
	int EndY = 0 ; 


	if(Zdir)
	{
		Y = static_cast<int>(vSrc.y - 10.0f) ; 
		EndY = static_cast<int>(vSrc.y + RANGE);
	}
	else
	{
		EndY = static_cast<int>(vSrc.y + 10.0f) ; 
		Y = static_cast<int>(vSrc.y - RANGE);

	}
	
  for (; Y < EndY; ++Y)
    {
        // 範圍判斷
        F_CONTINUE(Y >= 0);
        F_CONTINUE(Y < (int)GetGameCellDepth());

		if(Xdir)
		{
			X = static_cast<int>(vSrc.x - 10.0f) ; 
			EndX = static_cast<int>(vSrc.x + RANGE);
		}
		else
		{
			EndX = static_cast<int>(vSrc.x + 10.0f) ; 
			X = static_cast<int>(vSrc.x - RANGE);

		}
        for (; X < EndX; ++X)
        {
            // 範圍判斷
            F_CONTINUE(X >= 0);
            F_CONTINUE(X < (int)GetGameCellWidth());
            // 不合法的格子判斷(碰撞層)
            F_CONTINUE(IsValidGameCell(X, Y));

            GameCell gameCell;
            F_CONTINUE(GetGameCell(&gameCell, X, Y));
            F_CONTINUE(gameCell.Hit(RAY(vRayOrig, vRayDir)));
            // 紀錄目的格
            D3DXVECTOR2 vDest(static_cast<float>(X), static_cast<float>(Y));
            // 計算目的格與來源格距離
            float fDist = D3DXVec2LengthSq(&(vDest - vSrc));

            if ((fDist > fCurrDist) && (fCurrDist > 0.0f))
            {
                continue;
            }

            //HeightMapCell hmCell;
            //if (GetGameCell(&hmCell, mSeleteCell.x, mSeleteCell.y))
            //{
            //    D3DCOLOR color;
            //    unsigned int index = GetGameCellIndex(mSeleteCell.x, mSeleteCell.y);

            //    if (mCollisionMap->GetColorAt(&color, index))
            //    {
            //        SetHeightMapCellColor(hmCell, color);
            //    }
            //    else
            //    {
            //        SetHeightMapCellColor(hmCell, WHITE);
            //    }
            //}
            //mSeleteCell = TPoint(X, Y);

            *pCellX = X;
            *pCellY = Y;
            fCurrDist = fDist;
            bHit = true;
        }
    }

    // 巢狀迴圈搜尋，以來源為中心點，搜尋設定的範圍
    //int Y    = static_cast<int>(vSrc.y - RANGE);
    //int EndY = static_cast<int>(vSrc.y + RANGE);
    //for (; Y < EndY; ++Y)
    //{
    //    // 範圍判斷
    //    F_CONTINUE(Y >= 0);
    //    F_CONTINUE(Y < (int)GetGameCellDepth());

    //    int X    = static_cast<int>(vSrc.x - RANGE);
    //    int EndX = static_cast<int>(vSrc.x + RANGE);
    //    for (; X < EndX; ++X)
    //    {
    //        // 範圍判斷
    //        F_CONTINUE(X >= 0);
    //        F_CONTINUE(X < (int)GetGameCellWidth());
    //        // 不合法的格子判斷(碰撞層)
    //        F_CONTINUE(IsValidGameCell(X, Y));

    //        GameCell gameCell;
    //        F_CONTINUE(GetGameCell(&gameCell, X, Y));
    //        F_CONTINUE(gameCell.Hit(RAY(vRayOrig, vRayDir)));
    //        // 紀錄目的格
    //        D3DXVECTOR2 vDest(static_cast<float>(X), static_cast<float>(Y));
    //        // 計算目的格與來源格距離
    //        float fDist = D3DXVec2LengthSq(&(vDest - vSrc));

    //        if ((fDist > fCurrDist) && (fCurrDist > 0.0f))
    //        {
    //            continue;
    //        }

    //        HeightMapCell hmCell;
    //        if (GetGameCell(&hmCell, mSeleteCell.x, mSeleteCell.y))
    //        {
    //            D3DCOLOR color;
    //            unsigned int index = GetGameCellIndex(mSeleteCell.x, mSeleteCell.y);

    //            if (mCollisionMap->GetColorAt(&color, index))
    //            {
    //                SetHeightMapCellColor(hmCell, color);
    //            }
    //            else
    //            {
    //                SetHeightMapCellColor(hmCell, WHITE);
    //            }
    //        }
    //        mSeleteCell = TPoint(X, Y);

    //        *pCellX = X;
    //        *pCellY = Y;
    //        fCurrDist = fDist;
    //        bHit = true;
    //    }
    //}

    return bHit;
}

void Terrain::DrawHelpers(SCNPtr scn)
{
    HeightMapCell hmCell;
    if (GetGameCell(&hmCell, mSeleteCell.x, mSeleteCell.y))
    {
        SetHeightMapCellColor(hmCell, scn->GetSelectionColor());
    }

    DWORD dwLight;
    _device->GetRenderState(D3DRS_LIGHTING, &dwLight);
    _device->SetRenderState(D3DRS_LIGHTING, FALSE); //關閉光源
    _device->SetTransform(D3DTS_WORLD, &(this->GetWorldMatrix()));
    _device->SetFVF(fvfof_ImmediateVertex);
    _device->SetTexture(0, NULL);
    _device->DrawPrimitiveUP
    (
        D3DPT_LINELIST,
        (UINT)GetHeightMapVertexList().size() / 2,
        &(GetHeightMapVertexList().front()),
        sizeof_ImmediateVertex
    );
    _device->SetRenderState(D3DRS_LIGHTING, dwLight);
}

int Terrain::getGameCellSpacing()
{
    return static_cast<int>(mCellSpacing);
}
//---------------------------------------------------------------------------
} // namespace FDO