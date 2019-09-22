#include "StdAfx.h"
#include "FdoSkinMeshDefine.h"

namespace FDO
{

// static member
D3DXFRAME_DERIVED::FrameFactory D3DXFRAME_DERIVED::sFrameFactory;
D3DXMESHCONTAINER_DERIVED::MeshContainerFactory D3DXMESHCONTAINER_DERIVED::sMeshContainerFactory;

void* D3DXMESHCONTAINER_DERIVED::operator new (size_t)
{
    return sMeshContainerFactory.Create();
}

void D3DXMESHCONTAINER_DERIVED::operator delete (void* p)
{
    if (p != NULL)
    {
        sMeshContainerFactory.Destroy(p);
    }
}

void* D3DXFRAME_DERIVED::operator new (size_t)
{
    return sFrameFactory.Create();
}

void D3DXFRAME_DERIVED::operator delete (void* p)
{
    if (p != NULL)
    {
        sFrameFactory.Destroy(p);
    }
}

HRESULT D3DXMESHCONTAINER_DERIVED::SetupBoneMatrixPointers(D3DXFRAME* pFrameRoot)
{
    // if there is a skinmesh, then setup the bone matrices
    if (this->pSkinInfo != NULL)
    {
        UINT cBones = this->pSkinInfo->GetNumBones();

        this->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
        if (this->ppBoneMatrixPtrs == NULL)
            return E_OUTOFMEMORY;

        _ULOOP(cBones, iBone)
        {
            D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind( pFrameRoot, this->pSkinInfo->GetBoneName(iBone) );
            if (pFrame == NULL)
                return E_FAIL;

            this->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
        }
    }

    return S_OK;
}

} // namespace FDO