/**
 * @file terrain.h
 * @author Frank Luna
 * @modifier Yuming Ho
 * @brief Represents a 3D terrain
 */
#ifndef terrainH
#define terrainH

#include "FdoDrawable.h"
#include "FdoWorldPreRequest.h"

//---------------------------------------------------------------------------
namespace FDO
{

typedef std::vector<D3DXVECTOR3> HeightMap;
//---------------------------------------------------------------------------
typedef struct SGameCell
{
    bool Hit(const RAY& ray);

    D3DXVECTOR3* v0;
    D3DXVECTOR3* v1;
    D3DXVECTOR3* v2;
    D3DXVECTOR3* v3;

    unsigned int index;
} GameCell, *GameCellPtr;

typedef struct SHeightMapLine
{
    enum { count = 2 };
    size_t vertexIndex[2];
} HeightMapLine, *HeightMapLinePtr;
typedef std::vector<HeightMapLine> HeightMapLineList;

typedef struct SHeightMapCell
{
    enum { count = 4 };
    HeightMapLinePtr lines[count];
} HeightMapCell, *HeightMapCellPtr;
//---------------------------------------------------------------------------
class Terrain : public CDrawable
{
    typedef CDrawable base;

private:
    /// d3d
	IDirect3DDevice9* _device;
    /// collision map
    CollisionMapPtr mCollisionMap;
    /// altitude map
    AltitudeMapPtr mAltitudeMap;
    /// height map
    HeightMap mHeightMap;
    /// number of cell count per row
    unsigned int mCellWidth;
    /// number of cell count per column
    unsigned int mCellDepth;
    /// should be 66049
    unsigned int mGameVertexCount;
    /// game cell count times 2 should be 131072
    unsigned int mGameTriangleCount;

    float mCellSpacing;
    float mHeightSpacing;

    mutable HeightMapLineList mHeightMapLineList;
    mutable ImmediateVertexList mHeightMapVertexList;
    mutable bool mHeightMapVertexListOutOfData;
    void UpdateHeightMapVertexList() const;
    const ImmediateVertexList& GetHeightMapVertexList() const;
    TPoint mSeleteCell;

private: // helper
    void UpdateHeightMap();
    float GetGameVertexHeight(int index) const;
    bool GetGameCellHeight(float* height, int index, unsigned int CellZ) const;
    bool GetGameCellColor(DWORD* value, int index, unsigned int CellZ) const;
    /// 判斷是否可計算的遊戲格子
    bool IsCalcableGameCell(int index, unsigned int CellZ) const;
    DWORD GetGameVertexColor(int index) const;
    bool GetGameCell(GameCellPtr gameCell, unsigned int CellX, unsigned int CellZ) const;
    bool GetGameCell(HeightMapCellPtr heightMapCell, unsigned int CellX, unsigned int CellZ) const;

    void SetCellWidth(unsigned int value);
    void SetCellDepth(unsigned int value);

    unsigned int GetWidth() const;
    unsigned int GetDepth() const;
    unsigned int GetCellWidth() const;
    unsigned int GetCellDepth() const;
    unsigned int GetVertexWidth() const;
    unsigned int GetVertexDepth() const;
    unsigned int GetCellCount() const;
    unsigned int GetVertexCount() const;
    unsigned int GetCellIndex(unsigned int x, unsigned int y) const;
    unsigned int GetVertexIndex(unsigned int x, unsigned int y) const;

    unsigned int GetGameCellWidth() const;
    unsigned int GetGameCellDepth() const;
    unsigned int GetGameVertexWidth() const;
    unsigned int GetGameVertexDepth() const;
    unsigned int GetGameCellCount() const;
    unsigned int GetGameVertexCount() const;
    unsigned int GetGameCellIndex(unsigned int x, unsigned int y) const;
    unsigned int GetGameVertexIndex(unsigned int x, unsigned int y) const;
    unsigned int GetGameIndexCount() const;
    bool GetGameVertexXY(unsigned int* x, unsigned int* y, unsigned int index) const;
    bool GetGameCellXY(unsigned int* x, unsigned int* y, unsigned int index) const;

    void AddLine(const Line& line, D3DCOLOR color) const;
    void SetHeightMapCellColor(const HeightMapCell& hmCell, D3DCOLOR color) const;

public: // CDrawable stuff
    virtual bool Update(const UpdateArgsPtr &args);
    virtual bool Draw();
    virtual void RegisterForDrawing(SCNPtr scn);
    virtual void CleanUp();
    virtual void DrawHelpers(SCNPtr scn);

public:
	/// default constructor
	Terrain(IDirect3DDevice9* d3dDevice);
	/// defaultdestructor
	virtual ~Terrain();

    /// collision map
    void SetCollisionMap(CollisionMapPtr value);
    CollisionMapPtr GetCollisionMap() const;
    /// height map
    void SetAltitudeMap(AltitudeMapPtr value);
    AltitudeMapPtr GetAltitudeMap() const;
    /// 判斷小格是否碰撞層
    bool IsValidGameCell(int CellX, int CellZ) const;
    float GetGameHeight(float PosX, float PosZ) const;
    float GetGameHeight(unsigned int CellX, unsigned int CellZ) const;
    bool LoadRawFileInMemory(const BYTE* data, unsigned int size);
    void SetCellSpacing(float value);
    void SetHeightSpacing(float value);
    bool GetCell(const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, int* pCellX, int* pCellY , bool XDir , bool ZDir);
    /// 小格數
    int getNumofGameCellH() { return GetGameCellWidth(); }
    int getNumofGameCellV() { return GetGameCellDepth(); }
    /// 取得遊戲格子縮放比例
    int getGameCellSpacing();
	/// 取得遊戲格子四個頂點的值
    bool getGameCellVertices(int CellX, int CellZ,
        D3DXVECTOR3 *v0, D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR3 *v3);
};
//---------------------------------------------------------------------------
} // namespace FDO
//---------------------------------------------------------------------------
#endif // terrainH
