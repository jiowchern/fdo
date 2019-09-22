/**
 * @file Ocean.h
 * @author Yuming Ho
 * @brief sea object
 */
#ifndef OceanH
#define OceanH

#include "FdoDrawable.h"
#include <vector>
//----------------------------------------------------------------------------
namespace FDO
{
//----------------------------------------------------------------------------
typedef struct OceanVertex
{
    D3DXVECTOR3 Position; // The original position of the vertex
    D3DXVECTOR3 NORMAL;

    float _u, _v; // §÷½è¶b
    static const DWORD FVF;
} *OceanVertexPtr;

const UINT sizeof_OceanVertex = sizeof(OceanVertex);
const UINT NUMOF_WAVE_POINT = 5;

class Ocean : public CDrawable
{
    typedef CDrawable base;

public:
    Ocean();
    virtual ~Ocean();

public: // CDrawable attribute
    virtual bool Draw();
    virtual bool Update(const UpdateArgsPtr &args);
    virtual void RegisterForDrawing(SCNPtr scn);
    virtual void CleanUp();

public: // attribute
    void SetDevice(LPDIRECT3DDEVICE9 value);
    LPDIRECT3DDEVICE9 GetDevice() const;

    void SetColor(const D3DXCOLOR& value);
    const D3DXCOLOR& GetColor() const;

    void SetNumVertsPerRow(int value);
    void SetNumVertsPerCol(int value);

    int GetNumCellsPerRow() const;
    int GetNumCellsPerCol() const;
    int GetNumIndices() const;

    void SetTexCount(size_t value);
    size_t GetTexCount() const;
    void SetTextureSpeed(float value);
    void SetTexture(size_t index, LPDIRECT3DTEXTURE9 value);
    void SetTU(float value);
    void SetTV(float value);

    void SetAmplitude(float value);
    void SetInterval(float value);

private:
    LPDIRECT3DDEVICE9 mDevice;
    // ®¶´T´T«×
    float mAmplitude;
    // ®¶´TÀW²v
    float mInterval;
    // §÷½è¤Á´«ÀW²v
    float mTextureSpeed;
    // ÃC¦â
    D3DXCOLOR mColor;
    mutable D3DMATERIAL9 mMaterial;
    mutable bool mMaterialOutOfData;
    // vertex buffer
    mutable IDirect3DVertexBuffer9* mVertexBuffer;
    mutable bool mVertexBufferOutOfData;
    // index buffer
    mutable IDirect3DIndexBuffer9* mIndexBuffer;
    mutable bool mIndexBufferOutOfData;

    int mNumVertsPerRow;
    int mNumVertsPerCol;

    UINT mNumVertices;
    UINT mNumTriangles;

    float mTU;
    float mTV;

    UINT mActiveTexture;
    std::vector<LPDIRECT3DTEXTURE9> mTextures;

private:
    const D3DMATERIAL9& GetMaterial() const;
    IDirect3DIndexBuffer9* GetIndexBuffer() const;
    IDirect3DVertexBuffer9* GetVertexBuffer() const;
    bool UpdateIndexBuffer() const;
    bool UpdateVertexBuffer() const;
};
//----------------------------------------------------------------------------
} // end of namespace FDO
//----------------------------------------------------------------------------
#endif // OceanH