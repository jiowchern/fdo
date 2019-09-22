#include "stdafx.h"
#pragma hdrstop
//-----------------------------------------------------------------------------
#include "JL_SkyBox.h"
#include "PacketReader.h"
#include "PhenixAssert.h"
#include "PhenixD3D9RenderSystem.h"
#include "Common.h"

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("管理模式啟動") 

#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------

namespace FDO
{

//---------------------------------------------------------------------------
JLSkyBox::JLSkyBox()
{
    m_pD3DDevice  = NULL;
    m_pMesh       = NULL;
    m_pAdjBuffer  = NULL;
    m_pMtrlBuffer = NULL;
    m_FileName    = "";
    m_IsCreate    = false;
    D3DXMatrixIdentity( &m_matWorld );
}
//---------------------------------------------------------------------------
JLSkyBox::~JLSkyBox()
{
    Destory();
}
//---------------------------------------------------------------------------
void JLSkyBox::Destory()
{
    SAFE_RELEASE( m_pMtrlBuffer );
    SAFE_RELEASE( m_pAdjBuffer );
    SAFE_RELEASE( m_pMesh );
    
    for( UINT i=0; i<m_pTextures.size(); ++i )
    {
        SAFE_RELEASE( m_pTextures[i] );
    }
    
    m_pTextures.clear();
    m_pMaterials.clear();

    m_FileName = "";
    m_IsCreate = false;
}   
//---------------------------------------------------------------------------
bool JLSkyBox::LoadFromFile( const string& strFileName )
{
    if( m_FileName == strFileName )
        return true;

    m_pD3DDevice = g_RenderSystem.GetD3DDevice();

    Destory();

    //-----------------------------------------------------------------------
    // Load the XFile data.
    //-----------------------------------------------------------------------
    HRESULT hr = D3DXLoadMeshFromX( AnsiToUnicode( strFileName.c_str() ),
                                    D3DXMESH_SYSTEMMEM,
                                    m_pD3DDevice,
                                    &m_pAdjBuffer,                              // 用於描述網格的鄰接資訊
                                    &m_pMtrlBuffer,                             // materials buffer 包含網格的材質資料
                                    NULL,
                                    &m_dwNumMaterials,                          // materials number
                                    &m_pMesh );                                 // Our mesh

    if( FAILED( hr ) )
    {
    Assert( 0, "D3DXLoadMeshFromX Failed !!" );
    return false;
    }

    m_pD3DXMaterials = (D3DXMATERIAL*)m_pMtrlBuffer->GetBufferPointer();        // has mtrl and tex information
    m_pAdjacency     = (DWORD*)m_pAdjBuffer->GetBufferPointer();

    // Optimize the mesh for performance
    hr = m_pMesh->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                               m_pAdjacency, NULL, NULL, NULL );

    if( FAILED( hr ) )
    {
    Assert( 0, "OptimizeInplace Failed !!" );
    return false;
    }

    //-----------------------------------------------------------------------
    // Load Texture
    //-----------------------------------------------------------------------
    m_pMaterials.resize( m_dwNumMaterials );
    m_pTextures.resize( m_dwNumMaterials );

    // 取得檔案路徑
    string strFilePath = strFileName;
    strFilePath.erase( strFilePath.find_last_of( "\\" ) +1 );
    char szFileName[100];

    for( DWORD i=0; i<m_dwNumMaterials; ++i )
    {
    // Copy the material
    m_pMaterials[i] = m_pD3DXMaterials[i].MatD3D;

    // Set the ambient color for the material (D3DX does not do this)
    m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;

    if( m_pD3DXMaterials[i].pTextureFilename )
    {
        sprintf( szFileName, "%s%s", strFilePath.c_str(), m_pD3DXMaterials[i].pTextureFilename );  
        hr = D3DXCreateTextureFromFile( m_pD3DDevice, AnsiToUnicode( szFileName ), &m_pTextures[i] );

        if( FAILED( hr ) )
        {
            Assert( 0, "D3DXCreateTextureFromFile Failed !!" );
        }
    }
    else
    {
        m_pTextures[i] = NULL;
    }
    }

    m_FileName = strFileName;
    m_IsCreate = true;

    return true;
}
//----------------------------------------------------------------------------
// 從記憶體載入天空盒模型
//----------------------------------------------------------------------------
bool JLSkyBox::LoadFromFileInMemory( const string& strFileName )
{
    if( m_FileName == strFileName )
        return true;

    Destory();

    m_pD3DDevice = g_RenderSystem.GetD3DDevice();

    // 讀取封裝檔
    memPack* pMem = LoadFromPacket( strFileName );

#ifdef RESMGR

	//cosiann
	if( pMem->Data == NULL )
        return false;
#else

    if( pMem == NULL )
        return false;
#endif



    //-----------------------------------------------------------------------
    // Load the XFile data.
    //-----------------------------------------------------------------------
    HRESULT hr = D3DXLoadMeshFromXInMemory( pMem->Data, pMem->DataSize,
                                            D3DXMESH_SYSTEMMEM,         // 旗標--該網格將放於受管理的記憶體區
                                            m_pD3DDevice,               // Our d3dDevice
                                            &m_pAdjBuffer,              // 用於描述網格的鄰接資訊
                                            &m_pMtrlBuffer,             // materials buffer 包含網格的材質資料
                                            NULL,                       //
                                            &m_dwNumMaterials,          // materials number
                                            &m_pMesh );                 // Our mesh

    if( FAILED( hr ) )
    {
        Assert( 0, "D3DXLoadMeshFromXInMemory Failed !!" );
        return false;
    }

    m_pD3DXMaterials = (D3DXMATERIAL*)m_pMtrlBuffer->GetBufferPointer();        // has mtrl and tex information
    m_pAdjacency     = (DWORD*)m_pAdjBuffer->GetBufferPointer();

    // Optimize the mesh for performance
    hr = m_pMesh->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                                   m_pAdjacency, NULL, NULL, NULL );

    if( FAILED( hr ) )
    {
        Assert( 0, "OptimizeInplace Failed !!" );
        return false;
    }

    //-----------------------------------------------------------------------
    // Load Texture
    //-----------------------------------------------------------------------
    m_pMaterials.resize( m_dwNumMaterials );
    m_pTextures.resize( m_dwNumMaterials );

    // 取得檔案路徑
    string strFilePath = strFileName;
    strFilePath.erase( strFilePath.find_last_of( "\\" ) +1 );
    char szFileName[100];

    for( DWORD i=0; i<m_dwNumMaterials; ++i )
    {
        // Copy the material
        m_pMaterials[i] = m_pD3DXMaterials[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;

        if( m_pD3DXMaterials[i].pTextureFilename )
        {
            sprintf( szFileName, "%s%s", strFilePath.c_str(), m_pD3DXMaterials[i].pTextureFilename );

            pMem = LoadFromPacket( szFileName );

#ifdef RESMGR

			//cosiann
			if( pMem->Data == NULL )
                continue;
#else

            if( pMem == NULL )
                continue;
#endif

            hr = D3DXCreateTextureFromFileInMemory( m_pD3DDevice, pMem->Data,
                                               pMem->DataSize, &m_pTextures[i] );

            if( FAILED( hr ) )
            {
                Assert( 0, "D3DXCreateTextureFromFileInMemory Failed !!" );
            }
        }
        else
        {
            m_pTextures[i] = NULL;
        }
    }

    m_FileName = strFileName;
    m_IsCreate = true;

    return true;
}
//---------------------------------------------------------------------------
void JLSkyBox::SetScale( const D3DXVECTOR3& vScale )
{
    m_vScale = vScale;

    // 計算縮放
    D3DXMatrixScaling( &m_matWorld, m_vScale.x ,m_vScale.y, m_vScale.z );
}
//---------------------------------------------------------------------------
void JLSkyBox::Render()
{
    if( !m_IsCreate )
        return;

    m_pD3DDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    // Center view matrix for skybox and disable zbuffer
    static D3DXMATRIXA16 matView, matViewSave;
    m_pD3DDevice->GetTransform( D3DTS_VIEW, &matViewSave );

    matView = matViewSave;
    matView._41 = 0.0f;
    matView._42 = -3.0f;
    matView._43 = 0.0f;

    m_pD3DDevice->SetTransform( D3DTS_VIEW, &matView );
    m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, false ); 
    m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, false );
    m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, false );

    // 繪製天空盒模型
    for( DWORD i=0; i<m_dwNumMaterials; ++i )
    {
        // Set the material and texture for this subset
        m_pD3DDevice->SetMaterial( &m_pMaterials[i] );
        m_pD3DDevice->SetTexture( 0, m_pTextures[i] );

        // Draw the mesh subset
        m_pMesh->DrawSubset( i );
    }

    // Restore the render states
    m_pD3DDevice->SetTransform( D3DTS_VIEW, &matViewSave );
    m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, true );
    m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, true);
    m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, true );
}

//---------------------------------------------------------------------------
} //namespace FDO




