#include "stdafx.h"
#include "R_3DInterface.h"
#include "FDO_ParticleObject.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "ParticleSystem.h"
#include "ParticleTemplate.h"
#include "ResourceManager.h"
//---------------------------------------------------------------------------
EXTERN_API R_3DInterface *CreateInterface(void);
EXTERN_API void	DestroyInterface(R_3DInterface *pInt);
//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
const DWORD SParticleVertex:: fvfof_ParticleVertex = D3DFVF_XYZ|D3DFVF_TEX1;
const UINT  SParticleVertex::sizeof_ParticleVertex = sizeof(ParticleVertex);
//---------------------------------------------------------------------------
void DrawPrimitive(LPDIRECT3DDEVICE9 pd3dDevice, const Plane& plane)
{
    // v2   v3
    //  *---*
    //  | / |
    //  *---*
    // v0   v1

    ImmediatePlane p;

    p.ImmediateVerties[0].position = plane.pts[0];
    p.ImmediateVerties[1].position = plane.pts[1];
    p.ImmediateVerties[2].position = plane.pts[3];
    p.ImmediateVerties[3].position = plane.pts[0];
    p.ImmediateVerties[4].position = plane.pts[3];
    p.ImmediateVerties[5].position = plane.pts[2];

    p.ImmediateVerties[0].uv = D3DXVECTOR2(0.f, 1.f);
    p.ImmediateVerties[1].uv = D3DXVECTOR2(1.f, 1.f);
    p.ImmediateVerties[2].uv = D3DXVECTOR2(1.f, 0.f);
    p.ImmediateVerties[3].uv = D3DXVECTOR2(0.f, 1.f);
    p.ImmediateVerties[4].uv = D3DXVECTOR2(1.f, 0.f);
    p.ImmediateVerties[5].uv = D3DXVECTOR2(0.f, 0.f);

    pd3dDevice->SetFVF(SParticleVertex::fvfof_ParticleVertex);
    pd3dDevice->DrawPrimitiveUP
    (
        D3DPT_TRIANGLELIST,
        2, // two triangle per plane
        &(p.ImmediateVerties[0]),
        SParticleVertex::sizeof_ParticleVertex
    );
}
//--------------------------------------------------------------------------------------
ParticleSystem::ParticleSystem()
{    
	D3DXMatrixIdentity(&mInvViewMatrix) ; 
	D3DXMatrixIdentity(&mViewMatrix) ; 

    m_p3DInterface = CreateInterface();
}

ParticleSystem::~ParticleSystem()
{
    this->CleanUp();
    DestroyInterface(m_p3DInterface);
}

const D3DXMATRIX& ParticleSystem::GetInvViewMatrix() const
{
    return mInvViewMatrix;
}

void ParticleSystem::SetInvViewMatrix(const D3DXMATRIX& value)
{
    mInvViewMatrix = value;
}

void ParticleSystem::CleanUp()
{
    mTextureMap.clear();
    mMeshMap.clear();

    _FOREACH(ParticleTempMap, mParticleTempMap, pos)
    {
        delete pos->second;
    }
    mParticleTempMap.clear();
}

R_3DInterfacePtr ParticleSystem::GetInterfacePtr() const
{
    return m_p3DInterface;
}

ParticleTempPtr ParticleSystem::GetParticleTemp(const fstring& fileName)
{
    ConstParticleTempMapIterator pos = mParticleTempMap.find(fileName);
    if (pos != mParticleTempMap.end())
    {
        return pos->second;
    }

    MemPackPtr stream = LoadFromPacket(fileName);
    if (stream == NULL)
    {
        return NULL;
    }

    if (!stream->IsValid())
    {
        return NULL;
    }

    ParticleTempPtr Result = new ParticleTemplate(this);
    if (Result == NULL)
    {
        return NULL;
    }

    if (!Result->LoadFromFileInMemory(stream->Data, stream->DataSize))
    {
        delete Result;
        return NULL;   
    }

    mParticleTempMap[fileName] = Result;
    return Result;
}

LPDIRECT3DTEXTURE9 ParticleSystem::GetTexture(const fstring& fileName, bool staticResource)
{
	static char name[256] ; 

	memset(name , 0 , 256) ;  

	for(int i = 0 ; i < fileName.size() ; ++i)
	{
		name[i] = tolower(fileName[i]);

	}

	fstring lowerFileName(name) ; 

    TextureMapIterator pos = mTextureMap.find(lowerFileName);
    if (pos != mTextureMap.end())
    {
        return pos->second;
    }
    // create new texture
    LPDIRECT3DTEXTURE9 Result = NULL;




    if (staticResource)
    {
        Result = LoadStaticTexture(lowerFileName);
    }
    else
    {
        Result = LoadParticleTexture(lowerFileName);
    }

    if (Result != NULL)
    {
        mTextureMap[lowerFileName] = Result;
    }

    return Result;
}

LPD3DXMESH ParticleSystem::GetMesh(const fstring& fileName, bool staticResource)
{
	static char name[256] ; 

	memset(name , 0 , 256) ;  

	for(int i = 0 ; i < fileName.size() ; ++i)
	{

		name[i] = tolower(fileName[i]);
	}

	fstring lowerFileName(name) ; 


    MeshMapIterator pos = mMeshMap.find(lowerFileName);
    if (pos != mMeshMap.end())
    {
        return pos->second;
    }
    // create new mesh
    LPD3DXMESH Result = NULL;



    if (staticResource)
    {
        Result = LoadStaticMesh(lowerFileName);
    }
    else
    {
        Result = LoadParticleMesh(lowerFileName);
    }

    if (Result != NULL)
    {
        mMeshMap[lowerFileName] = Result;
    }

    return Result;
}

void ParticleSystem::SetViewMatrix(const D3DXMATRIX& value)
{
    mViewMatrix = value;
}

const D3DXMATRIX& ParticleSystem::GetViewMatrix() const
{
    return mViewMatrix;
}

ParticleSystemPtr GetParticleSystemPtr()
{
    return &GetParticleSystem();
}

ParticleSystem& GetParticleSystem()
{
    static ParticleSystem particleSystem;
    return particleSystem;
}

//--------------------------------------------------------------------------------------
// Internal functions forward declarations
//--------------------------------------------------------------------------------------
ParticleObject* AddParticleObject( const char* pFilename, bool bStaticRes );
void DelParticleObject( ParticleObject* pPartObj );

//--------------------------------------------------------------------------------------
typedef list< ParticleObject* > PTCS; // Particles
typedef PTCS::iterator itPO; // ParticleObject
typedef map< void*, PTCS > OPS; // Object Particles
typedef OPS::iterator it;
typedef OPS::value_type value_t;
typedef pair< it, bool > ib;
typedef pair< void*, PTCS > OPSPair;
//--------------------------------------------------------------------------------------
OPS& GetOPS()
{
    static OPS ObjectPtcs;
    return ObjectPtcs;
}
//--------------------------------------------------------------------------------------
void UnRegisterObject( void* pObject )
{
    OPS& ObjectPtcs = GetOPS();
    it pos( ObjectPtcs.find( pObject ) );
    if( pos != ObjectPtcs.end() )
    {
        PTCS& Parts = pos->second;
        for_each( Parts.begin(), Parts.end(), DelParticleObject ); Parts.clear();
        ObjectPtcs.erase( pos );
    }
    //ObjectPtcs.erase( pos );
    //ObjectPtcs.erase( pObject );
}
//--------------------------------------------------------------------------------------
/* void RegisterObject( void* pObject )
{
    ib ret( GetOPS().insert( OPSPair(pObject,PTCS()) ) );
    it pos( ret.first );

    if( ret.second == false )
    { // already existed
        PTCS& Parts = pos->second;
        for_each( Parts.begin(), Parts.end(), DelParticleObject );
    }
    else
    { // not existed
    }
} */
//--------------------------------------------------------------------------------------
ParticleObject* AddParticle( void* pObject, const char* pFilename, bool bStaticRes )
{
    ib ret( GetOPS().insert( OPSPair(pObject,PTCS()) ) );
    it pos( ret.first );

    /* if( ret.second == false )
    { // already existed
    }
    else
    { // not existed
    } */
    ParticleObject* pPartObj = AddParticleObject( pFilename, bStaticRes );

    PTCS& Parts = pos->second;
    Parts.push_back( pPartObj );

    return pPartObj;

    /* OPS& ObjectPtcs = GetOPS();
    it pos( ObjectPtcs.find( pObject ) );
    if( pos != ObjectPtcs.end() )
    {
        PTCS& Parts = pos->second;
        Parts.push_back( AddParticleObject(pFilename) );
    }
    else
    {
        OutputDebugString( L"ERROR! UnRegistered Role Object!" );
    } */
}


//--------------------------------------------------------------------------------------
class CtrlPart
{
private:
    float m_fElapsedTime;
    D3DXVECTOR3 m_vPosition;
    LPD3DXMATRIX m_pWorldMatrix;
	D3DXVECTOR4 m_rectRange;
public:
    CtrlPart( float fElapsedTime, LPD3DXMATRIX pWorldMatrix, const D3DXVECTOR3& vPosition , 
				const D3DXVECTOR4 &rectRange)
    : m_fElapsedTime(fElapsedTime), m_pWorldMatrix(pWorldMatrix), m_vPosition(vPosition) ,
	m_rectRange(rectRange){}
    bool operator () ( ParticleObject* pPartObj ) const
    {
        //if( pPartObj )
        {
			unsigned int count = 0 ; 
			unsigned int count1 = 0 ; 
            pPartObj->SetPosition( m_vPosition );
            pPartObj->Control(0.0f , 0.0f , m_fElapsedTime, m_pWorldMatrix , m_rectRange , true  , count1 , count);
            if( pPartObj->IsPlayEnd() )
            {
                DelParticleObject( pPartObj );
                return true;
            }
            else
            {
                return false;
            }
        }
        /* else
        { // 測試：不會進來這裡
            int i=0;
        } */
    }
};
void ControlParticles( void* pObject, float fElapsedTime, LPD3DXMATRIX pWorldMatrix, const D3DXVECTOR3& vPosition  , 
					  const D3DXVECTOR4 &rectRange)
{
    OPS& ObjectPtcs = GetOPS();

    it pos( ObjectPtcs.find( pObject ) );
    if( pos != ObjectPtcs.end() )
    {
        PTCS& Parts = pos->second;
        Parts.remove_if( CtrlPart(fElapsedTime,pWorldMatrix,vPosition , rectRange) );

        /* PTCS::iterator pos( Parts.begin() );
        while( pos != Parts.end() )
        {
            ParticleObject* pPartObj = (*pos);
            if( pPartObj )
            {
                pPartObj->SetPosition( vPosition );
                pPartObj->Control( fElapsedTime, pWorldMatrix );
                if( pPartObj->IsPlayEnd() )
                {
                    DelParticleObject( pPartObj );
                    pos = Parts.erase( pos );
                }
                else
                {
                    ++pos;
                }
            }
            else
            { // 測試：不會進來這裡
                int i=0;
            }
        } */
    }
}

void RenderParticles( void* pObject, const D3DXMATRIX& matInvView )
{
    OPS& ObjectPtcs = GetOPS();

    it pos( ObjectPtcs.find( pObject ) );
    if( pos != ObjectPtcs.end() )
    {
        PTCS& Parts = pos->second;
		int count = 0 ;

		PTCS::iterator iter = Parts.begin() ; 

		for( ; iter != Parts.end() ; ++iter)
		{
			if((*iter)->IsFlush())
			{
				
				(*iter)->RenderParticle( 0, matInvView , true); 
				++count; 
			}
		}
    }
}

//--------------------------------------------------------------------------------------
struct H_PartMgr
{
    list< ParticleObject* > PartObjs;
    list< ParticleObject* > FreeObjs;

     H_PartMgr();
    ~H_PartMgr();

    ParticleObject* AddPartObj( const char* pFilename, bool bStaticRes );
    void DelPartObj( ParticleObject* pPartObj );
	void Clear();
};

H_PartMgr::H_PartMgr()
{
}

H_PartMgr::~H_PartMgr()
{
	Clear();
}

ParticleObject* H_PartMgr::AddPartObj( const char* pFilename, bool bStaticRes )
{
    ParticleObject* pPartObj = NULL;
    if( FreeObjs.empty() )
    {
        pPartObj = new ParticleObject;
    }
    else
    {
        pPartObj = FreeObjs.front();
        FreeObjs.pop_front();
    }   
    pPartObj->CreateParticle( pFilename, bStaticRes );
    PartObjs.push_back( pPartObj );
    return pPartObj;
}

void H_PartMgr::DelPartObj( ParticleObject* pPartObj )
{
    pPartObj->StopEffect();
    pPartObj->Reset();
    FreeObjs.push_back( pPartObj );
    PartObjs.remove( pPartObj );

    /* list< ParticleObject* >::iterator pos( find( PartObjs.begin(), PartObjs.end(), pPartObj ) );
    if( pos != PartObjs.end() )
    {
        //delete (*pos);
        (*pos)->StopEffect();
        (*pos)->Reset();
        FreeObjs.push_back( *pos );
        PartObjs.erase( pos );
    }
    else
    { // 測試：不會進來這裡
        int i=0;
    } */
}

void H_PartMgr::Clear()
{
    while( !PartObjs.empty() )
    {
        FreeObjs.push_back( PartObjs.front() );
        PartObjs.pop_front();
    }
    while( !FreeObjs.empty() )
    {
        delete FreeObjs.front();
        FreeObjs.pop_front();
    }
    //for_each( PartObjs.begin(), PartObjs.end(), DeletePO ); PartObjs.clear();
    //for_each( FreeObjs.begin(), FreeObjs.end(), DeletePO ); FreeObjs.clear();
}

H_PartMgr& GetPartMgr()
{
    static H_PartMgr StaticPartMgr;
    return StaticPartMgr;
}

void DelParticleObject( ParticleObject* pPartObj )
{
    GetPartMgr().DelPartObj( pPartObj );
}

ParticleObject* AddParticleObject( const char* pFilename, bool bStaticRes )
{
    return GetPartMgr().AddPartObj( pFilename, bStaticRes );
}

void ClearParticles()
{
	GetPartMgr().Clear();
    GetOPS().clear();
}
//--------------------------------------------------------------------------------------
} // end of namespace FDO
