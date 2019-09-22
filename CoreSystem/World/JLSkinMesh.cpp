#include "stdafx.h"
#include "JLSkinMesh.h"
//---------------------------------------------------------------------------
//#include "PhenixAssert.h"
#include <algorithm>
#include <assert.h>
//#include "PhenixD3D9RenderSystem.h"
#include "ResourceManager.h"

// <yuming> include header
#include "HModel.h"
#include "FdoAllocateHierarchy.h"
#include "FdoSkinMeshHelper.h"
// </yuming>

//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("管理模式啟動") 

#else
#pragma message("一般模式啟動") 
#endif

//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
// Class JLSkinMesh
//---------------------------------------------------------------------------
JLSkinMesh::JLSkinMesh(const fstring& fileName) 
: Object(fileName)
, m_pDevice(NULL)
{
    //m_pDevice = GetDevice();
    //m_pDevice = g_RenderSystem.GetD3DDevice();
    //m_pDevice->GetDeviceCaps( &m_d3dCaps );
    //D3DDEVICE_CREATION_PARAMETERS cp;
    //m_pDevice->GetCreationParameters( &cp );
    //m_dwBehaviorFlags = cp.BehaviorFlags;

    Clear();
}
//---------------------------------------------------------------------------
JLSkinMesh::~JLSkinMesh()
{
    Destory();
}
//---------------------------------------------------------------------------
void JLSkinMesh::Clear()
{
    m_pFrameRoot         = NULL;
    m_pAnimController    = NULL;
    m_pEffect            = NULL;
    //m_SkinningMethod     = D3DNONINDEXED;
    m_pBoneMatrices      = NULL;
    m_NumBoneMatricesMax = 0;

    //m_fAlpha             = 1.0f
    //m_fObjectRadius      = 0.0f;
    //m_vObjectCenter      = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    //m_vMinBounds         = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    //m_vMaxBounds         = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

    _LOOP(4, pos)
    {
		m_pIndexedVertexShader[pos] = NULL;
    }
}
//---------------------------------------------------------------------------
void JLSkinMesh::SetDevice(LPDIRECT3DDEVICE9 value)
{
    m_pDevice = value;
    m_pDevice->GetDeviceCaps( &m_d3dCaps );
    D3DDEVICE_CREATION_PARAMETERS cp;
    m_pDevice->GetCreationParameters( &cp );
    m_dwBehaviorFlags = cp.BehaviorFlags;
}

//-----------------------------------------------------------------------------
// Name: JLSkinMesh::LoadFromFile()
// Desc: The class is initialized with this method.
//       We create the effect from the fx file, and load the animation mesh
//       from the given X file.  We then call SetupBonePtrs() to initialize
//       the mesh containers to enable bone matrix lookup by index.  The
//       Allocation Hierarchy is passed by pointer to allow an app to subclass
//       it for its own implementation.
//-----------------------------------------------------------------------------
HRESULT JLSkinMesh::LoadFromFile( char* szXFile, char* szFilePath, char* szFxFile )
{
    CAllocateHierarchy AH(szFilePath , false);

    HRESULT hr = S_OK;
    WCHAR wszPath[ MAX_PATH ];

    if (szFxFile)
    {
        // Increase the palette size if the shader allows it. We are sort
        // of cheating here since we know tiny has 35 bones. The alternative
        // is use the maximum number that vs_2_0 allows.
        D3DXMACRO mac[2] =
        {
            { "MATRIX_PALETTE_SIZE_DEFAULT", "35" },
            { NULL,                          NULL }
        };

        // If we support VS_2_0, increase the palette size; else, use the default
        // of 26 bones from the .fx file by passing NULL
        D3DXMACRO *pmac = NULL;
        if( m_d3dCaps.VertexShaderVersion > D3DVS_VERSION( 1, 1 ) )
            pmac = mac;

        // 設定檔名路徑
        if( szFilePath )
        {
            char szFullFilePath[MAX_PATH];
            sprintf( szFullFilePath, "%s\\%s", szFilePath, szFxFile );
            MultiByteToWideChar( CP_ACP, 0, szFullFilePath, -1, wszPath, MAX_PATH );
        }
        else
        {
            MultiByteToWideChar( CP_ACP, 0, szFxFile, -1, wszPath, MAX_PATH );
        }

        wszPath[ MAX_PATH-1 ] = L'\0';

        // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the shader debugger.
        // Debugging vertex shaders requires either REF or software vertex processing, and debugging
        // pixel shaders requires REF.  The D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug
        // experience in the shader debugger.  It enables source level debugging, prevents instruction
        // reordering, prevents dead code elimination, and forces the compiler to compile against the next
        // higher available software target, which ensures that the unoptimized shaders do not exceed
        // the shader model limitations.  Setting these flags will cause slower rendering since the shaders
        // will be unoptimized and forced into software.  See the DirectX documentation for more information
        // about using the shader debugger.

      //  DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;  update
        #ifdef DEBUG_VS
            dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
        #endif
        #ifdef DEBUG_PS
            dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
        #endif

        // create effect -- do this first, so LMHFX has access to the palette size
  /*  update    hr = D3DXCreateEffectFromFile( m_pDevice,
                                       wszPath,
                                       pmac,
                                       NULL,
                                       dwShaderFlags,
                                       NULL,
                                       &m_pEffect,
                                       NULL );*/

        if( FAILED( hr ) )
            goto e_Exit;
    }

    // 設定檔名路徑
    if( szFilePath )
    {
        char szFullFilePath[MAX_PATH];
        sprintf( szFullFilePath, "%s\\%s", szFilePath, szXFile );
        MultiByteToWideChar( CP_ACP, 0, szFullFilePath, -1, wszPath, MAX_PATH );
    }
    else
    {
        MultiByteToWideChar( CP_ACP, 0, szXFile, -1, wszPath, MAX_PATH );
    }

    wszPath[ MAX_PATH-1 ] = L'\0';

    // create the mesh, frame hierarchy, and animation controller from the x file
    hr = D3DXLoadMeshHierarchyFromX
    ( 
        wszPath,
        D3DXMESH_MANAGED,
        m_pDevice,
        &AH,
        NULL,
        (LPD3DXFRAME*)&m_pFrameRoot,
        &m_pAnimController
    );

    if( FAILED( hr ) )
        goto e_Exit;

    // set up bone pointers
    hr = SetupBoneMatrixPointers( m_pFrameRoot );
    if( FAILED( hr ) )
        goto e_Exit;

    // get bounding radius
    hr = D3DXFrameCalculateBoundingSphere( m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius );
    if( FAILED( hr ) )
        goto e_Exit;

    mBoundOutOfData = true;

e_Exit:

    if( FAILED( hr ) )
    {
        SafeRelease( m_pAnimController );

        if( m_pFrameRoot )
        {
            D3DXFrameDestroy( m_pFrameRoot, &AH );
            m_pFrameRoot = NULL;
        }

        SafeRelease( m_pEffect );

        assert( 0 && "Call new failed !!" );
    }

    return hr;
}
//-----------------------------------------------------------------------------
// Name: JLSkinMesh::Clean()
// Desc: Performs clean up work and free up memory.
//-----------------------------------------------------------------------------
void JLSkinMesh::Destory()
{
    SafeRelease(m_pAnimController);

    if (m_pFrameRoot)
    {
        ReleaseAttributeTable(m_pFrameRoot);

        CAllocateHierarchy AH;
        D3DXFrameDestroy(m_pFrameRoot, &AH);
        m_pFrameRoot = NULL;
    }

    SafeRelease(m_pEffect);

    _LOOP(4, pos)
    {
		SafeRelease(m_pIndexedVertexShader[pos]);
    }
}

//---------------------------------------------------------------------------
// 更改模型方法
//---------------------------------------------------------------------------
//void JLSkinMesh::ChangeSkinMethod(const METHOD SkinMethod)
//{
//    if (m_SkinningMethod != SkinMethod)
// 	{
// 		m_SkinningMethod = SkinMethod;
// 		UpdateSkinningMethod(m_pFrameRoot);
// 	}
//}

//---------------------------------------------------------------------------
LPDIRECT3DDEVICE9 JLSkinMesh::GetDevice() const
{
    return m_pDevice;
}
//---------------------------------------------------------------------------
// 更新球體邊界盒
//---------------------------------------------------------------------------
HRESULT JLSkinMesh::RefreshBoundingVolumes()
{
    return D3DXFrameCalculateBoundingSphere( m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius );
}
//---------------------------------------------------------------------------
// 取得球體邊界盒中心點
//---------------------------------------------------------------------------
const D3DXVECTOR3& JLSkinMesh::GetBoundingCenter() const
{
    return m_vObjectCenter;
}
//---------------------------------------------------------------------------
// 取得球體邊界盒半徑
//---------------------------------------------------------------------------
float JLSkinMesh::GetBoundingRadius() const
{
    return m_fObjectRadius;
}
//-----------------------------------------------------------------------------
LPD3DXFRAME JLSkinMesh::GetFrameRoot() const
{
    return m_pFrameRoot;
}
//-----------------------------------------------------------------------------
bool JLSkinMesh::Create( BYTE *Data, UINT DataSize )
{
    Destory();
    Clear();

	std::string filter("\\") ; 

	size_t pos = m_FileName.find(filter) ; 

	std::string name(m_FileName.substr(pos + 1)) ; 

	bool bPlaneModel = false ; 

	if(!name.empty() && name.at(0) == '0')
	{
		bPlaneModel = true ;  
	}

    CAllocateHierarchy AH(m_FileName.substr(0, m_FileName.rfind("\\") + 1) , bPlaneModel);

    HRESULT hr;

    //memPack *m = LoadFromPacket( m_FileName );
    F_RETURN(m_pDevice != NULL);

    // create the mesh, frame hierarchy, and animation controller from the x file in memory
    hr = D3DXLoadMeshHierarchyFromXInMemory(
        Data,                           // LPCVOID pMemory,
        DataSize,                       // DWORD SizeOfMemory,
        D3DXMESH_MANAGED,               // DWORD MeshOptions,
        m_pDevice,                      // LPDIRECT3DDEVICE9 pDevice,
        &AH,                            // LPD3DXALLOCATEHIERARCHY pAlloc,
        NULL,                           // LPD3DXLOADUSERDATA pUserDataLoader,
        (LPD3DXFRAME*)&m_pFrameRoot,    // LPD3DXFRAME *ppFrameHeirarchy,
        &m_pAnimController              // LPD3DXANIMATIONCONTROLLER *ppAnimController
    );

    if (FAILED(hr))
	{
		if (hr == D3DERR_INVALIDCALL)
		{
			//因為mesh所綁的material中的filename，有時會找不到資源。
			//但是不能讓他當機，所以讓他繼續跑。
			//goto e_Exit ;
			//Assert(0 , "D3DERR_INVALIDCALL") ;
		}
		else if (hr == E_OUTOFMEMORY)
		{
			Assert(0 , "E_OUTOFMEMORY") ;
		}
		else if (hr == E_INVALIDARG)
		{
			Assert(0 , "E_INVALIDARG") ; 
		}
		else if (hr == E_FAIL)
		{
			Assert(0 , "E_FAIL") ; 
		}
		else if (hr == S_FALSE)
		{
			Assert(0 , "S_FALSE") ; 
		}
		else if (hr == D3DERR_WASSTILLDRAWING)
		{
			Assert(0 , "D3DERR_WASSTILLDRAWING") ; 
		}
		else
		{
			Assert(0 , "D3DXLoadMeshHierarchyFromXInMemory error") ;
		}
	}

   // if( FAILED( hr ) )
  //      goto e_Exit;

    assert( m_pFrameRoot && m_FileName.c_str() );

	if(m_pFrameRoot == 0)
	{
		std::string str;
		StringFormat8(str , "m_pFrameRoot == NULL , JLSkinMesh::Create error . %s " , m_FileName.c_str());
		Assert(0 ,str.c_str() );
	}
	
    // set up bone pointers
    hr = SetupBoneMatrixPointers( m_pFrameRoot );
    if( FAILED( hr ) )
        goto e_Exit;

    // get bounding radius
    hr = D3DXFrameCalculateBoundingSphere( m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius );
    if( FAILED( hr ) )
        goto e_Exit;

    mBoundOutOfData = true;

e_Exit:

    if( FAILED( hr ) )
    {
        SafeRelease( m_pAnimController );

        if( m_pFrameRoot )
        {
            D3DXFrameDestroy( m_pFrameRoot, &AH );
            m_pFrameRoot = NULL;
        }

        SafeRelease( m_pEffect );

        return false;
    }

    return true;
}

void JLSkinMesh::UpdateBounds() const
{
    //Set xTreme Values So They Will Get Replaced
    mMinBounds = D3DXVECTOR3( FLT_MAX,  FLT_MAX,  FLT_MAX);
    mMaxBounds = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    //Give Root Frame Current Min & Max. They Will Be Overwritten If A New Min/Max
    //Value Is Found In The Hierarchy To Replace It
    GetMinMaxPts(m_pFrameRoot, &mMinBounds, &mMaxBounds);
    mBoundOutOfData = false;
}

//-----------------------------------------------------------------------------
void JLSkinMesh::GetBoundingBox(D3DXVECTOR3 * pvMin, D3DXVECTOR3 * pvMax)
{
    if (mBoundOutOfData)
    {
        UpdateBounds();
    }
    if (pvMin != NULL) *pvMin = mMinBounds;
    if (pvMax != NULL) *pvMax = mMaxBounds;
}
//-----------------------------------------------------------------------------
SkinMeshInstPtr JLSkinMesh::CreateInstance()
{
    SkinMeshInstPtr Result(new HModel());

    if (Result != NULL)
    {
        Result->SetSkinMeshTemp(SkinMeshTempPtr(this));
        // set animation controller
        LPD3DXANIMATIONCONTROLLER pNewAC = NULL;
        if (m_pAnimController != NULL)
        {
            m_pAnimController->CloneAnimationController
            (
                m_pAnimController->GetMaxNumAnimationOutputs(),
                m_pAnimController->GetMaxNumAnimationSets(),
                m_pAnimController->GetMaxNumTracks(),
                m_pAnimController->GetMaxNumEvents(),
                &pNewAC
            );
        }
        Result->SetAnimation(pNewAC);
    }

    return Result;
}
//--------------------------------------------------------------------------------------
// Called to setup the pointers for a given bone to its transformation matrix
//--------------------------------------------------------------------------------------
HRESULT JLSkinMesh::SetupBoneMatrixPointers( D3DXFRAME* pFrame )
{
    HRESULT hr;

    if (pFrame->pMeshContainer != NULL)
    {
        V_RETURN(((D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer)->SetupBoneMatrixPointers(m_pFrameRoot));
    }

    if (pFrame->pFrameSibling != NULL)
    {
        V_RETURN(SetupBoneMatrixPointers(pFrame->pFrameSibling));
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        V_RETURN(SetupBoneMatrixPointers(pFrame->pFrameFirstChild));
    }

    return S_OK;
}
//-----------------------------------------------------------------------------
} // namespace FDO
