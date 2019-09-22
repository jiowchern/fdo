#ifndef FdoSkinMeshHelperH
#define FdoSkinMeshHelperH

#include "FdoSkinMeshDefine.h"

namespace FDO
{
    void UpdateFrameMatrices(LPD3DXFRAME pFrameBase, const D3DXMATRIX* pParentMatrix);
    void DrawFrames(LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXFRAME pFrame,
        SKINNING_METHOD::E skinningMethod=SKINNING_METHOD::D3DNONINDEXED);
    void ReleaseAttributeTable(LPD3DXFRAME pFrameBase);

    HRESULT GetMinMaxPts(LPD3DXFRAME pFrame, D3DXVECTOR3* pvMin, D3DXVECTOR3* pvMax);
    HRESULT GetIntersect(LPD3DXFRAME pFrame, bool * pHit, const D3DXVECTOR3 * pRayPos,
                     const D3DXVECTOR3 * pRayDir, float * pDist);
}

#endif // FdoSkinMeshHelperH