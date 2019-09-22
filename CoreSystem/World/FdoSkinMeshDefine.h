#ifndef FdoSkinMeshDefineH
#define FdoSkinMeshDefineH

#include <d3d9.h>
#include <d3dx9.h>
//#include <vector>
#include "elementdef.h" // ref: ObjFactory

namespace FDO
{

// enum for various skinning modes possible
namespace SKINNING_METHOD
{
    enum E
    {
        D3DNONINDEXED,
        D3DINDEXED,
        SOFTWARE,
        D3DINDEXEDVS,
        D3DINDEXEDHLSLVS,
        NONE
    };
}

//---------------------------------------------------------------------------
// Name: struct D3DXFRAME_DERIVED
// Desc: Structure derived from D3DXFRAME so we can add some app-specific
//       info that will be stored with each frame
//---------------------------------------------------------------------------
typedef struct D3DXFRAME_DERIVED : public D3DXFRAME
{
    D3DXMATRIXA16 CombinedTransformationMatrix;

    void* operator new (size_t);
    void operator delete (void* p);

private:
    typedef ObjFactory<D3DXFRAME_DERIVED> FrameFactory;
    static FrameFactory sFrameFactory;
} *PD3DXFRAME_DERIVED, *LPD3DXFRAME_DERIVED;

//---------------------------------------------------------------------------
// Name: struct D3DXMESHCONTAINER_DERIVED
// Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
//       info that will be stored with each mesh
//---------------------------------------------------------------------------
typedef struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
{
  //  LPDIRECT3DTEXTURE9*  ppTextures;       // array of textures, entries are NULL if no texture specified

	std::vector<sImage*>	imageVec ; 

    // SkinMesh info
    LPD3DXMESH           pOrigMesh;
    LPD3DXATTRIBUTERANGE pAttributeTable;
    DWORD                NumAttributeGroups;
    DWORD                NumInfl;
    LPD3DXBUFFER         pBoneCombinationBuf;
    D3DXMATRIX**         ppBoneMatrixPtrs;
    D3DXMATRIX*          pBoneOffsetMatrices;
    DWORD                NumPaletteEntries;
    bool                 UseSoftwareVP;
    DWORD                iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning

    HRESULT SetupBoneMatrixPointers(D3DXFRAME* pFrameRoot);

    void* operator new (size_t);
    void operator delete (void* p);

private:
    typedef ObjFactory<D3DXMESHCONTAINER_DERIVED> MeshContainerFactory;
    static MeshContainerFactory sMeshContainerFactory;
} *PD3DXMESHCONTAINER_DERIVED, *LPD3DXMESHCONTAINER_DERIVED;

const unsigned int SIZE_OF_D3DXMESHCONTAINER_DERIVED = sizeof(D3DXMESHCONTAINER_DERIVED);

} // namespace FDO

#endif // FdoSkinMeshDefineH