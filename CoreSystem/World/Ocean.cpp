#include "stdafx.h"
#pragma hdrstop
#include "Ocean.h"
#include "SCN.h"
#include "FdoUpdateArgs.h"

//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
const DWORD OceanVertex::FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
//----------------------------------------------------------------------------
Ocean::Ocean()
: mVertexBuffer(NULL)
, mIndexBuffer(NULL)
{
    this->CleanUp();
}
//----------------------------------------------------------------------------
Ocean::~Ocean()
{
    this->CleanUp();
}

void Ocean::SetTexture(size_t index, LPDIRECT3DTEXTURE9 value)
{
    mTextures[index] = value;
}

bool Ocean::Draw()
{
    F_RETURN(!mTextures.empty());

    //DWORD dwColorOP, dwColorARG1, dwColorARG2;
    //_device->GetTextureStageState( 0, D3DTSS_COLOROP, &dwColorOP );//D3DTOP_SELECTARG1
    //_device->GetTextureStageState( 0, D3DTSS_COLORARG1, &dwColorARG1 );//D3DTA_TEXTURE
    //_device->GetTextureStageState( 0, D3DTSS_COLORARG2, &dwColorARG2 );//D3DTA_CURRENT
    //DWORD dwAlphaOP, dwAlphaARG1, dwAlphaARG2;
    //_device->GetTextureStageState( 0, D3DTSS_ALPHAOP, &dwAlphaOP );//D3DTOP_SELECTARG1
    //_device->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &dwAlphaARG1 );//D3DTA_TEXTURE
    //_device->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &dwAlphaARG2 );//D3DTA_TEXTURE
    
    //設定地形紋理第一層
    //_device->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
    //_device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
    //_device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
    //_device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
    //_device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_BLENDDIFFUSEALPHA);
    //_device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
    //_device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEMP);

	//DWORD dwAmbient;
	//_device->GetRenderState( D3DRS_AMBIENT, &dwAmbient );
	//_device->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(0,0,0) );

    mDevice->SetTransform(D3DTS_WORLD, &(this->GetWorldMatrix()));

    mDevice->SetStreamSource(0, this->GetVertexBuffer(), 0, sizeof_OceanVertex);
    mDevice->SetFVF(OceanVertex::FVF);
    mDevice->SetIndices(this->GetIndexBuffer());
    mDevice->SetMaterial(&(this->GetMaterial())); // 設定質料
    mDevice->SetTexture(0, mTextures[mActiveTexture]); // 設定材質

    //_device->LightEnable(4, true);

    //_device->SetRenderState( D3DRS_LIGHTING, FALSE );
    //_device->SetRenderState( D3DRS_ZENABLE, FALSE );

    //_device->LightEnable(3, false);
    //_device->LightEnable(4, false);
    //_device->LightEnable(5, false);
    //_device->LightEnable(6, false);

	HRESULT hr = mDevice->DrawIndexedPrimitive
    (
        D3DPT_TRIANGLELIST,
        0,
        0,
        mNumVertices,
        0,
        mNumTriangles
    );

    //_device->LightEnable(3, true);
    //_device->LightEnable(4, true);
    //_device->LightEnable(5, true);
    //_device->LightEnable(6, true);
    //_device->LightEnable(4,false);

	//_device->SetRenderState( D3DRS_AMBIENT, dwAmbient );

	// 還原材質設定值
    //_device->SetTextureStageState( 0, D3DTSS_COLOROP, dwColorOP );//D3DTOP_SELECTARG1
    //_device->SetTextureStageState( 0, D3DTSS_COLORARG1, dwColorARG1 );//D3DTA_TEXTURE
    //_device->SetTextureStageState( 0, D3DTSS_COLORARG2, dwColorARG2 );//D3DTA_CURRENT
    //_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, dwAlphaOP );//D3DTOP_SELECTARG1
    //_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, dwAlphaARG1 );//D3DTA_TEXTURE
    //_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, dwAlphaARG2 );//D3DTA_TEXTURE

    // 還原狀態...
    //_device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    //_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    //_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

    F_RETURN(hr == S_OK);

    return true;
}

IDirect3DIndexBuffer9* Ocean::GetIndexBuffer() const
{
    if (mIndexBufferOutOfData)
    {
        UpdateIndexBuffer();
    }
    return mIndexBuffer;
}

void Ocean::SetNumVertsPerRow(int value)
{
    mNumVertsPerRow = value;

    mNumVertices = mNumVertsPerRow * mNumVertsPerCol;
    mNumTriangles = GetNumCellsPerRow() * GetNumCellsPerCol() * 2;

    mVertexBufferOutOfData = true;
    mIndexBufferOutOfData = true;

	// coordinates to start generating vertices at
	int startX = -GetNumCellsPerRow() / 2;
	int startZ = -GetNumCellsPerCol() / 2;
	// coordinates to end generating vertices at
	int endX = -startX;
	int endZ = -startZ;

    mBox.min = D3DXVECTOR3(static_cast<float>(startX), 0.f, static_cast<float>(startZ));
    mBox.max = D3DXVECTOR3(static_cast<float>(endX), 0.f, static_cast<float>(endZ));
    mBoundingBox = mBox;
    mBoundsOutOfData = true;
}

void Ocean::SetNumVertsPerCol(int value)
{
    mNumVertsPerCol = value;

    mNumVertices = mNumVertsPerRow * mNumVertsPerCol;
    mNumTriangles = GetNumCellsPerRow() * GetNumCellsPerCol() * 2;

    mVertexBufferOutOfData = true;
    mIndexBufferOutOfData = true;

	// coordinates to start generating vertices at
	int startX = -GetNumCellsPerRow() / 2;
	int startZ = -GetNumCellsPerCol() / 2;
	// coordinates to end generating vertices at
	int endX = -startX;
	int endZ = -startZ;

    mBox.min = D3DXVECTOR3(static_cast<float>(startX), 0.f, static_cast<float>(startZ));
    mBox.max = D3DXVECTOR3(static_cast<float>(endX), 0.f, static_cast<float>(endZ));
    mBoundingBox = mBox;
    mBoundsOutOfData = true;
}

int Ocean::GetNumCellsPerRow() const
{
    return mNumVertsPerRow - 1;
}

int Ocean::GetNumCellsPerCol() const
{
    return mNumVertsPerCol - 1;
}

int Ocean::GetNumIndices() const
{
    // 3 indices per triangle
    return mNumTriangles * 3;
}

bool Ocean::UpdateIndexBuffer() const
{
    SafeRelease(mIndexBuffer);
    F_RETURN(mIndexBuffer == NULL);

    F_RETURN(mDevice != NULL);

    HRESULT hr;
    hr = mDevice->CreateIndexBuffer
    ( 
        GetNumIndices() * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &mIndexBuffer,
        0
    );

    F_RETURN(hr == S_OK);

    WORD* indices = NULL;

	hr = mIndexBuffer->Lock(0, 0, (void**)&indices, 0);
    F_RETURN(hr == S_OK);

	// index to start of a group of 6 indices that describe the
	// two triangles that make up a quad
	int baseIndex = 0;

	// loop through and compute the triangles of each quad
	_LOOP(GetNumCellsPerCol(), i)
	{
		_LOOP(GetNumCellsPerRow(), j)
		{
		    // 第一個三角形
			indices[baseIndex    ] = (i+1) * mNumVertsPerRow + j;
			indices[baseIndex + 1] =   i   * mNumVertsPerRow + j + 1;
			indices[baseIndex + 2] =   i   * mNumVertsPerRow + j;

            // 第二個三角形
			indices[baseIndex + 3] = (i+1) * mNumVertsPerRow + j + 1;
			indices[baseIndex + 4] =   i   * mNumVertsPerRow + j + 1;
			indices[baseIndex + 5] = (i+1) * mNumVertsPerRow + j;

			// next quad
			baseIndex += 6;
		}
	}

    hr = mIndexBuffer->Unlock();
    F_RETURN(hr == S_OK);

    mIndexBufferOutOfData = false;
    return true;
}

const D3DXCOLOR& Ocean::GetColor() const
{
    return mColor;
}

void Ocean::SetColor(const D3DXCOLOR& value)
{
    mColor = value;
    mMaterialOutOfData = true;
}

const D3DMATERIAL9& Ocean::GetMaterial() const
{
    if (mMaterialOutOfData)
    {
        mMaterial.Ambient = mColor;
        mMaterial.Diffuse = mColor;
        mMaterialOutOfData = false;
    }
    return mMaterial;
}

void Ocean::CleanUp()
{
    base::CleanUp();
    // vertex buffer
    SafeRelease(mVertexBuffer);
    mVertexBufferOutOfData = false;
    // index buffer
    SafeRelease(mIndexBuffer);
    mIndexBufferOutOfData = false;
    // material
    mColor = WHITE;
    memset(&mMaterial, 0, sizeof(D3DMATERIAL9));
    mMaterialOutOfData = false;
    // texture
    mTextures.clear();
    mActiveTexture = 0;

    mTU = 0.f;
    mTV = 0.f;

    mAmplitude = 0.f;
    mInterval = 0.f;
    mTextureSpeed = 0.f;
}

void Ocean::SetTexCount(size_t value)
{
    mTextures.resize(value, NULL);
}

void Ocean::SetTextureSpeed(float value)
{
    mTextureSpeed = value * 0.001f / 5.f;
}

size_t Ocean::GetTexCount() const
{
    return mTextures.size();
}

bool Ocean::Update(const UpdateArgsPtr &args)
{
    mElapsedTime += args->GetElapsedTime();
    // calc visible
    this->SetVisible
    (
        args->IsInBounding(this->GetMinBox(), this->GetMaxBox()) &&
        args->IsInFieldOfView(this->GetPosition() + this->GetOffset(), this->GetRadius())
    );
    // check viaible
    F_RETURN(this->IsVisible());

    F_RETURN(!mTextures.empty());

    mActiveTexture += static_cast<UINT>(mElapsedTime / mTextureSpeed);
    mActiveTexture %= static_cast<UINT>(mTextures.size());
    // clear elapsed time
    mElapsedTime = fmod(mElapsedTime, mTextureSpeed);

//    if( _amplitude != 0.0f )
//    {
//        float fElapsedTimeA = fElapsedTime + m_fReservedTimeA;
//        m_fReservedTimeA = fmod( fElapsedTimeA, _interval );
//
//        float fAngle = fElapsedTimeA / _interval * D3DX_PI * 2.0f;
//        //float fValue = sinf( fAngle );
//
//        OceanVertex* v = 0;
//	    if( SUCCEEDED( _vb->Lock(0, 0, (void**)&v, 0) ) ) // 取得緩衝器的記憶體
//        {
//            //for(int z=0; z<_numVertsPerCol; ++z )
//            {
//                //for(int x=0; x<_numVertsPerRow; ++x )
//                for( UINT i=0; i<m_numofVertices; ++i )
//                {
//                    //int i = z * _numVertsPerRow + x;
//                    double fValue = sinf( fAngle + 2.0 * D3DX_PI * (float)(i%(NUMOF_WAVE_POINT+1)) / (float)NUMOF_WAVE_POINT );
//                    OceanVertex& Vertex = v[i];
//                    Vertex.Position.y = _amplitude * fValue;
//                }
//            }
//            _vb->Unlock();
//        }
//    }

    return true;
}

/**
 * @brief 
 */
void Ocean::RegisterForDrawing(SCNPtr scn)
{
    if (this->IsVisible())
    {
        scn->RegisterForDrawing(DRAWKIND::OCEAN, this);
    }
}

IDirect3DVertexBuffer9* Ocean::GetVertexBuffer() const
{
    if (mVertexBufferOutOfData)
    {
        UpdateVertexBuffer();
    }
    return mVertexBuffer;
}

bool Ocean::UpdateVertexBuffer() const
{
    SafeRelease(mVertexBuffer);
    F_RETURN(mVertexBuffer == NULL);

    F_RETURN(mDevice != NULL);

    HRESULT hr = S_OK;
    hr = mDevice->CreateVertexBuffer
    ( 
        mNumVertices * sizeof_OceanVertex,
		D3DUSAGE_WRITEONLY, 
        OceanVertex::FVF, 
        D3DPOOL_MANAGED, 
        &mVertexBuffer, 
        0 
    );

    F_RETURN(hr == S_OK);

	OceanVertexPtr v = NULL;
	hr = mVertexBuffer->Lock(0, 0, (void**)&v, 0); // 取得緩衝器的記憶體
    F_RETURN(hr == S_OK);

	// coordinates to start generating vertices at
	int startX = -GetNumCellsPerRow() / 2;
	int startZ = -GetNumCellsPerCol() / 2;
	// coordinates to end generating vertices at
	int endX = -startX;
	int endZ = -startZ;
	// compute the increment size of the texture coordinates
	// from one vertex to the next.
	float uCoordIncrementSize = mTU / (float)GetNumCellsPerRow();
	float vCoordIncrementSize = mTV / (float)GetNumCellsPerCol();

    int i = 0;
    for (int z = startZ; z <= endZ; ++z)
    {
	    int j = 0;
	    for (int x = startX; x <= endX; ++x)
	    {
		    // compute the correct index into the vertex buffer and heightmap
		    // based on where we are in the nested loop.
		    int index = i * mNumVertsPerRow + j;

            v[index].Position.x = static_cast<float>(x);
            v[index].Position.y = 0.f;
            v[index].Position.z = static_cast<float>(z);
            v[index].NORMAL     = VEC3_UNIT_Y;
            v[index]._u         = (float)j * uCoordIncrementSize;
            v[index]._v         = mTV - (float)i * vCoordIncrementSize;

		    ++j; // next column
	    }
	    ++i; // next row
    }

    hr = mVertexBuffer->Unlock();
    F_RETURN(hr == S_OK);

    mVertexBufferOutOfData = false;
    return true;
}

void Ocean::SetTU(float value)
{
    mTU = value;
    mVertexBufferOutOfData = true;
}

void Ocean::SetTV(float value)
{
    mTV = value;
    mVertexBufferOutOfData = true;
}

void Ocean::SetDevice(LPDIRECT3DDEVICE9 value)
{
    mDevice = value;
}

LPDIRECT3DDEVICE9 Ocean::GetDevice() const
{
    return mDevice;
}

void Ocean::SetAmplitude(float value)
{
    mAmplitude = value;
}

void Ocean::SetInterval(float value)
{
    mInterval = value * 0.001f * 50.f;
}
//----------------------------------------------------------------------------
} // end of namespace FDO
