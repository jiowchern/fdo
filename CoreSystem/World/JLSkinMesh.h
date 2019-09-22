/**
 * @file JLSkinMesh.h
 * @modifier Yuming Ho
 */
#ifndef JLSkinMeshH
#define JLSkinMeshH

#include "FdoWorldPreRequest.h"
#include "Object.h"

namespace FDO
{

/**
 * @class JLSkinMesh
 */
class JLSkinMesh : public Object
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;
    D3DCAPS9                m_d3dCaps;

    LPD3DXFRAME m_pFrameRoot;
    LPD3DXANIMATIONCONTROLLER m_pAnimController;

    mutable bool mBoundOutOfData;
    mutable D3DXVECTOR3 mMinBounds;
    mutable D3DXVECTOR3 mMaxBounds;

    D3DXVECTOR3             m_vObjectCenter;
    float                   m_fObjectRadius;
    float                   m_fAlpha;

    //METHOD                  m_SkinningMethod;
    LPD3DXEFFECT            m_pEffect;
    D3DXMATRIXA16*          m_pBoneMatrices;
    UINT                    m_NumBoneMatricesMax;
    IDirect3DVertexShader9* m_pIndexedVertexShader[4];
    DWORD                   m_dwBehaviorFlags;

public:
    JLSkinMesh(const fstring& fileName);
    virtual ~JLSkinMesh();

    bool    Create( BYTE *Data, UINT DataSize );
    void    Clear();
    void    Destory();

    HRESULT LoadFromFile( char* szXFile, char* szFilePath = NULL, char* szFxFile = NULL );
    HRESULT RefreshBoundingVolumes();
    //void ChangeSkinMethod(const METHOD SkinMethod);

// <yuming>
public: // attribute
    /// device
    void SetDevice(LPDIRECT3DDEVICE9 value);
    LPDIRECT3DDEVICE9 GetDevice() const;
    /// get
    LPD3DXFRAME GetFrameRoot() const;
    float GetBoundingRadius() const;
    void GetBoundingBox(D3DXVECTOR3 * pvMin, D3DXVECTOR3 * pvMax);
    const D3DXVECTOR3& GetBoundingCenter() const;

public:
    SkinMeshInstPtr CreateInstance();

private: // helper
    void UpdateBounds() const;
    HRESULT SetupBoneMatrixPointers(D3DXFRAME* pFrame);
// </yuming>
};
//---------------------------------------------------------------------------
} // namespace FDO
//---------------------------------------------------------------------------
#endif // end of JLSkinMeshH
