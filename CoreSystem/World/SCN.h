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
	SCN_LightStatus SCN_Light;							// �������O������(�s�l�[)
														// set resource render state
	void setRenderAlphaState( bool bSet );				// alpha
	void setRenderModelState( bool bSet );				// model
	void setRenderOceanState( bool bSet );				// ocean
    void setRenderTerrainState( bool bSet );            // terrain
    void SetRenderState();								// �]�mø�s���A
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
    // �O���C��
    int m_ColorR[MAX_TIME_ZONE];
    int m_ColorG[MAX_TIME_ZONE];
    int m_ColorB[MAX_TIME_ZONE];
    // �O����V
    D3DXVECTOR3 m_Dir[MAX_TIME_ZONE];

	//���oserver�ɶ�
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
    /// �]�w�I���M���C��
	void setClearColor();
    /// �]�w�����d��
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
    /// Ū���I���h
	bool loadCollisionData(const BYTE* data);
    /// Ū�����׼h
	bool loadAltitudeData(const BYTE* data);	
    /// Ū������
	bool loadObjectData(const BYTE* data);		
    /// Ū���S����
	bool loadObjectExData(const BYTE* data);	
    /// Ū�����l�S��
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
    /// �@�ɯx�}
    mutable D3DXMATRIX mWorldMatrixCache;
    /// ��m
    D3DXVECTOR3 mPosition;
    /// ���l���|
    fstring mParticlePath;
    /// �ҫ����|
    fstring mModelPath;
    /// �a�ϸ��|
    fstring mMapPath;
    /// �I���h
    CollisionMapPtr mCollisionMap;
    /// ���׼h
    AltitudeMapPtr mAltitudeMap;
    /// �����T
	ObjNfoList m_ObjectNfo;
    /// �S�����T(ocean)
    ExternObjList m_ObjectEXData;
    /// ���l�S��
    PCLInforList m_ParticleNfo;
    /// scene header
    SCN_Header m_SCN_Header;
    /// map information
	SCN_MapNfo m_SCN_MapNfo;
    /// ��ͪ��z������
    FStringList mNativeTransparentNames;
    /// �b�z���ĪG��ݤ���������
    FStringList mTransparentNames;
    /// ���z��������
    FStringList mOpaqueNames;
    /// �a�Ϊ���
    FStringList mTerrainNames;
    /// terrain
	TerrainPtr m_pTerrain;
    /// bounding box
    ImmediateVertexList mImmediateLines;
    /// bounding box color
    D3DCOLOR mSelectionColor;
    /// total passed time
    float mPassedTime;
    /// ��ܻ��U����
    bool mShowHelpers;
/// </yuming>
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif // SCNH
