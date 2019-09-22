/**
 * @file SCN.h
 * @author Yuming Ho
 * @brief scene file
 */
#ifndef SCNH
#define SCNH
//---------------------------------------------------------------------------
#include "SCN_Struct.h"
#include "FdoString.h"
#include "elementdef.h" // ref: ObjFactory
#include "FdoWorldPreRequest.h"
#include <time.h>
//---------------------------------------------------------------------------
namespace FDO
{

class SCN
{
private:
	SCN_LightStatus SCN_Light;							// 場景的燈光控制(新追加)
														// set resource render state
	void setRenderAlphaState( bool bSet );				// alpha
	void setRenderModelState( bool bSet );				// model
	void setRenderOceanState( bool bSet );				// ocean
    void setRenderTerrainState( bool bSet );            // terrain
    void SetRenderState();								// 設置繪製狀態
	// flag parameter
    DWORD m_dwCullMode;
    DWORD m_dwAlphaREF_Trans;
    float m_fSceneAlpha;
	bool m_bLightEnabled;
    BOOL mSortModel;

	LPDIRECT3DDEVICE9 m_pd3dDevice;
	int m_iCellSpacing;
	// render state flag
	DWORD m_dwZBuf, m_dwAmbient, m_dwLight, m_dwSpec;
	DWORD m_dwAlphaREF, m_dwAlphaTest, m_dwAlphaFunc;
	DWORD m_dwAlphaOP, m_dwAlphaARG1, m_dwAlphaARG2;
	DWORD m_dwColorOP, m_dwColorARG1, m_dwColorARG2;
    DWORD m_dwAlphaOP_1, m_dwAlphaARG1_1, m_dwAlphaARG2_1;
	DWORD m_dwColorOP_1, m_dwColorARG1_1, m_dwColorARG2_1;
    // 燈光顏色
    int m_ColorR[MAX_TIME_ZONE];
    int m_ColorG[MAX_TIME_ZONE];
    int m_ColorB[MAX_TIME_ZONE];
    // 燈光方向
    D3DXVECTOR3 m_Dir[MAX_TIME_ZONE];

	//取得server時間
    tm Server_time;
    double Quick_time;

	//<gino>
	static bool sm_bEnableTransparent;
	//</gino>

public:

	// construct
    SCN();

	// destruct
    ~SCN();

    //bool Create( const string& strFilename, const string& strModelPath, float fX, float fZ );

    void FrameMove(float fElapsedTime, float particleRange, float modelRange,
		float width , float height, const D3DXMATRIX& matInvView,
        const D3DXVECTOR3& vPosOfCamera, const D3DXVECTOR3& vPosOfLock,
		const D3DXVECTOR3& mainRolePos);

	void render();

	void renderAlpha();

	// set
    /// 設定背景清除顏色
	void setClearColor();
    /// 設定裁切範圍
    void setClipField();
    void setCullMode( DWORD dwCullMode ) { m_dwCullMode=dwCullMode; }
	void setLightEnabled( bool bLightEnabled ) { m_bLightEnabled=bLightEnabled; }
    void setAlphaTest( DWORD dwAlphaREF_Trans ) { m_dwAlphaREF_Trans=dwAlphaREF_Trans; }
    void setLightColor( int iR, int iG, int iB, int iTimeZone );
    void setLightDir( float fX, float fY, float fZ, int iTimeZone );
    void SetSortModelEnabled(BOOL value) { mSortModel = value; }

	//<gino>
	static void EnableTransparent( bool bTrue ){ sm_bEnableTransparent=bTrue; }
	static bool IsEnableTransparent() { return sm_bEnableTransparent; }
	//</gino>
	static bool	LoadCollisionData(const string& strFilename,vector<BYTE>& datas,sint& nWidth,sint& nHeight);

/// <yuming>
public: // attribute
    void RegisterForDrawing(DRAWKIND::E kind, DrawablePtr draw);
    const D3DXMATRIX& GetWorldMatrix() const;
    void SetParticlePath(const fstring& value);
    void SetModelPath(const fstring& value);
    void SetMapPath(const fstring& value);
    void SetPosition(float x, float y, float z);
    bool Create(const BYTE* data, UINT size);
    void CleanUp();
    void setSceneAlpha(float value);
    TerrainPtr getTerrain() const;
    CollisionMapPtr GetCollisionMap() const;
    void SetTerrainNameList(const fstring& value);
    void SetNativeTransparentNameList(const fstring& value);
    void SetOpaqueNameList(const fstring& value);
    void SetTransparentNameList(const fstring& value);
    bool IsNativeTransparent(const fstring& fileName) const;
    bool IsTerrain(const fstring& fileName) const;
    bool IsWithoutAlphaTest(const fstring& fileName) const;
    float GetAlphaDest(const fstring& fileName) const;
    D3DCOLOR GetSelectionColor() const;
    void SetShowHelpers(bool value);

private:
    typedef ObjFactory<SCN> SCNFactory;
    static SCNFactory sSCNFactory;

public: // operator
    void* operator new (size_t);
    void operator delete (void* p);
    
    void DrawLine(const Line& line, D3DCOLOR color);
    void DrawBox(const Box& box, D3DCOLOR color);

private: // helper
    bool FlushTerrain();
    bool FlushModel();
    bool FlushParticle();
    bool FlushOcean();
    bool FlushTransparency();
    void FlushHelpers();
    void FlushLines();
    bool InitializeHeader(const BYTE* data);
    /// 讀取碰撞層
	bool loadCollisionData(const BYTE* data);
    /// 讀取高度層
	bool loadAltitudeData(const BYTE* data);	
    /// 讀取物件
	bool loadObjectData(const BYTE* data);		
    /// 讀取特殊物件
	bool loadObjectExData(const BYTE* data);	
    /// 讀取分子特效
	bool loadParticleData(const BYTE *data);				
    /// generate render resource form data
	bool generateTerrain();
	bool generateModel();
	bool generateOcean(const BYTE* data);
	bool generateParticle();

    LPDIRECT3DTEXTURE9 GetOceanTexture(const fstring& name);
    static void GetNameToken(FStringList& nameList, const fstring& names);

private:
    UpdateArgsPtr mUpdateArgs;
    DRAWLIST mDrawList[DRAWKIND::COUNT];
    DRAWLIST mSceneItems;
    /// world matrix flag
    mutable bool mWorldMatrixOutOfData;
    /// 世界矩陣
    mutable D3DXMATRIX mWorldMatrixCache;
    /// 位置
    D3DXVECTOR3 mPosition;
    /// 分子路徑
    fstring mParticlePath;
    /// 模型路徑
    fstring mModelPath;
    /// 地圖路徑
    fstring mMapPath;
    /// 碰撞層
    CollisionMapPtr mCollisionMap;
    /// 高度層
    AltitudeMapPtr mAltitudeMap;
    /// 物件資訊
	ObjNfoList m_ObjectNfo;
    /// 特殊物件資訊(ocean)
    ExternObjList m_ObjectEXData;
    /// 分子特效
    PCLInforList m_ParticleNfo;
    /// scene header
    SCN_Header m_SCN_Header;
    /// map information
	SCN_MapNfo m_SCN_MapNfo;
    /// 原生的透明物件
    FStringList mNativeTransparentNames;
    /// 半透明效果到看不見的物件
    FStringList mTransparentNames;
    /// 不透明的物件
    FStringList mOpaqueNames;
    /// 地形物件
    FStringList mTerrainNames;
    /// terrain
	TerrainPtr m_pTerrain;
    /// bounding box
    ImmediateVertexList mImmediateLines;
    /// bounding box color
    D3DCOLOR mSelectionColor;
    /// total passed time
    float mPassedTime;
    /// 顯示輔助物件
    bool mShowHelpers;
/// </yuming>
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif // SCNH
