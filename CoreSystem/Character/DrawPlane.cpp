#include "stdafx.h"
#include "DrawPlane.h"
#include "PhenixD3D9RenderSystem.h"
#include "ResourceManager.h"
#include "Common.h"
#include "FDO_RoleSkin.h"

#include "CRATFileStruct.h"
#include "CRPCFileStruct.h"

#include "CRoleSkinStruct.h"
#include <algorithm>



//---------------------------------------------------------------------------
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
//----------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
const DWORD PlaneVertex::FVF  = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD PlaneVertex1::FVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD PlaneVertex2::FVF = D3DFVF_XYZ|D3DFVF_TEX1;
const DWORD PlaneVertex3::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD PlaneVertex4::FVF = D3DFVF_XYZ|D3DFVF_TEX1;
const DWORD PlaneVertex5::FVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX2;
//----------------------------------------------------------------------------
// Class DrawPlane
//----------------------------------------------------------------------------
DrawPlane::DrawPlane()
{
    m_pDevice  = g_RenderSystem.GetD3DDevice();
    m_Color    = WHITE;
    Create();
}
//----------------------------------------------------------------------------
DrawPlane::~DrawPlane()
{
    m_pDevice = NULL;
    SAFE_RELEASE( m_pVB );
}
//----------------------------------------------------------------------------
bool DrawPlane::Create()
{
    // 建立頂點緩衝區
	if(FAILED(m_pDevice->CreateVertexBuffer(6*sizeof(PlaneVertex3), 0,
                                            PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED,
                                            &m_pVB, 0)))
    {
        return false;
    }

	PlaneVertex3* v = 0;
	if( SUCCEEDED( m_pVB->Lock( 0, 0, (void**)&v, 0 ) ) )
    {
	    v[0].Position = D3DXVECTOR3(-5.0f,-5.0f, 0.0f);
        v[0].Color    = m_Color;
        v[0].tu       = 1.0f;
        v[0].tv       = 1.0f;
        v[1].Position = D3DXVECTOR3(-5.0f, 5.0f, 0.0f);
        v[1].Color    = m_Color;
        v[1].tu       = 1.0f;
        v[1].tv       = 0.0f;
        v[2].Position = D3DXVECTOR3( 5.0f,-5.0f, 0.0f);
        v[2].Color    = m_Color;
        v[2].tu       = 0.0f;
        v[2].tv       = 1.0f;
        v[3].Position = D3DXVECTOR3( 5.0f, 5.0f, 0.0f);
        v[3].Color    = m_Color;
        v[3].tu       = 0.0f;
        v[3].tv       = 0.0f;

	    m_pVB->Unlock();
    }

    return true;
}
//----------------------------------------------------------------------------
void DrawPlane::RenderPlane( const sFrame* pFrame, const D3DCOLOR& Color )
{

	D3DSURFACE_DESC desc ; 
	pFrame->pImage->pTexture->GetLevelDesc(0 , &desc) ; 

	
    PlaneVertex3* v = 0;
    if( SUCCEEDED( m_pVB->Lock(0, 0, (void**)&v, 0) ) )
    {

        v[0].Position = D3DXVECTOR3( -pFrame->fWidth, 0.0f, 0.0f );
        v[0].Color    = Color;
        v[1].Position = D3DXVECTOR3( pFrame->fWidth, 0.0f, 0.0f );
        v[1].Color    = Color;
        v[2].Position = D3DXVECTOR3( -pFrame->fWidth, pFrame->fHeight, 0.0f );
        v[2].Color    = Color;
        v[3].Position = D3DXVECTOR3( pFrame->fWidth, pFrame->fHeight, 0.0f );
        v[3].Color    = Color;
		
		float offsetU = 0.5f / (float)desc.Width ; 
		float offsetV = 0.5f / (float)desc.Height ; 
		
        // 反向貼材質
        if( pFrame->bTexInvert )
        {
            v[0].tu = pFrame->fTU1 + offsetU;
            v[0].tv = pFrame->fTV2 + offsetV;
            v[1].tu = pFrame->fTU2 + offsetU;
            v[1].tv = pFrame->fTV2 + offsetV;
            v[2].tu = pFrame->fTU1 + offsetU;
            v[2].tv = pFrame->fTV1 + offsetV;
            v[3].tu = pFrame->fTU2 + offsetU;
            v[3].tv = pFrame->fTV1 + offsetV;
        }
        else
        {
            v[0].tu = pFrame->fTU2 + offsetU;
            v[0].tv = pFrame->fTV2 + offsetV;
            v[1].tu = pFrame->fTU1 + offsetU;
            v[1].tv = pFrame->fTV2 + offsetV;
            v[2].tu = pFrame->fTU2 + offsetU;
            v[2].tv = pFrame->fTV1 + offsetV;
            v[3].tu = pFrame->fTU1 + offsetU;
            v[3].tv = pFrame->fTV1 + offsetV;
        }
        
        m_pVB->Unlock();
    }

    // 繪製圖形
    m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(PlaneVertex3) );
    m_pDevice->SetFVF( PlaneVertex3::FVF );
    m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );


}
//----------------------------------------------------------------------------
void DrawPlane::RenderPlane( const SkinFrmInst* pSFrmInst, const D3DCOLOR& Color )
{
    sFrame* pFrame = pSFrmInst->pFrame;
    SkinFrm* pSFrm = pSFrmInst->pSkinFrm;

    PlaneVertex3* v = 0;
    if( SUCCEEDED( m_pVB->Lock(0, 0, (void**)&v, 0) ) )
    {
        v[0].Position = D3DXVECTOR3( -pSFrm->fWidth, 0.0f, 0.0f );
        v[0].Color    = Color;
        v[1].Position = D3DXVECTOR3( pSFrm->fWidth, 0.0f, 0.0f );
        v[1].Color    = Color;
        v[2].Position = D3DXVECTOR3( -pSFrm->fWidth, pSFrm->fHeight, 0.0f );
        v[2].Color    = Color;
        v[3].Position = D3DXVECTOR3( pSFrm->fWidth, pSFrm->fHeight, 0.0f );
        v[3].Color    = Color;

        // 反向貼材質
        if( pFrame->bTexInvert )
        {
            v[0].tu = pSFrm->fTU1;
            v[0].tv = pSFrm->fTV2;
            v[1].tu = pSFrm->fTU2;
            v[1].tv = pSFrm->fTV2;
            v[2].tu = pSFrm->fTU1;
            v[2].tv = pSFrm->fTV1;
            v[3].tu = pSFrm->fTU2;
            v[3].tv = pSFrm->fTV1;
        }
        else
        {
            v[0].tu = pSFrm->fTU2;
            v[0].tv = pSFrm->fTV2;
            v[1].tu = pSFrm->fTU1;
            v[1].tv = pSFrm->fTV2;
            v[2].tu = pSFrm->fTU2;
            v[2].tv = pSFrm->fTV1;
            v[3].tu = pSFrm->fTU1;
            v[3].tv = pSFrm->fTV1;
        }
        
        m_pVB->Unlock();
    }

    // 繪製圖形
    m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(PlaneVertex3) );
    m_pDevice->SetFVF( PlaneVertex3::FVF );
    m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

//////////////////////////////////////////////////////////////////////////
//新結構用的
//////////////////////////////////////////////////////////////////////////
void DrawPlane::RenderPlaneNew( const SRoleSkinImageNew* pSkinImage, const D3DCOLOR& Color )
{
	SRATImageFile*	pRATImg = pSkinImage->pRATImg;
	SRPCImageFile*	pRPCImg = pSkinImage->pRPCImg;

	PlaneVertex3* v = 0;
	if( SUCCEEDED( m_pVB->Lock(0, 0, (void**)&v, 0) ) )
	{
		v[0].Position = D3DXVECTOR3( -pRPCImg->fWidth, 0.0f, 0.0f );
		v[0].Color    = Color;
		v[1].Position = D3DXVECTOR3( pRPCImg->fWidth, 0.0f, 0.0f );
		v[1].Color    = Color;
		v[2].Position = D3DXVECTOR3( -pRPCImg->fWidth, pRPCImg->fHeight, 0.0f );
		v[2].Color    = Color;
		v[3].Position = D3DXVECTOR3( pRPCImg->fWidth, pRPCImg->fHeight, 0.0f );
		v[3].Color    = Color;

		// 反向貼材質
		if( pRATImg->bTexInvert )
		{
			v[0].tu = pRPCImg->fTU1;
			v[0].tv = pRPCImg->fTV2;
			v[1].tu = pRPCImg->fTU2;
			v[1].tv = pRPCImg->fTV2;
			v[2].tu = pRPCImg->fTU1;
			v[2].tv = pRPCImg->fTV1;
			v[3].tu = pRPCImg->fTU2;
			v[3].tv = pRPCImg->fTV1;
		}
		else
		{
			v[0].tu = pRPCImg->fTU2;
			v[0].tv = pRPCImg->fTV2;
			v[1].tu = pRPCImg->fTU1;
			v[1].tv = pRPCImg->fTV2;
			v[2].tu = pRPCImg->fTU2;
			v[2].tv = pRPCImg->fTV1;
			v[3].tu = pRPCImg->fTU1;
			v[3].tv = pRPCImg->fTV1;
		}

		m_pVB->Unlock();
	}

	// 繪製圖形
	m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(PlaneVertex3) );
	m_pDevice->SetFVF( PlaneVertex3::FVF );
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

//----------------------------------------------------------------------------
// Class DrawPlane1
//----------------------------------------------------------------------------
DrawPlane1::DrawPlane1( const D3DCOLOR& Color )
{
    m_pDevice = g_RenderSystem.GetD3DDevice();
    m_Color   = Color;
}
//----------------------------------------------------------------------------
DrawPlane1::~DrawPlane1()
{
    m_pDevice = NULL;
}
//----------------------------------------------------------------------------
void DrawPlane1::RenderPlane( const RECT& rect )
{
    // 設定位置與材質座標
    PlaneVertex1 Vertices[4] =
    {
        (float)rect.left,  (float)rect.bottom, 0.0f, 1.0f, m_Color, 0.0f, 1.0f,
        (float)rect.left,  (float)rect.top,    0.0f, 1.0f, m_Color, 0.0f, 0.0f,
        (float)rect.right, (float)rect.bottom, 0.0f, 1.0f, m_Color, 1.0f, 1.0f,
        (float)rect.right, (float)rect.top,    0.0f, 1.0f, m_Color, 1.0f, 0.0f,
    };

	m_pDevice = g_RenderSystem.GetD3DDevice();

	HRESULT hr ; 

    // 繪製圖形
    hr = m_pDevice->SetFVF( PlaneVertex1::FVF );

	if(FAILED(hr))
	{
		Assert(0, "setFVF is error in RenderPlane") ; 
		return ; 
	}

    hr = m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex1 ) );

	if(FAILED(hr))
	{
		Assert(0, "DrawPrimitiveUP is error in RenderPlane") ; 

		return ; 
	}
}

//----------------------------------------------------------------------------
// Class DrawPlane2
//----------------------------------------------------------------------------
DrawPlane2::DrawPlane2( const char* szTexName )
{
    m_pDevice = g_RenderSystem.GetD3DDevice();
    D3DXMatrixIdentity( &m_matWorld );
    Create( szTexName );
}
//----------------------------------------------------------------------------
DrawPlane2::~DrawPlane2()
{
    m_pDevice = NULL;
}
//----------------------------------------------------------------------------
bool DrawPlane2::Create( const char* szTexName )
{
    //讀取材質
    m_Texture = LoadStaticTexture( szTexName );
    //m_Texture = LoadTextureFromFile( szTexName );
    return true;
}
//----------------------------------------------------------------------------
void DrawPlane2::RenderPlane( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 )
{
    // 設定位置與材質座標
    PlaneVertex4 Vertices[4] =
    {
        v0.x, v0.y, v0.z, 0.0f, 1.0f,
        v1.x, v1.y, v1.z, 0.0f, 0.0f,
        v2.x, v2.y, v2.z, 1.0f, 1.0f,
        v3.x, v3.y, v3.z, 1.0f, 0.0f,
    };



    //PlaneVertex4 Vertices[12] =
    //{
    //    v0.x, v0.y, v0.z, 0.0f, 0.0f,
    //    v1.x, v1.y, v1.z, 0.0f, 1.0f,
    //    v3.x, v3.y, v3.z, 1.0f, 1.0f,
    //    v0.x, v0.y, v0.z, 0.0f, 0.0f,
    //    v3.x, v3.y, v3.z, 1.0f, 1.0f,
    //    v2.x, v2.y, v2.z, 1.0f, 0.0f,

    //};

    // 繪製圖形
    m_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pDevice->SetTexture( 0, m_Texture );
    m_pDevice->SetFVF( PlaneVertex4::FVF );
    m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2, Vertices, sizeof( PlaneVertex4 ) );


}

//----------------------------------------------------------------------------
// Class CDrawFrame
//----------------------------------------------------------------------------
CDrawFrame::CDrawFrame()
{
    m_pDevice = g_RenderSystem.GetD3DDevice();
    D3DXMatrixIdentity( &m_matWorld );
}
//----------------------------------------------------------------------------
CDrawFrame::~CDrawFrame()
{
    m_pDevice = NULL;
}
//----------------------------------------------------------------------------
struct sDrawFrameVertex
{
    float x, y, z;
    D3DCOLOR Color;
};

void CDrawFrame::RenderPlane( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 )
{
    //--------------------------------------------------
    // See FDO::Terrain::getGCellVertices().
    //
    // v2   v3
    //  *---*
    //  | / |
    //  *---*
    // v0   v1
    //--------------------------------------------------

    const DWORD dwFVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;

    // 設定位置與顏色
    sDrawFrameVertex Vertices[4] =
    {
        v0.x, v0.y, v0.z, 0xFFFFFFFF,
        v1.x, v1.y, v1.z, 0xFFFFFFFF,
        v2.x, v2.y, v2.z, 0xFFFFFFFF,
        v3.x, v3.y, v3.z, 0xFFFFFFFF,
    };

    // Save old render states.
    DWORD dwFillMode, dwSrcBlend, dwDestBlend;
    m_pDevice->GetRenderState(D3DRS_FILLMODE, &dwFillMode);
    m_pDevice->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
    m_pDevice->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
    
    // Set new render states.
    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR);

    // 繪製圖形
    m_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pDevice->SetFVF( dwFVF );
    m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( sDrawFrameVertex ) );

    // Restore old render state.
    m_pDevice->SetRenderState(D3DRS_FILLMODE, dwFillMode);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
}
//----------------------------------------------------------------------------
// Class DrawPlane3
//----------------------------------------------------------------------------
DrawPlane3::DrawPlane3()
{
    m_pDevice  = g_RenderSystem.GetD3DDevice();
    Create();
}
//----------------------------------------------------------------------------
DrawPlane3::~DrawPlane3()
{
    m_pDevice = NULL;

    for( int i=0; i<13; ++i )
        SAFE_RELEASE( m_pVB[i] );
}
//----------------------------------------------------------------------------
bool DrawPlane3::Create()
{
    for( int i=0; i<13; ++i )
        m_Color[i] = WHITE;

    const float fUnit = 0.125f;     // 0.125f 代表是 1/8
    PlaneVertex3 Vertex[4];
    PlaneVertex3 *pV = 0;

    //建立初始模型頂點參數

    //位置
    Vertex[0].Position  = D3DXVECTOR3( -0.5f, 0.5f, 0.0f );
    Vertex[1].Position  = D3DXVECTOR3(  0.5f, 0.5f, 0.0f );
    Vertex[2].Position  = D3DXVECTOR3( -0.5f,  -0.5f, 0.0f );
    Vertex[3].Position  = D3DXVECTOR3(  0.5f,  -0.5f, 0.0f );

    //顏色
    Vertex[0].Color     = m_Color[0];   // WHITE
    Vertex[1].Color     = m_Color[0];
    Vertex[2].Color     = m_Color[0];
    Vertex[3].Color     = m_Color[0];

    //0材質軸
    Vertex[0].tu = fUnit * 0.0f;    Vertex[0].tv = fUnit * 0.0f;
    Vertex[1].tu = fUnit * 1.0f;    Vertex[1].tv = fUnit * 0.0f;
    Vertex[2].tu = fUnit * 0.0f;    Vertex[2].tv = fUnit * 2.0f;
    Vertex[3].tu = fUnit * 1.0f;    Vertex[3].tv = fUnit * 2.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[0], 0)))

    {
        return false;
    }

    if( SUCCEEDED( m_pVB[0]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[0]->Unlock();
    }

    //1材質軸
    Vertex[0].tu = fUnit * 1.0f;    Vertex[0].tv = fUnit * 0.0f;
    Vertex[1].tu = fUnit * 2.0f;    Vertex[1].tv = fUnit * 0.0f;
    Vertex[2].tu = fUnit * 1.0f;    Vertex[2].tv = fUnit * 2.0f;
    Vertex[3].tu = fUnit * 2.0f;    Vertex[3].tv = fUnit * 2.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[1], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[1]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[1]->Unlock();
    }

    //2材質軸
    Vertex[0].tu = fUnit * 2.0f;   Vertex[0].tv = fUnit * 0.0f;
    Vertex[1].tu = fUnit * 3.0f;   Vertex[1].tv = fUnit * 0.0f;
    Vertex[2].tu = fUnit * 2.0f;   Vertex[2].tv = fUnit * 2.0f;
    Vertex[3].tu = fUnit * 3.0f;   Vertex[3].tv = fUnit * 2.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[2], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[2]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[2]->Unlock();
    }

    //3材質軸
    Vertex[0].tu = fUnit * 3.0f;   Vertex[0].tv = fUnit * 0.0f;
    Vertex[1].tu = fUnit * 4.0f;   Vertex[1].tv = fUnit * 0.0f;
    Vertex[2].tu = fUnit * 3.0f;   Vertex[2].tv = fUnit * 2.0f;
    Vertex[3].tu = fUnit * 4.0f;   Vertex[3].tv = fUnit * 2.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[3], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[3]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[3]->Unlock();
    }

    //4材質軸
    Vertex[0].tu = fUnit * 4.0f;   Vertex[0].tv = fUnit * 0.0f;
    Vertex[1].tu = fUnit * 5.0f;   Vertex[1].tv = fUnit * 0.0f;
    Vertex[2].tu = fUnit * 4.0f;   Vertex[2].tv = fUnit * 2.0f;
    Vertex[3].tu = fUnit * 5.0f;   Vertex[3].tv = fUnit * 2.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[4], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[4]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[4]->Unlock();
    }

    //5材質軸
    Vertex[0].tu = fUnit * 5.0f;   Vertex[0].tv = fUnit * 0.0f;
    Vertex[1].tu = fUnit * 6.0f;   Vertex[1].tv = fUnit * 0.0f;
    Vertex[2].tu = fUnit * 5.0f;   Vertex[2].tv = fUnit * 2.0f;
    Vertex[3].tu = fUnit * 6.0f;   Vertex[3].tv = fUnit * 2.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[5], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[5]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[5]->Unlock();
    }

    //6材質軸
    Vertex[0].tu = fUnit * 6.0f;   Vertex[0].tv = fUnit * 0.0f;
    Vertex[1].tu = fUnit * 7.0f;   Vertex[1].tv = fUnit * 0.0f;
    Vertex[2].tu = fUnit * 6.0f;   Vertex[2].tv = fUnit * 2.0f;
    Vertex[3].tu = fUnit * 7.0f;   Vertex[3].tv = fUnit * 2.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[6], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[6]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[6]->Unlock();
    }

    //7材質軸
    Vertex[0].tu = fUnit * 7.0f;   Vertex[0].tv = fUnit * 0.0f;
    Vertex[1].tu = fUnit * 8.0f;   Vertex[1].tv = fUnit * 0.0f;
    Vertex[2].tu = fUnit * 7.0f;   Vertex[2].tv = fUnit * 2.0f;
    Vertex[3].tu = fUnit * 8.0f;   Vertex[3].tv = fUnit * 2.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[7], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[7]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[7]->Unlock();
    }

    //8材質軸
    Vertex[0].tu = fUnit * 0.0f;   Vertex[0].tv = fUnit * 2.0f;
    Vertex[1].tu = fUnit * 1.0f;   Vertex[1].tv = fUnit * 2.0f;
    Vertex[2].tu = fUnit * 0.0f;   Vertex[2].tv = fUnit * 4.0f;
    Vertex[3].tu = fUnit * 1.0f;   Vertex[3].tv = fUnit * 4.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[8], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[8]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[8]->Unlock();
    }

    //9材質軸
    Vertex[0].tu = fUnit * 1.0f;   Vertex[0].tv = fUnit * 2.0f;
    Vertex[1].tu = fUnit * 2.0f;   Vertex[1].tv = fUnit * 2.0f;
    Vertex[2].tu = fUnit * 1.0f;   Vertex[2].tv = fUnit * 4.0f;
    Vertex[3].tu = fUnit * 2.0f;   Vertex[3].tv = fUnit * 4.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[9], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[9]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[9]->Unlock();
    }

    //爆擊
    Vertex[0].Position = D3DXVECTOR3( -3.0f, -3.0f, -0.01f );
    Vertex[1].Position = D3DXVECTOR3(  3.0f, -3.0f, -0.01f );
    Vertex[2].Position = D3DXVECTOR3( -3.0f,  3.0f, -0.01f );
    Vertex[3].Position = D3DXVECTOR3(  3.0f,  3.0f, -0.01f );

    Vertex[0].tu = fUnit * 6.0f;   Vertex[0].tv = fUnit * 8.0f;
    Vertex[1].tu = fUnit * 4.0f;   Vertex[1].tv = fUnit * 8.0f;
    Vertex[2].tu = fUnit * 6.0f;   Vertex[2].tv = fUnit * 4.0f;
    Vertex[3].tu = fUnit * 4.0f;   Vertex[3].tv = fUnit * 4.0f;


    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[10], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[10]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[10]->Unlock();
    }

    //挌擋
    Vertex[0].Position = D3DXVECTOR3( -2.0f, -2.0f, -0.01f );
    Vertex[1].Position = D3DXVECTOR3(  2.0f, -2.0f, -0.01f );
    Vertex[2].Position = D3DXVECTOR3( -2.0f,  2.0f, -0.01f );
    Vertex[3].Position = D3DXVECTOR3(  2.0f,  2.0f, -0.01f );
    
    Vertex[0].tu = fUnit * 8.0f;   Vertex[0].tv = fUnit * 8.0f;
    Vertex[1].tu = fUnit * 6.0f;   Vertex[1].tv = fUnit * 8.0f;
    Vertex[2].tu = fUnit * 8.0f;   Vertex[2].tv = fUnit * 4.0f;
    Vertex[3].tu = fUnit * 6.0f;   Vertex[3].tv = fUnit * 4.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[11], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[11]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[11]->Unlock();
    }

    //Miss
    Vertex[0].tu = fUnit * 4.0f;   Vertex[0].tv = fUnit * 8.0f;
    Vertex[1].tu = fUnit * 2.0f;   Vertex[1].tv = fUnit * 8.0f;
    Vertex[2].tu = fUnit * 4.0f;   Vertex[2].tv = fUnit * 4.0f;
    Vertex[3].tu = fUnit * 2.0f;   Vertex[3].tv = fUnit * 4.0f;

    if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0, PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED, &m_pVB[12], 0)))
    {
        return false;
    }

    if( SUCCEEDED( m_pVB[12]->Lock(0, 0, (void**)&pV, 0 ) ) )
    {
        memcpy( pV, Vertex, 4*sizeof(PlaneVertex3) );
        m_pVB[12]->Unlock();
    }

    //載入材質  
    m_Texture = LoadStaticTexture( "UI\\Number.tga" );
    //m_Texture = LoadTextureFromFile( "Number.tga" );

    return true;
}
//----------------------------------------------------------------------------
const LPDIRECT3DVERTEXBUFFER9 DrawPlane3::GetVertexBuf( int iIndex, const DWORD& Color )
{
    if( (iIndex < 0) || (iIndex > 12) )
        return NULL;

    // 更新顏色
    if( m_Color[iIndex] != Color )
    {
        PlaneVertex3 *pV = 0;

        if( SUCCEEDED( m_pVB[iIndex]->Lock( 0, 0, (void**)&pV, 0 ) ) )
        {
            pV[0].Color = pV[1].Color = Color;
            pV[2].Color = pV[3].Color = Color;
            m_pVB[iIndex]->Unlock();

            m_Color[iIndex] = Color;
        }
    }

    return m_pVB[iIndex];
}
//----------------------------------------------------------------------------
void DrawPlane3::RenderPlane( const BYTE& Type, const int& iNum, const D3DCOLOR& Color, const D3DXMATRIX& matWorld )
{
    // 繪製圖形
    m_pDevice->SetTexture( 0, m_Texture );
    m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );
    LPDIRECT3DVERTEXBUFFER9 pVB = 0;

    switch( Type )
    {
        case 0x04:     // 爆擊
        {
            DWORD dwColor = (Color | 0x00FFFFFF);
            pVB = GetVertexBuf( 10, dwColor );
            m_pDevice->SetStreamSource( 0, pVB, 0, sizeof(PlaneVertex3) );
            m_pDevice->SetFVF( PlaneVertex3::FVF );
            m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
            break;
        }

        case 0x05:     // 隔擋
        {
            DWORD dwColor = (Color | 0x00FFFFFF);
            pVB = GetVertexBuf( 11, dwColor );
            m_pDevice->SetStreamSource( 0, pVB, 0, sizeof(PlaneVertex3) );
            m_pDevice->SetFVF( PlaneVertex3::FVF );
            m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
            break;
        }

        case 0x06:     // Miss
            pVB = GetVertexBuf( 12, Color );
            m_pDevice->SetStreamSource( 0, pVB, 0, sizeof(PlaneVertex3) );
            m_pDevice->SetFVF( PlaneVertex3::FVF );
            m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
            return;
    }

    if( iNum == 0 ) return;

    //繪製數字 ( 0 ~ 99999 )
    int iNum1, iNum2, iNum3, iNum4, iNum5;
    float fOffSet1, fOffSet2, fOffSet3, fOffSet4, fOffSet5;
    D3DXMATRIX matTemp, matScale;

    // 計算數字內容
    iNum1 = iNum % 10;
    iNum2 = (iNum / 10) % 10;
    iNum3 = (iNum / 100) % 10;
    iNum4 = (iNum / 1000) % 10;
    iNum5 = iNum / 10000;

    // 計算數字偏移位置
    if( iNum5 )  // 五位數
    {
        fOffSet1 = 1.64f;
        fOffSet2 = 0.82f;
        fOffSet3 = 0.0f;
        fOffSet4 = -0.82f;
        fOffSet5 = -1.64f;
        D3DXMatrixScaling( &matScale, 1.5f, 1.5f, 1.5f );
    }
    else if( iNum4 )  // 四位數
    {
        fOffSet1 = 1.2f;
        fOffSet2 = 0.4f;
        fOffSet3 = -0.4f;
        fOffSet4 = -1.2f;
        D3DXMatrixScaling( &matScale, 1.3f, 1.3f, 1.3f );
    }
    else if( iNum3 )  // 三位數
    {
        fOffSet1 = 0.8f;
        fOffSet2 =  0.0f;
        fOffSet3 = -0.8f;
        D3DXMatrixScaling( &matScale, 1.2f, 1.2f, 1.2f );
    }
    else if( iNum2 )  // 二位數
    {
        fOffSet1 = 0.4f;
        fOffSet2 = -0.4f;
        D3DXMatrixScaling( &matScale, 1.1f, 1.1f, 1.1f );
    }
    else  // 一位數
    {
        fOffSet1 = 0.0f;
        D3DXMatrixScaling( &matScale, 1.0f, 1.0f, 1.0f );
    }

    if( iNum5 || iNum4 || iNum3 ||iNum2 || iNum1 )  //個位數
    {
        pVB = GetVertexBuf( iNum1, Color );
        D3DXMatrixTranslation( &matTemp, fOffSet1, 0.0f, 0.0f );
        D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
        D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );
        m_pDevice->SetTransform( D3DTS_WORLD, &matTemp );
        m_pDevice->SetFVF( PlaneVertex3::FVF );
        m_pDevice->SetStreamSource( 0, pVB, 0, sizeof(PlaneVertex3) );
        m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }
    if( iNum5 || iNum4 || iNum3 ||iNum2 )   //十位數
    {
        pVB = GetVertexBuf( iNum2, Color );
        D3DXMatrixTranslation( &matTemp, fOffSet2, 0.0f, 0.0f );
        D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
        D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );
        m_pDevice->SetTransform( D3DTS_WORLD, &matTemp );
        m_pDevice->SetFVF( PlaneVertex3::FVF );
        m_pDevice->SetStreamSource( 0, pVB, 0, sizeof(PlaneVertex3) );
        m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }
    if( iNum5 || iNum4 || iNum3 )   //百位數
    {
        pVB = GetVertexBuf( iNum3, Color );
        D3DXMatrixTranslation( &matTemp, fOffSet3, 0.0f, 0.0f );
        D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
        D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );
        m_pDevice->SetTransform( D3DTS_WORLD, &matTemp );
        m_pDevice->SetFVF( PlaneVertex3::FVF );
        m_pDevice->SetStreamSource( 0, pVB, 0, sizeof(PlaneVertex3) );
        m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }
    if( iNum5 || iNum4 )    //千位數
    {
        pVB = GetVertexBuf( iNum4, Color );
        D3DXMatrixTranslation( &matTemp, fOffSet4, 0.0f, 0.0f );
        D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
        D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );
        m_pDevice->SetTransform( D3DTS_WORLD, &matTemp );
        m_pDevice->SetFVF( PlaneVertex3::FVF );
        m_pDevice->SetStreamSource( 0, pVB, 0, sizeof(PlaneVertex3) );
        m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }
    if( iNum5 )     //萬位數
    {
        pVB = GetVertexBuf( iNum5, Color );
        D3DXMatrixTranslation( &matTemp, fOffSet5, 0.0f, 0.0f );
        D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
        D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );
        m_pDevice->SetTransform( D3DTS_WORLD, &matTemp );
        m_pDevice->SetFVF( PlaneVertex3::FVF );
        m_pDevice->SetStreamSource( 0, pVB, 0, sizeof(PlaneVertex3) );
        m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }  
}

//----------------------------------------------------------------------------
// Class DrawPlane4
//----------------------------------------------------------------------------
DrawPlane4::DrawPlane4()
{
    m_pDevice = g_RenderSystem.GetD3DDevice();
}
//----------------------------------------------------------------------------
DrawPlane4::~DrawPlane4()
{
    m_pDevice = NULL;
}
//----------------------------------------------------------------------------
void DrawPlane4::RenderPlane( const RECT& rect, const D3DCOLOR& Color )
{
    //   
    // 設定位置與材質座標
    //PlaneVertex1 Vertices[4] =
    //{ (float)rect.left,  (float)rect.bottom, 0.0f, 1.0f, Color, 0.0f, 1.0f,
    //    (float)rect.left,  (float)rect.top,    0.0f, 1.0f, Color, 0.0f, 0.0f,
    //    (float)rect.right, (float)rect.bottom, 0.0f, 1.0f, Color, 1.0f, 1.0f,
    //    (float)rect.right, (float)rect.top,    0.0f, 1.0f, Color, 1.0f, 0.0f,
    //};

    PlaneVertex5 Vertices[4] =
    {
        (float)rect.left-0.5f,  (float)rect.bottom-0.5f, 0.0f, 1.0f, Color, 0.0f, 1.0f,0.0f , 1.0f ,
        (float)rect.left-0.5f,  (float)rect.top-0.5f,    0.0f, 1.0f, Color, 0.0f, 0.0f,0.0f , 0.0f ,
        (float)rect.right-0.5f, (float)rect.bottom-0.5f, 0.0f, 1.0f, Color, 1.0f, 1.0f,1.0f , 1.0f ,
        (float)rect.right-0.5f, (float)rect.top-0.5f,    0.0f, 1.0f, Color, 1.0f, 0.0f,1.0f , 0.0f ,
    };


    // 繪製圖形
    m_pDevice->SetFVF( PlaneVertex5::FVF );
    m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex5 ) );
}

void DrawPlane4::RenderPlane( int w, int h, const D3DCOLOR& Color )
{
    //   
    // 設定位置與材質座標
    //PlaneVertex1 Vertices[4] =
    //{ (float)rect.left,  (float)rect.bottom, 0.0f, 1.0f, Color, 0.0f, 1.0f,
    //    (float)rect.left,  (float)rect.top,    0.0f, 1.0f, Color, 0.0f, 0.0f,
    //    (float)rect.right, (float)rect.bottom, 0.0f, 1.0f, Color, 1.0f, 1.0f,
    //    (float)rect.right, (float)rect.top,    0.0f, 1.0f, Color, 1.0f, 0.0f,
    //};

	float fw = (float)w;
	float fh = (float)h;
	//assume the pic is 4:3
	float pic_width = (4*fh)/3;
	float half_texU = (fw/pic_width)*0.5f;;
	float start_u = 0.5f - half_texU;
	float end_u = 0.5f + half_texU;
    PlaneVertex5 Vertices[4] =
    {
        (float)-0.5f,  (float)fh-0.5f, 0.0f, 1.0f, Color, start_u, 1.0f, 0.0f, 1.0f ,
        (float)-0.5f,  (float)-0.5f,    0.0f, 1.0f, Color, start_u, 0.0f, 0.0f , 0.0f ,
        (float)fw-0.5f, (float)fh-0.5f, 0.0f, 1.0f, Color, end_u, 1.0f, 1.0f , 1.0f ,
        (float)fw-0.5f, (float)-0.5f,    0.0f, 1.0f, Color, end_u, 0.0f, 1.0f , 0.0f ,
    };


    // 繪製圖形
    m_pDevice->SetFVF( PlaneVertex5::FVF );
    m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex5 ) );
}
//----------------------------------------------------------------------------
// Class DrawPlane5
//----------------------------------------------------------------------------
DrawPlane5::DrawPlane5()
{
    m_pDevice  = g_RenderSystem.GetD3DDevice();
    Create();
}
//----------------------------------------------------------------------------
DrawPlane5::~DrawPlane5()
{
    m_pDevice = NULL;
    SAFE_RELEASE( m_pVB );
}
//----------------------------------------------------------------------------
bool DrawPlane5::Create()
{
    // 建立頂點緩衝區
	if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0,
                                            PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED,
                                            &m_pVB, 0)))
    {
        return false;
    }

	PlaneVertex3* v = 0;
	if( SUCCEEDED( m_pVB->Lock( 0, 0, (void**)&v, 0 ) ) )
    {     
        v[0].Position = D3DXVECTOR3( -2.0f, -2.0f, 0.0f );
        v[0].Color    = WHITE;
        v[0].tu       = 1.0f;
        v[0].tv       = 1.0f;
        v[1].Position = D3DXVECTOR3(  2.0f, -2.0f, 0.0f );
        v[1].Color    = WHITE;
        v[1].tu       = 0.0f;
        v[1].tv       = 1.0f;
        v[2].Position = D3DXVECTOR3( -2.0f,  2.0f, 0.0f );
        v[2].Color    = WHITE;
        v[2].tu       = 1.0f;
        v[2].tv       = 0.0f;
        v[3].Position = D3DXVECTOR3(  2.0f,  2.0f, 0.0f );
        v[3].Color    = WHITE;
        v[3].tu       = 0.0f;
        v[3].tv       = 0.0f;

	    m_pVB->Unlock();
    }

    return true;
}
//----------------------------------------------------------------------------
void DrawPlane5::RenderPlane()
{
    // 繪製圖形
    m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(PlaneVertex3) );
    m_pDevice->SetFVF( PlaneVertex3::FVF );
    m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );  
}
//----------------------------------------------------------------------------
// Class DrawPlane6
//----------------------------------------------------------------------------
DrawPlane6::DrawPlane6()
{
    m_pDevice  = g_RenderSystem.GetD3DDevice();
    Create();
}
//----------------------------------------------------------------------------
DrawPlane6::~DrawPlane6()
{
    m_pDevice = NULL;
    SAFE_RELEASE( m_pVB );
}
//----------------------------------------------------------------------------
bool DrawPlane6::Create() // => _DrawChargeBack (charge-bar background)
{
    // 建立頂點緩衝區
	if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0,
                                            PlaneVertex3::FVF,
                                            D3DPOOL_MANAGED,
                                            &m_pVB, 0)))
    {
        return false;
    }

	PlaneVertex3* v = 0;
	if( SUCCEEDED( m_pVB->Lock( 0, 0, (void**)&v, 0 ) ) )
    {
	    v[0].Position = D3DXVECTOR3(-2.05f, -0.18f, 0.0f);
        v[0].Color    = 0x88000000;
        v[0].tu       = 1.0f;
        v[0].tv       = 1.0f;
        v[1].Position = D3DXVECTOR3( 2.05f, -0.18f, 0.0f);
        v[1].Color    = 0x88000000;
        v[1].tu       = 0.0f;
        v[1].tv       = 1.0f;
        v[2].Position = D3DXVECTOR3(-2.05f,  0.18f, 0.0f);
        v[2].Color    = 0x88000000;
        v[2].tu       = 1.0f;
        v[2].tv       = 0.0f;
        v[3].Position = D3DXVECTOR3( 2.05f,  0.18f, 0.0f);
        v[3].Color    = 0x88000000;
        v[3].tu       = 0.0f;
        v[3].tv       = 0.0f;

	    m_pVB->Unlock();
    }

    return true;
}
//----------------------------------------------------------------------------
void DrawPlane6::RenderPlane()
{
    // 繪製圖形
    m_pDevice->SetTexture( 0, NULL );
    m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(PlaneVertex3) );
    m_pDevice->SetFVF( PlaneVertex3::FVF );
    m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

//----------------------------------------------------------------------------
// Class DrawPlane8
//----------------------------------------------------------------------------
DrawPlane8::DrawPlane8()
{
    m_pDevice = g_RenderSystem.GetD3DDevice();
}
//----------------------------------------------------------------------------
DrawPlane8::~DrawPlane8()
{
    m_pDevice = NULL;
}
//----------------------------------------------------------------------------
void DrawPlane8::RenderPlane( const RECT& rect, bool bInverse )
{
    // 反向材質
    if( bInverse )
    {
        // 設定位置與材質座標
        PlaneVertex1 Vertices[4] =
        {
            (float)rect.left,  (float)rect.bottom, 0.0f, 1.0f, WHITE, 1.0f, 1.0f,
            (float)rect.left,  (float)rect.top,    0.0f, 1.0f, WHITE, 1.0f, 0.0f,
            (float)rect.right, (float)rect.bottom, 0.0f, 1.0f, WHITE, 0.0f, 1.0f,
            (float)rect.right, (float)rect.top,    0.0f, 1.0f, WHITE, 0.0f, 0.0f,
        };

        // 繪製圖形
        m_pDevice->SetFVF( PlaneVertex1::FVF );
        m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex1 ) );
    }
    else
    {
        // 設定位置與材質座標
        PlaneVertex1 Vertices[4] =
        {
            (float)rect.left,  (float)rect.bottom, 0.0f, 1.0f, WHITE, 0.0f, 1.0f,
            (float)rect.left,  (float)rect.top,    0.0f, 1.0f, WHITE, 0.0f, 0.0f,
            (float)rect.right, (float)rect.bottom, 0.0f, 1.0f, WHITE, 1.0f, 1.0f,
            (float)rect.right, (float)rect.top,    0.0f, 1.0f, WHITE, 1.0f, 0.0f,
        };

        // 繪製圖形
        m_pDevice->SetFVF( PlaneVertex1::FVF );
        m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex1 ) );
    }
}

//----------------------------------------------------------------------------
// Class DrawPlane9
//----------------------------------------------------------------------------
DrawPlane9::DrawPlane9()
{
    m_pDevice = g_RenderSystem.GetD3DDevice();
}
//----------------------------------------------------------------------------
DrawPlane9::~DrawPlane9()
{
    m_pDevice = NULL;
}
//----------------------------------------------------------------------------
void DrawPlane9::RenderPlane( const RECT& rect, const sFrame* pFrame )
{
    // 反向材質
    if( pFrame->bTexInvert )
    {
        // 設定位置與材質座標
        PlaneVertex1 Vertices[4] =
        {
            (float)rect.left,  (float)rect.bottom, 0.0f, 1.0f, WHITE, pFrame->fTU2, pFrame->fTV2,
            (float)rect.left,  (float)rect.top,    0.0f, 1.0f, WHITE, pFrame->fTU2, pFrame->fTV1,
            (float)rect.right, (float)rect.bottom, 0.0f, 1.0f, WHITE, pFrame->fTU1, pFrame->fTV2,
            (float)rect.right, (float)rect.top,    0.0f, 1.0f, WHITE, pFrame->fTU1, pFrame->fTV1,
        };

        // 繪製圖形
        m_pDevice->SetFVF( PlaneVertex1::FVF );
        m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex1 ) );
    }
    else
    {
        // 設定位置與材質座標
        PlaneVertex1 Vertices[4] =
        {
            (float)rect.left,  (float)rect.bottom, 0.0f, 1.0f, WHITE, pFrame->fTU1, pFrame->fTV2,
            (float)rect.left,  (float)rect.top,    0.0f, 1.0f, WHITE, pFrame->fTU1, pFrame->fTV1,
            (float)rect.right, (float)rect.bottom, 0.0f, 1.0f, WHITE, pFrame->fTU2, pFrame->fTV2,
            (float)rect.right, (float)rect.top,    0.0f, 1.0f, WHITE, pFrame->fTU2, pFrame->fTV1,
        };

        // 繪製圖形
        m_pDevice->SetFVF( PlaneVertex1::FVF );
        m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex1 ) );
    }
}


//----------------------------------------------------------------------------
// Class DrawPlane11
//----------------------------------------------------------------------------
DrawPlane11::DrawPlane11( const char* szTexName )
{
    m_pDevice = g_RenderSystem.GetD3DDevice();
    D3DXMatrixIdentity( &m_matWorld );
    Create( szTexName );
}
//----------------------------------------------------------------------------
DrawPlane11::~DrawPlane11()
{
    m_pDevice = NULL;
}
//----------------------------------------------------------------------------
bool DrawPlane11::Create( const char* szTexName )
{
    //讀取材質
    m_Texture = LoadStaticTexture( szTexName );
    //m_Texture = LoadTextureFromFile( szTexName );

    return true;
}
//----------------------------------------------------------------------------
void DrawPlane11::RenderPlane( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
                               const D3DXVECTOR3& v3, const D3DXVECTOR3& v4 )
{
    // 設定位置與材質座標
    PlaneVertex4 Vertices[6] =
    {
        v0.x, v0.y, v0.z, 0.5f, 0.5f,
        v1.x, v1.y, v1.z, 0.0f, 1.0f,
        v2.x, v2.y, v2.z, 0.0f, 0.0f,
        v3.x, v3.y, v3.z, 1.0f, 0.0f,
        v4.x, v4.y, v4.z, 1.0f, 1.0f,
        v1.x, v1.y, v1.z, 0.0f, 1.0f,
    };

    // 繪製圖形
    m_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pDevice->SetTexture( 0, m_Texture );
    m_pDevice->SetFVF( PlaneVertex4::FVF );
    m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 4, Vertices, sizeof( PlaneVertex4 ) );  
}

//----------------------------------------------------------------------------
// Class DrawPlane12
//----------------------------------------------------------------------------
DrawPlane12::DrawPlane12( const D3DCOLOR& Color )
{
    m_pDevice = g_RenderSystem.GetD3DDevice();
    m_Color = Color;
}
//----------------------------------------------------------------------------
DrawPlane12::~DrawPlane12()
{
    m_pDevice = NULL;
}
//----------------------------------------------------------------------------
void DrawPlane12::RenderPlane( const RECT& rect, const D3DCOLOR& Color )
{
    // 設定位置與材質座標
    PlaneVertex1 Vertices[4] =
    {
        (float)rect.left,  (float)rect.bottom, 0.0f, 1.0f, m_Color, 0.0f, 1.0f,
        (float)rect.left,  (float)rect.top,    0.0f, 1.0f, Color,   0.0f, 0.0f,
        (float)rect.right, (float)rect.bottom, 0.0f, 1.0f, Color,   1.0f, 1.0f,
        (float)rect.right, (float)rect.top,    0.0f, 1.0f, m_Color, 1.0f, 0.0f,
    };

    // 繪製圖形
    m_pDevice->SetFVF( PlaneVertex1::FVF );
    m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex1 ) );
}
//////////////////////////////////////////////////////////////////////////
DrawPlane13::DrawPlane13()
{
	m_pDevice = g_RenderSystem.GetD3DDevice();

	if(FAILED(m_pDevice->CreateVertexBuffer(4*sizeof(PlaneVertex3), 0,
		PlaneVertex3::FVF,
		D3DPOOL_MANAGED,
		&m_pVB, 0)))
	{
		assert(0);
	}
}
//////////////////////////////////////////////////////////////////////////
DrawPlane13::~DrawPlane13()
{
	SAFE_RELEASE( m_pVB );
	m_pDevice = NULL;
}
//////////////////////////////////////////////////////////////////////////
void DrawPlane13::RenderPlane( const PlaneVertex3* p0, const PlaneVertex3* p1 , const PlaneVertex3* p2,const D3DCOLOR& Color )
{

	PlaneVertex3* pvVertexZC ;
	if (m_pVB->Lock (0, 0, (void**)&pvVertexZC, D3DLOCK_DISCARD) != D3D_OK)
		//	if (p2DVertex->Lock (0, 0, (void**)&pvVertexZC, 0) != D3D_OK)
	{
		assert (0) ;
		return ;
	}

	//	DWORD _color = D3DCOLOR_ARGB (color.A, color.R, color.G, color.B) ;

	//left, top

	pvVertexZC[0].Position.x = p0->Position.x ;
	pvVertexZC[0].Position.y = p0->Position.y ;
	
	pvVertexZC[0].Color = Color  ;

	//right, top
	pvVertexZC[1].Position.x = p1->Position.x ;
	pvVertexZC[1].Position.y = p1->Position.y ;
	
	pvVertexZC[1].Color = Color  ;

	//left,bottom
	pvVertexZC[2].Position.x = p2->Position.x ;
	pvVertexZC[2].Position.y = p2->Position.y ;
	
	pvVertexZC[2].Color = Color  ;

	m_pVB->Unlock () ;

	m_pDevice->SetTexture (0, NULL);

	//		if (color.A < 255)//alpha
// 	if (1)
// 	{
		m_pDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_pDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_pDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
// 

		DWORD tmps[4];
		m_pDevice->GetRenderState(D3DRS_BLENDOP,&tmps[0]);
		m_pDevice->GetRenderState(D3DRS_SRCBLEND,&tmps[1]);
		m_pDevice->GetRenderState(D3DRS_DESTBLEND,&tmps[2]);
		m_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&tmps[3]);

		m_pDevice->SetRenderState (D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_pDevice->SetRenderState (D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState (D3DRS_ALPHABLENDENABLE, TRUE) ;
// 	}
// 
	m_pDevice->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	//m_pDevice->DrawPrimitive (D3DPT_TRIANGLESTRIP, 0, 1);

	

	// 繪製圖形
	m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(PlaneVertex3) );
	m_pDevice->SetFVF( PlaneVertex3::FVF );
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );


	m_pDevice->SetRenderState (D3DRS_BLENDOP, tmps[0]);
	m_pDevice->SetRenderState (D3DRS_SRCBLEND, tmps[1]);
	m_pDevice->SetRenderState (D3DRS_DESTBLEND, tmps[2]);
	m_pDevice->SetRenderState (D3DRS_ALPHABLENDENABLE, tmps[3]) ;
	
}
	

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//新的drwarole
//////////////////////////////////////////////////////////////////////////
void DrawRole( const SRoleSkinImageNew* pSkinImage, const D3DCOLOR& Color )
{
	static DrawPlane _DrawRole;
	_DrawRole.RenderPlaneNew( pSkinImage, Color );
}

//----------------------------------------------------------------------------
//  static 物件之函式
//----------------------------------------------------------------------------
void DrawRole( const SkinFrmInst* pSFrmInst, const D3DCOLOR& Color )
{
    static DrawPlane _DrawRole;
    _DrawRole.RenderPlane( pSFrmInst, Color );
}
void DrawRole( const sFrame* pFrame, const D3DCOLOR& Color)
{
    static DrawPlane _DrawRole0 ; //, _DrawRole1, _DrawRole2, _DrawRole3, _DrawRole4, _DrawRole5;
    static int iDrawNumber = 0;

  //  if( iDrawNumber == 0 )
        _DrawRole0.RenderPlane( pFrame, Color );
    //else if( iDrawNumber == 1 )
    //    _DrawRole1.RenderPlane( pFrame, Color );
    //else if( iDrawNumber == 2 )
    //    _DrawRole2.RenderPlane( pFrame, Color);
    //else if( iDrawNumber == 3 )
    //    _DrawRole3.RenderPlane( pFrame, Color);
    //else if( iDrawNumber == 4 )
    //    _DrawRole4.RenderPlane( pFrame, Color);
    //else
    //    _DrawRole5.RenderPlane( pFrame, Color);

    //if( ++iDrawNumber > 5 )
    //    iDrawNumber = 0;
}

//----------------------------------------------------------------------------
void DrawEmotion()
{
    static DrawPlane5 _DrawEmotion0, _DrawEmotion1, _DrawEmotion2;
    static int iDrawNumber = 0;

    if( iDrawNumber == 0 )
        _DrawEmotion0.RenderPlane();
    else if( iDrawNumber == 1 )
        _DrawEmotion1.RenderPlane();
    else
        _DrawEmotion2.RenderPlane();

    if( ++iDrawNumber > 2 )
        iDrawNumber = 0;
}

//----------------------------------------------------------------------------
void DrawChargeBackBar()
{
    static DrawPlane6 _DrawChargeBack;
    _DrawChargeBack.RenderPlane();
}



//----------------------------------------------------------------------------
void DrawBackground( const RECT& rect )
{
    static DrawPlane1 _DrawBackground( 0x88000000 );
    _DrawBackground.RenderPlane( rect );
}
//----------------------------------------------------------------------------
void DrawBackground( const RECT& rect , const D3DCOLOR& Color )
{	
	static DrawPlane4 _DrawBackground;
	_DrawBackground.RenderPlane( rect , Color);
}



//----------------------------------------------------------------------------
void DrawBar( const RECT& rect, const D3DCOLOR& Color )
{
    static DrawPlane12 _DrawBar( WHITE );
    _DrawBar.RenderPlane( rect, Color );
}

//----------------------------------------------------------------------------
void DrawFadeOut( const RECT& rect, const D3DCOLOR& Color )
{
    static DrawPlane4 _DrawFadeOut;
    _DrawFadeOut.RenderPlane( rect, Color );
}

void DrawFadeOut(int w, int h, const D3DCOLOR& Color )
{
    static DrawPlane4 _DrawFadeOut;
    _DrawFadeOut.RenderPlane(w, h, Color );
}
//----------------------------------------------------------------------------
void DrawCursor( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 )
{
    static DrawPlane2 _DrawCursor( "UI\\Cursor.tga" );
    _DrawCursor.RenderPlane( v0, v1, v2, v3 );
}
//----------------------------------------------------------------------------
void DrawFrame( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 )
{
    static CDrawFrame _DrawFrame;
    _DrawFrame.RenderPlane( v0, v1, v2, v3 );
}
//----------------------------------------------------------------------------
void DrawDamage( const BYTE& Type, const int& iNum, const D3DCOLOR& Color, const D3DXMATRIX& matWorld )
{
    static DrawPlane3  _DrawDamage0 ; //, _DrawDamage1, _DrawDamage2;
  //  static int iDrawNumber = 0;
       _DrawDamage0.RenderPlane( Type, iNum, Color, matWorld );

    //if( iDrawNumber == 0 )
    //else if( iDrawNumber == 1 )
    //    _DrawDamage1.RenderPlane( Type, iNum, Color, matWorld );
    //else
    //    _DrawDamage2.RenderPlane( Type, iNum, Color, matWorld );

//    if( ++iDrawNumber > 2 )
 //       iDrawNumber = 0; 
}

//----------------------------------------------------------------------------
void DrawAnimation( const RECT& rect, bool bInverse )
{
    static DrawPlane8 _DrawAnimation;
    _DrawAnimation.RenderPlane( rect, bInverse );
}

//----------------------------------------------------------------------------
void DrawDisplayRole( const RECT& rect, const sFrame* pFrame )
{
    static DrawPlane9 _DrawDisplayRole;     	
	
    _DrawDisplayRole.RenderPlane( rect, pFrame );
	
}
//
void DrawTimeSquare( float rate , const RECT& rect, const D3DCOLOR& Color, bool DrawBlack )
{
	struct TimeSquareVertex
	{
		PlaneVertex3 vPoint[8];

		TimeSquareVertex()
		{
			D3DCOLOR color = 0x99000000 ;

			//順時針移動
			vPoint[0].Position.x = 0 ;
			vPoint[0].Position.y = -1 ;
			vPoint[0].Color = color ;

			vPoint[1].Position.x = 1 ;
			vPoint[1].Position.y = -1 ;
			vPoint[1].Color = color ;

			vPoint[2].Position.x = 1 ;
			vPoint[2].Position.y = 0 ;
			vPoint[2].Color = color ;

			vPoint[3].Position.x = 1 ;
			vPoint[3].Position.y = 1 ;
			vPoint[3].Color = color ;

			vPoint[4].Position.x = 0 ;
			vPoint[4].Position.y = 1 ;
			vPoint[4].Color = color ;

			vPoint[5].Position.x = -1 ;
			vPoint[5].Position.y = 1 ;
			vPoint[5].Color = color ;

			vPoint[6].Position.x = -1 ;
			vPoint[6].Position.y = 0 ;
			vPoint[6].Color = color ;

			vPoint[7].Position.x = -1 ;
			vPoint[7].Position.y = -1 ;
			vPoint[7].Color = color ;
		}

		inline operator PlaneVertex3*() { return vPoint; }
	};
	static TimeSquareVertex vPoint;
	static DrawPlane13 _draw;
	if (rate <= 0.0f)
		return ;
	float rateSegment = 1.0f/8.0f ;
	float curMaxRate = 0.0f ;
	PlaneVertex3 vTmpPnt[3] ;
	D3DXVECTOR2 offset ;
	offset.x = (rect.left+rect.right)/2.0f ;
	offset.y = (rect.top+rect.bottom)/2.0f ;
	vTmpPnt[0].Position.x = offset.x ;
	vTmpPnt[0].Position.y = offset.y ;
	vTmpPnt[0].Color = 0x99000000;	


	POINT size ;
	
	size.x = RectWidth(rect)/2 ;
	size.y = RectHeight(rect)/2 ;

	bool mBlackPlus = DrawBlack;
	enum
	{
		DrawAllBlack = 0,
		DrawSomeBlack,
		DrawNull,
	};

	for (int i = 0; i<8; i++)
	{
		uint DrawBlack = DrawNull;
		if (curMaxRate < rate)
		{
			//只有部分
			//			if (rate > curMaxRate-rateSegment)
			if (curMaxRate+rateSegment > rate)
			{
				DrawBlack = DrawSomeBlack;
			}
			else 
			{
				if( !mBlackPlus )
					DrawBlack = DrawNull;
				else
					DrawBlack = DrawAllBlack;
			}


		}else
		{
			if( !mBlackPlus )
			{
				DrawBlack = DrawAllBlack;
			}
		}

		if( DrawBlack == DrawSomeBlack )
		{
				float leftRate = 1.0f - (curMaxRate+rateSegment-rate)/rateSegment ;

				vTmpPnt[1] = vPoint[i] ;
				vTmpPnt[1].Position.x = vTmpPnt[1].Position.x*size.x+offset.x ;
				vTmpPnt[1].Position.y = vTmpPnt[1].Position.y*size.y+offset.y ;

				if (i == 7)
					vTmpPnt[2] = vPoint[0] ;
				else
					vTmpPnt[2] = vPoint[i+1] ;

				uint i1;
				if( !mBlackPlus )
					i1 = 1;
				else
					i1 = 2;


				vTmpPnt[2].Position.x = vTmpPnt[2].Position.x*size.x+offset.x ;
				vTmpPnt[2].Position.y = vTmpPnt[2].Position.y*size.y+offset.y ;

				vTmpPnt[i1].Position.x = vTmpPnt[1].Position.x+(vTmpPnt[2].Position.x-vTmpPnt[1].Position.x)*leftRate ;
				vTmpPnt[i1].Position.y = vTmpPnt[1].Position.y+(vTmpPnt[2].Position.y-vTmpPnt[1].Position.y)*leftRate ;

				_draw.RenderPlane(vTmpPnt+0, vTmpPnt+2, vTmpPnt+1,Color);
				//RenderTriangle (vTmpPnt+0, vTmpPnt+1, vTmpPnt+2) ;	
		}
		else if( DrawBlack == DrawAllBlack )
		{
				//一塊完整
				vTmpPnt[1] = vPoint[i] ;
				vTmpPnt[1].Position.x = vTmpPnt[1].Position.x*size.x+offset.x ;
				vTmpPnt[1].Position.y = vTmpPnt[1].Position.y*size.y+offset.y ;

				if (i == 7)
					vTmpPnt[2] = vPoint[0] ;
				else
					vTmpPnt[2] = vPoint[i+1] ;
				vTmpPnt[2].Position.x = vTmpPnt[2].Position.x*size.x+offset.x ;
				vTmpPnt[2].Position.y = vTmpPnt[2].Position.y*size.y+offset.y ;

				_draw.RenderPlane(vTmpPnt+0, vTmpPnt+2, vTmpPnt+1,Color);
				//RenderTriangle (vTmpPnt+0, vTmpPnt+1, vTmpPnt+2) ;
		}

		curMaxRate += rateSegment ;
	}
}


//----------------------------------------------------------------------------
// Class CDrawArrow
//----------------------------------------------------------------------------
CDrawArrow::CDrawArrow( const char* szTexName )
{
    m_pDevice = g_RenderSystem.GetD3DDevice();
    D3DXMatrixIdentity( &m_matWorld );
    Create( szTexName );
}
//----------------------------------------------------------------------------
CDrawArrow::~CDrawArrow()
{
    m_pDevice = NULL;
}
//----------------------------------------------------------------------------
bool CDrawArrow::Create( const char* szTexName )
{
    //讀取材質
    m_Texture = LoadStaticTexture( szTexName );
    //m_Texture = LoadTextureFromFile( szTexName );
    return true;
}
//----------------------------------------------------------------------------
void CDrawArrow::RenderPlane( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 )
{
    // 設定位置與材質座標
    PlaneVertex4 Vertices[4] =
    {
        v0.x, v0.y, v0.z, 0.0f, 1.0f,
        v1.x, v1.y, v1.z, 0.0f, 0.0f,
        v2.x, v2.y, v2.z, 1.0f, 1.0f,
        v3.x, v3.y, v3.z, 1.0f, 0.0f,
    };

	m_pDevice->SetRenderState( D3DRS_ZENABLE , false );
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE  , false );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 ) ;
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) ;
    
	// 繪製圖形
    m_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pDevice->SetTexture( 0, m_Texture );
    m_pDevice->SetFVF( PlaneVertex4::FVF );
    m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex4 ) );

	m_pDevice->SetRenderState( D3DRS_ZENABLE , true ) ;
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE , true ) ;
}
//----------------------------------------------------------------------------
void DrawHalo( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
               const D3DXVECTOR3& v3, const D3DXVECTOR3& v4 )
{
    static DrawPlane11 _DrawHalo( "UI\\Halo.tga" );
    _DrawHalo.RenderPlane( v0, v1, v2, v3, v4 );
}

//----------------------------------------------------------------------------
void DrawShadow( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
                     const D3DXVECTOR3& v3, const D3DXVECTOR3& v4 )
{
    static DrawPlane11 _DrawShadow( "UI\\shadow.png" );
    _DrawShadow.RenderPlane( v0, v1, v2, v3, v4 );
}

//----------------------------------------------------------------------------
void DrawShadow( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 )
{
    static DrawPlane2 _DrawShadow( "UI\\shadow.png" );
    _DrawShadow.RenderPlane( v0, v1, v2, v3 );
}
//----------------------------------------------------------------------------
void DrawArrow( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 )
{
    static CDrawArrow _DrawArrow( "UI\\Arrow.png" );
    _DrawArrow.RenderPlane( v0, v1, v2, v3 );
}

void GetProjection2D(D3DXMATRIX &outMat , unsigned int width , unsigned int hegith) 
{
	D3DXMatrixOrthoOffCenterLH(&outMat , 0 ,  (float)width   , 0.0f , (float)hegith  , 0.0f , 100.0f) ; 
}

void BeginRender2D(LPDIRECT3DDEVICE9 pd3d , D3DVIEWPORT9 &tempViewPort ,D3DXMATRIX &tempProjMat , D3DXMATRIX &tempViewMat ,
				    D3DVIEWPORT9 &viewPort , D3DXMATRIX &projMat , D3DXMATRIX &viewMat)
{
    pd3d->GetViewport( &tempViewPort );
    pd3d->SetViewport( &viewPort );


    pd3d->GetTransform( D3DTS_PROJECTION, &tempProjMat );
    pd3d->SetTransform( D3DTS_PROJECTION, &projMat );

	pd3d->GetTransform( D3DTS_VIEW, &tempViewMat );
	pd3d->SetTransform( D3DTS_VIEW, &viewMat );

}
void EndRender2D(LPDIRECT3DDEVICE9 pd3d , D3DVIEWPORT9 &viewPort , D3DXMATRIX &projMat , D3DXMATRIX &viewMat)
{
    pd3d->SetViewport( &viewPort );


    pd3d->SetTransform( D3DTS_PROJECTION, &projMat );

	pd3d->SetTransform( D3DTS_VIEW, &viewMat );

}
void Draw2DTexture(LPDIRECT3DDEVICE9 pDevice , IDirect3DTexture9* pTxr , const RECT &screen , const RECT* prcSrc , 
				   unsigned int texW , unsigned int texH , DWORD color)
{
	unsigned int nWidth = texW;
	unsigned int nHeight = texH ;
	const RECT& rcScreen = screen;
	D3DCOLOR	colors[4];

	
	colors[0] = color;
	colors[1] = color;
	colors[2] = color;
	colors[3] = color;
	

	RECT srcRect;
	if(prcSrc == 0)			
	{		
		SetRect(&srcRect,0,0,nWidth,nHeight);
	}
	else
	{
		srcRect = *prcSrc;
	}
	static DWORD nFVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;
	
	float fu0 = (srcRect.left+0.5f) / (float) nWidth ;			
	float fv0 = (srcRect.bottom+0.5f) / (float) nHeight ;			
	
	float fu1 = (srcRect.left+0.5f) / (float) nWidth ;			
	float fv1 = (srcRect.top+0.5f) / (float) nHeight ;			
	
	float fu2 = (srcRect.right+0.5f) / (float) nWidth ;			
	float fv2 = (srcRect.bottom+0.5f) / (float) nHeight ;			
	
	float fu3 = (srcRect.right+0.5f) / (float) nWidth ;			
	float fv3 = (srcRect.top+0.5f) / (float) nHeight ;	
	
		// 設定位置與材質座標
	PlaneVertex1 Vertices[4] =
	{
		
		(float)rcScreen.left,  (float)rcScreen.bottom , 0.0f, 1.0f, colors[0], fu0,fv0,
		(float)rcScreen.right , (float)rcScreen.bottom , 0.0f, 1.0f, colors[2] , fu2,fv2,
		(float)rcScreen.left,  (float)rcScreen.top ,    0.0f, 1.0f, colors[1], fu1,fv1,
		(float)rcScreen.right , (float)rcScreen.top ,    0.0f, 1.0f, colors[3] , fu3,fv3,
	};
	
	// 繪製圖形	
	pDevice->SetTexture(0 , pTxr);
	pDevice->SetFVF( nFVF );
	pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex1 ) );
		
}

//----------------------------------------------------------------------------
void Draw2DTexture	(const Draw2DTextureInfo& inf)
{
	LPDIRECT3DDEVICE9 pDevice = inf.pDevice;
	IDirect3DTexture9* pTxr = inf.pTxr;
	int nWidth = inf.nWidth;
	int nHeight = inf.nHeight ;
	const RECT* prcSrc = inf.prcSrc;
	const RECT& rcScreen = inf.rcScreen;
	D3DCOLOR	colors[4];
	if(inf.bDiffColor == false)
	{
		colors[0] = 0xffffffff;
		colors[1] = 0xffffffff;
		colors[2] = 0xffffffff;
		colors[3] = 0xffffffff;
	}
	else
	{
		colors[0] = inf.colors[0] ;
		colors[1] = inf.colors[1] ;
		colors[2] = inf.colors[2] ;
		colors[3] = inf.colors[3] ;
	}

	RECT srcRect;
	if(prcSrc == 0)			
	{		
		SetRect(&srcRect,0,0,nHeight,nWidth);
	}
	else
	{
		srcRect = *prcSrc;
	}
	static DWORD nFVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;
	
	float fu0 = (srcRect.left+0.5f) / (float) nWidth ;			
	float fv0 = (srcRect.bottom+0.5f) / (float) nHeight ;			
	
	float fu1 = (srcRect.left+0.5f) / (float) nWidth ;			
	float fv1 = (srcRect.top+0.5f) / (float) nHeight ;			
	
	float fu2 = (srcRect.right+0.5f) / (float) nWidth ;			
	float fv2 = (srcRect.bottom+0.5f) / (float) nHeight ;			
	
	float fu3 = (srcRect.right+0.5f) / (float) nWidth ;			
	float fv3 = (srcRect.top+0.5f) / (float) nHeight ;	
	
		// 設定位置與材質座標
	PlaneVertex1 Vertices[4] =
	{
		
		(float)rcScreen.left,  (float)rcScreen.bottom , 0.0f, 1.0f, colors[0], fu0,fv0,
		(float)rcScreen.left,  (float)rcScreen.top ,    0.0f, 1.0f, colors[1], fu1,fv1,
		(float)rcScreen.right , (float)rcScreen.bottom , 0.0f, 1.0f, colors[2] , fu2,fv2,
		(float)rcScreen.right , (float)rcScreen.top ,    0.0f, 1.0f, colors[3] , fu3,fv3,
	};


	
	// 繪製圖形	
	pDevice->SetTexture(0 , pTxr);
	pDevice->SetFVF( PlaneVertex3::FVF );
	pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex3 ) );
		
	
}


///////////////////////////////////////////////////////////////////////////////////////////////////////



CRenderer::CRenderer(LPDIRECT3DDEVICE9 pDevice , unsigned int shadowVerCount , unsigned int frameVerCount) : m_shadowOpVec()  , 
m_frameOpPool(sizeof(stFreameEx)) , m_damageOpList() , m_divideOpList() , m_uiOpVec() , m_uiOpPool(sizeof(stUiStream)) , m_freeDamageOpList()
{	
	m_numberUnit = 0.125f ; // 0.125f 代表是 1/8

	m_divideTime = 3000 ; 

	m_pShadowBuffer = new CGeometryBuffer(pDevice) ; 

//	m_pUiBuffer = new CGeometryBuffer(pDevice) ; 

	m_pFrameBuffer = new CGeometryBuffer(pDevice) ; 


	m_pShadowBuffer->CreateGeometryBuffer(shadowVerCount * 6 * sizeof(PlaneVertex4) , PlaneVertex4::FVF) ;
//	m_pUiBuffer->CreateGeometryBuffer( shadowVerCount * 6 * sizeof(PlaneVertex1) , PlaneVertex1::FVF) ;

	m_pFrameBuffer->CreateGeometryBuffer(frameVerCount * 6 * sizeof(PlaneVertex3) , PlaneVertex3::FVF) ; 

	m_frameCount = frameVerCount ; 
	m_maxShadowVertexCount = shadowVerCount * 6 ; 
	//預留m_shadowOpVec最大容量
	m_shadowOpVec.reserve(m_maxShadowVertexCount);


	m_pDevice = pDevice ; 

	//0材質
	m_numberV[0].width = 0.5f ; m_numberV[0].height = 0.5f ; m_numberV[0].layer = 1 ; m_numberV[0].Color = WHITE ; 
	m_numberV[0].uv = D3DXVECTOR2(m_numberUnit * 0.0f , m_numberUnit * 0.0f) ; 
	m_numberV[0].Uv = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 0.0f) ; 
	m_numberV[0].uV = D3DXVECTOR2(m_numberUnit * 0.0f , m_numberUnit * 2.0f) ; 
	m_numberV[0].UV = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 2.0f) ; 
	//1材質
	m_numberV[1].width = 0.5f ; m_numberV[1].height = 0.5f ; m_numberV[1].layer = 1 ; m_numberV[1].Color = WHITE ; 
	m_numberV[1].uv = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 0.0f) ; 
	m_numberV[1].Uv = D3DXVECTOR2(m_numberUnit * 2.0f , m_numberUnit * 0.0f) ; 
	m_numberV[1].uV = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 2.0f) ; 
	m_numberV[1].UV = D3DXVECTOR2(m_numberUnit * 2.0f , m_numberUnit * 2.0f) ; 
	//2材質
	m_numberV[2].width = 0.5f ; m_numberV[2].height = 0.5f ; m_numberV[2].layer = 1 ; m_numberV[2].Color = WHITE ; 
	m_numberV[2].uv = D3DXVECTOR2(m_numberUnit * 2.0f , m_numberUnit * 0.0f) ; 
	m_numberV[2].Uv = D3DXVECTOR2(m_numberUnit * 3.0f , m_numberUnit * 0.0f) ; 
	m_numberV[2].uV = D3DXVECTOR2(m_numberUnit * 2.0f , m_numberUnit * 2.0f) ; 
	m_numberV[2].UV = D3DXVECTOR2(m_numberUnit * 3.0f , m_numberUnit * 2.0f) ; 
	//3材質
	m_numberV[3].width = 0.5f ; m_numberV[3].height = 0.5f ; m_numberV[3].layer = 1 ; m_numberV[3].Color = WHITE ; 
	m_numberV[3].uv = D3DXVECTOR2(m_numberUnit * 3.0f , m_numberUnit * 0.0f) ; 
	m_numberV[3].Uv = D3DXVECTOR2(m_numberUnit * 4.0f , m_numberUnit * 0.0f) ; 
	m_numberV[3].uV = D3DXVECTOR2(m_numberUnit * 3.0f , m_numberUnit * 2.0f) ; 
	m_numberV[3].UV = D3DXVECTOR2(m_numberUnit * 4.0f , m_numberUnit * 2.0f) ; 
	//4材質
	m_numberV[4].width = 0.5f ; m_numberV[4].height = 0.5f ; m_numberV[4].layer = 1 ; m_numberV[4].Color = WHITE ; 
	m_numberV[4].uv = D3DXVECTOR2(m_numberUnit * 4.0f , m_numberUnit * 0.0f) ; 
	m_numberV[4].Uv = D3DXVECTOR2(m_numberUnit * 5.0f , m_numberUnit * 0.0f) ; 
	m_numberV[4].uV = D3DXVECTOR2(m_numberUnit * 4.0f , m_numberUnit * 2.0f) ; 
	m_numberV[4].UV = D3DXVECTOR2(m_numberUnit * 5.0f , m_numberUnit * 2.0f) ; 
	//5材質
	m_numberV[5].width = 0.5f ; m_numberV[5].height = 0.5f ; m_numberV[5].layer = 1 ; m_numberV[5].Color = WHITE ; 
	m_numberV[5].uv = D3DXVECTOR2(m_numberUnit * 5.0f , m_numberUnit * 0.0f) ; 
	m_numberV[5].Uv = D3DXVECTOR2(m_numberUnit * 6.0f , m_numberUnit * 0.0f) ; 
	m_numberV[5].uV = D3DXVECTOR2(m_numberUnit * 5.0f , m_numberUnit * 2.0f) ; 
	m_numberV[5].UV = D3DXVECTOR2(m_numberUnit * 6.0f , m_numberUnit * 2.0f) ; 
	//6材質
	m_numberV[6].width = 0.5f ; m_numberV[6].height = 0.5f ; m_numberV[6].layer = 1 ; m_numberV[6].Color = WHITE ; 
	m_numberV[6].uv = D3DXVECTOR2(m_numberUnit * 6.0f , m_numberUnit * 0.0f) ; 
	m_numberV[6].Uv = D3DXVECTOR2(m_numberUnit * 7.0f , m_numberUnit * 0.0f) ; 
	m_numberV[6].uV = D3DXVECTOR2(m_numberUnit * 6.0f , m_numberUnit * 2.0f) ; 
	m_numberV[6].UV = D3DXVECTOR2(m_numberUnit * 7.0f , m_numberUnit * 2.0f) ; 
	//7材質
	m_numberV[7].width = 0.5f ; m_numberV[7].height = 0.5f ; m_numberV[7].layer = 1 ; m_numberV[7].Color = WHITE ; 
	m_numberV[7].uv = D3DXVECTOR2(m_numberUnit * 7.0f , m_numberUnit * 0.0f) ; 
	m_numberV[7].Uv = D3DXVECTOR2(m_numberUnit * 8.0f , m_numberUnit * 0.0f) ; 
	m_numberV[7].uV = D3DXVECTOR2(m_numberUnit * 7.0f , m_numberUnit * 2.0f) ; 
	m_numberV[7].UV = D3DXVECTOR2(m_numberUnit * 8.0f , m_numberUnit * 2.0f) ; 
	//8材質
	m_numberV[8].width = 0.5f ; m_numberV[8].height = 0.5f ; m_numberV[8].layer = 1 ; m_numberV[8].Color = WHITE ; 
	m_numberV[8].uv = D3DXVECTOR2(m_numberUnit * 0.0f , m_numberUnit * 2.0f) ; 
	m_numberV[8].Uv = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 2.0f) ; 
	m_numberV[8].uV = D3DXVECTOR2(m_numberUnit * 0.0f , m_numberUnit * 4.0f) ; 
	m_numberV[8].UV = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 4.0f) ; 
	//9材質
	m_numberV[9].width = 0.5f ; m_numberV[9].height = 0.5f ; m_numberV[9].layer = 1 ; m_numberV[9].Color = WHITE ; 
	m_numberV[9].uv = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 2.0f) ; 
	m_numberV[9].Uv = D3DXVECTOR2(m_numberUnit * 2.0f , m_numberUnit * 2.0f) ; 
	m_numberV[9].uV = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 4.0f) ; 
	m_numberV[9].UV = D3DXVECTOR2(m_numberUnit * 2.0f , m_numberUnit * 4.0f) ; 

	//+材質
	m_numberV[10].width = 0.5f ; m_numberV[10].height = 0.5f ; m_numberV[10].layer = 1 ; m_numberV[10].Color = WHITE ; 
	m_numberV[10].uv = D3DXVECTOR2(m_numberUnit * 0.0f , m_numberUnit * 4.0f) ; 
	m_numberV[10].Uv = D3DXVECTOR2(m_numberUnit * 0.5f , m_numberUnit * 4.0f) ; 
	m_numberV[10].uV = D3DXVECTOR2(m_numberUnit * 0.0f , m_numberUnit * 5.0f) ; 
	m_numberV[10].UV = D3DXVECTOR2(m_numberUnit * 0.5f , m_numberUnit * 5.0f) ; 

	//-材質
	m_numberV[11].width = 0.5f ; m_numberV[11].height = 0.5f ; m_numberV[11].layer = 1 ; m_numberV[11].Color = WHITE ; 
	m_numberV[11].uv = D3DXVECTOR2(m_numberUnit * 0.5f , m_numberUnit * 4.0f) ; 
	m_numberV[11].Uv = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 4.0f) ; 
	m_numberV[11].uV = D3DXVECTOR2(m_numberUnit * 0.5f , m_numberUnit * 5.0f) ; 
	m_numberV[11].UV = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 5.0f) ; 

	//H材質
	m_numberV[12].width = 0.5f ; m_numberV[12].height = 0.5f ; m_numberV[12].layer = 1 ; m_numberV[12].Color = WHITE ; 
	m_numberV[12].uv = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 4.0f) ; 
	m_numberV[12].Uv = D3DXVECTOR2(m_numberUnit * 1.5f , m_numberUnit * 4.0f) ; 
	m_numberV[12].uV = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 5.0f) ; 
	m_numberV[12].UV = D3DXVECTOR2(m_numberUnit * 1.5f , m_numberUnit * 5.0f) ; 

	//P材質
	m_numberV[13].width = 0.5f ; m_numberV[13].height = 0.5f ; m_numberV[13].layer = 1 ; m_numberV[13].Color = WHITE ; 
	m_numberV[13].uv = D3DXVECTOR2(m_numberUnit * 1.5f , m_numberUnit * 4.0f) ; 
	m_numberV[13].Uv = D3DXVECTOR2(m_numberUnit * 2.0f , m_numberUnit * 4.0f) ; 
	m_numberV[13].uV = D3DXVECTOR2(m_numberUnit * 1.5f , m_numberUnit * 5.0f) ; 
	m_numberV[13].UV = D3DXVECTOR2(m_numberUnit * 2.0f , m_numberUnit * 5.0f) ; 

	//S材質
	m_numberV[14].width = 0.5f ; m_numberV[14].height = 0.5f ; m_numberV[14].layer = 1 ; m_numberV[14].Color = WHITE ; 
	m_numberV[14].uv = D3DXVECTOR2(m_numberUnit * 0.0f , m_numberUnit * 5.0f) ; 
	m_numberV[14].Uv = D3DXVECTOR2(m_numberUnit * 0.5f , m_numberUnit * 5.0f) ; 
	m_numberV[14].uV = D3DXVECTOR2(m_numberUnit * 0.0f , m_numberUnit * 6.0f) ; 
	m_numberV[14].UV = D3DXVECTOR2(m_numberUnit * 0.5f , m_numberUnit * 6.0f) ; 

	//M材質
	m_numberV[15].width = 0.5f ; m_numberV[15].height = 0.5f ; m_numberV[15].layer = 1 ; m_numberV[15].Color = WHITE ; 
	m_numberV[15].uv = D3DXVECTOR2(m_numberUnit * 0.5f , m_numberUnit * 5.0f) ; 
	m_numberV[15].Uv = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 5.0f) ; 
	m_numberV[15].uV = D3DXVECTOR2(m_numberUnit * 0.5f , m_numberUnit * 6.0f) ; 
	m_numberV[15].UV = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 6.0f) ;

	//Ex材質
	m_numberV[16].width = 0.5f ; m_numberV[16].height = 0.5f ; m_numberV[16].layer = 1 ; m_numberV[16].Color = WHITE ; 
	m_numberV[16].uv = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 5.0f) ; 
	m_numberV[16].Uv = D3DXVECTOR2(m_numberUnit * 1.5f , m_numberUnit * 5.0f) ; 
	m_numberV[16].uV = D3DXVECTOR2(m_numberUnit * 1.0f , m_numberUnit * 6.0f) ; 
	m_numberV[16].UV = D3DXVECTOR2(m_numberUnit * 1.5f , m_numberUnit * 6.0f) ;
}
CRenderer::~CRenderer()
{
		
	m_shadowOpVec.clear() ; 

	m_uiOpVec.clear() ; 

	std::set<stFreameEx*> tempFreeFrameOps ; 
	std::pair<std::set<stFreameEx*>::iterator , bool> ret ; 


	while(!m_freeFrameOpList.empty())
	{
		stFreameEx *frame = m_freeFrameOpList.front() ; 

		ret = tempFreeFrameOps.insert(frame) ; 

		if(!ret.second)
		{
			Assert(0 , "重複刪除Frame的危機") ; 
		}

		m_freeFrameOpList.pop_front() ; 
	}

	std::set<stFreameEx*>::iterator iter(tempFreeFrameOps.begin()) ; 

	while(iter != tempFreeFrameOps.end())
	{
		stFreameEx *frame = *iter ; 

		iter = tempFreeFrameOps.erase(iter) ; 

		delete frame ; 

		frame = NULL ; 
		
	}




	while(!m_freeDamageOpList.empty())
	{
		stFreameEx *frame = m_freeDamageOpList.front() ; 

		delete frame ; 
		frame = NULL ; 
	

		m_freeDamageOpList.pop_front() ; 


	}

	//while(!m_freeshadowOpPool.empty())
	//{
	//	PlaneVertex4 *op = m_freeshadowOpPool.front() ; 

	//	delete op ; 
	//	op = NULL ; 
	//	m_freeshadowOpPool.pop_front() ; 

	//}	

	
	delete m_pShadowBuffer ; 
	m_pShadowBuffer = NULL ; 


	delete m_pFrameBuffer ; 
	m_pFrameBuffer = NULL ; 

//	delete m_pUiBuffer ; 
//	m_pUiBuffer = NULL ; 

}

void CRenderer::Update(DWORD dwPassTime)
{
	std::list<stFreameEx*>::iterator iter(m_divideOpList.begin()) ; 

	for(; iter != m_divideOpList.end() ; ++iter)
	{
		stFreameEx *frame = *iter ; 

		frame->curDivided += dwPassTime ; 

		if(frame->curDivided > m_divideTime)
		{
			frame->divide = false ; 
		}
		else
		{

			PlaneVertex3* v0 = &frame->ver[0] ;
			PlaneVertex3* v1 = &frame->ver[1];
			PlaneVertex3* v2 = &frame->ver[2] ; 
			PlaneVertex3* v3 = &frame->ver[3];
			PlaneVertex3* v4 = &frame->ver[4];
			PlaneVertex3* v5 = &frame->ver[5];

			BYTE Alpha = (BYTE)(150 - (150 * frame->curDivided) / m_divideTime) ; 

			DWORD color = D3DCOLOR_ARGB( Alpha, 255, 250, 150 ) ; 

			v0->Color |= 0xFF000000;
			v0->Color &= color;

			v1->Color = v0->Color ; 
			v2->Color = v0->Color ; 
			v3->Color = v0->Color ; 
			v4->Color = v0->Color ; 
			v5->Color = v0->Color ; 

		}
	}

}

void CRenderer::CreateOp(float width , float height , short layer , const D3DCOLOR& Color , 
						 const D3DXVECTOR2 &uv , const D3DXVECTOR2 &Uv , 
						 const D3DXVECTOR2 &uV , const D3DXVECTOR2 &UV , 
						 const D3DXMATRIX& matWorld , LPDIRECT3DTEXTURE9 pTex)
{

	stFreameEx* frame = NULL ; 

    if( m_freeDamageOpList.empty() )
    {
        frame = new stFreameEx;
        Assert( frame != 0, "Call new failed !!" );
    }
    else
    {
        frame = m_freeDamageOpList.front();
        m_freeDamageOpList.pop_front();
    }


	
	//stFreameEx* frame = (stFreameEx*)m_frameOpPool.malloc();

	frame->layer = layer ; 
	frame->pTexture = pTex ; 
	frame->divide = false ;			//數字的frame，不需要作殘影的效果
	frame->curDivided = 0 ; 


	PlaneVertex3* v0 = &frame->ver[0] ;
	PlaneVertex3* v1 = &frame->ver[1];
	PlaneVertex3* v2 = &frame->ver[2] ; 
	PlaneVertex3* v3 = &frame->ver[3];
	PlaneVertex3* v4 = &frame->ver[4];
	PlaneVertex3* v5 = &frame->ver[5];

    //位置
	v0->Position  = D3DXVECTOR3( -width,  height, 0.0f );
    v1->Position  = D3DXVECTOR3( -width,  -height, 0.0f );
    v2->Position  = D3DXVECTOR3(  width,  -height, 0.0f );
	v3->Position  = v0->Position ; 
	v4->Position  = v2->Position ; 
    v5->Position  = D3DXVECTOR3(  width, height, 0.0f );

    //顏色
    v0->Color    = Color;
    v1->Color    = Color;
    v2->Color    = Color;
    v3->Color    = Color;
    v4->Color    = Color;
    v5->Color    = Color;

	v0->tu = Uv.x ; v0->tv = Uv.y ; 
	v1->tu = UV.x ; v1->tv = UV.y ; 
	v2->tu = uV.x ; v2->tv = uV.y ; 
	v3->tu = v0->tu ; v3->tv = v0->tv ;  
	v4->tu = v2->tu ; v4->tv = v2->tv ; 
	v5->tu = uv.x ; v5->tv = uv.y ; 



	D3DXVec3TransformCoord(&v0->Position , &v0->Position , &matWorld) ;
	D3DXVec3TransformCoord(&v1->Position , &v1->Position , &matWorld) ;
	D3DXVec3TransformCoord(&v2->Position , &v2->Position , &matWorld) ;

	v3->Position = v0->Position ; 
	v4->Position = v2->Position ; 


	D3DXVec3TransformCoord(&v5->Position , &v5->Position , &matWorld) ;


	m_damageOpList.push_back(frame) ; 

}

void CRenderer::CreateDamageOp(const BYTE& Type, const int& iNum, const D3DCOLOR& Color, const D3DXMATRIX& matWorld , LPDIRECT3DTEXTURE9 pTex , bool bIsPlus)
{
	const float fUnit = m_numberUnit;     // 0.125f 代表是 1/8


    switch( Type )
    {
        case 0x04:     // 爆擊
        {
            DWORD dwColor = (Color | 0x00FFFFFF);

			CreateOp(5.0f , 5.0f , -1 , dwColor , D3DXVECTOR2(fUnit * 4.0f , fUnit * 4.0f) , 
				D3DXVECTOR2(fUnit * 6.0f , fUnit * 4.0f) , D3DXVECTOR2(fUnit * 4.0f , fUnit * 8.0f) , 
				D3DXVECTOR2(fUnit * 6.0f , fUnit * 8.0f) , matWorld , pTex) ; 
           
        } break;

        case 0x05:     // 隔擋
        {
            DWORD dwColor = (Color | 0x00FFFFFF);

			CreateOp(4.0f , 4.0f , -1 , dwColor , D3DXVECTOR2(fUnit * 6.0f , fUnit * 4.0f) , 
				D3DXVECTOR2(fUnit * 8.0f , fUnit * 4.0f) , D3DXVECTOR2(fUnit * 6.0f , fUnit * 8.0f) , 
				D3DXVECTOR2(fUnit * 8.0f , fUnit * 8.0f) , matWorld , pTex) ; 

            
        }break;

        case 0x06:     // Miss
		{
			CreateOp(2.0f , 2.0f , -1 ,Color , D3DXVECTOR2(fUnit * 2.0f , fUnit * 4.0f) , 
				D3DXVECTOR2(fUnit * 4.0f , fUnit * 4.0f) , D3DXVECTOR2(fUnit * 2.0f , fUnit * 8.0f) , 
				D3DXVECTOR2(fUnit * 4.0f , fUnit * 8.0f) , matWorld , pTex) ; 

			return ; 

		}break ;
		default:
			{				
				
				
			}break ;

           
    };

	if( iNum == 0 ) return; 


	//繪製數字 ( 0 ~ 99999 )
	int iNum1, iNum2, iNum3, iNum4, iNum5;
	int iWordNum1, iWordNum2;
	float fOffSet1, fOffSet2, fOffSet3, fOffSet4, fOffSet5;
	float fWordOffSet1, fWordOffSet2, fWordOffSet3;
	D3DXMATRIX matTemp, matScale;

	D3DXMatrixIdentity(&matTemp) ; 
	D3DXMatrixIdentity(&matScale) ; 

	// 計算數字內容
	iNum1 = iNum % 10;
	iNum2 = (iNum / 10) % 10;
	iNum3 = (iNum / 100) % 10;
	iNum4 = (iNum / 1000) % 10;
	iNum5 = iNum / 10000;

	// 計算數字偏移位置
	if( iNum5 )  // 五位數
	{
		fOffSet1 = -2.5f;
		fOffSet2 = -1.7f;
		fOffSet3 = -0.9f;
		fOffSet4 = -0.1f;
		fOffSet5 =  0.7f;	//	因為P這個字比較偏左 所以後面的文字通通要往左修正
		fWordOffSet3 = 1.2f;
		fWordOffSet2 = 2.3f;
		fWordOffSet1 = 3.3f;
		/*fOffSet1 = -1.64f;
		fOffSet2 = -0.82f;
		fOffSet3 =  0.0f;
		fOffSet4 =  0.82f;
		fOffSet5 =  1.64f;
		fWordOffSet3 = 2.46f;
		fWordOffSet2 = 3.28f;
		fWordOffSet1 = 4.1f;*/
		D3DXMatrixScaling( &matScale, 1.1f, 1.1f, 1.1f );
	}
	else if( iNum4 )  // 四位數
	{
		fOffSet1 = -2.1f;
		fOffSet2 = -1.3f;
		fOffSet3 = -0.5f;
		fOffSet4 =  0.3f;
		fWordOffSet3 = 0.8f;
		fWordOffSet2 = 1.9f;
		fWordOffSet1 = 2.9f;
		/*fOffSet1 = -1.2f;
		fOffSet2 = -0.4f;
		fOffSet3 =  0.4f;
		fOffSet4 =  1.2f;
		fWordOffSet3 = 2.0f;
		fWordOffSet2 = 2.8f;
		fWordOffSet1 = 3.6f;*/
		D3DXMatrixScaling( &matScale, 1.1f, 1.1f, 1.1f );
	}
	else if( iNum3 )  // 三位數
	{
		fOffSet1 = -1.7f;
		fOffSet2 = -0.9f;
		fOffSet3 = -0.1f;
		fWordOffSet3 = 0.4f;
		fWordOffSet2 = 1.5f;
		fWordOffSet1 = 2.5f;
		/*fOffSet1 = -0.8f;
		fOffSet2 =  0.0f;
		fOffSet3 =  0.8f;
		fWordOffSet3 = 1.6f;
		fWordOffSet2 = 2.4f;
		fWordOffSet1 = 3.2f;*/
		D3DXMatrixScaling( &matScale, 1.1f, 1.1f, 1.1f );
	}
	else if( iNum2 )  // 二位數
	{
		fOffSet1 = -1.3f;
		fOffSet2 = -0.5f;
		fWordOffSet3 = 0.0f;
		fWordOffSet2 = 1.1f;
		fWordOffSet1 = 2.1f;
		D3DXMatrixScaling( &matScale, 1.1f, 1.1f, 1.1f );
	}
	else  // 一位數
	{
		fOffSet1 = -0.9f;
		fWordOffSet3 = -0.4f;
		fWordOffSet2 =  0.7f;
		fWordOffSet1 =  1.7f;
		D3DXMatrixScaling( &matScale, 1.1f, 1.1f, 1.1f );
	}

	if( iNum5 || iNum4 || iNum3 ||iNum2 || iNum1 )  //個位數
	{
		 
		D3DXMatrixTranslation( &matTemp, fOffSet1, 0.0f, 0.0f );
		D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );

		CreateOp(m_numberV[iNum1].width , m_numberV[iNum1].height , 
			m_numberV[iNum1].layer ,Color ,
			m_numberV[iNum1].uv , m_numberV[iNum1].Uv , m_numberV[iNum1].uV , 
			m_numberV[iNum1].UV , matTemp , pTex) ; 

	}
	if( iNum5 || iNum4 || iNum3 ||iNum2 )   //十位數
	{
		D3DXMatrixTranslation( &matTemp, fOffSet2, 0.0f, 0.0f );
		D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );

		CreateOp(m_numberV[iNum2].width , m_numberV[iNum2].height , 
			m_numberV[iNum2].layer ,Color ,
			m_numberV[iNum2].uv , m_numberV[iNum2].Uv , m_numberV[iNum2].uV , 
			m_numberV[iNum2].UV , matTemp , pTex) ; 


	}
	if( iNum5 || iNum4 || iNum3 )   //百位數
	{
		D3DXMatrixTranslation( &matTemp, fOffSet3, 0.0f, 0.0f );
		D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );

		CreateOp(m_numberV[iNum3].width , m_numberV[iNum3].height , 
			m_numberV[iNum3].layer ,Color ,
			m_numberV[iNum3].uv , m_numberV[iNum3].Uv , m_numberV[iNum3].uV , 
			m_numberV[iNum3].UV , matTemp , pTex) ; 
	}
	if( iNum5 || iNum4 )    //千位數
	{
		D3DXMatrixTranslation( &matTemp, fOffSet4, 0.0f, 0.0f );
		D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );

		CreateOp(m_numberV[iNum4].width , m_numberV[iNum4].height , 
			m_numberV[iNum4].layer ,Color ,
			m_numberV[iNum4].uv , m_numberV[iNum4].Uv , m_numberV[iNum4].uV , 
			m_numberV[iNum4].UV , matTemp , pTex) ; 

	}
	if( iNum5 )     //萬位數
	{
		D3DXMatrixTranslation( &matTemp, fOffSet5, 0.0f, 0.0f );
		D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );

		CreateOp(m_numberV[iNum5].width , m_numberV[iNum5].height , 
			m_numberV[iNum5].layer ,Color ,
			m_numberV[iNum5].uv , m_numberV[iNum5].Uv , m_numberV[iNum5].uV , 
			m_numberV[iNum5].UV , matTemp , pTex) ; 
	} 
	// Word
	if( Type == 0xFF )	//	FF是寫死的當作經驗值編號(client自訂的)
	{
		iWordNum2 = 16;	//	"Ex"P
	}
	else
	{
		switch( Type>>4 )
		{
			case 0 :	//	"H"P
			{
				iWordNum2 = 12;			
				break;
			}
			case 2 :	//	"M"P
			{
				iWordNum2 = 15;	
				break;
			}
			case 4 :	//	"S"P
			{
				iWordNum2 = 14;				
				break;
			}
		}
	}
	{
		D3DXMatrixTranslation( &matTemp, fWordOffSet2, 0.0f, 0.0f );
		D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );

		CreateOp(m_numberV[iWordNum2].width , m_numberV[iWordNum2].height , 
			m_numberV[iWordNum2].layer ,Color ,
			m_numberV[iWordNum2].uv , m_numberV[iWordNum2].Uv , m_numberV[iWordNum2].uV , 
			m_numberV[iWordNum2].UV , matTemp , pTex) ; 
		//	P
		D3DXMatrixTranslation( &matTemp, fWordOffSet3, 0.0f, 0.0f );
		D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );

		CreateOp(m_numberV[13].width , m_numberV[13].height , 
			m_numberV[13].layer ,Color ,
			m_numberV[13].uv , m_numberV[13].Uv , m_numberV[13].uV , 
			m_numberV[13].UV , matTemp , pTex) ; 
	}

	if( bIsPlus )	//	+
	{
		iWordNum1 = 10;
	}
	else			//	-
	{
		iWordNum1 = 11;
	}
	{
		D3DXMatrixTranslation( &matTemp, fWordOffSet1, 0.0f, 0.0f );
		D3DXMatrixMultiply( &matTemp, &matScale, &matTemp );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matWorld );

		CreateOp(m_numberV[iWordNum1].width , m_numberV[iWordNum1].height , 
			m_numberV[iWordNum1].layer ,Color ,
			m_numberV[iWordNum1].uv , m_numberV[iWordNum1].Uv , m_numberV[iWordNum1].uV , 
			m_numberV[iWordNum1].UV , matTemp , pTex) ; 
	}

}


void CRenderer::CreateOp(LPDIRECT3DTEXTURE9 pTex , const D3DXMATRIX &invWorldMat ,  float width , float height , float tu1 , float tv1 , float tu2 , float tv2 ,
						 DWORD color , short layer , bool bInvert , bool divide)
{

//	return ; 

	stFreameEx* frame = NULL ; 

    if( m_freeFrameOpList.empty() )
    {
        frame = new stFreameEx;
        Assert( frame != 0, "Call new failed !!" );
    }
    else
    {
        frame = m_freeFrameOpList.front();
        m_freeFrameOpList.pop_front();
    }


	//stFreameEx* frame = (stFreameEx*)m_frameOpPool.malloc();

	frame->layer = layer ; 
	frame->pTexture = pTex ; 
	frame->divide = divide ; //預設為flase。要小心使用！ 
	frame->curDivided = 0 ; 


	PlaneVertex3* v0 = &frame->ver[0] ;
	PlaneVertex3* v1 = &frame->ver[1];
	PlaneVertex3* v2 = &frame->ver[2] ; 
	PlaneVertex3* v3 = &frame->ver[3];
	PlaneVertex3* v4 = &frame->ver[4];
	PlaneVertex3* v5 = &frame->ver[5];


    v0->Position = D3DXVECTOR3( -width , height , 0.0f );//v0

	D3DXVec3TransformCoord(&v0->Position , &v0->Position , &invWorldMat) ; 

    v1->Position = D3DXVECTOR3( -width , 0.0f , 0.0f );//v1

	D3DXVec3TransformCoord(&v1->Position , &v1->Position , &invWorldMat) ; 

	
    v2->Position = D3DXVECTOR3( width , 0.0f , 0.0f );//v3

	D3DXVec3TransformCoord(&v2->Position , &v2->Position , &invWorldMat) ; 


	v3->Position = v0->Position ; //v0

	
	v4->Position = v2->Position ; //v3


    v5->Position = D3DXVECTOR3( width , height , 0.0f );//v2

	D3DXVec3TransformCoord(&v5->Position , &v5->Position , &invWorldMat) ; 


    v0->Color    = color;
    v1->Color    = color;
    v2->Color    = color;
    v3->Color    = color;
    v4->Color    = color;
    v5->Color    = color;


    // 反向貼材質
    if( bInvert )
    {
		//v0
		v0->tu = tu1; 
		v0->tv = tv1;

		//v1 
		v1->tu = tu1;
		v1->tv = tv2;
		
		//v3
		v2->tu = tu2;
		v2->tv = tv2;

		//v0
		v3->tu = v0->tu;
		v3->tv = v0->tv;

		//v3
		v4->tu = v2->tu;
		v4->tv = v2->tv;

		//v2
		v5->tu = tu2;
		v5->tv = tv1;


    }
    else
    {

		//v0
		v0->tu = tu2; 
		v0->tv = tv1;

		//v1 
		v1->tu = tu2;
		v1->tv = tv2;
		
		//v3
		v2->tu = tu1;
		v2->tv = tv2;

		//v0
		v3->tu = v0->tu;
		v3->tv = v0->tv;

		//v3
		v4->tu = v2->tu;
		v4->tv = v2->tv;

		//v2
		v5->tu = tu1;
		v5->tv = tv1;

    }
       

	if(frame->divide)
	{
		m_divideOpList.push_back(frame) ; 
	}
	else
	{
   		m_frameOpList.push_back(frame) ; 
	}


}

 
void CRenderer::CreateFrameOp(const SRoleSkinImageNew* pSkinImage, const D3DXMATRIX &invWorldMat , const D3DCOLOR& Color , bool divide)
{
	CreateOp(pSkinImage->pSkinTexture->pTexture , invWorldMat , pSkinImage->pRPCImg->fWidth , pSkinImage->pRPCImg->fHeight ,
		pSkinImage->pRPCImg->fTU1 , pSkinImage->pRPCImg->fTV1 , pSkinImage->pRPCImg->fTU2 ,
		pSkinImage->pRPCImg->fTV2 , Color , pSkinImage->pRATImg->shLayer , pSkinImage->pRATImg->bTexInvert , divide) ; 

}
void CRenderer::CreateFrameOp(const sFrame *pFrame , const D3DXMATRIX &invWorldMat , const D3DCOLOR& Color , bool divide)
{
	CreateOp(pFrame->pImage->pTexture , invWorldMat , pFrame->fWidth , pFrame->fHeight , pFrame->fTU1 , 
		pFrame->fTV1 , pFrame->fTU2 , pFrame->fTV2 , Color , pFrame->shLayer , pFrame->bTexInvert) ; 

}

//PlaneVertex4* CRenderer::GetShadowPoint()
//{
//
//	PlaneVertex4 *op = NULL ;
//
//	if( m_freeshadowOpPool.empty() )
//    {
//        op = new PlaneVertex4;
//        Assert( op != 0, "Call new failed !!" );
//    }
//    else
//    {
//        op = m_freeshadowOpPool.front();
//        m_freeshadowOpPool.pop_front();
//    }
//
//	return op ; 
//}
void CRenderer::CreateShadowOp(const D3DXVECTOR3 &center , const D3DXVECTOR3 &offset , float shadowSize)
{
	//check if the buffer is full
	if(m_shadowOpVec.size() >= m_maxShadowVertexCount)
		return;

    // 計算陰影大小與位置
    D3DXVECTOR3 v0, v1, v2, v3;
    v0 = center + D3DXVECTOR3( -shadowSize, 0.0f, -shadowSize ) - offset;
    v1 = center + D3DXVECTOR3( -shadowSize, 0.0f,  shadowSize ) - offset;
    v2 = center + D3DXVECTOR3(  shadowSize, 0.0f, -shadowSize ) - offset;
    v3 = center + D3DXVECTOR3(  shadowSize, 0.0f,  shadowSize ) - offset;

	//PlaneVertex4 *op = (PlaneVertex4*)m_shadowOpPool.malloc() ; 

	PlaneVertex4 op0 = { v0.x, v0.y, v0.z , 0.0f,  0.0f  };
	PlaneVertex4 op1 = { v1.x, v1.y, v1.z , 0.0f,  1.0f  };
	PlaneVertex4 op2 = { v2.x, v2.y, v2.z , 1.0f,  0.0f  };
	PlaneVertex4 op3 = { v3.x, v3.y, v3.z , 1.0f,  1.0f  };
	
	m_shadowOpVec.push_back(op0) ; 
	m_shadowOpVec.push_back(op1) ; 
	m_shadowOpVec.push_back(op3) ; 
	m_shadowOpVec.push_back(op0) ; 
	m_shadowOpVec.push_back(op3) ; 
	m_shadowOpVec.push_back(op2) ; 
}

void CRenderer::CreateUiOp(IDirect3DTexture9* pTxr , const RECT &screen , const RECT* prcSrc , 
				   unsigned int texW , unsigned int texH , DWORD color)
{
	stUiStream *pStream = (stUiStream*)m_uiOpPool.malloc() ; 


	unsigned int nWidth = texW;
	unsigned int nHeight = texH ;
	const RECT& rcScreen = screen;
//	D3DCOLOR	colors[4];

	


	RECT srcRect;
	if(prcSrc == 0)			
	{		
		SetRect(&srcRect,0,0,nHeight,nWidth);
	}
	else
	{
		srcRect = *prcSrc;
	}
	//static DWORD nFVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;
	
	float fu0 = (srcRect.left+0.5f) / (float) nWidth ;			
	float fv0 = (srcRect.bottom+0.5f) / (float) nHeight ;			
	
	float fu1 = (srcRect.left+0.5f) / (float) nWidth ;			
	float fv1 = (srcRect.top+0.5f) / (float) nHeight ;			
	
	float fu2 = (srcRect.right+0.5f) / (float) nWidth ;			
	float fv2 = (srcRect.bottom+0.5f) / (float) nHeight ;			
	
	float fu3 = (srcRect.right+0.5f) / (float) nWidth ;			
	float fv3 = (srcRect.top+0.5f) / (float) nHeight ;	
	
	pStream->ver[0].x = (float)rcScreen.left ; 
	pStream->ver[0].y = (float)rcScreen.bottom ; 
	pStream->ver[0].z = 0.0f ; 
	pStream->ver[0].h = 1.0f ; 
	pStream->ver[0].Color = color ; 
	pStream->ver[0].tu = fu0 ; 
	pStream->ver[0].tv = fv0 ; 


	pStream->ver[1].x = (float)rcScreen.left ; 
	pStream->ver[1].y = (float)rcScreen.top ; 
	pStream->ver[1].z = 0.0f ; 
	pStream->ver[1].h = 1.0f ; 
	pStream->ver[1].Color = color ; 
	pStream->ver[1].tu = fu1 ; 
	pStream->ver[1].tv = fv1 ; 


	pStream->ver[2].x = (float)rcScreen.right ; 
	pStream->ver[2].y = (float)rcScreen.top ; 
	pStream->ver[2].z = 0.0f ; 
	pStream->ver[2].h = 1.0f ; 
	pStream->ver[2].Color = color ; 
	pStream->ver[2].tu = fu3 ; 
	pStream->ver[2].tv = fv3 ; 


	pStream->ver[3] = pStream->ver[0] ; 
	pStream->ver[4] = pStream->ver[2] ; 

	pStream->ver[5].x = (float)rcScreen.right ; 
	pStream->ver[5].y = (float)rcScreen.bottom ; 
	pStream->ver[5].z = 0.0f ; 
	pStream->ver[5].h = 1.0f ; 
	pStream->ver[5].Color = color ; 
	pStream->ver[5].tu = fu2 ; 
	pStream->ver[5].tv = fv2 ; 

	pStream->tex = pTxr ; 

		// 設定位置與材質座標
	//PlaneVertex1 Vertices[4] =
	//{
	//	
	//	(float)rcScreen.left,  (float)rcScreen.bottom , 0.0f, 1.0f, colors[0], fu0,fv0,
	//	(float)rcScreen.left,  (float)rcScreen.top ,    0.0f, 1.0f, colors[1], fu1,fv1,
	//	(float)rcScreen.right , (float)rcScreen.bottom , 0.0f, 1.0f, colors[2] , fu2,fv2,
	//	(float)rcScreen.right , (float)rcScreen.top ,    0.0f, 1.0f, colors[3] , fu3,fv3,
	//};
	//
	// 繪製圖形	
//	pDevice->SetTexture(0 , pTxr);
	//pDevice->SetFVF( nFVF );
	//pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof( PlaneVertex1 ) );


	m_uiOpVec.push_back(pStream) ; 
}


void CRenderer::Flush(std::vector<stFreameEx*> &frameVec)
{

	if(frameVec.empty())
	{
		return ; 
	}

	std::sort(frameVec.begin() , frameVec.end() , sortFrameEx() );
	

	std::vector<stFrameStream> streamVec ; 



	unsigned int drawCount = 0 ; 

	unsigned int remain = frameVec.size() ; 

	unsigned int endUintFrame = frameVec.size() ; 

	unsigned int startFrame = 1; 
	unsigned int endFrame = 0 ; 

	if(frameVec.size() >= m_frameCount)
	{
		endUintFrame = m_frameCount ; 
	}

	drawCount = (frameVec.size() / m_frameCount) + 1 ; 



	for(unsigned int draw = 0 ; draw < drawCount ; ++draw)
	{
		if(remain == 0)
		{
			continue ; 
		}

		startFrame = (draw * endUintFrame) ; 

		PlaneVertex3 *ptr = (PlaneVertex3*)m_pFrameBuffer->Lock() ; 

		unsigned int verIndex = 0 ; 

		stFrameStream stream ; 

		//if(frameVec[0]->divide)
		//{
		//	m_divideOpList.push_back(frameVec[0]) ; 
		//}
		
		stream.layer = frameVec[startFrame]->layer ; 
		stream.verCount = 6 ;
		stream.offset = 0 ; 
		stream.tex = frameVec[startFrame]->pTexture ; 
		
		ptr[verIndex].Position = frameVec[startFrame]->ver[0].Position ; 
		ptr[verIndex].Color = frameVec[startFrame]->ver[0].Color ; 
		ptr[verIndex].tu = frameVec[startFrame]->ver[0].tu ; 
		ptr[verIndex].tv = frameVec[startFrame]->ver[0].tv ; 	

		ptr[verIndex + 1].Position = frameVec[startFrame]->ver[1].Position ; 
		ptr[verIndex + 1].Color = frameVec[startFrame]->ver[1].Color ; 
		ptr[verIndex + 1].tu = frameVec[startFrame]->ver[1].tu ; 
		ptr[verIndex + 1].tv = frameVec[startFrame]->ver[1].tv ; 

		ptr[verIndex + 2].Position = frameVec[startFrame]->ver[2].Position ; 
		ptr[verIndex + 2].Color = frameVec[startFrame]->ver[2].Color ; 
		ptr[verIndex + 2].tu = frameVec[startFrame]->ver[2].tu ; 
		ptr[verIndex + 2].tv = frameVec[startFrame]->ver[2].tv ; 	

		ptr[verIndex + 3].Position = frameVec[startFrame]->ver[3].Position ; 
		ptr[verIndex + 3].Color = frameVec[startFrame]->ver[3].Color ; 
		ptr[verIndex + 3].tu = frameVec[startFrame]->ver[3].tu ; 
		ptr[verIndex + 3].tv = frameVec[startFrame]->ver[3].tv ; 	

		ptr[verIndex + 4].Position = frameVec[startFrame]->ver[4].Position ; 
		ptr[verIndex + 4].Color = frameVec[startFrame]->ver[4].Color ; 
		ptr[verIndex + 4].tu = frameVec[startFrame]->ver[4].tu ; 
		ptr[verIndex + 4].tv = frameVec[startFrame]->ver[4].tv ; 	

		ptr[verIndex + 5].Position = frameVec[startFrame]->ver[5].Position ; 
		ptr[verIndex + 5].Color = frameVec[startFrame]->ver[5].Color ; 
		ptr[verIndex + 5].tu = frameVec[startFrame]->ver[5].tu ; 
		ptr[verIndex + 5].tv = frameVec[startFrame]->ver[5].tv ; 	

		streamVec.push_back(stream) ; 

		
		startFrame += 1 ; 
	
		if(remain < endUintFrame)
		{
			endUintFrame = remain ; 
		}

		endFrame = startFrame + endUintFrame - 1; 

		
		for(unsigned int i = startFrame ; i < endFrame ; ++i)
		{
			stFreameEx *iter = frameVec[i] ; 
			
			verIndex += 6 ;  

			//if(iter->divide)
			//{
			//	m_divideOpList.push_back(iter) ; 
			//}

			if(streamVec.back().layer != iter->layer && streamVec.back().tex == iter->pTexture)
			{
				
				stream.verCount = 0 ; 
				stream.layer = iter->layer ; 

				streamVec.push_back(stream) ; 

			}
			else
			if(streamVec.back().tex != iter->pTexture && streamVec.back().layer == iter->layer)
			{

				stream.verCount = 0 ; 

				stream.tex = iter->pTexture ; 

				streamVec.push_back(stream) ; 

				
			}else
			if(streamVec.back().layer != iter->layer && streamVec.back().tex != iter->pTexture)
			{
			
				stream.verCount = 0 ; 

		
				stream.tex = iter->pTexture ;

				stream.layer = iter->layer ; 

				streamVec.push_back(stream) ; 


			}



			ptr[verIndex].Position = iter->ver[0].Position ; 
			ptr[verIndex].Color = iter->ver[0].Color ; 
			ptr[verIndex].tu = iter->ver[0].tu ; 
			ptr[verIndex].tv = iter->ver[0].tv ; 	

			ptr[verIndex + 1].Position = iter->ver[1].Position ; 
			ptr[verIndex + 1].Color = iter->ver[1].Color ; 
			ptr[verIndex + 1].tu = iter->ver[1].tu ; 
			ptr[verIndex + 1].tv = iter->ver[1].tv ; 

			ptr[verIndex + 2].Position = iter->ver[2].Position ; 
			ptr[verIndex + 2].Color = iter->ver[2].Color ; 
			ptr[verIndex + 2].tu = iter->ver[2].tu ; 
			ptr[verIndex + 2].tv = iter->ver[2].tv ; 	

			ptr[verIndex + 3].Position = iter->ver[3].Position ; 
			ptr[verIndex + 3].Color = iter->ver[3].Color ; 
			ptr[verIndex + 3].tu = iter->ver[3].tu ; 
			ptr[verIndex + 3].tv = iter->ver[3].tv ; 	

			ptr[verIndex + 4].Position = iter->ver[4].Position ; 
			ptr[verIndex + 4].Color = iter->ver[4].Color ; 
			ptr[verIndex + 4].tu = iter->ver[4].tu ; 
			ptr[verIndex + 4].tv = iter->ver[4].tv ; 	

			ptr[verIndex + 5].Position = iter->ver[5].Position ; 
			ptr[verIndex + 5].Color = iter->ver[5].Color ; 
			ptr[verIndex + 5].tu = iter->ver[5].tu ; 
			ptr[verIndex + 5].tv = iter->ver[5].tv ; 	
			
		

			stream.offset += 6 * sizeof(PlaneVertex3) ; 

			streamVec.back().verCount += 6;


		} // end - for i 


		remain -= endUintFrame ; 


		m_pFrameBuffer->UnLock() ;


		D3DXMATRIX mat ; 

		D3DXMatrixIdentity(&mat) ;

		m_pDevice->SetTransform( D3DTS_WORLD, &mat );


		LPDIRECT3DTEXTURE9 preTex = streamVec[0].tex ; 


		m_pFrameBuffer->SetStreamSource(sizeof(PlaneVertex3) , streamVec[0].offset) ; 
		m_pDevice->SetFVF( PlaneVertex3::FVF );

		m_pDevice->SetTexture( 0, preTex );

		m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, streamVec[0].verCount / 3);


		for(unsigned int i = 1 ; i < streamVec.size() ; ++i)
		{
			m_pFrameBuffer->SetStreamSource(sizeof(PlaneVertex3) , streamVec[i].offset + 6 * sizeof(PlaneVertex3)) ; 

			if(streamVec[i].tex != preTex)
			{
				m_pDevice->SetTexture( 0, streamVec[i].tex );

				preTex = streamVec[i].tex ; 
			}

			m_pDevice->SetFVF( PlaneVertex3::FVF );

			m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, streamVec[i].verCount / 3 );

		}

		streamVec.clear() ; 

	}//end - for drawCount



	


}

void CRenderer::FlushDamage(void)
{
	Flush(m_damageOpList) ; 
}

void CRenderer::FlushFrame(void)
{

	std::list<stFreameEx*>::iterator iter(m_divideOpList.begin()) ; 

	while(iter != m_divideOpList.end())
	{
		stFreameEx *fream = *iter ; 

		if(fream->divide)
		{
			m_frameOpList.push_back(fream) ; 

			++iter ;
		}
		else
		{
			m_freeFrameOpList.push_back(fream);

			iter = m_divideOpList.erase(iter) ; 
		}
	}


	Flush(m_frameOpList) ; 
	

}

void CRenderer::FlushUI(void)
{

	//if(m_uiOpVec.empty())
	//{
	//	return ; 
	//}

	//if(m_uiOpVec.size() > 3 * m_shadowCount)
	//{
	//	Assert(0 , "vertex buffer is not enough space") ; 
	//}

	//PlaneVertex1 *ptr = (PlaneVertex1*)m_pUiBuffer->Lock() ; 

	//
	//std::vector<stFrameStream> streamVec ; 

	//unsigned int verIndex = 0 ; 

	//stFrameStream stream ; 

	//stream.layer = 0 ; 
	//stream.verCount = 6 ;
	//stream.offset = 0 ; 
	//stream.tex = m_uiOpVec[0]->tex ; 
	//
	//ptr[verIndex].x = m_uiOpVec[0]->ver[0].x ; 
	//ptr[verIndex].y = m_uiOpVec[0]->ver[0].y ; 
	//ptr[verIndex].z = m_uiOpVec[0]->ver[0].z ; 
	//ptr[verIndex].h = m_uiOpVec[0]->ver[0].h ; 
	//ptr[verIndex].Color = m_uiOpVec[0]->ver[0].Color ; 
	//ptr[verIndex].tu = m_uiOpVec[0]->ver[0].tu ; 
	//ptr[verIndex].tv = m_uiOpVec[0]->ver[0].tv ; 	

	//ptr[verIndex + 1].x = m_uiOpVec[0]->ver[1].x ; 
	//ptr[verIndex + 1].y = m_uiOpVec[0]->ver[1].y ; 
	//ptr[verIndex + 1].z = m_uiOpVec[0]->ver[1].z ; 
	//ptr[verIndex + 1].h = m_uiOpVec[0]->ver[1].h ; 
	//ptr[verIndex + 1].Color = m_uiOpVec[0]->ver[1].Color ; 
	//ptr[verIndex + 1].tu = m_uiOpVec[0]->ver[1].tu ; 
	//ptr[verIndex + 1].tv = m_uiOpVec[0]->ver[1].tv ;


	//ptr[verIndex + 2].x = m_uiOpVec[0]->ver[2].x ; 
	//ptr[verIndex + 2].y = m_uiOpVec[0]->ver[2].y ; 
	//ptr[verIndex + 2].z = m_uiOpVec[0]->ver[2].z ; 
	//ptr[verIndex + 2].h = m_uiOpVec[0]->ver[2].h ; 
	//ptr[verIndex + 2].Color = m_uiOpVec[0]->ver[2].Color ; 
	//ptr[verIndex + 2].tu = m_uiOpVec[0]->ver[2].tu ; 
	//ptr[verIndex + 2].tv = m_uiOpVec[0]->ver[2].tv ; 	

	//ptr[verIndex + 3].x = m_uiOpVec[0]->ver[3].x ; 
	//ptr[verIndex + 3].y = m_uiOpVec[0]->ver[3].y ; 
	//ptr[verIndex + 3].z = m_uiOpVec[0]->ver[3].z ; 
	//ptr[verIndex + 3].h = m_uiOpVec[0]->ver[3].h ; 
	//ptr[verIndex + 3].Color = m_uiOpVec[0]->ver[3].Color ; 
	//ptr[verIndex + 3].tu = m_uiOpVec[0]->ver[3].tu ; 
	//ptr[verIndex + 3].tv = m_uiOpVec[0]->ver[3].tv ; 	

	//ptr[verIndex + 4].x = m_uiOpVec[0]->ver[4].x ; 
	//ptr[verIndex + 4].y = m_uiOpVec[0]->ver[4].y ; 
	//ptr[verIndex + 4].z = m_uiOpVec[0]->ver[4].z ; 
	//ptr[verIndex + 4].h = m_uiOpVec[0]->ver[4].h ; 
	//ptr[verIndex + 4].Color = m_uiOpVec[0]->ver[4].Color ; 
	//ptr[verIndex + 4].tu = m_uiOpVec[0]->ver[4].tu ; 
	//ptr[verIndex + 4].tv = m_uiOpVec[0]->ver[4].tv ; 	

	//ptr[verIndex + 5].x = m_uiOpVec[0]->ver[5].x ; 
	//ptr[verIndex + 5].y = m_uiOpVec[0]->ver[5].y ; 
	//ptr[verIndex + 5].z = m_uiOpVec[0]->ver[5].z ; 
	//ptr[verIndex + 5].h = m_uiOpVec[0]->ver[5].h ; 
	//ptr[verIndex + 5].Color = m_uiOpVec[0]->ver[5].Color ; 
	//ptr[verIndex + 5].tu = m_uiOpVec[0]->ver[5].tu ; 
	//ptr[verIndex + 5].tv = m_uiOpVec[0]->ver[5].tv ; 	

	//streamVec.push_back(stream) ; 


	//for(unsigned int i = 1 ; i < m_uiOpVec.size() ; ++i)
	//{
	//	stUiStream *iter = m_uiOpVec[i] ; 
	//	
	//	verIndex += 6 ;  


	//	if(streamVec.back().tex != iter->tex)
	//	{
	//		stream.verCount = 0 ; 

	//		stream.tex = iter->tex ; 

	//		streamVec.push_back(stream) ; 
	//		
	//	}

	//	ptr[verIndex].x = iter->ver[0].x ; 
	//	ptr[verIndex].y = iter->ver[0].y ; 
	//	ptr[verIndex].z = iter->ver[0].z ; 
	//	ptr[verIndex].h = iter->ver[0].h ; 
	//	ptr[verIndex].Color = iter->ver[0].Color ; 
	//	ptr[verIndex].tu = iter->ver[0].tu ; 
	//	ptr[verIndex].tv = iter->ver[0].tv ; 	

	//	ptr[verIndex + 1].x = iter->ver[1].x ; 
	//	ptr[verIndex + 1].y = iter->ver[1].y ; 
	//	ptr[verIndex + 1].z = iter->ver[1].z ; 
	//	ptr[verIndex + 1].h = iter->ver[1].h ; 
	//	ptr[verIndex + 1].Color = iter->ver[1].Color ; 
	//	ptr[verIndex + 1].tu = iter->ver[1].tu ; 
	//	ptr[verIndex + 1].tv = iter->ver[1].tv ;


	//	ptr[verIndex + 2].x = iter->ver[2].x ; 
	//	ptr[verIndex + 2].y = iter->ver[2].y ; 
	//	ptr[verIndex + 2].z = iter->ver[2].z ; 
	//	ptr[verIndex + 2].h = iter->ver[2].h ; 
	//	ptr[verIndex + 2].Color = iter->ver[2].Color ; 
	//	ptr[verIndex + 2].tu = iter->ver[2].tu ; 
	//	ptr[verIndex + 2].tv = iter->ver[2].tv ; 	

	//	ptr[verIndex + 3].x = iter->ver[3].x ; 
	//	ptr[verIndex + 3].y = iter->ver[3].y ; 
	//	ptr[verIndex + 3].z = iter->ver[3].z ; 
	//	ptr[verIndex + 3].h = iter->ver[3].h ; 
	//	ptr[verIndex + 3].Color = iter->ver[3].Color ; 
	//	ptr[verIndex + 3].tu = iter->ver[3].tu ; 
	//	ptr[verIndex + 3].tv = iter->ver[3].tv ; 	

	//	ptr[verIndex + 4].x = iter->ver[4].x ; 
	//	ptr[verIndex + 4].y = iter->ver[4].y ; 
	//	ptr[verIndex + 4].z = iter->ver[4].z ; 
	//	ptr[verIndex + 4].h = iter->ver[4].h ; 
	//	ptr[verIndex + 4].Color = iter->ver[4].Color ; 
	//	ptr[verIndex + 4].tu = iter->ver[4].tu ; 
	//	ptr[verIndex + 4].tv = iter->ver[4].tv ; 	

	//	ptr[verIndex + 5].x = iter->ver[5].x ; 
	//	ptr[verIndex + 5].y = iter->ver[5].y ; 
	//	ptr[verIndex + 5].z = iter->ver[5].z ; 
	//	ptr[verIndex + 5].h = iter->ver[5].h ; 
	//	ptr[verIndex + 5].Color = iter->ver[5].Color ; 
	//	ptr[verIndex + 5].tu = iter->ver[5].tu ; 
	//	ptr[verIndex + 5].tv = iter->ver[5].tv ; 
	//	
	//

	//	stream.offset += 6 * sizeof(PlaneVertex1) ; 

	//	streamVec.back().verCount += 6;
	//}

	//
	//m_pUiBuffer->UnLock() ;

	//D3DXMATRIX mat ; 

	//D3DXMatrixIdentity(&mat) ;

	//m_pDevice->SetTransform( D3DTS_WORLD, &mat );


	//LPDIRECT3DTEXTURE9 preTex = streamVec[0].tex ; 


	//m_pUiBuffer->SetStreamSource(sizeof(PlaneVertex1) , streamVec[0].offset) ; 
	//m_pDevice->SetFVF( PlaneVertex1::FVF );

	//m_pDevice->SetTexture( 0, preTex );

	//m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, streamVec[0].verCount / 3);

	//for(unsigned int i = 1 ; i < streamVec.size() ; ++i)
	//{
	//	m_pUiBuffer->SetStreamSource(sizeof(PlaneVertex1) , streamVec[i].offset + 6 * sizeof(PlaneVertex1)) ; 

	//	if(streamVec[i].tex != preTex)
	//	{
	//		m_pDevice->SetTexture( 0, streamVec[i].tex );

	//		preTex = streamVec[i].tex ; 
	//	}

	//	m_pDevice->SetFVF( PlaneVertex1::FVF );

	//	m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, streamVec[i].verCount / 3 );

	//}

	//for(unsigned int i = 0 ; i < m_uiOpVec.size() ; ++i)
	//{
	//	m_uiOpPool.free(m_uiOpVec[i]) ; 
	//}

	//m_uiOpVec.clear() ; 

}


void CRenderer::FlushShadow(D3DXMATRIX	*pMat , IDirect3DTexture9 *pTxr)
{	
	if(m_shadowOpVec.empty())
	{
		return ; 
	}

	DWORD updateShadowCount = m_shadowOpVec.size() ;

	PlaneVertex4 *ptr = (PlaneVertex4*)m_pShadowBuffer->Lock() ; 

	memcpy(ptr, &m_shadowOpVec.front(), sizeof(PlaneVertex4)*updateShadowCount);
/*	for(unsigned int i = 0 ; i < updateShadowCount ; ++i)
	{
		ptr[i].x = m_shadowOpVec[i]->x ; 
		ptr[i].y = m_shadowOpVec[i]->y ; 
		ptr[i].z = m_shadowOpVec[i]->z ; 
		ptr[i].tu = m_shadowOpVec[i]->tu ; 
		ptr[i].tv = m_shadowOpVec[i]->tv ; 
	}*/

	m_pShadowBuffer->UnLock() ;

	m_pShadowBuffer->SetStreamSource(sizeof(PlaneVertex4) , 0) ; 
	m_pDevice->SetTransform( D3DTS_WORLD, pMat );
	m_pDevice->SetTexture( 0, pTxr );
	m_pDevice->SetFVF( PlaneVertex4::FVF );

    m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, updateShadowCount / 3 );

}
void CRenderer::Clear()
{
	//for(unsigned int i = 0 ; i < m_shadowOpVec.size() ; ++i)
	//{
	//	m_freeshadowOpPool.push_back(m_shadowOpVec[i]) ;
	//	//m_shadowOpPool.free(m_shadowOpVec[i]) ; 
	//}

	//for(unsigned int i = 0 ; i < m_uiOpVec.size() ; ++i)
	//{
	//	m_uiOpPool.free(m_uiOpVec[i]) ; 
	//}

	for(unsigned int i = 0 ; i < m_frameOpList.size() ; ++i)
	{
		if(!m_frameOpList[i]->divide)
		{
			m_freeFrameOpList.push_back(m_frameOpList[i]);
		//	m_frameOpPool.free(m_frameOpList[i]) ; 	
		}

	}

	for(unsigned int i = 0 ; i < m_damageOpList.size() ; ++i)
	{
		m_freeDamageOpList.push_back(m_damageOpList[i]);

	//	m_frameOpPool.free(m_damageOpList[i]) ; 	
	}



	m_frameOpList.clear() ; 

	m_damageOpList.clear() ; 

	m_shadowOpVec.resize(0) ; 

	m_uiOpVec.clear() ; 

}

////////////////////////////////////////////////////////////////////////////////////
//
BlurTextureHelper::BlurTextureHelper()
{
	
}

void BlurTextureHelper::InitHelper(int texW, int texH)
{
	InitGeometry((float)texW, (float)texH);
}

void BlurTextureHelper::InitGeometry(float texWidth, float texHeight)
{
	
	float fu0 = (0.5f) / (float) texWidth ;			
	float fv0 = (0.5f) / (float) texHeight ;			
	
	float fu1 = (0.5f) / (float) texWidth ;			
	float fv1 = (texHeight+0.5f) / (float) texHeight ;			
	
	float fu2 = (texWidth+0.5f) / (float) texWidth ;			
	float fv2 = (0.5f) / (float) texHeight ;			
	
	float fu3 = (texWidth+0.5f) / (float) texWidth ;			
	float fv3 = (texHeight+0.5f) / (float) texHeight ;	
		
	const int blurCoeffs[5][3]={
		{-1, -1, 30}, 
		{ 1, -1, 40},
		{-1,  1, 30}, 
		{ 1,  1, 40}, 
		{ 0,  0, 255-140}, 
	};

	float weight = 0.5f;
	for(int i=0; i<5;i++)
	{
		float uoffset = blurCoeffs[i][0] * weight/texWidth; 
		float voffset = blurCoeffs[i][1] * weight/texHeight; 
		BYTE alpha = blurCoeffs[i][2];
		DWORD color = D3DCOLOR_ARGB(alpha, alpha, alpha, alpha);
		InitVertex(Vertices[4*i+0], 0, 0, 0, 1,					color, fu0+uoffset, fv0+voffset);
		InitVertex(Vertices[4*i+1], texWidth, 0, 0, 1,			color, fu2+uoffset, fv2+voffset);
		InitVertex(Vertices[4*i+2], 0, texHeight, 0, 1,			color, fu1+uoffset, fv1+voffset);
		InitVertex(Vertices[4*i+3], texWidth, texHeight, 0, 1,	color, fu3+uoffset, fv3+voffset);

		indexBuffer[i*6 + 0] = 0+i*4;
		indexBuffer[i*6 + 1] = 2+i*4;
		indexBuffer[i*6 + 2] = 1+i*4;
		indexBuffer[i*6 + 3] = 1+i*4;
		indexBuffer[i*6 + 4] = 2+i*4;
		indexBuffer[i*6 + 5] = 3+i*4;
	}
}


void BlurTextureHelper::BlurTexture(LPDIRECT3DDEVICE9 pd3dDevice , IDirect3DTexture9* pTex)
{
	DWORD oldValue;
	DWORD oldValue2;
	DWORD oldValue3;
	pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldValue);
	pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &oldValue2);
	pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &oldValue3);

	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 0, 0);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	static DWORD nFVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;

	// 繪製圖形	
	pd3dDevice->SetTexture(0 , pTex);
	pd3dDevice->SetFVF( nFVF );
	pd3dDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4*5, 2*5, indexBuffer, D3DFMT_INDEX16 , Vertices, sizeof( PlaneVertex1 ) );

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldValue);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, oldValue2);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, oldValue3);
}

} // namespace FDO
