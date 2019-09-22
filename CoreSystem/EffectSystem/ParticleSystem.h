//---------------------------------------------------------------------------
//
// ���l�S�Ĩt��
//
// �]�t:
//      ���l�S�Ĩt�Τ����B���l�S�ĺ޲z��
//
//      �޲z���l�S�ĩҦ��Ψ쪺����B�ҫ��B����
//
//---------------------------------------------------------------------------
#ifndef ParticleSystemH
#define ParticleSystemH
//---------------------------------------------------------------------------
#include "ParticlePreRequest.h"
#include <map>
//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
class ParticleSystem
{
private:
    R_3DInterfacePtr m_p3DInterface;
    ParticleTempMap mParticleTempMap;
    TextureMap mTextureMap;
    MeshMap mMeshMap;
    D3DXMATRIX mInvViewMatrix;
    D3DXMATRIX mViewMatrix;

public:
    // ���oEmitter.DLL��������
    ParticleSystem();
    virtual ~ParticleSystem();

    R_3DInterfacePtr GetInterfacePtr() const;
    ParticleTempPtr GetParticleTemp(const fstring& fileName);
    void CleanUp();

    LPDIRECT3DTEXTURE9 GetTexture(const fstring& fileName, bool staticResource);
    LPD3DXMESH GetMesh(const fstring& fileName, bool staticResource);

    void SetInvViewMatrix(const D3DXMATRIX& value);
    const D3DXMATRIX& GetInvViewMatrix() const;

    void SetViewMatrix(const D3DXMATRIX& value);
    const D3DXMATRIX& GetViewMatrix() const;
};

ParticleSystem& GetParticleSystem();
ParticleSystemPtr GetParticleSystemPtr();
//---------------------------------------------------------------------------
void DrawPrimitive(LPDIRECT3DDEVICE9 pd3dDevice, const Plane& plane);

void ControlParticles( void* pObject, float fElapsedTime, LPD3DXMATRIX pWorldMatrix, const D3DXVECTOR3& vPosition , 
					  const D3DXVECTOR4 &rectRange);
void RenderParticles( void* pObject, const D3DXMATRIX& matInvView );
ParticleObject* AddParticle( void* pObject, const char* pFilename, bool bStaticRes=false );
void UnRegisterObject( void* pObject );
//void RegisterObject( void* pObject );

//add by gino
void ClearParticles();
//---------------------------------------------------------------------------
typedef struct SParticleVertex
{
    D3DXVECTOR3 position;
    D3DXVECTOR2 uv;
    static const DWORD fvfof_ParticleVertex;
    static const UINT sizeof_ParticleVertex;
} ParticleVertex, *ParticleVertexPtr;

typedef struct SImmediatePlane
{
    ParticleVertex ImmediateVerties[6];
}ImmediatePlane, *ImmediatePlanePtr;
//---------------------------------------------------------------------------
} // end of namespace FDO
//---------------------------------------------------------------------------
#endif // ParticleSystemH
