#include "StdAfx.h"
#include "FdoSkinMeshHelper.h"

namespace FDO
{

/**
 * @brief update the frame matrices
 */
void UpdateFrameMatrices(LPD3DXFRAME pFrameBase, const D3DXMATRIX* pParentMatrix)
{
    D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )pFrameBase;

    if( pParentMatrix != NULL )
    {
        D3DXMatrixMultiply( &pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix );
    }
    else
    {
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
    }

    if( pFrame->pFrameSibling != NULL )
    {
        UpdateFrameMatrices( pFrame->pFrameSibling, pParentMatrix );
    }

    if( pFrame->pFrameFirstChild != NULL )
    {
        UpdateFrameMatrices( pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix );
    }
}

/**
 * @brief
 */
void ReleaseAttributeTable(LPD3DXFRAME pFrameBase)
{
    if (pFrameBase == NULL)
    {
        return;
    }

    D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
    D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

    while (pMeshContainer != NULL)
    {
        SafeDeleteArray(pMeshContainer->pAttributeTable);

        pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pMeshContainer->pNextMeshContainer;
    }

    if( pFrame->pFrameSibling != NULL )
    {
        ReleaseAttributeTable( pFrame->pFrameSibling );
    }

    if( pFrame->pFrameFirstChild != NULL )
    {
        ReleaseAttributeTable( pFrame->pFrameFirstChild );
    }
}

/**
 * @brief Called to render a mesh in the hierarchy
 */
void DrawMeshContainer( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
{
    HRESULT hr;
    D3DXMESHCONTAINER_DERIVED* pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pMeshContainerBase;
    D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )pFrameBase;

    // first check for skinning
    if( pMeshContainer->pSkinInfo != NULL )
    {
        D3DCAPS9 d3dCaps;
        pd3dDevice->GetDeviceCaps( &d3dCaps );
        D3DXMATRIXA16 matTemp;

        // SKINNING_METHOD::D3DNONINDEXED
        DWORD AttribIdPrev = UNUSED32;
        LPD3DXBONECOMBINATION pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>( pMeshContainer->pBoneCombinationBuf->GetBufferPointer
                                                             () );

        // Draw using default vtx processing of the device (typically HW)
        for( UINT iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; ++iAttrib )
        {
            UINT NumBlend = 0;
            for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i )
            {
                if( pBoneComb[iAttrib].BoneId[i] != UINT_MAX )
                {
                    NumBlend = i;
                }
            }

            if( d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1 )
            {
                // first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
                for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i )
                {
                    UINT iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
                    if( iMatrixIndex != UINT_MAX )
                    {
                        D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
                                            pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
                        V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
                    }
                }

                V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, NumBlend ) );

			//	if(pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] != NULL)
				if(pMeshContainer->imageVec[pBoneComb[iAttrib].AttribId] && pMeshContainer->imageVec[pBoneComb[iAttrib].AttribId]->pTexture != NULL)
				{
					// lookup the material used for this subset of faces
					if( ( AttribIdPrev != pBoneComb[iAttrib].AttribId ) || ( AttribIdPrev == UNUSED32 ) )
					{

						V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D ) );
						//V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );
						V( pd3dDevice->SetTexture( 0, pMeshContainer->imageVec[pBoneComb[iAttrib].AttribId]->pTexture ) );

						AttribIdPrev = pBoneComb[iAttrib].AttribId;
						// draw the subset now that the correct material and matrices are loaded

					}
						V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );

				}

            }
        }

        // If necessary, draw parts that HW could not handle using SW
        if( pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups )
        {
            AttribIdPrev = UNUSED32;
            V( pd3dDevice->SetSoftwareVertexProcessing( TRUE ) );
            for( UINT iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++ )
            {
                UINT NumBlend = 0;
                for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i )
                {
                    if( pBoneComb[iAttrib].BoneId[i] != UINT_MAX )
                    {
                        NumBlend = i;
                    }
                }

                if( d3dCaps.MaxVertexBlendMatrices < NumBlend + 1 )
                {
                    // first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
                    for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i )
                    {
                        UINT iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
                        if( iMatrixIndex != UINT_MAX )
                        {
                            D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
                                                pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
                            V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
                        }
                    }

                    V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, NumBlend ) );

					//if(pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] != NULL)
					if(pMeshContainer->imageVec[pBoneComb[iAttrib].AttribId] && pMeshContainer->imageVec[pBoneComb[iAttrib].AttribId]->pTexture != NULL)
					{
						// lookup the material used for this subset of faces
						if( ( AttribIdPrev != pBoneComb[iAttrib].AttribId ) || ( AttribIdPrev == UNUSED32 ) )
						{
							V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D ) );
							//V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );
							V( pd3dDevice->SetTexture( 0, pMeshContainer->imageVec[pBoneComb[iAttrib].AttribId]->pTexture ) );
							AttribIdPrev = pBoneComb[iAttrib].AttribId;
							// draw the subset now that the correct material and matrices are loaded

						}
							V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );
					}


                }
            }
            V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
        }

        V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0 ) );
    }
    else  // standard mesh, just draw it after setting material properties
    {
        V( pd3dDevice->SetTransform( D3DTS_WORLD, &pFrame->CombinedTransformationMatrix ) );

        for( UINT iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++ )
        {
            V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
           // if (pMeshContainer->ppTextures[iMaterial])
			if (pMeshContainer->imageVec[iMaterial] && pMeshContainer->imageVec[iMaterial]->pTexture)
            {
               // V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] ) );
				V( pd3dDevice->SetTexture( 0, pMeshContainer->imageVec[iMaterial]->pTexture ) );
				V( pMeshContainer->MeshData.pMesh->DrawSubset( iMaterial ) );

            }
        }
    }
}


/**
 * @brief
 */
void DrawFrames( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXFRAME pFrame, SKINNING_METHOD::E skinningMethod )
{
    if (pFrame == NULL)
    {
        return;
    }

    LPD3DXMESHCONTAINER pMeshContainer = pFrame->pMeshContainer;
    while( pMeshContainer != NULL )
    {
        //DrawMeshContainer( pd3dDevice, pMeshContainer, pFrame, skinningMethod );
        DrawMeshContainer( pd3dDevice, pMeshContainer, pFrame );

        pMeshContainer = pMeshContainer->pNextMeshContainer;
    }

    if( pFrame->pFrameSibling != NULL )
    {
        DrawFrames( pd3dDevice, pFrame->pFrameSibling, skinningMethod );
    }

    if( pFrame->pFrameFirstChild != NULL )
    {
        DrawFrames( pd3dDevice, pFrame->pFrameFirstChild, skinningMethod );
    }
}

/**
 * @brief
 */
HRESULT GetMinMaxPts(LPD3DXFRAME pFrame, D3DXVECTOR3* pvMin, D3DXVECTOR3* pvMax)
{
    HRESULT hr = S_OK;

    //See If New Min/Max Found In This Frame's Mesh Container
    if( pFrame->pMeshContainer )
    {
        LPD3DXMESH pMesh = pFrame->pMeshContainer->MeshData.pMesh;
        if( pMesh )
        {
            D3DXVECTOR3 vMin, vMax;
            //Lock Mesh's Vertex Buffer
            VOID* pVertex;
            if( FAILED( hr = pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pVertex)) )
            {
                //App::err("AABB: LockVertexBuffer Failed", hr, TRUE);
                return hr;
            }

            //Find Min/Max Points In Mesh
            if( FAILED(hr = D3DXComputeBoundingBox((D3DXVECTOR3*)pVertex,
                pMesh->GetNumVertices(), pMesh->GetNumBytesPerVertex(),
                &vMin, &vMax)) )
            {
                //App::err("AABB: D3DXComputeBoundingBox Failed", hr, TRUE);
                return hr;
            }

            //Unlock VB
            if( FAILED(hr = pMesh->UnlockVertexBuffer() ) )
            {
                //App::err("AABB: UnlockVertexBuffer Failed", hr, TRUE);
                return hr;
            }
            if( vMin.x < pvMin->x ) { pvMin->x = vMin.x; }
            if( vMin.y < pvMin->y ) { pvMin->y = vMin.y; }
            if( vMin.z < pvMin->z ) { pvMin->z = vMin.z; }
            if( vMax.x > pvMax->x ) { pvMax->x = vMax.x; }
            if( vMax.y > pvMax->y ) { pvMax->y = vMax.y; }
            if( vMax.z > pvMax->z ) { pvMax->z = vMax.z; }
        }
    }

    if( pFrame->pFrameSibling )
    {
        if( FAILED(hr = GetMinMaxPts(pFrame->pFrameSibling, pvMin, pvMax)) )
            return hr;
    }

    if( pFrame->pFrameFirstChild )
    {
        if( FAILED(hr = GetMinMaxPts(pFrame->pFrameFirstChild, pvMin,pvMax)) )
            return hr;
    }

    return hr;
}
/**
 *
 */
bool MeshIntersect(LPD3DXBASEMESH pMesh, const D3DXVECTOR3 * pRayPos, 
                   const D3DXVECTOR3 * pRayDir, float * pDist)
{
    BOOL HIT;
    FLOAT fDist;
    if (FAILED(D3DXIntersect(pMesh, pRayPos, pRayDir, &HIT, NULL, NULL, NULL, &fDist, NULL, NULL)))
        return false;

    if (HIT == FALSE)
        return false;

    if (fDist > (*pDist))
        return false;

    return true;
    //return (HIT == TRUE);
}
/**
 *
 */
HRESULT GetIntersect(LPD3DXFRAME pFrame, bool * pHit, const D3DXVECTOR3 * pRayPos,
                     const D3DXVECTOR3 * pRayDir, float * pDist)
{
    HRESULT hr = S_OK;

    //See If New Min/Max Found In This Frame's Mesh Container
    if( pFrame->pMeshContainer )
    {
        LPD3DXMESH pMesh = pFrame->pMeshContainer->MeshData.pMesh;
        if( pMesh != NULL )
        {
            if( MeshIntersect( pMesh, pRayPos, pRayDir, pDist ) )
            {
                (*pHit) = true;
                return S_OK;
            }
        }
    }

    if( pFrame->pFrameSibling )
    {
        if( FAILED(hr = GetIntersect(pFrame->pFrameSibling, pHit, pRayPos, pRayDir, pDist)) )
            return hr;
    }

    if( pFrame->pFrameFirstChild )
    {
        if( FAILED(hr = GetIntersect(pFrame->pFrameFirstChild, pHit, pRayPos, pRayDir, pDist)) )
            return hr;
    }

    return hr;
}

} // namespace FDO