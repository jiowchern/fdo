#include "StdAfx.h"
#include "FdoAllocateHierarchy.h"
#include "ResourceManager.h" 
namespace FDO
{
std::map<string , LPDIRECT3DTEXTURE9> FDO::CAllocateHierarchy::s_meshTextures;

D3DXMATRIXA16* g_pBoneMatrices = NULL;
unsigned int g_NumBoneMatricesMax = 0;
//---------------------------------------------------------------------------
// Name: AllocateName()
// Desc: Allocates memory for a string to hold the name of a frame or mesh
//---------------------------------------------------------------------------
HRESULT AllocateName(LPCSTR Name, LPSTR *pNewName)
{
    UINT cbLength;

    if( Name != NULL )
    {
        cbLength = (UINT)strlen(Name) + 1;
        *pNewName = new CHAR[cbLength];
        if (*pNewName == NULL)
        {
            assert( 0 && "Call new failed !!" );
            return E_OUTOFMEMORY;
        }
        memcpy(*pNewName, Name, cbLength*sizeof(CHAR));
    }
    else
    {
        *pNewName = NULL;
    }

    return S_OK;
}

//---------------------------------------------------------------------------
// Called either by CreateMeshContainer when loading a skin mesh, or when
// changing methods.  This function uses the pSkinInfo of the mesh
// container to generate the desired drawable mesh and bone combination
// table.
//---------------------------------------------------------------------------
HRESULT GenerateSkinnedMesh(LPDIRECT3DDEVICE9 pd3dDevice, 
                            D3DXMESHCONTAINER_DERIVED *pMeshContainer,
                            SKINNING_METHOD::E eSkinningMethod)
{
    if (pMeshContainer->pSkinInfo == NULL)
    {
        return S_OK;
    }

    D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

    SafeRelease(pMeshContainer->MeshData.pMesh);
    SafeRelease(pMeshContainer->pBoneCombinationBuf);

    HRESULT hr;

    // if non-indexed skinning mode selected, use ConvertToBlendedMesh to generate drawable mesh
    if (eSkinningMethod == SKINNING_METHOD::D3DNONINDEXED)
    {

        hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh
                                   (
                                       pMeshContainer->pOrigMesh,
                                       D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE,
                                       pMeshContainer->pAdjacency,
                                       NULL, NULL, NULL,
                                       &pMeshContainer->NumInfl,
                                       &pMeshContainer->NumAttributeGroups,
                                       &pMeshContainer->pBoneCombinationBuf,
                                       &pMeshContainer->MeshData.pMesh
                                   );
        if (FAILED(hr))
            goto e_Exit;


        // If the device can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all meshes to it
        // Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
        // drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing.
        LPD3DXBONECOMBINATION rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

        // look for any set of bone combinations that do not fit the caps
        for (pMeshContainer->iAttributeSW = 0; pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups; ++pMeshContainer->iAttributeSW)
        {
            DWORD cInfl   = 0;

            for (DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; ++iInfl)
            {
                if (rgBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX)
                {
                    ++cInfl;
                }
            }

            if (cInfl > d3dCaps.MaxVertexBlendMatrices)
            {
                break;
            }
        }

        // if there is both HW and SW, add the Software Processing flag
        if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
        {
            LPD3DXMESH pMeshTmp;

            hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|pMeshContainer->MeshData.pMesh->GetOptions(),
                                                pMeshContainer->MeshData.pMesh->GetFVF(),
                                                pd3dDevice, &pMeshTmp);
            if (FAILED(hr))
            {
                goto e_Exit;
            }

            pMeshContainer->MeshData.pMesh->Release();
            pMeshContainer->MeshData.pMesh = pMeshTmp;
            pMeshTmp = NULL;
        }
    }
    // if indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
    else if (eSkinningMethod == SKINNING_METHOD::D3DINDEXED)
    {
        DWORD NumMaxFaceInfl;
        DWORD Flags = D3DXMESHOPT_VERTEXCACHE;

        LPDIRECT3DINDEXBUFFER9 pIB;
        hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB);
        if (FAILED(hr))
            goto e_Exit;

        hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, pMeshContainer->pOrigMesh->GetNumFaces(), &NumMaxFaceInfl);
        pIB->Release();
        if (FAILED(hr))
            goto e_Exit;

        // 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
        // can be handled
		NumMaxFaceInfl = min(NumMaxFaceInfl, (DWORD)12);

        if( d3dCaps.MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl )
        {
			// HW does not support indexed vertex blending. Use SW instead
			pMeshContainer->NumPaletteEntries = min((DWORD)256, pMeshContainer->pSkinInfo->GetNumBones());
            pMeshContainer->UseSoftwareVP = true;
            Flags |= D3DXMESH_SYSTEMMEM;
        }
        else
        {
            // using hardware - determine palette size from caps and number of bones
            // If normals are present in the vertex data that needs to be blended for lighting, then
            // the number of matrices is half the number specified by MaxVertexBlendMatrixIndex.
            pMeshContainer->NumPaletteEntries = min( ( d3dCaps.MaxVertexBlendMatrixIndex + 1 ) / 2,
                                                     pMeshContainer->pSkinInfo->GetNumBones() );
            pMeshContainer->UseSoftwareVP = false;
            Flags |= D3DXMESH_MANAGED;
        }

        hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
                                                (
                                                pMeshContainer->pOrigMesh,
                                                Flags,
                                                pMeshContainer->NumPaletteEntries,
                                                pMeshContainer->pAdjacency,
                                                NULL, NULL, NULL,
                                                &pMeshContainer->NumInfl,
                                                &pMeshContainer->NumAttributeGroups,
                                                &pMeshContainer->pBoneCombinationBuf,
                                                &pMeshContainer->MeshData.pMesh);
        if (FAILED(hr))
            goto e_Exit;
    }
    // if vertex shader indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
    else if ((eSkinningMethod == SKINNING_METHOD::D3DINDEXEDVS) || (eSkinningMethod == SKINNING_METHOD::D3DINDEXEDHLSLVS))
    {
        // Get palette size
        // First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
        // (96 - 9) /3 i.e. Maximum constant count - used constants
        UINT MaxMatrices = 26;
		pMeshContainer->NumPaletteEntries = min((DWORD)MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

		DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
        if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
        {
            pMeshContainer->UseSoftwareVP = false;
            Flags |= D3DXMESH_MANAGED;
        }
        else
        {
            pMeshContainer->UseSoftwareVP = true;
            Flags |= D3DXMESH_SYSTEMMEM;
        }

        SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

        hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
                                                (
                                                pMeshContainer->pOrigMesh,
                                                Flags,
                                                pMeshContainer->NumPaletteEntries,
                                                pMeshContainer->pAdjacency,
                                                NULL, NULL, NULL,
                                                &pMeshContainer->NumInfl,
                                                &pMeshContainer->NumAttributeGroups,
                                                &pMeshContainer->pBoneCombinationBuf,
                                                &pMeshContainer->MeshData.pMesh);
        if (FAILED(hr))
            goto e_Exit;


        // FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
        DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
        if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
        {
            LPD3DXMESH pMesh;
            hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
            if (!FAILED(hr))
            {
                pMeshContainer->MeshData.pMesh->Release();
                pMeshContainer->MeshData.pMesh = pMesh;
                pMesh = NULL;
            }
        }

        D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
        LPD3DVERTEXELEMENT9 pDeclCur;
        hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
        if (FAILED(hr))
            goto e_Exit;

        // the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type
        //   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
        //          this is more of a "cast" operation
        pDeclCur = pDecl;
        while (pDeclCur->Stream != 0xff)
        {
            if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
                pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
            ++pDeclCur;
        }

        hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
        if (FAILED(hr))
            goto e_Exit;

        // allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
        if( g_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones() )
        {
            g_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

            // Allocate space for blend matrices
            SafeDeleteArray(g_pBoneMatrices);
            g_pBoneMatrices  = new D3DXMATRIXA16[g_NumBoneMatricesMax];
            if( g_pBoneMatrices == NULL )
            {
                assert( 0 && "Call new failed !!" );
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
        }

    }
    // if software skinning selected, use GenerateSkinnedMesh to create a mesh that can be used with UpdateSkinnedMesh
    else if (eSkinningMethod == SKINNING_METHOD::SOFTWARE)
    {
        hr = pMeshContainer->pOrigMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshContainer->pOrigMesh->GetFVF(),
                                              pd3dDevice, &pMeshContainer->MeshData.pMesh);
        if (FAILED(hr))
            goto e_Exit;

        hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL, &pMeshContainer->NumAttributeGroups);
        if (FAILED(hr))
            goto e_Exit;

        delete[] pMeshContainer->pAttributeTable;
        pMeshContainer->pAttributeTable  = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
        if (pMeshContainer->pAttributeTable == NULL)
        {
            assert( 0 && "Call new failed !!" );
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, NULL);
        if (FAILED(hr))
            goto e_Exit;

        // allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
        if (g_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
        {
            g_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

            // Allocate space for blend matrices
            delete[] g_pBoneMatrices;
            g_pBoneMatrices  = new D3DXMATRIXA16[g_NumBoneMatricesMax];
            if( g_pBoneMatrices == NULL )
            {
                assert( 0 && "Call new failed !!" );
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
        }
    }
    else  // invalid m_SkinningMethod value
    {
        // return failure due to invalid skinning method value
        hr = E_INVALIDARG;
        goto e_Exit;
    }

e_Exit:
    return hr;
}
/**
 * @brief update the skinning method
 */
//void UpdateSkinningMethod( LPD3DXFRAME pFrameBase )
//{
//    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
//    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pFrame->pMeshContainer;
//
//    while (pMeshContainer != NULL)
//    {
//        GenerateSkinnedMesh( pMeshContainer );
//        pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pMeshContainer->pNextMeshContainer;
//    }
//
//    if (pFrame->pFrameSibling != NULL)
//    {
//        UpdateSkinningMethod(pFrame->pFrameSibling);
//    }
//
//    if (pFrame->pFrameFirstChild != NULL)
//    {
//        UpdateSkinningMethod(pFrame->pFrameFirstChild);
//    }
//}
//---------------------------------------------------------------------------
// Class CAllocateHierarchy
//---------------------------------------------------------------------------
CAllocateHierarchy::CAllocateHierarchy(const std::string& path , bool bPlaneModel)
: mPath(path) , m_bPlaneModel(bPlaneModel)
{
}
//---------------------------------------------------------------------------
// Class CAllocateHierarchy
//---------------------------------------------------------------------------
CAllocateHierarchy::CAllocateHierarchy()
{
}
//---------------------------------------------------------------------------
// CreateFrame
//---------------------------------------------------------------------------
HRESULT WINAPI CAllocateHierarchy::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
{
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame;

    *ppNewFrame = NULL;

    pFrame = new D3DXFRAME_DERIVED();
    if (pFrame == NULL)
    {
        assert( 0 && "Call new failed !!" );
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    hr = AllocateName(Name, &pFrame->Name);
    if (FAILED(hr))
        goto e_Exit;

    // initialize other data members of the frame
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

    pFrame->pMeshContainer   = NULL;
    pFrame->pFrameSibling    = NULL;
    pFrame->pFrameFirstChild = NULL;

    *ppNewFrame = pFrame;
    pFrame = NULL;

e_Exit:
    delete pFrame;
    return hr;
}
//---------------------------------------------------------------------------
// CreateMeshContainer
//---------------------------------------------------------------------------
HRESULT WINAPI CAllocateHierarchy::CreateMeshContainer(
    LPCSTR Name,
    CONST D3DXMESHDATA *pMeshData,
    CONST D3DXMATERIAL *pMaterials,
    CONST D3DXEFFECTINSTANCE *pEffectInstances,
	DWORD NumMaterials,
    CONST DWORD *pAdjacency,
    LPD3DXSKININFO pSkinInfo,
    LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
    HRESULT hr;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
    UINT NumFaces;
    //UINT iMaterial;
    UINT iBone, cBones;
    LPDIRECT3DDEVICE9 pd3dDevice = NULL;

    LPD3DXMESH pMesh = NULL;

    *ppNewMeshContainer = NULL;

    // this sample does not handle patch meshes, so fail when one is found
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // get the pMesh interface pointer out of the mesh data structure
    pMesh = pMeshData->pMesh;

    // this sample does not FVF compatible meshes, so fail when one is found
    if (pMesh->GetFVF() == 0)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // allocate the overloaded structure to return as a D3DXMESHCONTAINER
    pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
    if (pMeshContainer == NULL)
    {
        assert( 0 && "Call new failed !!" );
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(pMeshContainer, 0, SIZE_OF_D3DXMESHCONTAINER_DERIVED);

    // make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
    hr = AllocateName(Name, &pMeshContainer->Name);
    if (FAILED(hr))
        goto e_Exit;

    pMesh->GetDevice( &pd3dDevice );
    NumFaces = pMesh->GetNumFaces();

    // if no normals are in the mesh, add them
    if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
    {
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        // clone the mesh to make room for the normals
        hr = pMesh->CloneMeshFVF
        (
            pMesh->GetOptions(),
            pMesh->GetFVF() | D3DFVF_NORMAL,
            pd3dDevice,
            &pMeshContainer->MeshData.pMesh
        );

        if (FAILED(hr))
        {
            goto e_Exit;
        }

        // get the new pMesh pointer back out of the mesh container to use
        // NOTE: we do not release pMesh because we do not have a reference to it yet
        pMesh = pMeshContainer->MeshData.pMesh;

        // now generate the normals for the pmesh
        D3DXComputeNormals(pMesh, NULL);
    }
    else  // if no normals, just add a reference to the mesh for the mesh container
    {
        pMeshContainer->MeshData.pMesh = pMesh;
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
        pMesh->AddRef();
    }

    // allocate memory to contain the material information.  This sample uses
    //   the D3D9 materials and texture names instead of the EffectInstance style materials
	pMeshContainer->NumMaterials = max((DWORD)1, NumMaterials);
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
//	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	pMeshContainer->imageVec.resize(pMeshContainer->NumMaterials , NULL); 

	pMeshContainer->pAdjacency = new DWORD[NumFaces*3];

    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL))
    {
        assert( 0 && "Call new failed !!" );
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);
  //  memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

    // if materials provided, copy them
    if (NumMaterials > 0)
    {
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

        _ULOOP(NumMaterials, iMaterial)
        {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
            {
                string strFilename(mPath + pMeshContainer->pMaterials[iMaterial].pTextureFilename);


#ifdef RESMGR

				if(!m_bPlaneModel)
				{
					pMeshContainer->imageVec[iMaterial] = g_RoleImageManager.GetDynamicImageRes(strFilename.c_str()) ; 
				}
				else
				{
					pMeshContainer->imageVec[iMaterial] = g_RoleImageManager.GetStaticImageRes(strFilename.c_str()) ; 
				}

				//std::map<string , LPDIRECT3DTEXTURE9>::iterator iter ; 

				//iter = s_meshTextures.find(strFilename)  ; 

				//if(iter != s_meshTextures.end())
				//{
				//	pMeshContainer->ppTextures[iMaterial] = iter->second;
				//}
				//else
				//{

					//memPack *m = LoadFromPacket(strFilename);

					//if(m->Data == NULL)
					//{
					//	int i ; 

					//	i = 0 ; 
					//}
				//	D3DXCreateTextureFromFileInMemory(pd3dDevice, m->Data, m->DataSize, &pMeshContainer->ppTextures[iMaterial] );

				//	s_meshTextures.insert(std::map<string , LPDIRECT3DTEXTURE9>::value_type(strFilename , pMeshContainer->ppTextures[iMaterial]) ) ;
				//}
#else
                memPack *m = LoadFromPacket(strFilename);
                D3DXCreateTextureFromFileInMemory(pd3dDevice, m->Data, m->DataSize, &pMeshContainer->ppTextures[iMaterial]);
#endif
                // don't remember a pointer into the dynamic memory, just forget the name after loading
                pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
            }
        }
    }
    else // if no materials provided, use a default one
    {
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
    }

    // if there is skinning information, save off the required data and then setup for HW skinning
    if (pSkinInfo != NULL)
    {
        // first save off the SkinInfo and original mesh data
        pMeshContainer->pSkinInfo = pSkinInfo;
        pSkinInfo->AddRef();

        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();

        // Will need an array of offset matrices to move the vertices from the figure space to the bone's space
        cBones = pSkinInfo->GetNumBones();
        pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->pBoneOffsetMatrices == NULL)
        {
            assert( 0 && "Call new failed !!" );
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // get each of the bone offset matrices so that we don't need to get them later
        for (iBone = 0; iBone < cBones; ++iBone)
        {
            pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
        }

        // GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
        hr = GenerateSkinnedMesh(pd3dDevice, pMeshContainer, SKINNING_METHOD::D3DNONINDEXED);
        if (FAILED(hr))
            goto e_Exit;
    }

    *ppNewMeshContainer = pMeshContainer;
    pMeshContainer = NULL;

e_Exit:

    // call Destroy function to properly clean up the memory allocated
    if (pMeshContainer != NULL)
    {
        DestroyMeshContainer(pMeshContainer);
    }

    return hr;
}
//---------------------------------------------------------------------------
// DestroyFrame
//---------------------------------------------------------------------------
HRESULT WINAPI CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
    D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameToFree;

    if (pFrame == NULL)
    {
        return S_FALSE;
    }

    SafeDeleteArray(pFrame->Name);
    SafeDelete(pFrame);

    return S_OK;
}
//---------------------------------------------------------------------------
// DestroyMeshContainer
//---------------------------------------------------------------------------
HRESULT WINAPI CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
    //UINT iMaterial;
    D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

    if (pMeshContainer == NULL)
    {
        return S_FALSE;
    }

    SafeDeleteArray(pMeshContainer->Name);
    SafeDeleteArray(pMeshContainer->pAdjacency);
    SafeDeleteArray(pMeshContainer->pMaterials);
    SafeDeleteArray(pMeshContainer->pBoneOffsetMatrices);

    _FOREACH(TextureMap, s_meshTextures, pos)
    {
        SafeRelease(pos->second);
    }
    s_meshTextures.clear();

    // release all the allocated textures
    //if (pMeshContainer->ppTextures != NULL)
    //{
    //    _ULOOP(pMeshContainer->NumMaterials, iMaterial)
    //    {
    //        //pMeshContainer->ppTextures[iMaterial]->Release();
    //        pMeshContainer->ppTextures[iMaterial] = NULL;
    //    }
    //}

	_ULOOP(pMeshContainer->NumMaterials, iMaterial)
	{
		pMeshContainer->imageVec[iMaterial] = NULL ; 
	}


   // SafeDeleteArray(pMeshContainer->ppTextures);
    SafeDeleteArray(pMeshContainer->ppBoneMatrixPtrs);

    SafeRelease(pMeshContainer->pBoneCombinationBuf);
    SafeRelease(pMeshContainer->MeshData.pMesh);
    SafeRelease(pMeshContainer->pSkinInfo);
    SafeRelease(pMeshContainer->pOrigMesh);

    SafeDelete(pMeshContainer);

    return S_OK;
}

} // namespace FDO