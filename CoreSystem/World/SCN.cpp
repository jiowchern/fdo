#include "stdafx.h"
//---------------------------------------------------------------------------
#include "PacketReader.h"
#include "PhenixD3D9RenderSystem.h"
#include "TerTimer.h" // for GetServerTime
#include "Timer.h" // 計時器 for g_Timer.GetfTimeDelta()
#include "FOVClip.h" // 重新建立裁切範圍

#pragma hdrstop
#include "SCN.h"

/// <yuming> include header file
#include "FdoUpdateArgs.h"
#include "ResourceManager.h"
#include "JLSkinMesh.h" // mesh template
#include "HModel.h" // mesh instance
#include "Ocean.h" // 海水
#include "terrain.h" // 地表
#include "ParticleSystem.h"
#include "ParticleTemplate.h" // particle template
#include "FDO_ParticleObject.h" // particle instance
#include "FdoCollisionMap.h" // collision map
#include "FdoAltitudeMap.h" // altitude map
/// </yuming>

//////////////////////								////////////////////////////

#ifdef RESMGR
#pragma message("管理模式啟動") 
#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
namespace FDO
{
bool SCN::sm_bEnableTransparent = true;
SCN::SCNFactory SCN::sSCNFactory;

/**
 * @brief default constructor
 */
SCN::SCN()
: m_pd3dDevice(g_RenderSystem.GetD3DDevice())
, m_iCellSpacing(CELL_SPACING)
, mUpdateArgs(new CUpdateArgs())
, mCollisionMap(new CCollisionMap())
, mAltitudeMap(new CAltitudeMap())
, m_pTerrain(NULL)
, mPassedTime(0.f)
{
    Quick_time = -1;
    m_fSceneAlpha = 0.2f;

    m_Dir[MORNING]  = D3DXVECTOR3(0.0f,0.577f,1.0f);
    m_Dir[EVENING]  = D3DXVECTOR3(0.0f,0.577f,1.0f);
    m_Dir[NIGHT]    = D3DXVECTOR3(0.0f,0.577f,1.0f);
    m_Dir[EARLY]    = D3DXVECTOR3(0.0f,0.577f,1.0f);

    m_ColorR[MORNING] = 213;
    m_ColorG[MORNING] = 225;
    m_ColorB[MORNING] = 224;
    m_ColorR[EVENING] = 200;
    m_ColorG[EVENING] = 200;
    m_ColorB[EVENING] = 200;
    m_ColorR[NIGHT] = 182;
    m_ColorG[NIGHT] = 190;
    m_ColorB[NIGHT] = 210;
    m_ColorR[EARLY] = 200;
    m_ColorG[EARLY] = 200;
    m_ColorB[EARLY] = 100;
}
/**
 * @brief default destructor
 */
SCN::~SCN()
{
    this->CleanUp();
}
/**
 *
 */
bool SCN::InitializeHeader(const BYTE* data)
{
    // copy scene header
    memcpy(&m_SCN_Header, data, sizeof_SCN_Header);
    // confirm SCN file format
    F_RETURN
    (
        (!strcmp(m_SCN_Header.ID, "SCN\x01")) ||
        (!strcmp(m_SCN_Header.ID, "SCN\x02")) ||       
        (!strcmp(m_SCN_Header.ID, "SCN\x03")) ||
        (!strcmp(m_SCN_Header.ID, "SCN\x04")) ||      
        (!strcmp(m_SCN_Header.ID, "SCN\x05"))
    )
    // copy map information
    memcpy(&m_SCN_MapNfo, data + m_SCN_Header.MapInformationOffset, sizeof_SCN_MapNfo);
    g_RenderSystem.SetFogFlag(m_SCN_MapNfo.FogEnable);
    g_RenderSystem.SetFogColor(m_SCN_MapNfo.FogColor);
    g_RenderSystem.SetFogStart(*((DWORD*)(&m_SCN_MapNfo.FogStart)));
    g_RenderSystem.SetFogEnd(*((DWORD*)(&m_SCN_MapNfo.FogEnd)));

    return true;
}
//---------------------------------------------------------------------------
void* SCN::operator new (size_t)
{
    return sSCNFactory.Create();
}
//---------------------------------------------------------------------------
void SCN::operator delete (void* p)
{
    if (p != NULL)
    {
        sSCNFactory.Destroy(p);
    }
}
//---------------------------------------------------------------------------
bool SCN::Create(const BYTE* data, UINT size)
{
    this->CleanUp();

    F_RETURN(this->InitializeHeader(data));
	F_RETURN(this->loadCollisionData(data));
	F_RETURN(this->loadAltitudeData(data));
	F_RETURN(this->loadObjectData(data));
	F_RETURN(this->loadObjectExData(data));
	F_RETURN(this->loadParticleData(data));
	F_RETURN(this->generateTerrain());
	F_RETURN(this->generateModel());
	F_RETURN(this->generateOcean(data));
	F_RETURN(this->generateParticle());

    //setPosition( fX, fZ ); // 位置
    setClearColor(); // 設定背景清除顏色
    setClipField(); // 設定裁切範圍

    //設定server時間
    tm* TmpTime;
    time_t now = GetServerTime();
    TmpTime = localtime(&now); // 取得伺服器時間
    memcpy(&Server_time, (tm*)TmpTime, sizeof(tm));

    //long time_temp = g_Timer.GetServerTime();
    //Server_time = *(localtime(&time_temp));
    Quick_time = -1;

    return true;
}
//---------------------------------------------------------------------------
// 碰撞層
//---------------------------------------------------------------------------
bool SCN::loadCollisionData(const BYTE* data)
{
    mCollisionMap->Initialize(m_SCN_Header, m_SCN_MapNfo, data);
    return true;
}

//---------------------------------------------------------------------------
bool SCN::loadAltitudeData(const BYTE* data)
{
    mAltitudeMap->Initialize(m_SCN_Header, m_SCN_MapNfo, data);
    return true;
}
//---------------------------------------------------------------------------
bool SCN::loadObjectData(const BYTE* data)
{
    const SCN_ObjInforPtr p = (SCN_ObjInforPtr)(data + m_SCN_Header.ObjInformationIndexOffset);
    m_ObjectNfo.assign(p, p + m_SCN_Header.ObjCount);
    return true;
}
//---------------------------------------------------------------------------
// 讀取特殊物件
//---------------------------------------------------------------------------
bool SCN::loadObjectExData(const BYTE* data)
{
    const unsigned int* p = (unsigned int*)(data + m_SCN_Header.ExternObjInfotmationIndexOffset);
    std::vector<unsigned int> objectEXIndex(p, p + m_SCN_Header.ExternObjCount);

    m_ObjectEXData.resize(objectEXIndex.size());
    _ULOOP(m_ObjectEXData.size(), pos)
    {
        memcpy
        (
            &m_ObjectEXData[pos],
            data + objectEXIndex[pos],
            sizeof_SCN_ExternObj
        );
    }

    return true;
}
//---------------------------------------------------------------------------
bool SCN::loadParticleData(const BYTE *data)
{
    T_RETURN(m_SCN_Header.Version < 3);

    const SCN_PCLInforPtr p = (SCN_PCLInforPtr)(data + m_SCN_Header.PCLOffset);
    m_ParticleNfo.assign(p, p + m_SCN_Header.PCLCount);

    return true;
}
//---------------------------------------------------------------------------
bool SCN::generateTerrain()
{
    F_RETURN(m_pTerrain == NULL)
    m_pTerrain = new Terrain(m_pd3dDevice);
    F_RETURN(m_pTerrain != NULL);

    m_pTerrain->SetCellSpacing(m_SCN_MapNfo.vRatio);
    m_pTerrain->SetHeightSpacing(m_SCN_MapNfo.hRatio);

    memPack *m = LoadFromPacket(mMapPath + m_SCN_MapNfo.BWMapName);
	F_RETURN((m != NULL) && (m->Data != NULL) && (m->DataSize != 0));
    m_pTerrain->LoadRawFileInMemory(m->Data, m->DataSize);

    //if (strlen(m_SCN_MapNfo.Text0Name) > 0)
    //{ // 第一層材質
    //    memPack* m = LoadFromPacket(mMapPath + m_SCN_MapNfo.Text0Name);
    //    if ((m != NULL) && (m->Data != NULL) && (m->DataSize != 0))
    //    {
    //        //m_pTerrain->loadTexture0(m->Data, m->DataSize);
    //    }
    //}

    //if (strlen(m_SCN_MapNfo.Text1Name) > 0)
    //{ // 第二層材質
    //    memPack* m = LoadFromPacket(mMapPath + m_SCN_MapNfo.Text1Name);
    //    if ((m != NULL) && (m->Data != NULL) && (m->DataSize != 0))
    //    {
    //        //m_pTerrain->loadTexture1(m->Data, m->DataSize);
    //    }
    //}

    // 傳入地形去處理
    m_pTerrain->SetCollisionMap(mCollisionMap);
    m_pTerrain->SetAltitudeMap(mAltitudeMap);

    mSceneItems.push_back(m_pTerrain);

    return true;
}

//---------------------------------------------------------------------------
bool SCN::generateModel()
{
	

    _ULOOP(m_ObjectNfo.size(), pos)
	{
		const SCN_ObjInfor& ObjNfo = m_ObjectNfo.at(pos);

        const fstring name(_F(ObjNfo.ObjFileName));
        F_CONTINUE(!name.empty());

        const fstring fileName(mModelPath + name);
#ifdef _DEBUG
		fstring filter01("1OS244b.X") ; 
		fstring filter02("1OS244c.X") ; 
		fstring filter03("1OS244e.X") ; 
		fstring filter04("1OS244f.X") ; 


		if(name == filter01 || name == filter02 || name == filter03 ||
			name == filter04)
		{
			continue ; 
		}
#endif

        SkinMeshTempPtr skinMeshTemp(LoadSkinMesh(fileName.c_str()));
        F_CONTINUE(skinMeshTemp != NULL);

        SkinMeshInstPtr skinMeshInst = skinMeshTemp->CreateInstance();
        F_CONTINUE(skinMeshInst != NULL);

        skinMeshInst->SetOffset(mPosition);
        // 設定位置
        skinMeshInst->SetPosition(ObjNfo.PosX, ObjNfo.PosY, ObjNfo.PosZ);
        // 設定旋轉
        skinMeshInst->SetRotation(ObjNfo.RotX, ObjNfo.RotY, ObjNfo.RotZ);
        // 設定縮放
        skinMeshInst->SetScale(ObjNfo.ScalX, ObjNfo.ScalY, ObjNfo.ScalZ);

        skinMeshInst->SetAlphaDest(this->GetAlphaDest(name));

        skinMeshInst->SetControlEnabled(MODEL_FLAG::TERRAIN, this->IsTerrain(name));
        //if (skinMeshInst->IsControlEnabled(MODEL_FLAG::TERRAIN))
        //{
        //    skinMeshInst->SetAlphaDest(ALPHA_OPAQUE);
        //}

        skinMeshInst->SetControlEnabled(MODEL_FLAG::ALWAYS_TRANSPARENT, this->IsNativeTransparent(name));
        //if (skinMeshInst->IsControlEnabled(MODEL_FLAG::ALWAYS_TRANSPARENT))
        //{
        //    skinMeshInst->SetAlphaDest(ALPHA_OPAQUE);
        //}

        skinMeshInst->SetControlEnabled(MODEL_FLAG::WITHOUT_ALPHATEST, this->IsWithoutAlphaTest(name));        

        mSceneItems.push_back(skinMeshInst);
	}
    m_ObjectNfo.clear();
    return true;
}

bool SCN::IsWithoutAlphaTest(const fstring& fileName) const
{
    bool result = false;

    if ((fileName[0] == 'X') || (fileName[0] == 'x'))
        result = true;

    return result;
}

bool SCN::IsNativeTransparent(const fstring& fileName) const
{
    _FOREACH_CONST(FStringList, mNativeTransparentNames, pos)
    {
        if (strnicmp((*pos).c_str(), fileName.c_str(), (*pos).size()) == 0)
        {
            return true;
        }
    }
    return false;
}

bool SCN::IsTerrain(const fstring& fileName) const
{
    _FOREACH_CONST(FStringList, mTerrainNames, pos)
    {
        if (strnicmp((*pos).c_str(), fileName.c_str(), (*pos).size()) == 0)
        {
            return true;
        }
    }
    return false;
}

float SCN::GetAlphaDest(const fstring& fileName) const
{
    _FOREACH_CONST(FStringList, mOpaqueNames, pos)
    {
        if (strnicmp((*pos).c_str(), fileName.c_str(), (*pos).size()) == 0)
        {
            return ALPHA_OPAQUE;
        }
    }

    _FOREACH_CONST(FStringList, mTerrainNames, pos)
    {
        if (strnicmp((*pos).c_str(), fileName.c_str(), (*pos).size()) == 0)
        {
            return ALPHA_OPAQUE;
        }
    }

    _FOREACH_CONST(FStringList, mNativeTransparentNames, pos)
    {
        if (strnicmp((*pos).c_str(), fileName.c_str(), (*pos).size()) == 0)
        {
            return ALPHA_OPAQUE;
        }
    }

    _FOREACH_CONST(FStringList, mTransparentNames, pos)
    {
        if (strnicmp((*pos).c_str(), fileName.c_str(), (*pos).size()) == 0)
        {
            return ALPHA_TRANSPARENT;
        }
    }

    return m_fSceneAlpha;
}
//---------------------------------------------------------------------------
bool SCN::generateParticle()
{
    _ULOOP(m_ParticleNfo.size(), pos)
    {
        const SCN_PCLInfor& PTCNfo = m_ParticleNfo.at(pos);
		// confirm filename
        const fstring name(_F(PTCNfo.PCLFileName));
        F_CONTINUE(!name.empty());
		// get partile filename(*.emt)
        const fstring fileName(mParticlePath + name);

        ParticleTempPtr particleTemp = GetParticleSystem().GetParticleTemp(fileName);
        F_CONTINUE(particleTemp != NULL);

        ParticleInstPtr particleInst = particleTemp->CreateInstance();
        F_CONTINUE(particleInst != NULL);

        particleInst->SetOffset(mPosition);
        particleInst->SetPosition(PTCNfo.PosX, PTCNfo.PosY, PTCNfo.PosZ); // 設定位置
        particleInst->SetRotation(PTCNfo.RotX, PTCNfo.RotY, PTCNfo.RotZ); // 旋轉
        particleInst->SetScale(PTCNfo.ScalX, PTCNfo.ScalY, PTCNfo.ScalZ); // 縮放
        particleInst->PlayEffect();

        particleInst->SetUseFixSoundPosition(true);
        particleInst->SetFixSoundPosition
        (
            PTCNfo.PosX + (m_SCN_MapNfo.MapWidth / 2.f),
            PTCNfo.PosY,
            PTCNfo.PosZ + (m_SCN_MapNfo.MapHeight / 2.f)
        ); // 設定音效位置

        mSceneItems.push_back(particleInst);
    }
    m_ParticleNfo.clear();
    return true;
}

//---------------------------------------------------------------------------
// 產生沿伸物件：通常是海水
//---------------------------------------------------------------------------
bool SCN::generateOcean(const BYTE* data)
{
    _ULOOP(m_ObjectEXData.size(), i)
    {
        OceanPtr ocean = new OCEAN();
        F_CONTINUE(ocean != NULL);

        const SCN_ExternObj& ObjectEX = m_ObjectEXData[i];

        ocean->SetDevice(m_pd3dDevice);
        // 設定縮放
        ocean->SetScale(ObjectEX.Width, 1.f, ObjectEX.Height);
        // 設定位置
        ocean->SetPosition(ObjectEX.PosX, ObjectEX.PosY, ObjectEX.PosZ);
        // 設定位移
        ocean->SetOffset(mPosition);
        // 設定旋轉
        if(m_SCN_Header.Version >= 5)
        {
            ocean->SetRotation(ObjectEX.RotX, ObjectEX.RotY, ObjectEX.RotZ);
        }
        // 設定頂點數
        ocean->SetNumVertsPerRow(ObjectEX.VertexWCount);
        // 設定頂點數
        ocean->SetNumVertsPerCol(ObjectEX.VertexHCount);
        // 設定振幅
        ocean->SetAmplitude(ObjectEX.Amplitude);
        // 設定頻率
        ocean->SetInterval(ObjectEX.AmplitudeInterval);
        // 設定材質軸U
        ocean->SetTU(ObjectEX.TextureTU);
        // 設定材質軸V
        ocean->SetTV(ObjectEX.TextureTV);
        // 設定材質變換速度
        ocean->SetTextureSpeed(ObjectEX.TextureSpeed);
        // 設定顏色
        ocean->SetColor(ObjectEX.ObjColor);
        // 設定材質數量
		ocean->SetTexCount(static_cast<int>(ObjectEX.TextureCount));
        // 取得材質資料
        TextureNameList textures(ocean->GetTexCount());
        memcpy
        (
            &textures.front(), 
            data + ObjectEX.TextureFileNameOffset, 
            textures.size() * sizeof_SCN_EObjTexture
        );

        _ULOOP(textures.size(), j)
        {
            const SCN_EObjTexture& EOT = textures[j];
            const fstring name(EOT.Texture);
            F_CONTINUE(!name.empty());

            ocean->SetTexture(j, this->GetOceanTexture(name));
        }

        mSceneItems.push_back(ocean);
    }
    m_ObjectEXData.clear();
    return true;
}

LPDIRECT3DTEXTURE9 SCN::GetOceanTexture(const fstring& name)
{
    // 設定材質
    typedef FDOCh TEXTURE_PATH[20];

    const TEXTURE_PATH pathList[] =                
    {
        "Texture\\WATER\\",
        "Texture\\WATER_b\\",
        "Texture\\LAVA2\\",
        "Texture\\LAVA2_b\\",
        "Texture\\Mask\\",
        "Texture\\OPSKY\\",
        "Texture\\sky\\",
        "Texture\\Dn01a\\",
        "Texture\\sky_b\\"
    };
    const UINT pathCount = sizeof(pathList) / sizeof(pathList[0]);

    _ULOOP(pathCount, pos)
    {
        const fstring path(pathList[pos]);
        const fstring fileName(path + name);

        LPDIRECT3DTEXTURE9 texture = LoadParticleTexture(fileName);
        if (texture != NULL)
        {
            return texture;
        }
    }

    return NULL;
}

void SCN::SetPosition(float x, float y, float z)
{
    mPosition.x = x;
    mPosition.y = y;
    mPosition.z = z;
    mWorldMatrixOutOfData = true;

	// set models world matrix
   // _FOREACH(DRAWLIST, mSceneItems, pos)
	//{

	//	(*pos)->SetOffset(mPosition.x, 0.f, mPosition.z);
	//}

	_ULOOP(mSceneItems.size() , i)
	{
		mSceneItems[i]->SetOffset(mPosition.x, 0.f, mPosition.z);

	}
}
//---------------------------------------------------------------------------
const D3DXMATRIX& SCN::GetWorldMatrix() const
{
    if (mWorldMatrixOutOfData)
    {
        D3DXMatrixTranslation
        (
            &mWorldMatrixCache, 
            mPosition.x, 
            mPosition.y, 
            mPosition.z
        );
        mWorldMatrixOutOfData = false;
    }
    return mWorldMatrixCache;
}
//---------------------------------------------------------------------------
void SCN::setRenderModelState( bool bSet )
{
	if( bSet )
	{
		//m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, m_SCN_MapNfo.FogEnable ); // 開啟霧
        //m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, m_SCN_MapNfo.FogColor ); // Set the fog color.
        //m_pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD*)(&m_SCN_MapNfo.FogStart )));
        //m_pd3dDevice->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&m_SCN_MapNfo.FogEnd )));
		//m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );//D3DFOG_LINEAR
		//m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		//m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, 0 );
        g_RenderSystem.SetFogEnable( m_SCN_MapNfo.FogEnable );

		// 取得模型繪圖模式
		//DWORD dwZBuf, dwAmbient, dwLight, dwSpec;
		//m_pd3dDevice->GetRenderState( D3DRS_ZENABLE, &dwZBuf );
		m_pd3dDevice->GetRenderState( D3DRS_AMBIENT, &m_dwAmbient );
		m_pd3dDevice->GetRenderState( D3DRS_LIGHTING, &m_dwLight );
		//m_pd3dDevice->GetRenderState( D3DRS_SPECULARENABLE, &dwSpec );
		// 設定模型繪圖模式
		//m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); // 開啟ZBuffer
		m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(80,80,80) );
		// 如果關閉燈光旗標，只顯示白天燈光
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, m_bLightEnabled ); //關閉光源
		//m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, true );

        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &m_dwColorOP );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1, &m_dwColorARG1 );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &m_dwColorARG2 );

        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
        //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

		// Alpha Test
		//DWORD dwAlphaREF, dwAlphaTest, dwAlphaFunc;
		m_pd3dDevice->GetRenderState( D3DRS_ALPHAREF, &m_dwAlphaREF );
		m_pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &m_dwAlphaTest );
		m_pd3dDevice->GetRenderState( D3DRS_ALPHAFUNC, &m_dwAlphaFunc );

		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, (DWORD)0x00000050 );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, true );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, m_dwCullMode ); // 關閉雙面繪製
	}
	else
	{
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW ); // 關閉雙面繪製

		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, m_dwAlphaREF );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, m_dwAlphaTest );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, m_dwAlphaFunc );

        // 還原之前的材質狀態
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, m_dwColorOP );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, m_dwColorARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, m_dwColorARG2 );

		// 還原繪圖設定值
		//m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, m_dwZBuf );
		m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, m_dwAmbient );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, m_dwLight );
		//m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, m_dwSpec );

		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); // 關閉霧
	}
}
void SCN::setRenderOceanState( bool bSet )
{
    int i = 0;
    while( i < 7 )
    {
        m_pd3dDevice->LightEnable(i, !bSet);
        ++i;
    }
    m_pd3dDevice->LightEnable(i, bSet);

	if( bSet )
	{
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP, &m_dwAlphaOP );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &m_dwAlphaARG1 );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &m_dwAlphaARG2 );
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &m_dwColorOP );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1, &m_dwColorARG1 );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &m_dwColorARG2 );

        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_BLENDDIFFUSEALPHA );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,D3DTA_TEMP );

		//m_pd3dDevice->GetRenderState( D3DRS_AMBIENT, &m_dwAmbient ); 
		//m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(0,0,0) );

        //m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, m_SCN_MapNfo.FogEnable ); // 開啟霧
        //m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, m_SCN_MapNfo.FogColor ); // Set the fog color.
        //m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *((DWORD*)(&m_SCN_MapNfo.FogStart)));
        //m_pd3dDevice->SetRenderState(D3DRS_FOGEND, *((DWORD*)(&m_SCN_MapNfo.FogEnd )));
        //m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );//D3DFOG_LINEAR
        //m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
        //m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, 0 );
        g_RenderSystem.SetFogEnable( m_SCN_MapNfo.FogEnable );

        D3DXCOLOR color = D3DCOLOR_XRGB( 63, 55, 48 );// 燈的顏色
        D3DLIGHT9 light;
        memset( &light, 0,  sizeof(D3DLIGHT9) );
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color;
        light.Diffuse   = color;
        light.Specular  = color;
        light.Position  = D3DXVECTOR3( 0.0f, 50.0f, 0.0f );
        light.Direction = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
        light.Range     = 1000.0f;
        m_pd3dDevice->SetLight(i, &light);
	}
	else
	{
		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); // 關閉霧
		//m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, m_dwAmbient );

        // 還原之前的材質狀態		
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, m_dwAlphaOP );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, m_dwAlphaARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, m_dwAlphaARG2 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, m_dwColorOP );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, m_dwColorARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, m_dwColorARG2 );
	}
}
void SCN::setRenderTerrainState( bool bSet )
{
    if( bSet )
    {
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP  , &m_dwColorOP     );
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1, &m_dwColorARG1   );
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &m_dwColorARG2   );
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP  , &m_dwAlphaOP     );
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &m_dwAlphaARG1   );
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &m_dwAlphaARG2   );
        m_pd3dDevice->GetTextureStageState( 1, D3DTSS_COLOROP  , &m_dwColorOP_1   );
        m_pd3dDevice->GetTextureStageState( 1, D3DTSS_COLORARG1, &m_dwColorARG1_1 );
        m_pd3dDevice->GetTextureStageState( 1, D3DTSS_COLORARG2, &m_dwColorARG2_1 );
        m_pd3dDevice->GetTextureStageState( 1, D3DTSS_ALPHAOP  , &m_dwAlphaOP_1   );
        m_pd3dDevice->GetTextureStageState( 1, D3DTSS_ALPHAARG1, &m_dwAlphaARG1_1 );
        m_pd3dDevice->GetTextureStageState( 1, D3DTSS_ALPHAARG2, &m_dwAlphaARG2_1 );

        // 設定地表繪製狀態    
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP  , m_SCN_MapNfo.Text0ColOp   );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, m_SCN_MapNfo.Text0ColArg1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, m_SCN_MapNfo.Text0ColArg2 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP  , m_SCN_MapNfo.Text0AlpOp   );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, m_SCN_MapNfo.Text0AlpArg1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, m_SCN_MapNfo.Text0AlpArg2 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP  , m_SCN_MapNfo.Text1ColOp   );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, m_SCN_MapNfo.Text1ColArg1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, m_SCN_MapNfo.Text1ColArg2 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP  , m_SCN_MapNfo.Text1AlpOp   );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, m_SCN_MapNfo.Text1AlpArg1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, m_SCN_MapNfo.Text1AlpArg2 );    

        // 設定霧
        //m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, m_SCN_MapNfo.FogEnable); // Enable fog blending.
        //m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR,  m_SCN_MapNfo.FogColor ); // Set the fog color.
        //m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_LINEAR);
        //m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *((DWORD*)(&m_SCN_MapNfo.FogStart)));
        //m_pd3dDevice->SetRenderState(D3DRS_FOGEND,   *((DWORD*)(&m_SCN_MapNfo.FogEnd  )));
        g_RenderSystem.SetFogEnable( m_SCN_MapNfo.FogEnable );
    }
    else
    {
        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); // 關閉霧

        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP  , m_dwColorOP     );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, m_dwColorARG1   );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, m_dwColorARG2   );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP  , m_dwAlphaOP     );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, m_dwAlphaARG1   );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, m_dwAlphaARG2   );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP  , m_dwColorOP_1   );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, m_dwColorARG1_1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, m_dwColorARG2_1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP  , m_dwAlphaOP_1   );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, m_dwAlphaARG1_1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, m_dwAlphaARG2_1 );
    }
}
/**
 * @brief 更新地圖上所有建物距離攝影機的距離，然後作排序
 *        在人物後面的建物不做透明判斷
 */
bool DrawableSort(DrawablePtr lhs, DrawablePtr rhs)
{ 
	return (lhs->GetSqDistance() < rhs->GetSqDistance()); 
}
/**
 * @brief render model object
 */
bool SCN::FlushModel()
{
    T_RETURN(mDrawList[DRAWKIND::MODEL].empty());

    if (mSortModel == TRUE)
    {
      //  mDrawList[DRAWKIND::MODEL].sort(DrawableSort);

		std::sort(mDrawList[DRAWKIND::MODEL].begin() , mDrawList[DRAWKIND::MODEL].end() , DrawableSort) ; 
    }

	// render model object
    setRenderModelState(true);
 //   _FOREACH(DRAWLIST, mDrawList[DRAWKIND::MODEL], pos)
	//{
	//	(*pos)->Draw();
	//}

    DWORD preAlphaRef;
    m_pd3dDevice->GetRenderState(D3DRS_ALPHAREF, &preAlphaRef);

	_ULOOP(mDrawList[DRAWKIND::MODEL].size() , i)
	{
        DWORD currAlphaRef = 0x00000050;
        if (mDrawList[DRAWKIND::MODEL][i]->IsControlEnabled(MODEL_FLAG::WITHOUT_ALPHATEST)) 
            currAlphaRef = 0x00000001;        

        if (currAlphaRef != preAlphaRef) {
            m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, currAlphaRef);
            preAlphaRef = currAlphaRef;
        }
		mDrawList[DRAWKIND::MODEL][i]->Draw();
	}
    setRenderModelState(false);

    mDrawList[DRAWKIND::MODEL].clear();
    return true;
}

void SCN::SetShowHelpers(bool value)
{
    mShowHelpers = value;
}

void SCN::FlushHelpers()
{
    if (mShowHelpers)
    {
        //_FOREACH(DRAWLIST, mSceneItems, pos)
        //{
        //    (*pos)->DrawHelpers(this);
        //}
		_ULOOP(mSceneItems.size() , i)
		{
			mSceneItems[i]->DrawHelpers(this);

		}
    }
}

void SCN::FlushLines()
{
    if (!mImmediateLines.empty())
    {
        m_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &m_dwLight);
        m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE); //關閉光源

	    D3DXMATRIX mat;
	    D3DXMatrixIdentity(&mat);
        m_pd3dDevice->SetTransform(D3DTS_WORLD, &mat);
        m_pd3dDevice->SetFVF(fvfof_ImmediateVertex);
        m_pd3dDevice->SetTexture(0, NULL);
        m_pd3dDevice->DrawPrimitiveUP
        (
            D3DPT_LINELIST,
            (UINT)mImmediateLines.size() / 2,
            &mImmediateLines.front(),
            sizeof_ImmediateVertex
        );
        mImmediateLines.clear();
        m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, m_dwLight);
    }
}

void SCN::DrawLine(const Line& line, D3DCOLOR color)
{
    ImmediateVertex v;

    v.position = line.pts[0];
    v.color = color;
    v.uv.x = 0.f;
    v.uv.y = 0.f;
    mImmediateLines.push_back(v);

    v.position = line.pts[1];
    v.color = color;
    v.uv.x = 1.f;
    v.uv.y = 1.f;
	mImmediateLines.push_back(v);
}

void SCN::DrawBox(const Box& box, D3DCOLOR color)
{
	this->DrawLine(Line(box.pts[0], box.pts[1]), color);
	this->DrawLine(Line(box.pts[1], box.pts[2]), color);
	this->DrawLine(Line(box.pts[2], box.pts[3]), color);
	this->DrawLine(Line(box.pts[3], box.pts[0]), color);

	this->DrawLine(Line(box.pts[4], box.pts[5]), color);
	this->DrawLine(Line(box.pts[5], box.pts[6]), color);
	this->DrawLine(Line(box.pts[6], box.pts[7]), color);
	this->DrawLine(Line(box.pts[7], box.pts[4]), color);

	this->DrawLine(Line(box.pts[1], box.pts[5]), color);
	this->DrawLine(Line(box.pts[4], box.pts[0]), color);
	this->DrawLine(Line(box.pts[6], box.pts[2]), color);
	this->DrawLine(Line(box.pts[7], box.pts[3]), color);
}

/**
 * @brief render terrain
 */
bool SCN::FlushTerrain()
{
    T_RETURN(mDrawList[DRAWKIND::TERRAIN].empty());

  //  mDrawList[DRAWKIND::TERRAIN].sort(DrawableSort);

	std::sort(mDrawList[DRAWKIND::TERRAIN].begin() , mDrawList[DRAWKIND::TERRAIN].end() , DrawableSort) ; 


    setRenderTerrainState(true);
    //_FOREACH(DRAWLIST, mDrawList[DRAWKIND::TERRAIN], pos)
    //{ // render terrain object
    //    (*pos)->Draw();
    //}
	_ULOOP(mDrawList[DRAWKIND::TERRAIN].size() , i)
	{
		mDrawList[DRAWKIND::TERRAIN][i]->Draw();

	}
    setRenderTerrainState(false);

    mDrawList[DRAWKIND::TERRAIN].clear();
    return true;
}

void SCN::render()
{
	

	//_FOREACH(DRAWLIST, mSceneItems, pos)
	//{
	//    if((*pos)->Update(mUpdateArgs))
	//    {
	//        visibleCount++ ; 
	//    }

	//    (*pos)->RegisterForDrawing(this);
	//}
	



    SetRenderState();
    // render helper objects
    this->FlushHelpers();
	// render terrain
    this->FlushTerrain();
	// render model object
    this->FlushModel();
}
//---------------------------------------------------------------------------
// 繪製透明建物
//---------------------------------------------------------------------------
void SCN::setRenderAlphaState( bool bSet )
{
	if (bSet)
	{
		// 設定材質狀態
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &m_dwColorOP );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1, &m_dwColorARG1 );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &m_dwColorARG2 );
        m_pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP, &m_dwAlphaOP );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &m_dwAlphaARG1 );
		m_pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &m_dwAlphaARG2 );
        
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );		
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

		// 取得模型繪圖模式
		//DWORD dwZBuf, dwAmbient, dwLight, dwSpec;
		//m_pd3dDevice->GetRenderState( D3DRS_ZENABLE, &m_dwZBuf );
		//m_pd3dDevice->GetRenderState( D3DRS_AMBIENT, &m_dwAmbient );
		m_pd3dDevice->GetRenderState( D3DRS_LIGHTING, &m_dwLight );
		//m_pd3dDevice->GetRenderState( D3DRS_SPECULARENABLE, &m_dwSpec );
		// 設定模型繪圖模式
		//m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); // 開啟ZBuffer
		//m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(0,0,0) );
		// 如果關閉燈光旗標，只顯示白天燈光
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, m_bLightEnabled ); //關閉光源
		//m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );

		// Alpha Test
		//DWORD dwAlphaREF, dwAlphaTest, dwAlphaFunc;
		m_pd3dDevice->GetRenderState( D3DRS_ALPHAREF, &m_dwAlphaREF );
		m_pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &m_dwAlphaTest );
		m_pd3dDevice->GetRenderState( D3DRS_ALPHAFUNC, &m_dwAlphaFunc );

		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, m_dwAlphaREF_Trans );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, (DWORD)0x00000021 );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

		//m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, m_SCN_MapNfo.FogEnable ); // 開啟霧
        //m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, m_SCN_MapNfo.FogColor ); // Set the fog color.
        //m_pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD*)(&m_SCN_MapNfo.FogStart )));
        //m_pd3dDevice->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&m_SCN_MapNfo.FogEnd )));
		//m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );//D3DFOG_LINEAR
		//m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		//m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, 0 );
        g_RenderSystem.SetFogEnable( m_SCN_MapNfo.FogEnable );

		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, m_dwCullMode ); // 關閉雙面繪製
	}
	else
	{
		// 關閉雙面繪製
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

		// 關閉霧
		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); 

		// alpha test
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, m_dwAlphaREF );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, m_dwAlphaTest );
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, m_dwAlphaFunc );

		// 還原繪圖設定值
		//m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, m_dwZBuf );
		//m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, m_dwAmbient );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, m_dwLight );
		//m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, m_dwSpec );

		// 還原之前的材質狀態		
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, m_dwAlphaOP );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, m_dwAlphaARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, m_dwAlphaARG2 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, m_dwColorOP );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, m_dwColorARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, m_dwColorARG2 );
	}
}

bool SCN::FlushParticle()
{
    T_RETURN(mDrawList[DRAWKIND::PARTICLE].empty());

 //   mDrawList[DRAWKIND::PARTICLE].sort(DrawableSort);

	std::sort(mDrawList[DRAWKIND::PARTICLE].begin() , mDrawList[DRAWKIND::PARTICLE].end() , DrawableSort) ; 

    _LOOP(7, pos)
    {
        m_pd3dDevice->LightEnable(pos, FALSE);
    }

    bool preFogState = false;



 //   _FOREACH(DRAWLIST, mDrawList[DRAWKIND::PARTICLE], pos)
	//{
 //       bool currFogState = (*pos)->IsControlEnabled(PARTICLE_FLAG::FOG_EFFECT);
 //       if (currFogState != preFogState)
 //       {
 //           if (currFogState)
 //           {
 //               g_RenderSystem.SetFogEnable(m_SCN_MapNfo.FogEnable);
 //           }
 //           else
 //           {
 //               g_RenderSystem.SetFogEnable(false);
 //           }
 //           preFogState = currFogState;
 //       }
	//	(*pos)->Draw();
	//}

	_ULOOP(mDrawList[DRAWKIND::PARTICLE].size() , i)
	{
        bool currFogState = mDrawList[DRAWKIND::PARTICLE][i]->IsControlEnabled(PARTICLE_FLAG::FOG_EFFECT);
        if (currFogState != preFogState)
        {
            if (currFogState)
            {
                g_RenderSystem.SetFogEnable(m_SCN_MapNfo.FogEnable);
            }
            else
            {
                g_RenderSystem.SetFogEnable(false);
            }
            preFogState = currFogState;
        }
		mDrawList[DRAWKIND::PARTICLE][i]->Draw();
	}

    _LOOP(7, pos)
    {
        m_pd3dDevice->LightEnable(pos, TRUE);
    }

    mDrawList[DRAWKIND::PARTICLE].clear();
    return true;
}

bool SCN::FlushOcean()
{
    T_RETURN(mDrawList[DRAWKIND::OCEAN].empty());

 //   mDrawList[DRAWKIND::OCEAN].sort(DrawableSort);

	std::sort(mDrawList[DRAWKIND::OCEAN].begin() , mDrawList[DRAWKIND::OCEAN].end() , DrawableSort) ; 

    setRenderOceanState(true);
 //   _FOREACH(DRAWLIST, mDrawList[DRAWKIND::OCEAN], pos)
	//{
	//	(*pos)->Draw();
	//}
	_ULOOP(mDrawList[DRAWKIND::OCEAN].size() , i)
	{
		mDrawList[DRAWKIND::OCEAN][i]->Draw();

	}
    setRenderOceanState(false);

    mDrawList[DRAWKIND::OCEAN].clear();
    return true;
}

bool SCN::FlushTransparency()
{
    T_RETURN(mDrawList[DRAWKIND::TRANSPARENCY].empty());

	//mDrawList[DRAWKIND::TRANSPARENCY].sort(DrawableSort);

	std::sort(mDrawList[DRAWKIND::TRANSPARENCY].begin() , mDrawList[DRAWKIND::TRANSPARENCY].end() , DrawableSort) ; 


	setRenderAlphaState(true);
	//_FOREACH(DRAWLIST, mDrawList[DRAWKIND::TRANSPARENCY], pos)
	//{
	//	(*pos)->Draw();
	//}
	_ULOOP(mDrawList[DRAWKIND::TRANSPARENCY].size() , i)
	{
		mDrawList[DRAWKIND::TRANSPARENCY][i]->Draw();

	}
	setRenderAlphaState(false);

    mDrawList[DRAWKIND::TRANSPARENCY].clear();
    return true;
}

void SCN::renderAlpha()
{
    this->FlushOcean();
    this->FlushParticle();
    this->FlushTransparency();
    this->FlushLines();
    //m_pTerrain->DrawHeightMesh(m_matWorld);
}
//---------------------------------------------------------------------------
void SCN::SetRenderState()
{
    double Present_time = ((Server_time.tm_hour%2*60+Server_time.tm_min)*60+Server_time.tm_sec)*1000 + (clock()-Quick_time);
    while(Present_time >= 7200000.0f)
    {
        Present_time -= 7200000.0f;
    }

    TIME_ZONE CURR_TIME_ZONE = MORNING;

    float fPercentage = 0.0f;
    if(Present_time>=0.0f && Present_time<4500000.0f)
    { // 早上
        CURR_TIME_ZONE = MORNING;
    }
    else if(Present_time>=4500000.0f && Present_time<4650000.0f)
    { // 早上漸變至黃昏
        fPercentage = static_cast<float>((Present_time-4500000.0f) / 150000.0f);
        CURR_TIME_ZONE = EVENING;
    }
    else if(Present_time>=4650000.0f && Present_time<4800000.0f)
    { // 黃昏漸變至晚上
        fPercentage = static_cast<float>((Present_time-4650000.0f) / 150000.0f);
        CURR_TIME_ZONE = NIGHT;
    }
    else if(Present_time>=6900000.0f && Present_time<7050000.0f)
    { // 晚上漸變至清晨
        fPercentage = static_cast<float>((Present_time-6900000.0f) / 150000.0f);
        CURR_TIME_ZONE = EARLY;
    }
    else if(Present_time>=7050000.0f && Present_time<7200000.0f)
    { // 清晨漸變至早上
        fPercentage = static_cast<float>((Present_time-7050000.0f) / 150000.0f);
        CURR_TIME_ZONE = MORNING;
    }
    else
    { // 晚上
        CURR_TIME_ZONE = NIGHT;
    }

    TIME_ZONE NEXT_TIME_ZONE = static_cast<TIME_ZONE>(CURR_TIME_ZONE + 1);
    while( NEXT_TIME_ZONE >= MAX_TIME_ZONE )
    {
        NEXT_TIME_ZONE = static_cast<TIME_ZONE>(static_cast<int>(NEXT_TIME_ZONE) - MAX_TIME_ZONE);
    }

    D3DXCOLOR Color = D3DCOLOR_XRGB(
        static_cast<int>(m_ColorR[CURR_TIME_ZONE] + (m_ColorR[NEXT_TIME_ZONE]-m_ColorR[CURR_TIME_ZONE]) * fPercentage),
        static_cast<int>(m_ColorG[CURR_TIME_ZONE] + (m_ColorG[NEXT_TIME_ZONE]-m_ColorG[CURR_TIME_ZONE]) * fPercentage),
        static_cast<int>(m_ColorB[CURR_TIME_ZONE] + (m_ColorB[NEXT_TIME_ZONE]-m_ColorB[CURR_TIME_ZONE]) * fPercentage)
    );

    D3DXVECTOR3 Dir = m_Dir[CURR_TIME_ZONE];

    D3DLIGHT9 Light;
    memset( &Light, 0, sizeof_D3DLIGHT9 );
    Light.Type = D3DLIGHT_DIRECTIONAL;
    Light.Ambient = Color;
    Light.Diffuse = Color;
    Light.Specular = Color;
    Light.Direction = Dir;
    m_pd3dDevice->SetLight( 0, &Light);
    m_pd3dDevice->LightEnable( 0, true);

    /*
    color = D3DCOLOR_XRGB( 20, 20, 25 );
    dir = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );

    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    light.Direction = dir;

    m_pd3dDevice->SetLight(5, &light);
    m_pd3dDevice->LightEnable(5, true);
    */
    // 設定二個對角頂光源
    /*
    D3DXCOLOR color = D3DCOLOR_XRGB(255,255,255);// 燈的顏色

    D3DXVECTOR3 dir1(0.0f, 0.0f, 0.0f);
    D3DLIGHT9 light1;
    ::ZeroMemory(&light1, sizeof(light1));

    if(Quick_time == -1 )
    {
        Quick_time = clock();
    }

    double Present_time = ((Server_time.tm_hour%2*60+Server_time.tm_min)*60+Server_time.tm_sec)*1000 + (clock()-Quick_time);
    while(Present_time >= 7200000)
    {
        Present_time-=7200000;
    }

    if(Present_time>=0 && Present_time<4500000)              //白天
    {
        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(213,225,224);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(100,120,149);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(141,168,204);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(133,129,163);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(140,140,150);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    else if(Present_time>=4500000 && Present_time<4650000)   //白天漸變至黃昏
    {
        int Trans_time = (int)(Present_time - 4500000);

        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(213-13*Trans_time/150000,225-25*Trans_time/150000,224-24*Trans_time/150000);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(100+48*Trans_time/150000,120+20*Trans_time/150000,149-59*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(141+11*Trans_time/150000,168-16*Trans_time/150000,204-88*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(133+23*Trans_time/150000,129+27*Trans_time/150000,163-77*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(140-60*Trans_time/150000, 140-80*Trans_time/150000, 145-100*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    else if(Present_time>=4650000 && Present_time<4800000)   //黃昏漸變至晚上
    {
        int Trans_time = (int)(Present_time - 4650000);

        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(200-18*Trans_time/150000,200-10*Trans_time/150000,100+110*Trans_time/150000);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(148-54*Trans_time/150000,140-22*Trans_time/150000,90+59*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(152-22*Trans_time/150000,152,116+78*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(156-40*Trans_time/150000,156-36*Trans_time/150000,86+65*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(80-60*Trans_time/150000, 60-40*Trans_time/150000, 45-20*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    else if(Present_time>=6900000 && Present_time<7050000)   //晚上漸變至黃昏
    {
        int Trans_time = (int)(Present_time - 6900000);
        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(182+18*Trans_time/150000,190+10*Trans_time/150000,210-110*Trans_time/150000);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(94+54*Trans_time/150000,118+22*Trans_time/150000,149-59*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(128+24*Trans_time/150000,152,194-78*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(116+40*Trans_time/150000,120+36*Trans_time/150000,151-75*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(20+60*Trans_time/150000, 20+40*Trans_time/150000, 25+20*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    else if(Present_time>=7050000 && Present_time<7200000)   //黃昏漸變至白天
    {
        int Trans_time = (int)(Present_time - 7050000);

        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(200+13*Trans_time/150000,200+25*Trans_time/150000,100+124*Trans_time/150000);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(148-48*Trans_time/150000,140-20*Trans_time/150000,90+59*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(152-11*Trans_time/150000,152+16*Trans_time/150000,116+78*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(156-13*Trans_time/150000,156-30*Trans_time/150000,86+77*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(80+60*Trans_time/150000, 60+80*Trans_time/150000, 45+105*Trans_time/150000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    else                                                     //晚上
    {
        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(182,190,210);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(94,118,149);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(128,152,194);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(116,120,151);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(20, 20, 25);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    */
    /*
    if( Present_time>= 0 && Present_time <2340000 )             //白天時間   0~39分
    {
        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(200,200,100);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(148,140,90);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(152,152,116);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(156,156,86);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(80, 60, 45);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    else if( Present_time>=2340000 && Present_time <2400000 )   //漸變至晚上 39~40分
    {
        int Trans_time = Present_time - 2340000;

        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(213-31*Trans_time/60000,225-35*Trans_time/60000,224-14*Trans_time/60000);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(100-16*Trans_time/60000,120-2*Trans_time/60000,149);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(141-13*Trans_time/60000,168-16*Trans_time/60000,204-10*Trans_time/60000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(133-17*Trans_time/60000,129-9*Trans_time/60000,163-12*Trans_time/60000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(140-120*Trans_time/60000, 140-120*Trans_time/60000, 145-120*Trans_time/60000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    else if( Present_time >=3540000 && Present_time < 3600000)  //漸變至白天 59~60分
    {
        int Trans_time = Present_time - 3540000;

        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(182+31*Trans_time/60000,190+35*Trans_time/60000,210+14*Trans_time/60000);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(94+16*Trans_time/60000,118+2*Trans_time/60000,149);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(128+13*Trans_time/60000,152+16*Trans_time/60000,194+10*Trans_time/60000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(116+17*Trans_time/60000,120+9*Trans_time/60000,151+12*Trans_time/60000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(20+120*Trans_time/60000, 20+120*Trans_time/60000, 25+120*Trans_time/60000);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    else
    {
        D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

        color = D3DCOLOR_XRGB(182,190,210);
        D3DLIGHT9 light;
        ::ZeroMemory(&light, sizeof(light));
        light.Type      = D3DLIGHT_DIRECTIONAL;
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        light.Direction = dir;

        m_pd3dDevice->SetLight(0, &light);
        m_pd3dDevice->LightEnable(0, true);

        light.Direction.z = -1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(94,118,149);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(3, &light);
        m_pd3dDevice->LightEnable(3, true);

        light.Direction.z = 0.0f;
        light.Direction.x = 1.0f;
        light.Direction.y = -0.577f;
        color = D3DCOLOR_XRGB(128,152,194);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(4, &light);
        m_pd3dDevice->LightEnable(4, true);

        light.Direction.x = -1.0f;
        light.Direction.y = 0.577f;
        color = D3DCOLOR_XRGB(116,120,151);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(5, &light);
        m_pd3dDevice->LightEnable(5, true);

        light.Direction.x = 0.0f;
        light.Direction.y = -1.0f;
        color = D3DCOLOR_XRGB(20, 20, 25);
        light.Ambient   = color ;
        light.Diffuse   = color ;
        light.Specular  = color ;
        m_pd3dDevice->SetLight(6, &light);
        m_pd3dDevice->LightEnable(6, true);
    }
    */

    /* switch (SCN_Light.LightStatus)// 場景燈光狀態(0:關閉 1:開啟45度對角 2:開啟水平90度對角 3:開啟垂直90度對角)
    {
        case 0:
        {
            dir1.x = -1.0f;
            dir1.y = 0.0f;
            dir1.z = 0.0f;
            break;
        }
        case 1:
        {
            //45度對角
            dir1.x = -1.0f;
            dir1.y = -1.0f;
            dir1.z = -1.0f;
            break;
        }
        case 2:
        {       //水平90度對角
            dir1.x = -1.0f;
            dir1.y = -1.0f;
            dir1.z = 0.0f;
            break;
        }
        case 3:
        {       //垂直90度對角
            dir1.x = 0.0f;
            dir1.y = -1.0f;
            dir1.z = -1.0f;
            break;
        }
    } */

	/*
    dir1.x = 0.0f;
    dir1.y = 0.0f;
    dir1.z = -1.0f;
    color = D3DCOLOR_XRGB(255, 255, 255);
    light1.Type      = D3DLIGHT_DIRECTIONAL;
    light1.Ambient   = color;
    light1.Diffuse   = color;
    light1.Specular  = color;
    light1.Direction = dir1;
    light1.Range = 1.0f;
    m_pd3dDevice->SetLight(1, &light1);
    m_pd3dDevice->LightEnable(1, true);
	*/
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);

    /* if (SCN_Light.LightStatus == 0)
    {
        m_pd3dDevice->SetLight(1, &light1);
        m_pd3dDevice->LightEnable(1, true);
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true);
        m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
    }
    else
    {
        m_pd3dDevice->SetLight(1, &light1);
        m_pd3dDevice->LightEnable(1, true);
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true);
        m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
    } */

	//m_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

    /*
    D3DLIGHT9 test_spot_light;
    D3DXCOLOR c = D3DCOLOR_XRGB(255,255,255);
    ::ZeroMemory(&test_spot_light, sizeof(test_spot_light));
    D3DXVECTOR3 pos=g_Camera.GetLockPos()-g_Camera.GetPosition()+g_Camera.GetLockPos();
    pos.y = g_Camera.GetPosition().y;
    D3DXVECTOR3 dir=g_Camera.GetLockPos()-pos;
    test_spot_light = d3d::InitSpotLight(&pos, &dir, &c);
    test_spot_light.Falloff = 0.1f;
    test_spot_light.Attenuation0 = 1.0f;
    test_spot_light.Phi = 180.0f;
    m_pd3dDevice->SetLight(2, &test_spot_light);
    m_pd3dDevice->LightEnable(2, false);
    */

    //m_pd3dDevice->LightEnable(0,true);

    // 設定二個對角頂光源END

	// 設定材質運算模式
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

//---------------------------------------------------------------------------
// 設定背景清除顏色
//---------------------------------------------------------------------------
void SCN::setClearColor()
{ 
	g_RenderSystem.SetClearColor(m_SCN_MapNfo.FogColor); 
}
//---------------------------------------------------------------------------
void SCN::setClipField()
{
    // 重新建立裁切範圍
    float fovy    = D3DX_PI * 0.25f;
    float fAspect = static_cast<float>( g_RenderSystem.GetBackBufferWidth() ) / 
		static_cast<float>( g_RenderSystem.GetBackBufferHeight() );
    float zn = 1.0f;
	float zf = m_SCN_MapNfo.FogEnd;
    
    if( m_SCN_MapNfo.FogEnable ==false )
        zf = 700.0f;

	g_FOVClip.SetupFOVClipPlanes( fovy*2.0f, fAspect, zn, zf );
    //g_FOVClip.SetupFOVClipPlanes ( fovy*2.0f, Aspect, zn, MapNfo->FogEnd );
}

D3DCOLOR SCN::GetSelectionColor() const
{
    return mSelectionColor;
}

void SCN::FrameMove(float fElapsedTime, float particleRange, float modelRange,
					float width, float height,
					const D3DXMATRIX& matInvView,
					const D3DXVECTOR3& vPosOfCamera, const D3DXVECTOR3& vPosOfLock,
					const D3DXVECTOR3& mainRolePos)
{
    //m_fElapsedTime = fElapsedTime;
    //m_matViewInv = matInvView;
    mPassedTime += fElapsedTime;
    BYTE base = (BYTE)((::fmod(mPassedTime, 1.5f) * (255.0f / 1.5f)));
    mSelectionColor = D3DCOLOR_ARGB(255, base, 0xFF-base, base);

    const D3DXVECTOR3 vPickRayOrig(vPosOfLock / 2.0f);
    const D3DXVECTOR3 vPickRayDir(vPosOfLock - vPosOfCamera);
    const float fDistance = D3DXVec3Length(&vPickRayDir) - 1.0f;

	float roleLeft   = vPosOfLock.x - particleRange;
	float roleTop    = vPosOfLock.z + particleRange;
	float roleRight  = vPosOfLock.x + particleRange;
	float roleBottom = vPosOfLock.z - particleRange;

	//roleLeft = roleLeft < 0.0f ? 0.0f : roleLeft ; 
	//roleTop = roleTop < 0.0f ? 0.0f : roleTop ; 
	//roleRight = roleRight > 511.0f ? 511.0f : roleRight ; 
	//roleBottom = roleBottom > 511.0f ? 511.0f : roleBottom ; 

	D3DXVECTOR4 rectRange(roleLeft, roleTop ,roleRight ,roleBottom);
	D3DXVECTOR4 modelViewRect(vPosOfLock.x - modelRange,
                              vPosOfLock.z + modelRange,
                              vPosOfLock.x + modelRange,
                              vPosOfLock.z - modelRange);

	unsigned int count = 0;
	unsigned int parCount  = 0;

    mUpdateArgs->SetElapsedTime(fElapsedTime);
    mUpdateArgs->SetViewRect(modelViewRect);
    mUpdateArgs->SetRectRange(rectRange);
    mUpdateArgs->SetRay(RAY(vPosOfCamera, vPickRayDir));
    mUpdateArgs->SetViewDistance(D3DXVec3Length(&(vPosOfLock - vPosOfCamera)));
    mUpdateArgs->SetHeight(height);
    mUpdateArgs->SetWidth(width);

    D3DXMATRIX viewMatrix;
    m_pd3dDevice->GetTransform(D3DTS_VIEW, &viewMatrix);
    mUpdateArgs->SetViewMatrix(viewMatrix);
    //mUpdateArgs->SetInvViewMatrix(matViewInv);

    FDO::GetParticleSystemPtr()->SetViewMatrix(viewMatrix);
    FDO::GetParticleSystemPtr()->SetInvViewMatrix(matInvView);

	_ULOOP(DRAWKIND::COUNT, i)
	{
		mDrawList[i].clear();
	}

	static int visibleCount = 0 ; 
	_ULOOP(mSceneItems.size() , i)
	{
		if(mSceneItems[i]->Update(mUpdateArgs))
		{
			visibleCount++ ; 
		}

		mSceneItems[i]->RegisterForDrawing(this);
	}
	visibleCount = 0;
}
//---------------------------------------------------------------------------
void SCN::setLightColor( int iR, int iG, int iB, int iTimeZone )
{
    if( iTimeZone>=0 && iTimeZone<MAX_TIME_ZONE )
    {
        m_ColorR[iTimeZone] = iR;
        m_ColorG[iTimeZone] = iG;
        m_ColorB[iTimeZone] = iB;
    }
}
//---------------------------------------------------------------------------
void SCN::setLightDir( float fX, float fY, float fZ, int iTimeZone )
{
    if( iTimeZone>=0 && iTimeZone<MAX_TIME_ZONE )
    {
        m_Dir[iTimeZone] = D3DXVECTOR3( fX, fY, fZ );
    }
}
//---------------------------------------------------------------------------
bool SCN::LoadCollisionData(const string& strFilename,vector<BYTE>& datas,sint& nWidth,sint& nHeight)
{

	// 重封裝檔讀取 *.SCN
	memPack *m = LoadFromPacket( strFilename );


	if (m->Data ==NULL)
		return false;

	BYTE *pData = m->Data ;

	SCN_Header	header;
	SCN_MapNfo	info;
	memcpy( &header, pData, sizeof_SCN_Header );
	memcpy( &info, pData+header.MapInformationOffset, sizeof_SCN_MapNfo );

	// collision data pointer
	BYTE* pInforData = pData + info.CollideInforOffset;
	//

	//
	int iInforCount = (info.GridWCount-1) * (info.GridHCount-1);

	// infor
	datas.resize( iInforCount );
	if( !datas.empty() )
	{
		memcpy( &datas.front(), pInforData, datas.size()*sizeof_BYTE );
	}

	nWidth = (info.GridWCount-1);
	nHeight = (info.GridHCount-1);	
	return true;
}
//---------------------------------------------------------------------------
void SCN::RegisterForDrawing(DRAWKIND::E kind, DrawablePtr draw)
{
    if (kind < DRAWKIND::COUNT)
    {
        mDrawList[kind].push_back(draw);
    }
}
//---------------------------------------------------------------------------
void SCN::CleanUp()
{
    m_pTerrain = NULL;

    FDO::GetParticleSystemPtr()->CleanUp();

    //_FOREACH(DRAWLIST, mSceneItems, pos)
    //{
    //    (*pos)->CleanUp();
    //    delete (*pos);
    //}
	_ULOOP(mSceneItems.size() , i)
	{
        mSceneItems[i]->CleanUp();
        delete mSceneItems[i];
	}

    mSceneItems.clear();
}
//---------------------------------------------------------------------------
void SCN::SetParticlePath(const fstring& value)
{
    mParticlePath = value + fstring(_F("\\"));
}
//---------------------------------------------------------------------------
void SCN::SetModelPath(const fstring& value)
{
    mModelPath = value + fstring(_F("\\"));
}
//---------------------------------------------------------------------------
void SCN::SetMapPath(const fstring& value)
{
    mMapPath = value;
}

void SCN::setSceneAlpha(float value)
{
    m_fSceneAlpha = value;
}

TerrainPtr SCN::getTerrain() const
{
    return m_pTerrain;
}

CollisionMapPtr SCN::GetCollisionMap() const
{
    return mCollisionMap;
}

void SCN::SetTerrainNameList(const fstring& value)
{
    GetNameToken(mTerrainNames, value);
}

void SCN::GetNameToken(FStringList& nameList, const fstring& names)
{
    const fstring slash("/");

    nameList.clear();

    size_t currPos = names.npos;
    do
    {
        ++currPos;
        size_t nextPos = names.find(slash, currPos);
        size_t size = 0;
        if (nextPos == names.npos)
        {
            size = names.size() - currPos;
        }
        else
        {
            size = nextPos - currPos;
        }
        if (size > 0)
        {
            nameList.push_back(names.substr(currPos, size));
        }
        currPos = nextPos;
    } while (currPos != names.npos);
}

void SCN::SetNativeTransparentNameList(const fstring& value)
{
    GetNameToken(mNativeTransparentNames, value);
}

void SCN::SetOpaqueNameList(const fstring& value)
{
    GetNameToken(mOpaqueNames, value);
}

void SCN::SetTransparentNameList(const fstring& value)
{
    GetNameToken(mTransparentNames, value);
}
//---------------------------------------------------------------------------
} // end of namespace FDO
