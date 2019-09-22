#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
//#include "J_SoundManager.h"
//#include "FDO_ParticleManager.h"
#include "ResourceManager.h"
#include "ParticleSystem.h"
#include "PacketReader.h"
#include "PhenixD3D9RenderSystem.h"
//#include "FOVClip.h"
//#include "tracetool.h"

#include "R_3DInterface.h"
#include "R_EmitterPlayer.h"
#include "R_EmitterManager.h"
#include "R_ParticleRender.h"
#include "R_PRenderManager.h"
#include "R_ParticleEmitter.h"

//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("管理模式啟動") 
#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////

#pragma hdrstop
#include "FDO_ParticleObject.h"
#include "ParticleHelper.h"
#include "FdoUpdateArgs.h"
#include "SCN.h"
//---------------------------------------------------------------------------
//EXTERN_API R_3DInterface *CreateInterface(void);
//EXTERN_API void	DestroyInterface(R_3DInterface *pInt);
//---------------------------------------------------------------------------
namespace FDO
{

//const D3DXVECTOR3 g_vOriCenter( 0.0f, 2.0f, 0.0f );
//const float g_fRadius = 5.0f;

typedef map<string,LPDIRECT3DTEXTURE9> TEXS;
typedef TEXS::value_type vaTex;
typedef TEXS::iterator   itTex;
typedef map<string,LPD3DXMESH> MESHS;
typedef MESHS::value_type vaMesh;
typedef MESHS::iterator   itMesh;
//--------------------------------------------------------------------------------------
//const unsigned short sizeof_MATRIX4 = sizeof(float) * 16;
//const unsigned short sizeof_D3DMATERIAL9 = sizeof( D3DMATERIAL9 );
const unsigned short numof_ParticleLight = 7;
const char PARTICLE_WATCH_ID[] = "PARTICLE_WATCH_ID";
const char PARTICLE_WATCH_TITLE[] = "PARTICLE_WATCH_TITLE";

inline R_MATRIX4 GetUCS( LPDIRECT3DDEVICE9 pd3dDevice )
{
    D3DXMATRIX matView;
    pd3dDevice->GetTransform(D3DTS_VIEW,&matView);

    // 將D3DXMATRIX轉成R_MATRIX4的格式
    R_MATRIX4 viewMat;
    memcpy( &viewMat, &matView, sizeof_MATRIX4 );

    return viewMat;
}

/* inline void InternalSetPosition( LPDIRECT3DDEVICE9 pd3dDevice,
    R_EmitterManager* pEmitterManager, const D3DXMATRIX& matInvView,
    float x, float y, float z )
{
    unsigned int size = pEmitterManager->GetEmitterCount();
    for ( unsigned int i=0; i<size; ++i )
    {
        R_ParticleEmitter *pEmt = pEmitterManager->GetEmitterByIndex(i);
        if ( pEmt == NULL )
            continue;

        pEmt->SetCameraUCS(GetUCS(pd3dDevice));
        pEmt->ResetUCS();

        if ( pEmt->GetLevel() == 0 )
        {
            R_MATRIX4 mat;
            mat.Identity();
            //  TRANSNLATE TO LOCATOR'S UCS
            mat.SetPosition( R_VECTOR3(x,y,z) );

            // 可在這裡增加旋轉、縮放等

            R_MATRIX4 ucs( pEmt->GetUCS() );
            ucs = ucs * mat;

            pEmt->SetUCS(ucs);

            pEmt->UpdateUCS();
            // RecursiveUpdate(pEmt);
        }
    }
} */

inline void SetParticleLight( LPDIRECT3DDEVICE9 pd3dDevice, bool bPartLight )
{
    for(int i=0; i<numof_ParticleLight; ++i )
    {
        pd3dDevice->LightEnable( i, !bPartLight );
    }

    /* static bool bSetPartLight = false;
    if( bSetPartLight == false )
    {
        D3DLIGHT9 light; // 分子專用燈光
        ZeroMemory( &light, sizeof(D3DLIGHT9) );
        light.Type = D3DLIGHT_DIRECTIONAL;
        light.Diffuse = D3DXCOLOR(1,1,1,1);
        light.Ambient = D3DXCOLOR(1,1,1,1);
        light.Specular = D3DXCOLOR(1,1,1,1);
        light.Direction = D3DXVECTOR3(0,-1,0);
        pd3dDevice->SetLight( numof_ParticleLight, &light );
        bSetPartLight = true;
    } */
    //pd3dDevice->SetRenderState(D3DRS_AMBIENT,D3DXCOLOR(0,0,0,1));
    //pd3dDevice->LightEnable( numof_ParticleLight, bPartLight );
}
//---------------------------------------------------------------------------
//static variable for ParticleObject
MemDataMgr          ParticleObject::sm_MemDataCacheMgr;
ParticleObject::ParticleFactory ParticleObject::sParticleFactory;

ParticleObject::ParticleObject()
: m_pd3dDevice(g_RenderSystem.GetD3DDevice())
, mParticleTemp(NULL)
, mControled(false)
{
    // 註冊一個分子特效管理員
    R_3DInterfacePtr p3DInterface = this->GetParticleSystem()->GetInterfacePtr();
    if(p3DInterface != NULL)
    {
        p3DInterface->RegisterObject(I3D_EMITTERMANAGER, (void**)&m_pEmitterManager);
    }
    // 生成一個分子特效播放器
    m_pEmitterPlayer = new R_EmitterPlayer(m_pd3dDevice);
    m_pEmitterPlayer->SetEmitterManager(m_pEmitterManager);
    m_pEmitterPlayer->InitPRenderManager(0);

    //QueryPerformanceFrequency( &m_liTimerFrequency );
    Reset();
}
//---------------------------------------------------------------------------
ParticleObject::~ParticleObject()
{
    this->CleanUp();
}
//---------------------------------------------------------------------------
void* ParticleObject::operator new (size_t)
{
    return sParticleFactory.Create();
}
//---------------------------------------------------------------------------
void ParticleObject::operator delete (void* p)
{
    if (p != NULL)
    {
        sParticleFactory.Destroy(p);
    }
}
//---------------------------------------------------------------------------
void ParticleObject::Reset()
{

	CDrawable::CleanUp() ; 

 //   m_pEmitterPlayer->SetEmitterManager(m_pEmitterManager);
 //   m_pEmitterPlayer->InitPRenderManager(0);

	m_vFixSoundPosition = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    //m_vPosition = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    //m_vRotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    //m_vScale    = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
    //m_vOffset   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    //m_vCenter   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    //D3DXMatrixIdentity( &m_matWorld );

	m_bFixSoundPosition = false;
    m_bControl = false;
    //m_bHaveSound = false;
    //m_shChannelNum = -1;

	m_bFlush = true ;
}

//---------------------------------------------------------------------------
// 建立分子特效
//---------------------------------------------------------------------------
bool ParticleObject::CreateParticle(const string& FileName, bool staticResource)
{
#ifdef RESMGR

    memPack* pMem = LoadFromPacket( FileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;

#else
	//auto_ptr<TraceNodeEx> pNode( m_pParticleTrace->Debug()->CreateChildEx(FileName.c_str()) );
    //pNode->Send();
    // 建立分子特效
	BYTE* pData =  sm_MemDataCacheMgr.GetMemData( FileName );
	if( pData == NULL )
	{
		// 讀取封裝檔
		memPack* pMem = LoadFromPacket( FileName );
		if( pMem == NULL )
			return false;

		pData = pMem->Data;

		// 儲存記憶體資料
		sm_MemDataCacheMgr.AddMemData( FileName, pMem->Data, pMem->DataSize );
	}

#endif




    // 從記憶體讀取*.emt檔
    if( !m_pEmitterPlayer->LoadEmitterFromFileInMemory( &pData ) )
        return false;

    //LoadResource( bStaticRes, pNode.get() ); // 取得分子需要的所有資源
	//LoadResource( bStaticRes ); // 取得分子需要的所有資源
    //QueryPerformanceCounter(&m_liPreviousTime); // 取得計數時間
    PlayEffect(); // 播放分子特效
    SaveEmtPara(); // 紀錄發射器參數

    this->SetControlEnabled(PARTICLE_FLAG::INTERNAL_UPDATE, (this->GetPRenderPoolSize() == PRPS_INTERNAL_UPDATE));
    this->SetControlEnabled(PARTICLE_FLAG::FOG_EFFECT, (this->GetPRenderPoolSize() == PRPS_FOG_EFFECT));
    this->SetControlEnabled(PARTICLE_FLAG::FORCE_VISIBLE, (this->GetPRenderPoolSize() == PRPS_FORCE_VISIBLE));
    this->SetControlEnabled(PARTICLE_FLAG::FAR_VISIBLE, (this->GetPRenderPoolSize() == PRPS_FAR_VISIBLE));
    this->SetControlEnabled(PARTICLE_FLAG::STATIC_RESOURCE, staticResource);

    return true;
}

bool ParticleObject::LoadFromFileInMemory(const BYTE* data, UINT size)
{
    F_RETURN(m_pEmitterPlayer != NULL);
    
    BYTE* pData = const_cast<BYTE*>(data);
    F_RETURN(m_pEmitterPlayer->LoadEmitterFromFileInMemory(&pData));
    // 紀錄發射器參數
    SaveEmtPara();
    return true;
}
//---------------------------------------------------------------------------
//void ParticleObject::LoadResource( bool bStaticRes, TraceNodeEx* pNodeEx )
//void ParticleObject::LoadResource( bool bStaticRes )
//{
//    // Mesh
//    //auto_ptr<TraceNodeEx> pNode( pNodeEx->CreateChildEx("MESH") );
//    //pNode->Send();
//    m_pMeshs.clear();
//    unsigned short nCount = m_pEmitterManager->GetMeshFileCount();
//    for( unsigned short i=0; i<nCount; ++i )
//    {
//        char* pFilename = strupr( m_pEmitterManager->GetMeshFileByIndex(i) );
//        //pNode->Send( pFilename );
//        itMesh pos( m_pMeshs.find( pFilename ) );
//        if( pos != m_pMeshs.end() )
//            continue;
//
//        LPD3DXMESH pMesh = 0;
//        if( bStaticRes )
//            pMesh = LoadStaticMesh( pFilename );
//        else
//            pMesh = LoadParticleMesh( pFilename );
//        //if( pMesh == NULL )
//            //pNode->SetIconIndex( CST_ICO_ERROR );
//        m_pMeshs.insert( vaMesh(pFilename,pMesh) );
//    }
//
//    // 材質
//    //pNode.reset( pNodeEx->CreateChildEx("TEXTURE") );
//    //pNode->Send();
//    m_pTextures.clear();
//    nCount = m_pEmitterManager->GetTextureFileCount();
//    for( unsigned short i=0; i<nCount; ++i )
//    {
//        char* pFilename = strupr( m_pEmitterManager->GetTextureFileByIndex(i) );
//        //pNode->Send( pFilename );
//        itTex pos( m_pTextures.find(pFilename) );
//        if( pos != m_pTextures.end() )
//            continue;
//
//        LPDIRECT3DTEXTURE9 pTexture = 0;
//        if( bStaticRes )
//            pTexture = LoadStaticTexture( pFilename );
//        else
//            pTexture = LoadParticleTexture( pFilename );
//        //if( pTexture == NULL )
//            //pNode->SetIconIndex( CST_ICO_ERROR );
//        m_pTextures.insert( vaTex(pFilename,pTexture) );
//    }      
//
//    // 音樂
//    //pNode.reset( pNodeEx->CreateChildEx("SOUND") );
//    //pNode->Send();
//    if( m_pEmitterManager->GetSoundFileCount() > 0 )
//    {
//        m_bHaveSound = true;
//        m_strSoundName = "sound\\";
//        m_strSoundName += m_pEmitterManager->GetSoundFileByIndex(0);
//        //pNode->Send( m_strSoundName.c_str() );
//    }
//    else
//    {
//        m_bHaveSound = false;
//        m_shChannelNum = -1;
//    }
//}
//---------------------------------------------------------------------------
void ParticleObject::RecursiveUpdate(R_ParticleEmitterPtr pSrcEmt)
{
    //  RECURSIVE UPDATE RENDER OBJECT
    //  IF EMITTER HAVE CHILDREN EMITTER AND NEED BE RENDER.    

    for ( int i=0; i<(int)pSrcEmt->GetDummyEmitterCount(); ++i )
    {
        R_PRenderManager *m_pPRenderManager = m_pEmitterPlayer->GetPRenderManager();

        if ( pSrcEmt->IsDummyEmitterValid(i) )
        {
            R_ParticleEmitter *pEmt = pSrcEmt->GetDummyEmitterByIndex(i);
            if ( pEmt )
            {
                R_ParticleRender *pRnd = NULL;
                //pEmt->SetCameraUCS(m_pCamera->GetUCS());GetUCS
                pEmt->SetCameraUCS( GetUCS(m_pd3dDevice) );

                //  DETECT WHETHER THIS EMITTER ALREADY BEEN ATTCHED TO RENDER                
                if ( !m_pPRenderManager->IsEmitterAttached(pEmt) )
                {
                    pRnd = (R_ParticleRender *)m_pPRenderManager->Register();
                    if ( pRnd )
                    {
                        pRnd->AttachEmitter(pEmt);
                        pRnd->Play();
                    }
                }
                else
                {
                    pRnd = m_pPRenderManager->GetPRenderByEmitter(pEmt);
                    if ( pRnd )
                    {
                        if ( pRnd->IsEnabledControl(KEYFRAME_PAUSE) )
                        {
                            pRnd->Play();
                        }
                    }
                }
                //  **  RECURSIVE UPDATE    **
                if ( pEmt->GetDummyEmitterCount() )
                {
                    RecursiveUpdate(pEmt);
                }
            }
        }
        else
        {
            //  UNREGISTER PARTICLE RENDER OBJECT
            R_ParticleEmitter *pEmt = pSrcEmt->GetDummyEmitterByIndex(i);
            if ( pEmt )
            {
                R_ParticleRender *pRnd = m_pPRenderManager->GetPRenderByEmitter(pEmt);
                if ( pRnd )
                {
                    pRnd->Stop();
                    m_pPRenderManager->UnRegister(pRnd);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void ParticleObject::InternalSetPosition()
{
    for ( unsigned int i=0; i<m_pEmitterManager->GetEmitterCount(); ++i )
    {
        R_ParticleEmitter *pEmt = m_pEmitterManager->GetEmitterByIndex(i);
        if ( pEmt == NULL )
            continue;

        pEmt->SetCameraUCS( GetUCS(m_pd3dDevice) );
        pEmt->ResetUCS();

        if ( pEmt->GetLevel() == 0 )
        {
            R_MATRIX4 mat;
            mat.Identity();
            //  TRANSNLATE TO LOCATOR'S UCS
            mat.SetPosition
            (
                R_VECTOR3
                (
                    this->GetPosition().x + this->GetOffset().x,
                    this->GetPosition().y + this->GetOffset().y,
                    this->GetPosition().z + this->GetOffset().z
                ) 
            );

            // 可在這裡增加旋轉、縮放等

            R_MATRIX4 ucs( pEmt->GetUCS() );
            ucs = ucs * mat;

            pEmt->SetUCS(ucs);

            pEmt->UpdateUCS();

            RecursiveUpdate(pEmt);
        }

    }
}
//---------------------------------------------------------------------------
/* void InternalSetPosition( LPDIRECT3DDEVICE9 pd3dDevice,
    R_EmitterManager* pEmitterManager, float x, float y, float z )
{
    for ( unsigned int i=0; i<pEmitterManager->GetEmitterCount(); ++i )
    {
        R_ParticleEmitter *pEmt = pEmitterManager->GetEmitterByIndex(i);
        if ( pEmt == NULL )
            continue;

        pEmt->SetCameraUCS(GetUCS(pd3dDevice));
        pEmt->ResetUCS();

        if ( pEmt->GetLevel() == 0 )
        {
            R_MATRIX4 mat;
            mat.Identity();
            //  TRANSNLATE TO LOCATOR'S UCS
            mat.SetPosition( R_VECTOR3(x,y,z) );

            // 可在這裡增加旋轉、縮放等

            R_MATRIX4 ucs( pEmt->GetUCS() );
            ucs = ucs * mat;

            pEmt->SetUCS(ucs);

            pEmt->UpdateUCS();

            //RecursiveUpdate(pEmt);
        }

    }
} */

void ParticleObject::Control( const float width , const float height ,
							 float fElapsedTime, LPD3DXMATRIX pWorldMatrix , 
							 const D3DXVECTOR4 &rectRange , bool isMoster , unsigned int &groupCount , unsigned int &outParticCount)
{

	unsigned int updatePartic = 0 ; 

	float particleX = 0.0f ; 
	float particleZ = 0.0f ;
	static float sec = 1.0f ; 
	bool bVisible = false ; 

	if (isMoster)
	{
        //但是怪物是0~512。
		particleX = this->GetPosition().x;
		particleZ = this->GetPosition().z;
	}
	else
	{
        //一般particle的座標系為-255 ~ 255(天地劫)
		particleX = this->GetPosition().x + width  * 0.5f; 
		particleZ = this->GetPosition().z + height * 0.5f; 
	}

	if(rectRange.x < particleX && rectRange.y > particleZ &&
       rectRange.z > particleX && rectRange.w < particleZ)
	{
		bVisible = true ; 
	}


	sec += fElapsedTime ; 

	if(sec >= 1.0f && GetPRenderPoolSize() == 24 && !bVisible)
	{
		D3DXVECTOR4 goViewRect(rectRange.x - 10.0f , 
									rectRange.z + 10.0f , 
									rectRange.x + 10.0f, 
									rectRange.z - 10.0f) ; 


		if(goViewRect.x < particleX && goViewRect.y > particleZ &&
		   goViewRect.z > particleX && goViewRect.w < particleZ )
		{
			if(this->IsControlEnabled(PARTICLE_FLAG::INTERNAL_UPDATE))
			{
				InternalSetPosition();
				//InternalSetPosition(m_pd3dDevice,m_pEmitterManager,m_vPosition.x,m_vPosition.y,m_vPosition.z);
			}

            if (!mControled) {
			    m_pEmitterPlayer->Control( fElapsedTime , outParticCount , updatePartic , false );
                mControled = true;
            }

			m_bFlush = false ; 

			++groupCount; 
					
		}
		sec = 0.0f ; 

	}
	


	if(bVisible)
	{	
		if(this->IsControlEnabled(PARTICLE_FLAG::INTERNAL_UPDATE))
		{
			InternalSetPosition();
			//InternalSetPosition(m_pd3dDevice,m_pEmitterManager,m_vPosition.x,m_vPosition.y,m_vPosition.z);

		}
        if (!mControled) {
		    m_pEmitterPlayer->Control( fElapsedTime , outParticCount , updatePartic ,  true );
            mControled = true;
        }
		
		m_bFlush = true ; 

		++groupCount; 
	}
	else
	{
		m_bFlush = false ;
	}


	


}
//---------------------------------------------------------------------------
void ParticleObject::FrameMoveParticle( float fElapsedTime )
{
/*
    LARGE_INTEGER   currentTime;

	QueryPerformanceCounter(&currentTime);

    float fDeltaTime;

    fDeltaTime = (float)(currentTime.QuadPart - m_liPreviousTime.QuadPart) /
                     (float)(m_liTimerFrequency.QuadPart);

    //  STORE PREVIOUS TIME
    m_liPreviousTime = currentTime;

    m_pEmitterPlayer->Control(fDeltaTime);
*/
	unsigned int partCount = 0 ;
	unsigned int updatepartCount = 0 ; 

    if (!mControled)
        m_pEmitterPlayer->Control(fElapsedTime , partCount , updatepartCount , true);
    mControled = true;

	g_RenderSystem.AddParticleCount(partCount) ; 
	g_RenderSystem.AddUpdateParticleCount(updatepartCount) ;
}
//---------------------------------------------------------------------------
// 渲染分子特效
//---------------------------------------------------------------------------
void ParticleObject::RenderParticle( float fElapsedTime, const D3DXMATRIX& matInvView , bool isUpdate )
{
    //if( !g_FOVClip.MeshFOVCheck( &m_vCenter, g_fRadius ) )
        //return;
    //SetParticleLight( m_pd3dDevice, true );
    for( int i=0; i<numof_ParticleLight; ++i )
        m_pd3dDevice->LightEnable( i, FALSE );
    //m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);

  //  FrameMoveParticle( fElapsedTime ); // 計算時間差，移動分子

	if(isUpdate)
	{
		unsigned int parCount = 0 ; 
		unsigned int updatepartCount = 0 ; 

        if (!mControled)
		    m_pEmitterPlayer->Control( fElapsedTime , parCount ,updatepartCount , true );

		g_RenderSystem.AddParticleCount(parCount) ; 
		g_RenderSystem.AddUpdateParticleCount(updatepartCount) ;

	}
    mControled = true;

    this->Draw();

    //SetParticleLight( m_pd3dDevice, false );
    for( int i=0; i<numof_ParticleLight; ++i )
        m_pd3dDevice->LightEnable( i, TRUE );
    //m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
}

void ParticleObject::UpdateWorldMatrix() const
{
    // 計算縮放
    D3DXMatrixScaling(&mWorldMatrixCache, mScale.x, mScale.y, mScale.z);

    D3DXMATRIX matTemp;
    // 計算旋轉
    D3DXMatrixRotationYawPitchRoll(&matTemp, mRotation.y, mRotation.x, mRotation.z);
    D3DXMatrixMultiply(&mWorldMatrixCache, &mWorldMatrixCache, &matTemp);

    // 計算位移
    D3DXMatrixTranslation
    (
        &matTemp,
        mPosition.x + mOffset.x,
        mPosition.y + mOffset.y,
        mPosition.z + mOffset.z
    );
    D3DXMatrixMultiply(&mWorldMatrixCache, &mWorldMatrixCache, &matTemp);
    // clear dirty flag
    mWorldMatrixOutOfData = false;
}

//---------------------------------------------------------------------------
// 判斷是否播放結束
//---------------------------------------------------------------------------
bool ParticleObject::IsPlayEnd()
{
    return (m_pEmitterPlayer->GetPlayerStatus() == KEYFRAME_STATUS_STOP);
}
//---------------------------------------------------------------------------
// 播放音效
//---------------------------------------------------------------------------
void ParticleObject::PlaySound()   //音效 ***
{
    //if( m_bHaveSound )
        //m_shChannelNum = g_SoundManager._Play3DSoundInMemory( m_strSoundName, m_vPosition );
}
//---------------------------------------------------------------------------
// Stop音效
//---------------------------------------------------------------------------
void ParticleObject::StopSound()   //音效 ***
{
    //if( m_bHaveSound && ( m_shChannelNum != -1 ) )
        //g_SoundManager._Stop3DSound( m_shChannelNum );
}
//---------------------------------------------------------------------------
unsigned short ParticleObject::GetPRenderPoolSize()
{
    if (m_pEmitterManager == NULL)
    {
        return 0;
    }
    return m_pEmitterManager->GetPRenderPoolSize();
}
//--------------------------------------------------------------------------------------
// 播放特效
//--------------------------------------------------------------------------------------
bool ParticleObject::PlayEffect()
{
    F_RETURN(m_pEmitterPlayer != NULL);

    m_pEmitterPlayer->Play();

    return true;
}
//--------------------------------------------------------------------------------------
// 暫停播放
//--------------------------------------------------------------------------------------
bool ParticleObject::PauseEffect()
{
    F_RETURN(m_pEmitterPlayer != NULL);

    m_pEmitterPlayer->Pause();

    return true;
}
//--------------------------------------------------------------------------------------
// 停止播放
//--------------------------------------------------------------------------------------
bool ParticleObject::StopEffect()
{
    F_RETURN(m_pEmitterPlayer != NULL);

    m_pEmitterPlayer->Stop();

    return true;
}

void ParticleObject::UpdateEmtParas()
{
    if (!IsControlEnabled(PARTICLE_FLAG::INTERNAL_UPDATE))
    {
        return;
    }

    R_PRenderManager *pPRenderManager = m_pEmitterPlayer->GetPRenderManager();
    if (pPRenderManager == NULL)
    {
        return;
    }

    _LOOP(pPRenderManager->GetPRenderCount(), i)
    {
        //if( !pPRenderManager->IsPRenderValid(i) )
            //continue;

        R_ParticleRender* pRnd = pPRenderManager->GetPRenderByIndex(i);
        F_CONTINUE(pRnd != NULL);

        R_ParticleEmitter* pEmt = pRnd->GetEmitter();
        F_CONTINUE(pEmt != NULL);

        //pEmt->SetEmitterParameter(m_vEmtPosS.at(i).x+m_vPosition.x,EMITTER_UCS_TRANSLATE_X);
        //pEmt->SetEmitterParameter(m_vEmtPosS.at(i).y+m_vPosition.y,EMITTER_UCS_TRANSLATE_Y);
        //pEmt->SetEmitterParameter(m_vEmtPosS.at(i).z+m_vPosition.z,EMITTER_UCS_TRANSLATE_Z);
        //pEmt->SetEmitterParameter(m_vRotation.x,EMITTER_UCS_ROTATE_X);
        //pEmt->SetEmitterParameter(m_vRotation.y,EMITTER_UCS_ROTATE_Y);
        //pEmt->SetEmitterParameter(m_vRotation.z,EMITTER_UCS_ROTATE_Z);
        pEmt->SetEmitterParameter(m_vEmtSclS[i].x * this->GetScale().x, EMITTER_UCS_SCALE_X);
        pEmt->SetEmitterParameter(m_vEmtSclS[i].y * this->GetScale().y, EMITTER_UCS_SCALE_Y);
        pEmt->SetEmitterParameter(m_vEmtSclS[i].z * this->GetScale().z, EMITTER_UCS_SCALE_Z);
    }
}

void ParticleObject::SetScale(const D3DXVECTOR3& value)
{
    base::SetScale(value);
    this->UpdateEmtParas();
}

void ParticleObject::SetScale(float x, float y, float z)
{
    base::SetScale(x, y, z);
    this->UpdateEmtParas();
}

void ParticleObject::SetFixSoundPosition(const D3DXVECTOR3& value)
{
    m_vFixSoundPosition = value;
}

void ParticleObject::SetFixSoundPosition(float x, float y, float z)
{
    m_vFixSoundPosition.x = x;
    m_vFixSoundPosition.y = y;
    m_vFixSoundPosition.z = z;
}

const D3DXVECTOR3& ParticleObject::GetFixSoundPosition() const
{
    return m_vFixSoundPosition;
}

void ParticleObject::SetUseFixSoundPosition(bool value)
{
    m_bFixSoundPosition = value;
}

bool ParticleObject::IsUseFixSoundPosition() const
{
    return m_bFixSoundPosition;
}

void ParticleObject::SaveEmtPara()
{
    R_PRenderManager *pPRenderManager = m_pEmitterPlayer->GetPRenderManager();
    if (pPRenderManager == NULL)
    {
		assert(0 && "ParticleObject::SaveEmtPara ,pPRenderManager == NULL");
        return;
    }

    m_vEmtSclS.resize(pPRenderManager->GetPRenderCount(), VEC3_UNIT_SCALE);
    m_vEmtPosS.resize(pPRenderManager->GetPRenderCount(), VEC3_ZERO);
    m_vEmtRotS.resize(pPRenderManager->GetPRenderCount(), VEC3_ZERO);

    _LOOP(pPRenderManager->GetPRenderCount(), i)
    {
        R_ParticleRender* pRnd = pPRenderManager->GetPRenderByIndex(i);
        F_CONTINUE(pRnd != NULL);

        R_ParticleEmitter* pEmt = pRnd->GetEmitter();
        F_CONTINUE(pEmt != NULL);
        // scale
        m_vEmtSclS[i].x = pEmt->GetEmitterParameter(EMITTER_UCS_SCALE_X);
        m_vEmtSclS[i].y = pEmt->GetEmitterParameter(EMITTER_UCS_SCALE_Y);
        m_vEmtSclS[i].z = pEmt->GetEmitterParameter(EMITTER_UCS_SCALE_Z);
        // position
        m_vEmtPosS[i].x = pEmt->GetEmitterParameter(EMITTER_UCS_TRANSLATE_X);
        m_vEmtPosS[i].y = pEmt->GetEmitterParameter(EMITTER_UCS_TRANSLATE_Y);
        m_vEmtPosS[i].z = pEmt->GetEmitterParameter(EMITTER_UCS_TRANSLATE_Z);
        // rotation
        m_vEmtRotS[i].x = pEmt->GetEmitterParameter(EMITTER_UCS_ROTATE_X);
        m_vEmtRotS[i].y = pEmt->GetEmitterParameter(EMITTER_UCS_ROTATE_Y);
        m_vEmtRotS[i].z = pEmt->GetEmitterParameter(EMITTER_UCS_ROTATE_Z);
    }
}

void ParticleObject::ClearCaches()
{
	sm_MemDataCacheMgr.RemoveAllMemData();
}

bool ParticleObject::Update(const UpdateArgsPtr &args)
{
    mElapsedTime += args->GetElapsedTime();

    //一般particle的座標系為-255 ~ 255(天地劫)
    float particleX = this->GetPosition().x + args->GetWidth()  * 0.5f;
    float particleZ = this->GetPosition().z + args->GetHeight() * 0.5f;

    // set visible
    if (this->IsControlEnabled(PARTICLE_FLAG::FORCE_VISIBLE)) {    
        this->SetVisible(true);		
    }
    else {
        //if (isMoster)
        //{
        //    //但是怪物是0~512。
        //    particleX = this->GetPosition().x;
        //    particleZ = this->GetPosition().z;
        //}
        //else
        //{
        //    //一般particle的座標系為-255 ~ 255(天地劫)
        //    particleX = this->GetPosition().x + args->GetWidth()  * 0.5f;
        //    particleZ = this->GetPosition().z + args->GetHeight() * 0.5f;
        //}
        if (this->IsControlEnabled(PARTICLE_FLAG::FAR_VISIBLE))        
            this->SetVisible(args->IsInGoViewRect(particleX, particleZ));        
        else        
            this->SetVisible(args->IsInRectRange(particleX, particleZ));        
    }

    if (this->IsVisible()) {
        if (this->IsControlEnabled(PARTICLE_FLAG::INTERNAL_UPDATE))    
            InternalSetPosition();
        // frame move
        unsigned int outParticCount = 0; // what's this??
        unsigned int updatepartCount = 0; // what's this??

        if (!mControled) {
            /// <yuming> set elapsed time        
            //m_pEmitterPlayer->Control(mElapsedTime, outParticCount, updatepartCount , true);
            m_pEmitterPlayer->Control(args->GetElapsedTime(), outParticCount, updatepartCount , true);
            /// </yuming>
        }

	    g_RenderSystem.AddParticleCount(outParticCount);
	    g_RenderSystem.AddUpdateParticleCount(updatepartCount);
    }

    // clear elapsed time
    mControled = true;
    mElapsedTime = 0.f;
    return true;
}

void ParticleObject::RegisterForDrawing(SCNPtr scn)
{
    if (this->IsVisible())
    {
        scn->RegisterForDrawing(DRAWKIND::PARTICLE, this);
        m_bFlush = true;
    }
    mControled = false;
}

bool ParticleObject::IsRayIntersect(const RAY& ray, HitInfo& res) const
{
    return false;
}

void ParticleObject::DrawHelpers()
{
}

//void ParticleObject::UpdateBox() const
//{
//}

bool ParticleObject::Draw()
{
	mControled = false;

	if(m_bFlush)
	{
		unsigned int count = 0 ; 

	   FDO::RenderParticle(this , count);
	   g_RenderSystem.AddFlushParticleCount(count) ; 

	   return true ; 
	}

	return false ; 

}

R_PRenderManagerPtr ParticleObject::GetPRenderManager() const
{
    return m_pEmitterPlayer->GetPRenderManager();
}

ParticleSystemPtr ParticleObject::GetParticleSystem() const
{
    return GetParticleSystemPtr();
}

LPDIRECT3DDEVICE9 ParticleObject::GetD3DDevice() const
{
    return m_pd3dDevice;
}

R_EmitterManagerPtr ParticleObject::GetEmitterManager() const
{
    return m_pEmitterManager;
}

void ParticleObject::CleanUp()
{
    base::CleanUp();

    m_vFixSoundPosition = VEC3_ZERO;
    m_bFixSoundPosition = false;
    m_bControl = false;
    m_bFlush = true;

    mTextureMap.clear();
    mMeshMap.clear();

    SafeDelete(m_pEmitterPlayer);
    // 反註冊EmitterManager物件
    R_3DInterfacePtr p3DInterface = this->GetParticleSystem()->GetInterfacePtr();
    if ((p3DInterface != NULL) && (m_pEmitterManager != NULL))
    {
        p3DInterface->UnregisterObject(I3D_EMITTERMANAGER, m_pEmitterManager);
        m_pEmitterManager = NULL;
    }
}

LPDIRECT3DTEXTURE9 ParticleObject::GetTexture(const fstring& fileName,
                                              bool staticResource)
{
    //TextureMapIterator pos = mTextureMap.find(fileName);
    //if (pos != mTextureMap.end())
    //{
    //    return pos->second;
    //}
    // create new texture
    LPDIRECT3DTEXTURE9 Result = this->GetParticleSystem()->GetTexture(fileName, 
        this->IsControlEnabled(PARTICLE_FLAG::STATIC_RESOURCE));

    //mTextureMap[fileName] = Result;

    return Result;
}

LPD3DXMESH ParticleObject::GetMesh(const fstring& fileName, bool staticResource)
{
    //MeshMapIterator pos = mMeshMap.find(fileName);
    //if (pos != mMeshMap.end())
    //{
    //    return pos->second;
    //}
    // create new mesh
    LPD3DXMESH Result = this->GetParticleSystem()->GetMesh(fileName, 
        this->IsControlEnabled(PARTICLE_FLAG::STATIC_RESOURCE));

   // mMeshMap[fileName] = Result;

    return Result;
}
//---------------------------------------------------------------------------
} // namespace FDO
