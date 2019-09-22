//---------------------------------------------------------------------------
#ifndef __PhenixD3D9RENDERSYSTEM_H__
#define __PhenixD3D9RENDERSYSTEM_H__
//---------------------------------------------------------------------------
#include "PhenixD3D9DriverList.h"
#include "PhenixD3D9VideoModeList.h"
#include "PhenixConfigOption.h"
#include "D3DFont.h"
#include "singleton.h"
#include "renderer.h"
//---------------------------------------------------------------------------
#define g_RenderSystem     D3D9RenderSystem::GetSingleton()
#define g_RenderSystemPtr  D3D9RenderSystem::GetSingletonPtr()
//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
typedef void (*_CallbackOnTakeScreenShot)(bool bHighQuality, const WCHAR *fileName, const char *msg);

class CPostProcessFilter ; 
struct PostPassParameters ; 
class CAllDay ; 
class CRenderTarget;

enum enPostType
{
	PostBeging = 0 , 
	COLORMATRIX = 1 , 
	GLOW ,
	SPOTLIGHT ,
	EASYCOLORMIXER ,
	PostEnd , 
};

struct PostEffect
{
	unsigned int			id ; 
	enPostType				type ; 
	CPostProcessFilter		*pPostPass ; 
	bool					bUsed ; 
};

struct PostBinData
{
	unsigned int	id ; 
	enPostType		type ;
};

//static PostBinData g_postBinData[PostEnd - PostBeging - 1] ; 

class D3D9RenderSystem : public Singleton<D3D9RenderSystem>
{
	//物件資訊
private:
	unsigned int			m_particleCount ; 
	unsigned int			m_updateParticleCount ; 
	unsigned int			m_flushParticleCount ; 
	unsigned int			m_modelCount ; 
	unsigned int			m_updateModelCount ; 

	unsigned int			m_updateMonster ;
	unsigned int			m_updateNPC ; 
	unsigned int			m_updateCostar ; 
	unsigned int			m_updatePet ;

	unsigned int			m_freeMagicParticCount ;
	unsigned int			m_magicParticCount ;
	unsigned int			m_freeShootParticCount ;
	unsigned int			m_shootParticCount ;
	unsigned int			m_freeSystemParticCount ;
	unsigned int			m_systemParticCount ;
	unsigned int			m_freeParticCount ;
	unsigned int			m_particCount ;

	unsigned int			m_removeMagicParticCount ;
	unsigned int			m_removeShootParticCount ;
	unsigned int			m_removeSystemParticCount ;

	unsigned int			m_forceCount ;
	unsigned int			m_forceManagerCount ;
	unsigned int			m_caliberCount ;
	unsigned int			m_colorCount ;
	unsigned int			m_scaleCount ;
	unsigned int			m_audioCount ;
	unsigned int			m_textureCount ;
	unsigned int			m_multiTexCount ;
	unsigned int			m_actionManagerCount ;
	unsigned int			m_emitterCount ;



private:
	void ClearInformation(void)
	{
		m_particleCount  = 0; 
		m_updateParticleCount = 0 ; 
		m_flushParticleCount = 0 ; 
		m_modelCount = 0 ; 
		m_updateModelCount = 0 ; 

		m_updateMonster = 0 ;
		m_updateNPC = 0 ; 
		m_updateCostar = 0 ; 
		m_updatePet = 0  ;


		m_freeMagicParticCount = 0;
		m_magicParticCount = 0;
		m_freeShootParticCount = 0;
		m_shootParticCount = 0;
		m_freeSystemParticCount = 0;
		m_systemParticCount = 0;
		m_freeParticCount = 0;
		m_particCount = 0;

		m_removeMagicParticCount = 0;
		m_removeShootParticCount = 0;
		m_removeSystemParticCount = 0;

		//m_forceCount = 0;
		//m_forceManagerCount = 0;
		//m_caliberCount = 0;
		//m_colorCount = 0;
		//m_scaleCount = 0;
		//m_audioCount = 0;
		//m_textureCount = 0;
		//m_multiTexCount = 0;
		//m_actionManagerCount = 0;
		//m_emitterCount = 0;


	}
public:
	void AddParticleCount(unsigned int value) {m_particleCount += value; } 
	void AddUpdateParticleCount(unsigned int value) {m_updateParticleCount += value; } 
	void AddFlushParticleCount(unsigned int value){m_flushParticleCount += value; }
	void AddModelCount() {m_modelCount++; } 
	void AddUpdateModelCount() {m_updateModelCount++; } 

	void AddUpdateMonsterCount() {m_updateMonster++; } 
	void AddUpdateNPCCount() {m_updateNPC++; } 
	void AddUpdateCostarCount() {m_updateCostar++; } 
	void AddUpdatePetCount() {m_updatePet++; } 

	void SetFreeMagicParticCount(unsigned int value) {m_freeMagicParticCount = value ; } 
	void SetMagicParticCount(unsigned int value) {m_magicParticCount = value ; } 
	void SetFreeShootParticCount(unsigned int value) {m_freeShootParticCount = value ; } 
	void SetShootParticCount(unsigned int value) {m_shootParticCount = value ; } 
	void SetFreeSystemParticCount(unsigned int value) {m_freeSystemParticCount = value ; } 
	void SetSystemParticCount(unsigned int value) {m_systemParticCount = value ; } 
	void SetFreeParticCount(unsigned int value) {m_freeParticCount = value ; } 
	void SetParticCount(unsigned int value) {m_particCount = value ; } 

	void SetRemoveMagicParticCount(unsigned int value) {m_removeMagicParticCount = value ; } 
	void SetRemoveShootParticCount(unsigned int value) {m_removeShootParticCount = value ; } 
	void SetRemoveSystemParticCount(unsigned int value) {m_removeSystemParticCount = value ; } 

	void SetForceCount(unsigned int value) { m_forceCount = value; }
	void SetForceManagerCount(unsigned int value) { m_forceManagerCount = value; }
	void SetCaliberCount(unsigned int value) { m_caliberCount = value; }
	void SetColorCount(unsigned int value) { m_colorCount = value; }
	void SetScaleCount(unsigned int value) { m_scaleCount = value; }
	void SetAudioCount(unsigned int value) { m_audioCount = value; }
	void SetTextureCount(unsigned int value) { m_textureCount = value; }
	void SetMultiTexCount(unsigned int value) { m_multiTexCount = value; }
	void SetActionManagerCount(unsigned int value) { m_actionManagerCount = value; }
	void SetEmitterCount(unsigned int value) { m_emitterCount = value; }

	CPostProcessFilter* LoadEffect(unsigned int id , const std::string &filename) ; 
	void RunPrePass() ; 
	void RunPostPass() ; 
	void UnApplyFilter() ; 
	CRenderTarget * CreateRenderTarget(int renderStep, int w, int h, D3DFORMAT format, bool bDepthStencil);
	void SetMipMapLODBias(float bias);
	float GetMipMapLODBias(){ return mTexLODBias; }
	PostPassParameters* GetParameter(enPostType type) ; 
private:
	CPostProcessFilter* CreateEffect(LPD3DXEFFECT pEffect , unsigned int id) ; 
private:
    //視窗資訊
    unsigned int            mWidth;
    unsigned int            mHeight;
    unsigned int            mClientWidth;
    unsigned int            mClientHeight;
	unsigned int            iWidth;
	unsigned int            iHeight;
    RECT                    mRcWindowClient;
    HWND                    mHWnd;
    DWORD                   mWindowedStyle;

    //D3D裝置資訊
    unsigned int            mColourDepth;
    bool                    mIsFullScreen;
    bool                    mIsVSync;
    bool                    mIsDeviceLost;
    LPDIRECT3D9			    mpD3D;
    LPDIRECT3DDEVICE9	    mpD3DDevice;
    DWORD                   mClearFlags;
    D3DCOLOR                mColour;

    //畫面截取
    bool                    mIsPrintScreen;
    bool                    m_bRestoreVis;

    //D3D裝置建立資訊
    std::auto_ptr<D3D9DriverList> mDriverList;
    D3D9Driver*             mActiveD3DDriver;
    D3DDEVTYPE              mDevType;
    D3DPRESENT_PARAMETERS   md3dpp;
    D3DMULTISAMPLE_TYPE     mFSAAType;
    DWORD                   mFSAAQuality;

    //畫面狀態資訊
    float                   mfFPS;
    DWORD                   mFrameCount;
    DWORD                   mLastTime;
    WCHAR                   mFrameStatus1[128];
    WCHAR                   mFrameStatus2[128];
    WCHAR                   mFrameStatus3[128];
    WCHAR                   mFrameStatus4[128];
	WCHAR                   mFrameStatus5[128];
    RECT                    mRect1;
    RECT                    mRect2;
    RECT                    mRect3;
    RECT                    mRect4;
    StringVector            mDeviceStatus;
    bool                    mIsShowFrameStatus;

    //字型資訊
    std::auto_ptr<CD3DFont> mpFont;
    std::auto_ptr<CD3DFont> mpFont2;

    //設定資訊
    ConfigOptionMap         mOptions;

    //D3D裝置函式
    D3D9DriverList*         GetDirect3DDrivers();
    void                    RestoreDeviceObjects();
    void                    InvalidateDeviceObjects();
    bool                    InitializeD3D();
    void                    SetupStates();

    // ming add >> set callback
    typedef HRESULT (CALLBACK *LPDXUTCALLBACKDEVICECREATED)( IDirect3DDevice9* pd3dDevice );
    typedef HRESULT (CALLBACK *LPDXUTCALLBACKDEVICERESET)( IDirect3DDevice9* pd3dDevice );
    typedef void    (CALLBACK *LPDXUTCALLBACKDEVICEDESTROYED)();
    typedef void    (CALLBACK *LPDXUTCALLBACKDEVICELOST)();

    LPDXUTCALLBACKDEVICECREATED         m_DeviceCreatedFunc;        // device created callback
    LPDXUTCALLBACKDEVICERESET           m_DeviceResetFunc;          // device reset callback
    LPDXUTCALLBACKDEVICELOST            m_DeviceLostFunc;           // device lost callback
    LPDXUTCALLBACKDEVICEDESTROYED       m_DeviceDestroyedFunc;      // device destroyed callback

    bool m_bFogFlag;
    DWORD m_dwFogColor;
    DWORD m_dwFogStart;
    DWORD m_dwFogEnd;
    // ming add <<

    //<gino>
    BOOL m_bPauseRender;
    D3DFORMAT m_RTTFmt;

    bool m_bHighQualityScreenShot;
    _CallbackOnTakeScreenShot m_cbOnTakeScreenShot;
    //</gino>

	bool	m_bPostPass ; 

	std::vector<PostEffect>				m_postEffectVec ; 
	std::vector<CRenderTarget*>		    mRenderTargetList;
	std::vector<IRendererResource*>		mRendererResList;
	float mTexLODBias;
public:
    D3D9RenderSystem();
    ~D3D9RenderSystem();

    //初始函式
    void                    InitConfigOptions();
    bool                    Initialize(HWND externalHandle);
    void                    Shutdown();

    //D3D裝置函式
    bool                    BeginFrame();
    bool                    EndFrame();

    //畫面狀態函式
    void                    DrawText(LPCTSTR pString, LPRECT pRect, DWORD Color, bool IsShade = true );
    void                    DrawText2(LPCTSTR pString, LPRECT pRect, DWORD Color, bool IsShade = true );
    void                    DrawText2(LPCTSTR pString, INT cch, LPRECT pRect, DWORD Color, bool IsShade = true);
    INT                     CalcRect(LPCTSTR pString, INT cch, LPRECT pRect);
    void                    UpdateFrameStatus();

    //處理裝置狀態改變函式
    bool                    HandlePossibleSizeChange();
    bool                    ToggleFullScreen();

    //設定屬性函式
    void                    SetClearColor( D3DCOLOR Value )    { mColour = Value; }
    void                    SetClearFlag( DWORD Value )        { mClearFlags = Value; }
    void                    SetShowFrameStatus( bool Value )   { mIsShowFrameStatus = Value; }
    void                    SetPrintScreen( bool Value )       { mIsPrintScreen = Value; }

    //取得屬性函式
    const LPDIRECT3DDEVICE9	GetD3DDevice()                     { return mpD3DDevice; }
    bool                    GetIsFullScreen()                  { return mIsFullScreen; }
    unsigned int            GetBackBufferWidth()               { return md3dpp.BackBufferWidth; }
    unsigned int            GetBackBufferHeight()              { return md3dpp.BackBufferHeight; }
	unsigned int            GetWindowWidth() const			   { return iWidth; }
	unsigned int            GetWindowHeight() const			   { return iHeight; }
    unsigned int            GetClientWidth()                   { return mClientWidth; }
    unsigned int            GetClientHeight()                  { return mClientHeight; }
    HWND                    GetHWND()                          { return mHWnd; }
    const float&            GetFPS()                           { return mfFPS; }

    //Print Screen
    bool                    TakeScreenShot();
    bool                    TakeScreenShotHighQuality();
    bool                    TakeScreenShotLowQuality();

    //訊息處理函式
    LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    // ming add >>
    void SetCallbackDeviceCreated( LPDXUTCALLBACKDEVICECREATED pCallbackDeviceCreated )
        { m_DeviceCreatedFunc = pCallbackDeviceCreated; }
    void SetCallbackDeviceReset( LPDXUTCALLBACKDEVICERESET pCallbackDeviceReset )
        { m_DeviceResetFunc = pCallbackDeviceReset; }
    void SetCallbackDeviceLost( LPDXUTCALLBACKDEVICELOST pCallbackDeviceLost )
        { m_DeviceLostFunc = pCallbackDeviceLost; }
    void SetCallbackDeviceDestroyed( LPDXUTCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed )
        { m_DeviceDestroyedFunc = pCallbackDeviceDestroyed; }

    bool GetFogFlag() { return m_bFogFlag; }
    void SetFogFlag( bool bFogFlag ) { m_bFogFlag=bFogFlag; }
    void SetFogEnable( bool bFogEnable );
    void SetFogColor( DWORD dwFogColor ) { m_dwFogColor=dwFogColor; }
    void SetFogStart( DWORD dwFogStart ) { m_dwFogStart=dwFogStart; }
    void SetFogEnd( DWORD dwFogEnd ) { m_dwFogEnd=dwFogEnd; }
    // ming add <<

    //<gino>
    // 獲得 Render2Texture支援之格式 (在使用與back buffer相同的DepthStencil下)
    D3DFORMAT GetRTTFormat(){ return m_RTTFmt; }

    void SetHighQualityScreenShot( bool bHigh ){ m_bHighQualityScreenShot = bHigh; }
    bool IsHighQualityScreenShot(){ return m_bHighQualityScreenShot; }

    void SetCallbackOnTalkScreen(_CallbackOnTakeScreenShot func){ m_cbOnTakeScreenShot = func;}
    //</gino>


	void SetPostPass(bool post) { m_bPostPass = post ; } 
	bool GetPostPass(void) const { return m_bPostPass ; }  
};



struct VertexPT
{
	VertexPT()
		:pos(0.0f, 0.0f, 0.0f),
		tex0(0.0f, 0.0f){}
	VertexPT(float x, float y, float z, 
		float u, float v):pos(x,y,z), tex0(u,v){}
	VertexPT(const D3DXVECTOR3& v, const D3DXVECTOR2& uv)
		:pos(v), tex0(uv){}

	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex0;
};

struct PostPassParameters
{
public:
	PostPassParameters()
	{
		Clear() ; 
	}
	PostPassParameters(const PostPassParameters &o)
	{
		*this = o ; 
	}
	void operator =(const PostPassParameters &o)
	{
		brightness	= o.brightness; 
		contrast	= o.contrast; 
		saturation	= o.saturation; 
		hue			= o.hue; 

		glowness	= o.glowness; 
		sceneness	= o.sceneness; 
		glowspan	= o.glowspan; 



		spotColor = o.spotColor  ;

		ambientColor = o.ambientColor  ; 
		angle	= o.angle; 
		azimuth = o.azimuth; 
		centerX = o.centerX; 
		centerY = o.centerY ; 
		distance = o.distance; 
		coneAngle = o.coneAngle; 
		intensity = o.intensity ; 

		lookupName = o.lookupName ; 
		sampleValue = o.sampleValue ; 

	}
	void Clear()
	{
		brightness	= 0.0f; 
		contrast	= 0.0f; 
		saturation	= 0.0f; 
		hue			= 0.0f; 

		glowness	= 0.0f; 
		sceneness	= 0.0f; 
		glowspan	= 0.0f; 



		spotColor.x = 0.0f  ;
		spotColor.y = 0.0f  ;
		spotColor.z = 0.0f  ;

		ambientColor.x = 0.0f  ; 
		ambientColor.y = 0.0f  ; 
		ambientColor.z = 0.0f  ; 
		angle	= 0.0f; 
		azimuth = 0.0f; 
		centerX = 0.0f; 
		centerY = 0.0f ; 
		distance = 0.0f; 
		coneAngle = 0.0f; 
		intensity = 0.0f ; 
		sampleValue = 0.0f ; 

		lookupName = "" ; 

	}
	float			brightness ; 
	float			contrast ; 
	float			saturation ; 
	float			hue ; 

	float			glowness ; 
	float			sceneness ; 
	float			glowspan ; 

	D3DXVECTOR3		spotColor ; 
	D3DXVECTOR3		ambientColor ; 
	float			angle ; 
	float			azimuth ; 
	float			centerX ; 
	float			centerY ; 
	float			distance ; 
	float			coneAngle ; 
	float			intensity ; 

	std::string			lookupName ; 
	float			sampleValue ; 
};

class CPostProcessFilter : public IRendererResource
{
public:
	CPostProcessFilter(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect);
	virtual ~CPostProcessFilter() ; 

public:
	virtual void ApplyFiler() = 0 ; 
	virtual void SetParameter(const PostPassParameters &params) = 0 ; 
	virtual void OnLostDevice() = 0 ; 
	virtual void OnResetDevice() = 0 ; 
public:
	virtual PostPassParameters* GetParameter() { return &m_params ; }
protected:
	virtual void SetWorldViewProjectionMat(const D3DXMATRIX &worldMat , const D3DXMATRIX &viewMat , const D3DXMATRIX &projMat) = 0 ;

private:
	void BeginViewport(const RECT *scissor) ;
	void EndViewport();
	void BeginOrtho2D(FLOAT w, FLOAT h); 
	void EndOrtho2D();
	void BeginProjection(const D3DXMATRIX &pj,const D3DXMATRIX &view) ; 
	void EndProjection() ; 
protected:
	void DrawBuffer(D3DXHANDLE techHandle , unsigned int pass , float width , float height) ;
protected:
	LPD3DXEFFECT						m_pEffect ;	
	LPDIRECT3DDEVICE9					m_pDevice ; 
	PostPassParameters					m_params ; 
private:
	D3DXMATRIX							m_tempProjection ; 
	D3DXMATRIX							m_tempView ; 
	D3DXMATRIX							m_tempWorld ;
	D3DVIEWPORT9						m_tempViewPort ; 


};	

class CColorMixer : public CPostProcessFilter
{
public:
	CColorMixer(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height ); 
	~CColorMixer() ; 
private:
	void SetWorldViewProjectionMat(const D3DXMATRIX &worldMat , const D3DXMATRIX &viewMat , const D3DXMATRIX &projMat);
public:
	void ApplyFiler() ; 
	void SetParameter(const PostPassParameters &params) {}
	void OnLostDevice()  {}; 
	void OnResetDevice()  {}; 
	void OnRestoreDevice(){} ; 
private:
	IDirect3DTexture9					*m_pTextureA ; 
	int									m_width ; 
	int									m_height ; 
	D3DXHANDLE							m_texHandle ; 
	D3DXHANDLE							m_techHandle ; 
	D3DXHANDLE							m_viewPortSizeHandle ; 

};


class CColorMatrix : public CPostProcessFilter
{
public:
	CColorMatrix(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height ); 
	~CColorMatrix() ; 
private:
	void SetWorldViewProjectionMat(const D3DXMATRIX &worldMat , const D3DXMATRIX &viewMat , const D3DXMATRIX &projMat);
public:
	void ApplyFiler() ;
	void SetParameter(const PostPassParameters &params); 
	void OnLostDevice()  ; 
	void OnResetDevice()  ; 
	void OnRestoreDevice(){} ; 

private:
	IDirect3DTexture9					*m_pTextureA ; 
	int									m_width ; 
	int									m_height ; 
	D3DXHANDLE							m_texHandle ; 
	D3DXHANDLE							m_techHandle ; 
	D3DXHANDLE							m_viewPortSizeHandle ; 

private:


	D3DXHANDLE							m_brightnessHandle ; 
	D3DXHANDLE							m_contrastHandle ; 
	D3DXHANDLE							m_saturationHandle ; 
	D3DXHANDLE							m_hueHandle ; 

};


class CGlow : public CPostProcessFilter
{
public:
	CGlow(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height ); 
	~CGlow() ; 
private:
	void SetWorldViewProjectionMat(const D3DXMATRIX &worldMat , const D3DXMATRIX &viewMat , const D3DXMATRIX &projMat);
public:
	void ApplyFiler() ; 
	void SetParameter(const PostPassParameters &params) ; 
	void OnLostDevice()  ; 
	void OnResetDevice()  ; 
	void OnRestoreDevice(){} ; 

private:
	IDirect3DTexture9					*m_pTextureA ; 
	IDirect3DTexture9					*m_pTextureB ; 
	IDirect3DTexture9					*m_pSceneTex ; 

	int									m_width ; 
	int									m_height ; 
	D3DXHANDLE							m_texAHandle ; 
	D3DXHANDLE							m_texBHandle ; 
	D3DXHANDLE							m_sceneHandle ; 

	D3DXHANDLE							m_techHandle ; 
	D3DXHANDLE							m_viewPortSizeHandle ; 



private:

	D3DXHANDLE							m_glownessHandle ; 
	D3DXHANDLE							m_scenenessHandle ; 
	D3DXHANDLE							m_glowspanHandle ; 

};

class CSpotLight : public CPostProcessFilter
{
public:
	CSpotLight(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height); 
	~CSpotLight() ; 
private:
	void SetWorldViewProjectionMat(const D3DXMATRIX &worldMat , const D3DXMATRIX &viewMat , const D3DXMATRIX &projMat) {}
public:
	void ApplyFiler() ; 
	void SetParameter(const PostPassParameters &params) ;
	void OnLostDevice()  ; 
	void OnResetDevice()  ; 
	void OnRestoreDevice(){} ; 

private:
	IDirect3DTexture9					*m_pTextureA ; 


	D3DXHANDLE							m_techHandle ;
	D3DXHANDLE							m_texHandle ; 

	D3DXHANDLE							m_viewPortSizeHandle ; 
	int									m_width ; 
	int									m_height ; 

	D3DXHANDLE	m_spotColorHandle ; 
	D3DXHANDLE	m_ambientHandle ; 
	D3DXHANDLE	m_angleHandle ; 
	D3DXHANDLE	m_azimuthHandle ; 
	D3DXHANDLE	m_centerXHandle ; 
	D3DXHANDLE	m_centerYHandle ; 
	D3DXHANDLE	m_distanceHandle ; 
	D3DXHANDLE	m_coneAngleHandle ; 
	D3DXHANDLE	m_intensityHandle ; 

	
};


class CEasyColorMixer : public CPostProcessFilter
{
public:
	CEasyColorMixer(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height ); 
	~CEasyColorMixer() ; 
private:
	void SetWorldViewProjectionMat(const D3DXMATRIX &worldMat , const D3DXMATRIX &viewMat , const D3DXMATRIX &projMat) {}
public:
	void ApplyFiler() ; 
	void SetParameter(const PostPassParameters &params) ;
	void OnLostDevice()  ; 
	void OnResetDevice()  ; 
	void OnRestoreDevice(){} ; 

private:
	IDirect3DTexture9					*m_pTextureA ; 
	IDirect3DTexture9					*m_plookupTex ; 

	int									m_width ; 
	int									m_height ; 
	D3DXHANDLE							m_texHandle ; 
	D3DXHANDLE							m_techHandle ; 
	D3DXHANDLE							m_viewPortSizeHandle ; 

	D3DXHANDLE							m_texlookupHanlde; 
	D3DXHANDLE							m_sampleValueHanlde; 

	CAllDay								*m_pDay ; 
};


class CAllDay 
{
public:
	CAllDay() ; 
	~CAllDay(); 
public:
	void Register(PostPassParameters *pParam) ; 
	void Update() ; 
	void TestUpdate() ; 
private:
	PostPassParameters	*m_pEasyColorMixerParam ; 
	
	DWORD				m_startTime ; 
	DWORD				m_endTime ; 
	DWORD				m_tempTime ; 

	static DWORD		s_oneDayTime ; 
};

//---------------------------------------------------------------------------
} //namespace FDO
//---------------------------------------------------------------------------
#endif
