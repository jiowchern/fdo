//-----------------------------------------------------------------------------
#ifndef _JLSkyBox_H_
#define _JLSkyBox_H_
//-----------------------------------------------------------------------------
#include <d3dx9.h>
#include <string>
#include <vector>
//-----------------------------------------------------------------------------

namespace FDO
{

using namespace std;

class JLSkyBox
{
private:
    LPDIRECT3DDEVICE9               m_pD3DDevice;
    LPD3DXMESH                      m_pMesh;                                // Our mesh object in sysmem

    LPD3DXBUFFER                    m_pAdjBuffer;                           // for get DWORD*
    LPD3DXBUFFER                    m_pMtrlBuffer;                          // for get D3DXMATERIAL*

    // 以下三個儲存Mesh時會用到
    DWORD*                          m_pAdjacency;                           // 鄰接資訊
    D3DXMATERIAL*                   m_pD3DXMaterials;                       // has M & T information

    DWORD                           m_dwNumMaterials;                       // Number of mesh materials
    vector<D3DMATERIAL9>            m_pMaterials;
    vector<LPDIRECT3DTEXTURE9>      m_pTextures;

    D3DXVECTOR3                     m_vScale;
    D3DXMATRIXA16                   m_matWorld;

    string                          m_FileName;
    bool                            m_IsCreate;

public:
    JLSkyBox();
    ~JLSkyBox();

    void Destory();
    bool LoadFromFile( const string& strFileName );
    bool LoadFromFileInMemory( const string& strFileName );
    void SetScale( const D3DXVECTOR3& vScale );
    void Render();
};

//-----------------------------------------------------------------------------
} //namespace FDO

//-----------------------------------------------------------------------------
#endif // _JLSkyBox_H_



