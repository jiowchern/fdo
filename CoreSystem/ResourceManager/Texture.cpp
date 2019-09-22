#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "Texture.h"
#include "PhenixD3D9RenderSystem.h"
#include <d3dx9tex.h> // for D3DXCreateTextureFromFileInMemory

//---------------------------------------------------------------------------
bool FDO::Texture::LoadFromFileInMemory( BYTE *Data, UINT DataSize )
{
    Destory();

    HRESULT hr = D3DXCreateTextureFromFileInMemory(
        g_RenderSystem.GetD3DDevice(),
        Data,
        DataSize,
        &m_Texture );

    if( FAILED(hr) )
        return false;

    return true;
}

//---------------------------------------------------------------------------
void FDO::Texture::Clear()
{
    m_Texture = 0;
}

//---------------------------------------------------------------------------
void FDO::Texture::Destory()
{
    if( m_Texture )
    {
        m_Texture->Release();
        m_Texture = 0;
    }
}

//---------------------------------------------------------------------------
bool FDO::Texture::Release()
{
    return true;
}