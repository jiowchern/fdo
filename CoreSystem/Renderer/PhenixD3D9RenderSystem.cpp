#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "PhenixD3D9RenderSystem.h"
#include "PhenixAssert.h"
#include "iniReader.h"
#include "FDOUT.h"
#include "FOVClip.h"  // 剪裁員
#include "JLCamera.h"
#include "JLAnimation2DManager.h"
#include <stdio.h>
#include "J_SoundManager.h"
#include "iniReader.h"
//#include "Global.h"
//#include "HotKey.h"
//#include "UIManager.h"
#include "FDOJPEG.h" // for CompressToJPEG()
#include "Common.h" // for UnicodeToAnsi()
//#include "Debug.h"
//#include "BinaryTableManager.h"
#include "DrawPlane.h"
#include "RenderTarget.h"

//---------------------------------------------------------------------------

// Macro
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//---------------------------------------------------------------------------
namespace FDO
{

	DWORD CAllDay::s_oneDayTime = 24 * 3600 ; 

PostBinData g_postBinData[PostEnd - PostBeging - 1] = 
{
	{ 0 , COLORMATRIX } ,
	{ 1 , GLOW } ,
	{ 2 , SPOTLIGHT } , 
	{ 3 , EASYCOLORMIXER} , 

};


//---------------------------------------------------------------------------
D3D9RenderSystem::D3D9RenderSystem() : m_bHighQualityScreenShot(true) , m_postEffectVec()
{
    mpD3D = Direct3DCreate9(D3D_SDK_VERSION);
    //string str;
    //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 660, CodeInlineText_Text ) );//建立
   // str.append( "Direct3D9" );
    //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 1144, CodeInlineText_Text ) );//物件
    //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
    Assert( mpD3D != NULL, "建立 Direct3D9 物件失敗" );
    mpD3DDevice         = NULL;
    mActiveD3DDriver    = NULL;
    mHWnd               = NULL;
    mClientWidth        = 0;
    mClientHeight       = 0;
	iWidth				= 0;
	iHeight				= 0;
    mWindowedStyle      = 0;

    mIsFullScreen       = false;
    mIsVSync            = false;
    mIsDeviceLost       = false;
    mIsPrintScreen      = false;
    m_bRestoreVis       = false;
    mDevType            = D3DDEVTYPE_HAL;
    mClearFlags         = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
    mColour             = D3DCOLOR_ARGB(0, 0, 0, 170);
    mFSAAType           = D3DMULTISAMPLE_NONE;
    mFSAAQuality        = 0;

    mfFPS               = 0.0f;
    mFrameCount         = 0;
    mLastTime           = 0;
    mFrameStatus1[0]    = '\0';
    mFrameStatus2[0]    = '\0';
    mFrameStatus3[0]    = '\0';
    mFrameStatus4[0]    = '\0';
    SetRect( &mRect1, 5,  5, 1024, 20 );
    SetRect( &mRect2, 5, 22, 1024, 42 );
    SetRect( &mRect3, 5, 39, 1024, 59 );
    SetRect( &mRect4, 5, 56, 1024, 76 );
    mIsShowFrameStatus  = false;

    // ming add >> initial callback function ptr
    m_DeviceCreatedFunc   = 0;
    m_DeviceResetFunc     = 0;
    m_DeviceLostFunc      = 0;
    m_DeviceDestroyedFunc = 0;
    // ming add <<

    //<gino>
    m_bPauseRender = false;
    m_RTTFmt = D3DFMT_UNKNOWN;
    //m_bHighQualityScreenShot = true;
    m_cbOnTakeScreenShot = NULL;
    //</gino>


	m_forceCount = 0;
	m_forceManagerCount = 0;
	m_caliberCount = 0;
	m_colorCount = 0;
	m_scaleCount = 0;
	m_audioCount = 0;
	m_textureCount = 0;
	m_multiTexCount = 0;
	m_actionManagerCount = 0;
	m_emitterCount = 0;

	m_bPostPass = true ; 

	mTexLODBias = 0;

	ClearInformation() ; 

    InitConfigOptions();
}
//---------------------------------------------------------------------------
D3D9RenderSystem::~D3D9RenderSystem()
{
    mpFont.reset();
    mpFont2.reset();
    Shutdown();
    SAFE_RELEASE( mpD3D );

	for(unsigned int i = 0 ; i < this->mRendererResList.size() ; ++i)
	{
		SAFE_DELETE(mRendererResList[i]) ; 
	}
	mRendererResList.clear();
	mRenderTargetList.clear();
}

CPostProcessFilter* D3D9RenderSystem::CreateEffect(LPD3DXEFFECT pEffect , unsigned int id)
{
	CPostProcessFilter *pPostPass = NULL ; 

	switch(id)
	{
	case 0:
		{
			pPostPass = new CColorMatrix(GetD3DDevice() , pEffect , GetBackBufferWidth(), GetBackBufferHeight()) ; 

		}break ; 
	case 1:
		{
			pPostPass = new CGlow(GetD3DDevice() , pEffect , GetBackBufferWidth() ,GetBackBufferHeight()); 

		}break ; 
	case 2:
		{
			pPostPass = new CSpotLight(GetD3DDevice() , pEffect , GetBackBufferWidth() ,GetBackBufferHeight()); 
		}break ; 
	case 3:
		{
			pPostPass = new CEasyColorMixer(GetD3DDevice() , pEffect , GetBackBufferWidth() ,GetBackBufferHeight()); 
		}break ; 
	default:
		return pPostPass ; 
		break ; 
	};

	return pPostPass ; 
}


void D3D9RenderSystem::UnApplyFilter()
{
	for(unsigned int i = 0 ; i < m_postEffectVec.size(); ++i)
	{
		m_postEffectVec[i].bUsed = false ; 
	}
}

PostPassParameters* D3D9RenderSystem::GetParameter(enPostType type)
{
	for(unsigned int i = 0 ; i < m_postEffectVec.size(); ++i)
	{
		if(m_postEffectVec[i].type == type)
		{
			return m_postEffectVec[i].pPostPass->GetParameter() ; 
		}
	}
	return NULL;
}



CPostProcessFilter* D3D9RenderSystem::LoadEffect(unsigned int id , const std::string &filename)
{
	CPostProcessFilter *pPostPass = NULL ; 



	if(PostEnd - PostBeging - 1 < id)
	{
		return pPostPass; 
	}

	for(unsigned int i = 0 ; i < m_postEffectVec.size(); ++i)
	{
		if(m_postEffectVec[i].id == id)
		{
			m_postEffectVec[i].bUsed = true ; 

			return pPostPass; 
		}
	}

	memPack* pMem =	LoadFromPacket(filename) ; 

	if(pMem->Data == NULL)
	{
		return pPostPass; 
	}

	DWORD flag = D3DXFX_NOT_CLONEABLE;
#if D3DX_SDK_VERSION >31
			flag |= D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;
#endif 
	
	LPD3DXEFFECT pEffect = NULL ; 


	if(SUCCEEDED(D3DXCreateEffect(GetD3DDevice() , pMem->Data , pMem->DataSize , 
		NULL , NULL , flag , NULL , &pEffect , NULL)))
	{
		
		PostEffect effect ; 
		pPostPass = CreateEffect(pEffect , id) ; 

		effect.id = id ; 
		effect.type = g_postBinData[id].type ; 
		effect.pPostPass =  pPostPass ; 
		effect.bUsed = true ; 

		m_postEffectVec.push_back(effect) ; 
		if(pPostPass!=NULL)
		{
			this->mRendererResList.push_back(pPostPass);
		}
	}

	return pPostPass ; 
}

void D3D9RenderSystem::RunPrePass()
{
	if(!GetPostPass())
	{
		return ; 
	}

	for(unsigned int i = 0 ; i < m_postEffectVec.size(); ++i)
	{
		if(m_postEffectVec[i].bUsed && m_postEffectVec[i].id == 3)
		{
			m_postEffectVec[i].pPostPass->ApplyFiler() ; 
		}
	}	

}
 
void D3D9RenderSystem::RunPostPass()
{
	if(!GetPostPass())
	{
		return ; 
	}

	for(unsigned int i = 0 ; i < m_postEffectVec.size(); ++i)
	{
		if(m_postEffectVec[i].bUsed)
		{
			m_postEffectVec[i].pPostPass->ApplyFiler() ; 
		}
	}
}


//---------------------------------------------------------------------------
D3D9DriverList* D3D9RenderSystem::GetDirect3DDrivers()
{
    if( !mDriverList.get() )
        mDriverList.reset( new D3D9DriverList( mpD3D ) );

    return mDriverList.get();
}

inline ConfigOption GetConfigOption(const string &keyname, const string &defaultValue)
{
	ConfigOption option;
	option.name = keyname;
	option.currentValue =  GetConfig(keyname);
	if(option.currentValue.empty())
		option.currentValue =  defaultValue;
	return option;
}

//---------------------------------------------------------------------------
void D3D9RenderSystem::InitConfigOptions()
{
    if( GetConfig( "_DEBUG" ) == "true" )
        mIsShowFrameStatus  = true;

    //取第一個為預設的顯示卡
    D3D9Driver* driver = this->GetDirect3DDrivers()->item(0);

    //設定顯示卡
    ConfigOption optAdapter = GetConfigOption("Display Adapter", driver->DriverDescription());
    //設定裝置型態
    ConfigOption optDeviceType = GetConfigOption("Device Type", "D3DDEVTYPE_HAL");
    //設定解析度
    ConfigOption optVideoMode = GetConfigOption("Video Mode", "800 x 600 x 16");
    //設定全螢幕
    ConfigOption optFullScreen = GetConfigOption("Full Screen", "No");

    // GM用
#if defined _FDO_GM
    optFullScreen.currentValue = "No";
#endif

    //設定同步更新
    ConfigOption optVSync = GetConfigOption("VSync", "Yes");

    //設定反鋸齒
    ConfigOption optAA = GetConfigOption("Anti aliasing", "None");

    //儲存設定
    mOptions.insert(make_pair( optAdapter.name, optAdapter ));
    mOptions.insert(make_pair( optDeviceType.name, optDeviceType ));
    mOptions.insert(make_pair( optVideoMode.name, optVideoMode ));
    mOptions.insert(make_pair( optFullScreen.name, optFullScreen ));
    mOptions.insert(make_pair( optVSync.name, optVSync ));
    mOptions.insert(make_pair( optAA.name, optAA ));
//
    mDeviceStatus.clear();
    mDeviceStatus.push_back( optAdapter.currentValue );
    if( optDeviceType.currentValue == "D3DDEVTYPE_HAL" )
        mDeviceStatus.push_back( "HAL" );
    else if( optDeviceType.currentValue == "D3DDEVTYPE_REF" )
        mDeviceStatus.push_back( "REF" );
    mDeviceStatus.push_back( optVideoMode.currentValue );
    mDeviceStatus.push_back( "VSync_" + optVSync.currentValue );
    mDeviceStatus.push_back( "AntiAliasing_" + optAA.currentValue );
}

//---------------------------------------------------------------------------
bool D3D9RenderSystem::Initialize(HWND externalHandle)
{
    // Init using current settings
    mHWnd = externalHandle;

    // 取得視窗型態
    mWindowedStyle = GetWindowLong( mHWnd, GWL_STYLE );

    mActiveD3DDriver = NULL;

    //設定顯示卡
    ConfigOptionMap::iterator opt = mOptions.find( "Display Adapter" );

    for( size_t j=0; j < GetDirect3DDrivers()->count(); ++j )
    {
        if( GetDirect3DDrivers()->item(j)->DriverDescription() == opt->second.currentValue )
        {
            mActiveD3DDriver = GetDirect3DDrivers()->item(j);
            break;
        }
    }

    //設定預設顯示卡
    if( mActiveD3DDriver == NULL )
        mActiveD3DDriver = GetDirect3DDrivers()->item(0);

    //設定裝置型態
    mDevType = D3DDEVTYPE_HAL;

    //設定全螢幕
    opt = mOptions.find( "Full Screen" );

    mIsFullScreen = opt->second.currentValue == "Yes";

    D3D9VideoMode* videoMode = NULL;
    std::string temp;

    //設定解析度
    opt = mOptions.find( "Video Mode" );
    for( size_t j=0; j < mActiveD3DDriver->getVideoModeList()->count(); ++j )
    {
        temp = mActiveD3DDriver->getVideoModeList()->item(j)->getDescription();
        if( temp == opt->second.currentValue )
        {
            videoMode = mActiveD3DDriver->getVideoModeList()->item(j);
            break;
        }
    }
	if(videoMode!=NULL)
	{
		mWidth = videoMode->getWidth();
		mHeight = videoMode->getHeight();
		mColourDepth = videoMode->getColourDepth();
	}
	else
	{
		mWidth = 800;
		mHeight = 600;
		mColourDepth = 16;
	}


    //設定同步更新
    opt = mOptions.find( "VSync" );

    mIsVSync = ( opt->second.currentValue == "Yes" );

    //設定反鋸齒
    opt = mOptions.find( "Anti aliasing" );
 
    if (opt->second.currentValue == "None")
    {
        mFSAAType = D3DMULTISAMPLE_NONE;
        mFSAAQuality = 0;
    }
    else
    {
        D3DMULTISAMPLE_TYPE fsaa = D3DMULTISAMPLE_NONE;
        DWORD level = 0;

        if( strstr( opt->second.currentValue.c_str(), "NonMaskable" ) != NULL )
        {
            fsaa = D3DMULTISAMPLE_NONMASKABLE;
            size_t pos = opt->second.currentValue.find_last_of(" ");
            std::string sNum = opt->second.currentValue.substr(pos + 1);
            level = atoi(sNum.c_str());
        }
        else if( strstr( opt->second.currentValue.c_str(), "Level" ) != NULL )
        {
            size_t pos = opt->second.currentValue.find_last_of(" ");
            std::string sNum = opt->second.currentValue.substr(pos + 1);
            fsaa = (D3DMULTISAMPLE_TYPE)atoi(sNum.c_str());
        }

        mFSAAType = fsaa;
        mFSAAQuality = level;
    }

    // 設定視窗型態
	int extenSizeX = 0;
	int extenSizeY = 0;
    if( mIsFullScreen )
        SetWindowLong( mHWnd, GWL_STYLE, mWindowedStyle&(~WS_CAPTION) );
	else
	{
		//視窗模式下長寬修正的size
		int borderSize = ::GetSystemMetrics(SM_CYFIXEDFRAME);
		extenSizeX  = borderSize*2;
		extenSizeY = ::GetSystemMetrics(SM_CYCAPTION)+borderSize;
	}
    // 改變視窗位置大小
	int s_width, s_height;
	s_width = GetSystemMetrics(SM_CXSCREEN);
	s_height = GetSystemMetrics(SM_CYSCREEN);


	SetWindowPos( mHWnd, HWND_NOTOPMOST, ((s_width/2)-(mWidth/2)), ((s_height/2)-(mHeight/2)), mWidth+extenSizeX, mHeight+extenSizeY, SWP_HIDEWINDOW );


    //初始D3D環境
    if( !InitializeD3D() )
    {
        //更改D3D環境顏色深度
        if( mColourDepth == 16 )
            mColourDepth = 32;
        else
            mColourDepth = 16;

        return  InitializeD3D();
    }
    else
        return true;
}
//---------------------------------------------------------------------------
bool D3D9RenderSystem::InitializeD3D()
{
    // 取得視窗內容大小
    GetClientRect( mHWnd, &mRcWindowClient );
    mClientWidth  = mRcWindowClient.right - mRcWindowClient.left;
    mClientHeight = mRcWindowClient.bottom - mRcWindowClient.top;

	iWidth = mClientWidth;
	iHeight = mClientHeight;

    unsigned int iWidth;
    unsigned int iHeight;

    if( mIsFullScreen )
    {
        iWidth  = mWidth;
        iHeight = mHeight;
    }
    else
    {
        iWidth  = mClientWidth;
        iHeight = mClientHeight;
    }

    HRESULT hr;
    LPDIRECT3D9 pD3D = mActiveD3DDriver->getD3D();

    ZeroMemory( &md3dpp, sizeof(D3DPRESENT_PARAMETERS) );
    md3dpp.Windowed					= !mIsFullScreen;
    md3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;
    md3dpp.BackBufferCount			= 1;
    md3dpp.EnableAutoDepthStencil	= true;
    md3dpp.hDeviceWindow			= mHWnd;
    md3dpp.BackBufferWidth			= iWidth;
    md3dpp.BackBufferHeight			= iHeight;

    if (mIsVSync)
        md3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    else
        md3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;


    if( mColourDepth > 16 )
    {
        md3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
        mDeviceStatus.push_back( "X8R8G8B8" );
    }
    else
    {
        md3dpp.BackBufferFormat	= D3DFMT_R5G6B5;
        mDeviceStatus.push_back( "R5G6B5" );
    }

    if (mColourDepth > 16 )
    {
        // Try to create a 32-bit depth, 8-bit stencil
        if( FAILED( pD3D->CheckDeviceFormat(mActiveD3DDriver->getAdapterNumber(),
            mDevType,  md3dpp.BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL,
            D3DRTYPE_SURFACE, D3DFMT_D24S8 )))
        {
            // Bugger, no 8-bit hardware stencil, just try 32-bit zbuffer
            if( FAILED( pD3D->CheckDeviceFormat(mActiveD3DDriver->getAdapterNumber(),
                mDevType,  md3dpp.BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL,
                D3DRTYPE_SURFACE, D3DFMT_D32 )))
            {
                // Jeez, what a naff card. Fall back on 16-bit depth buffering
                md3dpp.AutoDepthStencilFormat = D3DFMT_D16;
                mDeviceStatus.push_back( "D16" );
            }
            else
            {
                md3dpp.AutoDepthStencilFormat = D3DFMT_D32;
                mDeviceStatus.push_back( "D32" );
            }
        }
        else
        {
            if( SUCCEEDED( pD3D->CheckDepthStencilMatch( mActiveD3DDriver->getAdapterNumber(), mDevType,
                md3dpp.BackBufferFormat, md3dpp.BackBufferFormat, D3DFMT_D24S8 ) ) )
            {
                md3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
                mDeviceStatus.push_back( "D24S8" );
            }
            else
            {
                md3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
                mDeviceStatus.push_back( "D24X8" );
            }
        }
    }
    else
    {
        // 16-bit depth, software stencil
        md3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
        mDeviceStatus.push_back( "D16" );
    }

	//檢查可使用的Antialising模式
	UINT adapterNo =  mActiveD3DDriver->getAdapterNumber(); 
	BOOL windowed = mIsFullScreen?FALSE:TRUE;
	while(mFSAAType!=D3DMULTISAMPLE_NONE)
	{
		if( SUCCEEDED(pD3D->CheckDeviceMultiSampleType(adapterNo, mDevType, md3dpp.BackBufferFormat, windowed, mFSAAType, NULL))&&
			SUCCEEDED(pD3D->CheckDeviceMultiSampleType(adapterNo, mDevType, md3dpp.AutoDepthStencilFormat, windowed, mFSAAType, NULL)) )
		{
			break;
		}

		mFSAAType = (D3DMULTISAMPLE_TYPE)(mFSAAType-1);
	}

    md3dpp.MultiSampleType = mFSAAType;
    md3dpp.MultiSampleQuality = (mFSAAQuality == 0) ? NULL : mFSAAQuality;
	md3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL ; 
	//check d3d caps
    D3DCAPS9 caps;
	pD3D->GetDeviceCaps( mActiveD3DDriver->getAdapterNumber(), mDevType, &caps ) ;


	DWORD vpFlag = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
    // then switch to SWVP.
    if( ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        vpFlag = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
	else
	{
		vpFlag = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}

    // If the hardware cannot do vertex blending, use software vertex processing.
    if( caps.MaxVertexBlendMatrices < 2 )
        vpFlag = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    // If using hardware vertex processing, change to mixed vertex processing
	// so there is a fallback. It would try to use HW for VP but if failed, it use SW. for compatible with old HW
	
  //  if( vpFlag & D3DCREATE_HARDWARE_VERTEXPROCESSING )
   //     vpFlag = D3DCREATE_MIXED_VERTEXPROCESSING;    


    hr = pD3D->CreateDevice( mActiveD3DDriver->getAdapterNumber(), mDevType, mHWnd,
        vpFlag, &md3dpp, &mpD3DDevice );

	switch(vpFlag)
	{
	case D3DCREATE_HARDWARE_VERTEXPROCESSING: 
		mDeviceStatus.push_back( "hw vp" ); break;
	case D3DCREATE_MIXED_VERTEXPROCESSING: 
		mDeviceStatus.push_back( "mixed vp" ); break;
	case D3DCREATE_SOFTWARE_VERTEXPROCESSING: 
		mDeviceStatus.push_back( "sw vp" ); break;
	}

    if( FAILED( hr ) )
    {
        SAFE_RELEASE( mpD3DDevice );
        //MessageBox(NULL, L"建立Direct3D9裝置失敗", L"D3D9RenderSystem::InitializeD3D", NULL );
        return false;
    }

    //<gino>
    // Check if the DepthStencil can work with D3DFMT_R5G6B5 rendertarget
    // if yes. use D3DFMT_R5G6B5 as RTT format; otherwise, use the same format with backbuffer
    //if( SUCCEEDED( pD3D->CheckDepthStencilMatch( mActiveD3DDriver->getAdapterNumber(), mDevType,
    //    md3dpp.BackBufferFormat, D3DFMT_R5G6B5, md3dpp.AutoDepthStencilFormat ) ) ){
    //    m_RTTFmt = D3DFMT_R5G6B5;
    //}else{
    //    m_RTTFmt =  md3dpp.BackBufferFormat;
    //}

	m_RTTFmt =  md3dpp.BackBufferFormat;

    //</gino>

    g_pD3DDevice = mpD3DDevice;

    // ming add >>
    if( m_DeviceCreatedFunc != NULL )
        m_DeviceCreatedFunc( mpD3DDevice );
    // ming add <<

    RestoreDeviceObjects();
    mpFont.reset( new CD3DFont( mpD3DDevice, 9, L"Arial" ) );
    mpFont2.reset( new CD3DFont( mpD3DDevice, 9, L"Arial" ) );

	//	ShowWindow(mHWnd , true) ; 



    return true;
}
//---------------------------------------------------------------------------
bool D3D9RenderSystem::HandlePossibleSizeChange()
{
    if( mIsFullScreen )
        return true;

    // 取得視窗內容大小
    RECT rcNewWinClient;
    GetClientRect(mHWnd, &rcNewWinClient);

    // Check if the window client rect has changed
  //  if( mRcWindowClient.right - mRcWindowClient.left != rcNewWinClient.right - rcNewWinClient.left ||
   //     mRcWindowClient.bottom - mRcWindowClient.top != rcNewWinClient.bottom - rcNewWinClient.top )
    {
        mRcWindowClient         = rcNewWinClient;
        mClientWidth            = mRcWindowClient.right - mRcWindowClient.left;
        mClientHeight           = mRcWindowClient.bottom - mRcWindowClient.top;
        md3dpp.BackBufferWidth  = mClientWidth;
        md3dpp.BackBufferHeight = mClientHeight;

        if( mpD3DDevice )
        {
            //在於視窗縮到最小時候，不需要再次作Reset的動作。
            //還原的時候，才要作Reset的動作。不然的話，當d3d呼叫TestCooperativeLevel時候，
            //會出現D3DERR_INVALIDCALL的嚴重錯誤，出現這種錯誤的可能性很多。
            //最後發現到下面這些是多餘的，也就是不正確的程序。

           // InvalidateDeviceObjects();

           // HRESULT hr = mpD3DDevice->Reset( &md3dpp );
           // if( FAILED( hr ) )
           //     return false;

           //RestoreDeviceObjects();
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void D3D9RenderSystem::RestoreDeviceObjects()
{
    Assert( mpD3DDevice != NULL, "mpD3DDevice is NULL !" );

    //建立字型
    if( mpFont.get() )
        mpFont->CreateFont();

    if( mpFont2.get() )
        mpFont2->CreateFont();

    //設定投影矩陣
    D3DXMATRIX matProj;
    float fovy   = D3DX_PI * 0.125f;         // 0.25f
    float Aspect = (float)md3dpp.BackBufferWidth / (float)md3dpp.BackBufferHeight;
    float zn     = 1.0f;
    float zf     = 700.0f;

    g_Camera.SetProjectMatrix(fovy, Aspect, zn, zf);

    // 宣告視野剪裁員
    static FOVClip  s_FOVClip;
    s_FOVClip.SetupFOVClipPlanes( fovy*2.0f, Aspect, zn, zf );

    // 限制滑鼠移動範圍
    if( mIsFullScreen )
    {
        //HCURSOR hCursor;
        //hCursor = (HCURSOR)GetClassLongPtr( mHWnd, GCLP_HCURSOR );
        //DXUTSetDeviceCursor( mpD3DDevice, hCursor, false );
        //mpD3DDevice->ShowCursor( true );

        RECT rcClipCursor;
        GetWindowRect( mHWnd, &rcClipCursor );
        rcClipCursor.top    = 0;
        rcClipCursor.left   = 3;
        rcClipCursor.right  -= 3;
        //rcClipCursor.bottom -= 3;
        ClipCursor( &rcClipCursor );
    }

    /*
    else
    {
        rcClipCursor.top = rcClipCursor.bottom - mClientHeight;
    }

    rcClipCursor.left   = 3;
    rcClipCursor.right  -= 3;
    rcClipCursor.bottom -= 3;
    ClipCursor( &rcClipCursor );
    */

    // 初始化淡出設定
    g_Animation2DManager.RestoreDeviceObjects();

    // 設定繪圖狀態
    SetupStates();

    // ming add >>
    if( m_DeviceResetFunc != NULL )
        m_DeviceResetFunc( mpD3DDevice );
    // ming add <<
}
//---------------------------------------------------------------------------
void D3D9RenderSystem::SetupStates()
{
    mpD3DDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    mpD3DDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );

    //開啟 ZBuffer
    mpD3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    //開啟三角形雙面繪製
    mpD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW ); // D3DCULL_NONE, D3DCULL_CCW, D3DCULL_CW

    //設定描繪方式(FLAT,GOURAUD,PHONG)
    mpD3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

    //設定光源
    mpD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
    mpD3DDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 255, 255, 255 ) );

    //設定材質運算模式
    mpD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    mpD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    mpD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

    //設定 Alpha 混色模式
    mpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    mpD3DDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
    mpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    //設定 alpha test
    mpD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);               // 開啟alpha test
    mpD3DDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000010);         // 設定比較alpha的參考值
    mpD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);      // 像素值大於或等於目前像素值則通過測試

    //設定材質渲染模式
    mpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP  , D3DTOP_MODULATE );
    mpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE   );
    mpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE   );
    mpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP  , D3DTOP_MODULATE );
    mpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE   );
    mpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE   );

    //
    // Setup a directional light.
    //
    D3DLIGHT9 light;
    memset( &light, 0, sizeof(light) );

    D3DXCOLOR color = D3DCOLOR_XRGB( 94, 118, 149 );
    D3DXVECTOR3 dir = D3DXVECTOR3( 0.0f, -0.577f, -1.0f );

    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    light.Direction = dir;
	light.Type = D3DLIGHT_DIRECTIONAL;

    mpD3DDevice->SetLight(3, &light);
    mpD3DDevice->LightEnable(3, true);
    //-----------------------------------------------------------------------
    color = D3DCOLOR_XRGB( 128, 152, 194 );
    dir = D3DXVECTOR3( 1.0f, -0.577f, 0.0f );

    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    light.Direction = dir;

    mpD3DDevice->SetLight(4, &light);
    mpD3DDevice->LightEnable(4, true);
    //-----------------------------------------------------------------------
    color = D3DCOLOR_XRGB( 116, 120, 151 );
    dir = D3DXVECTOR3( -1.0f, 0.577f, 0.0f );

    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    light.Direction = dir;

    mpD3DDevice->SetLight(5, &light);
    mpD3DDevice->LightEnable(5, true);
    //-----------------------------------------------------------------------
    color = D3DCOLOR_XRGB( 20, 20, 25 );
    dir = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );

    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    light.Direction = dir;

    mpD3DDevice->SetLight(6, &light);
    mpD3DDevice->LightEnable(6, true);
/*
    D3DXCOLOR color = D3DCOLOR_XRGB(213,215,224);
    D3DXVECTOR3 dir(0.0f,0.577f,1.0f);

    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));
    light.Type      = D3DLIGHT_DIRECTIONAL;
    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    light.Direction = dir;

    mpD3DDevice->SetLight(0, &light);
    mpD3DDevice->LightEnable(0, true);

    light.Direction.z = -1.0f;
    light.Direction.y = -0.577f;
    color = D3DCOLOR_XRGB(100,111,147);
    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    mpD3DDevice->SetLight(3, &light);
    mpD3DDevice->LightEnable(3, true);

    light.Direction.z = 0.0f;
    light.Direction.x = 1.0f;
    light.Direction.y = -0.577f;
    color = D3DCOLOR_XRGB(141,168,204);
    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    mpD3DDevice->SetLight(4, &light);
    mpD3DDevice->LightEnable(4, true);

    light.Direction.x = -1.0f;
    light.Direction.y = 0.577f;
    color = D3DCOLOR_XRGB(133,129,163);
    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    mpD3DDevice->SetLight(5, &light);
    mpD3DDevice->LightEnable(5, true);

    light.Direction.x = 0.0f;
    light.Direction.y = -1.0f;
    color = D3DCOLOR_XRGB(250, 250, 220);
    light.Ambient   = color ;
    light.Diffuse   = color ;
    light.Specular  = color ;
    mpD3DDevice->SetLight(6, &light);
    mpD3DDevice->LightEnable(6, true);
*/
}
//---------------------------------------------------------------------------
void D3D9RenderSystem::InvalidateDeviceObjects()
{
    if( mpFont.get() )
        mpFont->RemoveFont();

    if( mpFont2.get() )
        mpFont2->RemoveFont();

    g_Animation2DManager.InvalidateDeviceObjects();

    // ming add >>
    if( m_DeviceLostFunc != NULL )
        m_DeviceLostFunc();
    // ming add <<
}
//---------------------------------------------------------------------------
void D3D9RenderSystem::Shutdown()
{
    // ming add >>
    if( m_DeviceLostFunc != NULL )
        m_DeviceLostFunc();

    if( m_DeviceDestroyedFunc != NULL )
        m_DeviceDestroyedFunc();
    // ming add <<

    //<gino move>
    //the D3DDevice must be destroy after ALL D3D-Resources have been released!
    SAFE_RELEASE( mpD3DDevice );
    mActiveD3DDriver = NULL;
    //</gino>
}
//---------------------------------------------------------------------------
bool D3D9RenderSystem::BeginFrame()
{
    HRESULT hr;

    ///<gino>
    if(m_bPauseRender) return false;
    ///</gino>

    if( mIsDeviceLost )
    {
        Sleep( 500 );

        if( FAILED( hr = mpD3DDevice->TestCooperativeLevel() ) )
        {
			//D3DERR_INVALIDCALL
            if( hr == D3DERR_DEVICELOST )
                return false;

            if( hr == D3DERR_DEVICENOTRESET )
            {

				//這段程式碼，必須寫在InvalidateDeviceObjects之前
				for(unsigned int i = 0 ; i < this->mRendererResList.size() ; ++i)
				{
					mRendererResList[i]->OnLostDevice();
				}

                InvalidateDeviceObjects();



                hr = mpD3DDevice->Reset( &md3dpp );


                if( FAILED( hr ) )
                    return false;

				for(unsigned int i = 0 ; i < mRendererResList.size() ; ++i)
				{
					mRendererResList[i]->OnResetDevice();
				}

				RestoreDeviceObjects();
            }

            return false;
        }

        mIsDeviceLost = false;
    }

    string str;
    if( FAILED( hr = mpD3DDevice->Clear(0, 0, mClearFlags, mColour, 1.0f, 0 ) ) )
    {
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 712, CodeInlineText_Text ) );//清除
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 1146, CodeInlineText_Text ) );//背景
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        Assert( 0, "清除背景失敗" );
        return false;
    }

    if( FAILED( hr = mpD3DDevice->BeginScene() ) )
    {
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 1147, CodeInlineText_Text ) );//開始
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 1148, CodeInlineText_Text ) );//繪圖
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        Assert( 0, "開始繪圖失敗");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool D3D9RenderSystem::EndFrame()
{
    

    //不檢查,因為 DEVICE LOST 會使 EndScene() 失敗
    mpD3DDevice->EndScene();

    //抓取螢幕畫面
    if( mIsPrintScreen )
    {
        TakeScreenShot();
        mIsPrintScreen = false;
    }

    HRESULT hr;
    if( FAILED( hr = mpD3DDevice->Present(0, 0, 0, 0) ) )
    {
        if(hr == D3DERR_DEVICELOST)
        {
            mIsDeviceLost = true;
        }
        else
        {
            string str;
            //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 1149, CodeInlineText_Text ) );//顯示
            //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 1148, CodeInlineText_Text ) );//繪圖
            //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
            Assert( 0, "顯示繪圖失敗" );
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
void D3D9RenderSystem::DrawText( LPCTSTR pString, LPRECT pRect, DWORD Color, bool IsShade )
{
//     if( mpFont.get() )
//         mpFont->DrawText( pString, pRect, Color, IsShade );
}
//---------------------------------------------------------------------------
void D3D9RenderSystem::DrawText2( LPCTSTR pString, LPRECT pRect, DWORD Color, bool IsShade )
{
//     if( mpFont2.get() )
//         mpFont2->DrawText( pString, pRect, Color, IsShade );
}

void D3D9RenderSystem::DrawText2( LPCTSTR pString, INT cch, LPRECT pRect, DWORD Color, bool IsShade )
{
//     if( mpFont2.get() )
//         mpFont2->DrawText( pString, cch, pRect, Color, IsShade );
}

INT D3D9RenderSystem::CalcRect(LPCTSTR pString, INT cch, LPRECT pRect)
{
    if (mpFont2.get())
        return mpFont2->CalcRect(pString, cch, pRect);
    return 0;
}
//---------------------------------------------------------------------------
void D3D9RenderSystem::UpdateFrameStatus()
{
	if(mIsShowFrameStatus == false)
		return ;

    ++mFrameCount;
    DWORD NowTime = timeGetTime();

	RECT mRect5;
	RECT tempRect(mRect1) ; 
	mRect5 = mRect4;
	OffsetRect(&mRect5,0,20);

    if( NowTime - mLastTime > 1000 )
    {
        mfFPS = ( (float)mFrameCount / (float)(NowTime - mLastTime) ) * 1000.0f;
        mLastTime = NowTime ;
        mFrameCount = 0;

		POINT ptPos;
		::GetCursorPos(&ptPos);
		::ScreenToClient(GetHWND(),&ptPos);


        char FrameStatus[128];
        sprintf( FrameStatus, "%0.2f fps Pos ( %d, %d )", mfFPS ,
                 ptPos.x , ptPos.y );

        MultiByteToWideChar( CP_ACP, 0, FrameStatus, -1, mFrameStatus1, 128 );

        //sprintf( FrameStatus, "%s (%s) : %s", mDeviceStatus[1].c_str(),
        //          mDeviceStatus[7].c_str(), mDeviceStatus[0].c_str() );

        //MultiByteToWideChar( CP_ACP, 0, FrameStatus, -1, mFrameStatus2, 128 );

        //float fAngleV = g_Camera.GetAngleV();
        //float fAngleH = g_Camera.GetAngleH();
        //float fRadius = g_Camera.GetRadius();

        //sprintf( FrameStatus, "V : %0.2f,   H : %0.2f,   R : %0.2f", fAngleV, fAngleH, fRadius );
        //MultiByteToWideChar( CP_ACP, 0, FrameStatus, -1, mFrameStatus3, 128 );

        //D3DXVECTOR3 vLookAt = g_Camera.GetLookAtPos();
        //sprintf( FrameStatus, "LookAt ( %0.2f, %0.2f, %0.2f )", vLookAt.x, vLookAt.y, vLookAt.z );
        //MultiByteToWideChar( CP_ACP, 0, FrameStatus, -1, mFrameStatus4, 128 );

		//POINT ptPos;
		//::GetCursorPos(&ptPos);
		//::ScreenToClient(GetHWND(),&ptPos);
		//wsprintf( mFrameStatus5, L"Pos ( %d, %d )", ptPos.x, ptPos.y );		
		
    }

	static bool isUpdate = true ; 
	if(isUpdate)
	{
		if(CDXUTDialog::GetManager())
		{
			WCHAR status[128] ; 

			CDXUTDialog::GetManager()->DrawFont(0,mFrameStatus1,tempRect,0,0xffffffff,2);
			//CDXUTDialog::GetManager()->DrawFont(0,mFrameStatus2,mRect2,0,0xffffffff,2);
			//CDXUTDialog::GetManager()->DrawFont(0,mFrameStatus3,mRect3,0,0xffffffff,2);
			//CDXUTDialog::GetManager()->DrawFont(0,mFrameStatus4,mRect4,0,0xffffffff,2);
			//CDXUTDialog::GetManager()->DrawFont(0,mFrameStatus5,mRect5,0,0xffffffff,2);

			OffsetRect(&tempRect,0,20);

			wsprintf( status, L"可視粒子：%d 更新中粒子：%d 繪製中粒子：%d , 模組總和：%d 更新中模組：%d 更新中怪物：%d 更新中NPC：%d",
				m_particleCount , m_updateParticleCount ,m_flushParticleCount ,  m_modelCount , m_updateModelCount , m_updateMonster , m_updateNPC);		

			CDXUTDialog::GetManager()->DrawFont(0,status,tempRect,0,0xffffffff,2);

			OffsetRect(&tempRect,0,20);

			wsprintf( status, L"FreeMagic：%d Magic：%d FreeShoot：%d , Shoot：%d FreeSystem：%d System：%d FreeParticle：%d Particle：%d", 
				m_freeMagicParticCount , m_magicParticCount ,m_freeShootParticCount ,  m_shootParticCount ,
				m_freeSystemParticCount , m_systemParticCount , m_freeParticCount , m_particCount);		

	
			CDXUTDialog::GetManager()->DrawFont(0,status,tempRect,0,0xffffffff,2);

			OffsetRect(&tempRect,0,20);

			wsprintf( status, L"Force：%d ForceManager：%d Caliber：%d Color : %d Scale : %d Audio : %d Tex : %d MultiTex : %d Action : %d Emitter : %d" ,
				m_forceCount , m_forceManagerCount ,m_caliberCount , m_colorCount , m_scaleCount , m_audioCount , m_textureCount , 
				m_multiTexCount , m_actionManagerCount , m_emitterCount);

			CDXUTDialog::GetManager()->DrawFont(0,status,tempRect,0,0xffffffff,2);

		}
	}

	
	ClearInformation() ; 

//     DrawText( mFrameStatus1, &mRect1, 0xffffffff );
//     DrawText( mFrameStatus2, &mRect2, 0xffffffff );
//     DrawText( mFrameStatus3, &mRect3, 0xffffffff );
//     DrawText( mFrameStatus4, &mRect4, 0xffffffff );
// 	DrawText( mFrameStatus5, &mRect5, 0xffffffff );
}
//--------------------------------------------------------------------------------------
// Toggle between full screen and windowed
//--------------------------------------------------------------------------------------
bool D3D9RenderSystem::ToggleFullScreen()
{
    if( mpD3DDevice )
    {
        mIsFullScreen   = !mIsFullScreen;
        md3dpp.Windowed = !md3dpp.Windowed;

        if( mIsFullScreen )
        {
            md3dpp.BackBufferWidth  = mWidth;
            md3dpp.BackBufferHeight = mHeight;

            // 設定視窗型態
            SetWindowLong( mHWnd, GWL_STYLE, mWindowedStyle&(~WS_CAPTION) );

            // 改變視窗位置大小
            SetWindowPos( mHWnd, HWND_TOPMOST, 0, 0, mWidth, mHeight, SWP_SHOWWINDOW );
        }
        else
        {
            md3dpp.BackBufferWidth  = mClientWidth;
            md3dpp.BackBufferHeight = mClientHeight;

            // 恢復視窗型態
            SetWindowLong( mHWnd, GWL_STYLE, mWindowedStyle );

            // 改變視窗位置大小
            SetWindowPos( mHWnd, HWND_NOTOPMOST, 0, 0, mWidth, mHeight, SWP_SHOWWINDOW );
        }

        InvalidateDeviceObjects();

        HRESULT hr = mpD3DDevice->Reset( &md3dpp );
        if( FAILED( hr ) )
            return false;

        RestoreDeviceObjects();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
// Print Screen
//---------------------------------------------------------------------------
bool D3D9RenderSystem::TakeScreenShotHighQuality()
// Save as TGA.
{
    if (!mpD3DDevice)
        return false;

    IDirect3DSurface9* surf = NULL;

    HRESULT hr = mpD3DDevice->GetRenderTarget( 0, &surf );

    if( FAILED( hr ) )
        return false;

    // 取得本地的時間
    SYSTEMTIME st;
    GetLocalTime( &st );

    // 產生檔名
    WCHAR wszFilename[40];

    swprintf( wszFilename,
        L"PrintScreen\\PHOTO%04d%02d%02d%02d%02d%02d%03d.tga",
        st.wYear,           // 年
        st.wMonth,          // 月
        st.wDay,            // 日
        st.wHour,           // 時
        st.wMinute,         // 分
        st.wSecond,         // 秒
        st.wMilliseconds ); // 微秒

    hr = D3DXSaveSurfaceToFile( wszFilename, D3DXIFF_TGA, surf, NULL, NULL );
    //surf->Release();

    //string str;
    //wstring wstr;

    if (FAILED(hr)) {
		if(m_cbOnTakeScreenShot)
			m_cbOnTakeScreenShot(true, NULL, NULL);
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 1150, CodeInlineText_Text ) );//拍照
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 1073, CodeInlineText_Text ) );//。
        //wstr = AnsiToUnicode(str.c_str());
        //TalkBar_AddMessage(wstr, RED);
        return false;
    } // end if

	if(m_cbOnTakeScreenShot)
		m_cbOnTakeScreenShot(true, wszFilename, NULL);
   // std::wstring wstrMsg;
    //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 828, CodeInlineText_Text ) );//最高品質截圖
    //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 39, CodeInlineText_Text ) );//：
    //wstr = AnsiToUnicode(str.c_str());
    //wstrMsg = wstr;
    //wstrMsg += wszFilename;
    //TalkBar_AddMessage(wstrMsg, YELLOW);

    return true;
}

bool D3D9RenderSystem::TakeScreenShotLowQuality()
// Save as JPEG, quality = 75.
{
    if (!mpD3DDevice)
        return false;

    HRESULT hr;

    const int _iQuality = 75;

    IDirect3DSurface9* surf = NULL;
    hr = mpD3DDevice->GetRenderTarget( 0, &surf );
    if( FAILED( hr ) )
        return false;

    // 取得本地的時間
    SYSTEMTIME st;
    GetLocalTime( &st );

    // 產生檔名
    WCHAR wszInFilename[40];
    WCHAR wszOutFilename[40];

    swprintf( wszInFilename,
        L"PrintScreen\\PHOTO%04d%02d%02d%02d%02d%02d%03d.tga",
        st.wYear,           // 年
        st.wMonth,          // 月
        st.wDay,            // 日
        st.wHour,           // 時
        st.wMinute,         // 分
        st.wSecond,         // 秒
        st.wMilliseconds ); // 微秒

    swprintf( wszOutFilename,
        L"PrintScreen\\PHOTO%04d%02d%02d%02d%02d%02d%03d.jpg",
        st.wYear,           // 年
        st.wMonth,          // 月
        st.wDay,            // 日
        st.wHour,           // 時
        st.wMinute,         // 分
        st.wSecond,         // 秒
        st.wMilliseconds ); // 微秒

    hr = D3DXSaveSurfaceToFile( wszInFilename, D3DXIFF_TGA, surf, NULL, NULL );
    //surf->Release();

    string str;
    wstring wstr;

    if (FAILED(hr)) {
		if(m_cbOnTakeScreenShot)
			m_cbOnTakeScreenShot(false, NULL, NULL);
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 1150, CodeInlineText_Text ) );//拍照
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ) );//失敗
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 1073, CodeInlineText_Text ) );//。
        //wstr = AnsiToUnicode(str.c_str());
        //TalkBar_AddMessage(wstr, RED);
        return false;
    } // end if

    //音效 *** modify
   // str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 1150, CodeInlineText_Text ) );//拍照
   // g_SoundManager._Play2DSoundInMemory(GetSoundMap(str));

    // CompressToJPEG
    std::string infilename = UnicodeToAnsi(wszInFilename);
    std::string outfilename = UnicodeToAnsi(wszOutFilename);
    std::string strErr;
    bool bResult = TGA_To_JPEG(infilename.c_str(), outfilename.c_str(), _iQuality, strErr);
    std::wstring wstrMsg;
    if (false == bResult) {
		if(m_cbOnTakeScreenShot)
			m_cbOnTakeScreenShot(true, wszInFilename, strErr.c_str());
        //TalkBar_AddMessage(strErr, RED);
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 828, CodeInlineText_Text ) );//最高品質截圖
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 39, CodeInlineText_Text ) );//：
        //wstr = AnsiToUnicode(str.c_str());
        //wstrMsg = wstr;
        //wstrMsg += wszInFilename;
        //TalkBar_AddMessage(wstrMsg, YELLOW);
    } else {
        DeleteFile(wszInFilename);
		if(m_cbOnTakeScreenShot)
			m_cbOnTakeScreenShot(false, wszOutFilename, NULL);
        //str.assign( g_BinTable.mCodeInlineTextBin.pFastFindW( 1151, CodeInlineText_Text ) );//一般品質截圖
        //str.append( g_BinTable.mCodeInlineTextBin.pFastFindW( 39, CodeInlineText_Text ) );//：
        //wstr = AnsiToUnicode(str.c_str());
        //wstrMsg = wstr;
        //wstrMsg += wszOutFilename;
        //TalkBar_AddMessage(wstrMsg, YELLOW);
    } // end else

    return true;
}

bool D3D9RenderSystem::TakeScreenShot()
// 讀取「高品質截圖」選項：
// 打勾 = TakeScreenShotHighQuality()
// 取消 = TakeScreenShotLowQuality()
{
    if( true == m_bHighQualityScreenShot ) 
	{
        return TakeScreenShotHighQuality();
    } 
	else 
	{
        return TakeScreenShotLowQuality();
    } // end else
}
//---------------------------------------------------------------------------
// Handles messages
//---------------------------------------------------------------------------

LRESULT D3D9RenderSystem::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_SIZE:
			if(SIZE_MINIMIZED == wParam){
				m_bPauseRender = true;
			}else if( wParam == SIZE_RESTORED )
			{
				m_bPauseRender = false;
	            HandlePossibleSizeChange();

			}
            break;

        case WM_SYSCHAR:
        {
            switch( wParam )
            {
                case VK_RETURN:
                {
                    // Toggle full screen upon alt-enter
                    DWORD dwMask = (1 << 29);
                    if( (lParam & dwMask) != 0 )
                    {
                    #ifndef _FDO_GM
                        // Toggle the full screen/window mode
                        //ToggleFullScreen();
                    #endif
                    }
                    break;
                }
            }
            break;
        }
        default:
            return S_FALSE;
    }

    return S_OK;
}
//---------------------------------------------------------------------------
// fog
//---------------------------------------------------------------------------
void D3D9RenderSystem::SetFogEnable( bool bFogEnable )
{
    mpD3DDevice->SetRenderState( D3DRS_FOGENABLE, bFogEnable );
    if( bFogEnable )
    {
        mpD3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE ); // 開啟霧
        mpD3DDevice->SetRenderState( D3DRS_FOGCOLOR, m_dwFogColor ); // Set the fog color.
        mpD3DDevice->SetRenderState( D3DRS_FOGSTART, m_dwFogStart );
        mpD3DDevice->SetRenderState( D3DRS_FOGEND, m_dwFogEnd );
		mpD3DDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
		mpD3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		mpD3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE, 0 );
    }
}


void D3D9RenderSystem::SetMipMapLODBias(float bias)
{
	mTexLODBias = bias;
	mpD3DDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *(DWORD*)&bias);
}


CRenderTarget * D3D9RenderSystem::CreateRenderTarget(int renderStep, int w, int h, D3DFORMAT format, bool bDepthStencil)
{
	char str[100];
	// a simple (dirty?) way to reuse RT
	// renderStep is the key to manage RT if need 2 (or more) the same format/size RT at the same time
	sprintf(str, "%d_%d_%d_%d", renderStep, w, h, format);
	for(size_t i=0; i<mRenderTargetList.size(); ++i)
	{
		if(strcmp(mRenderTargetList[i]->GetResName(), str)==0)
		{
			return mRenderTargetList[i];
		}
	}

	CRenderTarget *pRT = new CRenderTarget(str, mpD3DDevice, w, h, format, bDepthStencil);
	if(pRT!=NULL)
	{
		mRendererResList.push_back(pRT);
		mRenderTargetList.push_back(pRT);
	}
	return pRT;
}

/////////////////							PostPass				///////////////////////////////

CPostProcessFilter::CPostProcessFilter(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect) : m_pDevice(pDevice) , m_pEffect(pEffect)
{ 

}

CPostProcessFilter::~CPostProcessFilter()
{
	if(m_pEffect != NULL)
		m_pEffect->Release() ; 
}

void CPostProcessFilter::BeginViewport(const RECT *scissor)
{
	D3DVIEWPORT9 newvp ; 

	m_pDevice->GetViewport(&m_tempViewPort) ; 

	newvp.X = scissor->left ; 
	newvp.Y = scissor->top ; 
	newvp.Width = scissor->right - scissor->left ; 
	newvp.Height = scissor->bottom - scissor->top ; 
	newvp.MinZ = 0 ; 
	newvp.MaxZ = 1; 

	m_pDevice->SetViewport(&newvp) ; 

}
void CPostProcessFilter::EndViewport()
{
	m_pDevice->SetViewport(&m_tempViewPort) ; 
}
void CPostProcessFilter::BeginOrtho2D(FLOAT w, FLOAT h)
{
	D3DXMATRIX mat ; 
	D3DXMATRIX id ; 

	D3DXMatrixIdentity(&id) ; 

	D3DXMatrixOrthoOffCenterLH(&mat , 0 , w , h , 0 , 0 , 100.0f) ; 
	//D3DXMatrixOrthoOffCenterLH(&outMat , 0 ,  (float)width   , 0.0f , (float)hegith  , 0.0f , 100.0f) ; 

	BeginProjection(mat , id) ; 
}
void CPostProcessFilter::EndOrtho2D()
{
	EndProjection() ; 

}
void CPostProcessFilter::BeginProjection(const D3DXMATRIX &pj,const D3DXMATRIX &view)
{
	m_pDevice->GetTransform(D3DTS_PROJECTION , &m_tempProjection) ; 
	m_pDevice->GetTransform(D3DTS_VIEW  , &m_tempView) ; 
	m_pDevice->GetTransform(D3DTS_WORLD  , &m_tempWorld) ; 

	D3DXMATRIX worldMat ; 

	D3DXMatrixIdentity(&worldMat) ; 


	SetWorldViewProjectionMat(worldMat , view , pj) ; 

}
void CPostProcessFilter::EndProjection()
{
	m_pDevice->SetTransform(D3DTS_PROJECTION , &m_tempProjection) ; 
	m_pDevice->SetTransform(D3DTS_VIEW  , &m_tempView) ; 
	m_pDevice->SetTransform(D3DTS_WORLD  , &m_tempWorld) ; 

}

void CPostProcessFilter::DrawBuffer(D3DXHANDLE techHandle , unsigned int pass , float width , float height) 
{


	if(m_pEffect == NULL)
	{
		return ; 

	}
	//PlaneVertex4 quad[] = 
	//{
	//	{-1,0,1 , 0,0}, //0
	//	{(float)width,0,1 ,1,0}, //1
	//	{(float)width,(float)height,1 ,1,1}, //2
	//	{-1,0,1,0,0}, //0
	//	{(float)width,(float)height,1,1,1}, //2
	//	{-1,(float)height,1,0,1} //3

	//};
	VertexPT quad[6] ; 

	quad[0] = VertexPT(-1.0f ,  1.0f , 0.9f , 0.0f , 0.0f) ; //0
	quad[1] = VertexPT( 1.0f ,  1.0f , 0.9f , 1.0f , 0.0f) ; //1
	quad[2] = VertexPT( 1.0f , -1.0f , 0.9f , 1.0f , 1.0f) ; //2
	quad[3] = VertexPT(-1.0f ,  1.0f , 0.9f , 0.0f , 0.0f) ; //0
	quad[4] = VertexPT( 1.0f , -1.0f , 0.9f , 1.0f , 1.0f) ; //2
	quad[5] = VertexPT(-1.0f , -1.0f , 0.9f , 0.0f , 1.0f) ; //3



	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = (LONG)width;
	rect.bottom = (LONG)height;

	//BeginViewport(&rect) ; 
	//BeginOrtho2D(width , height) ; 

	UINT allPass = 0 ; 


	m_pDevice->SetFVF(PlaneVertex4::FVF) ; 

	if(FAILED(m_pEffect->SetTechnique(techHandle)))
	{
		return ; 
	}

	if(FAILED(m_pEffect->Begin(&allPass , 0)))
	{
		return ; 
	}

	if(FAILED(m_pEffect->BeginPass(pass)))
	{
		return ; 
	}

	
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,quad,sizeof(VertexPT));
	

	if(FAILED(m_pEffect->EndPass()))
	{
		return ; 
	}






	if(FAILED(m_pEffect->End()))
	{
		return ; 
	}

	//EndOrtho2D() ; 
	//EndViewport() ; 

}

//////////////////////////					CColorMixer					////////////////////////////////////

CColorMixer::CColorMixer(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height ) 
:CPostProcessFilter(pDevice , pEffect) 
{
	if(pEffect != NULL)
	{
		m_techHandle = pEffect->GetTechniqueByName("ColorMixerTech") ;

		m_texHandle = pEffect->GetParameterBySemantic( NULL, "TEXTURE" );


		m_viewPortSizeHandle = pEffect->GetParameterBySemantic( NULL, "VIEWPORTPIXELSIZE" ); 


		D3DXVECTOR2 viwePortSize((float)width , (float)height) ; 
		pEffect->SetValue(m_viewPortSizeHandle , &viwePortSize , sizeof(D3DXVECTOR2)) ; 


	}

	m_width = width ; 
	m_height = height ; 

	m_pTextureA = LoadResource<IDirect3DTexture9 , ResTextargetObj>("ColorTexture.bmp" , m_width , m_height) ;


}

CColorMixer::~CColorMixer()
{

}

void CColorMixer::SetWorldViewProjectionMat(const D3DXMATRIX &worldMat , const D3DXMATRIX &viewMat , const D3DXMATRIX &projMat)
{
	m_pDevice->SetTransform(D3DTS_PROJECTION , &projMat) ; 
	m_pDevice->SetTransform(D3DTS_VIEW  , &viewMat) ; 
	m_pDevice->SetTransform(D3DTS_WORLD , &worldMat) ; 

}

void CColorMixer::ApplyFiler()
{
	if(m_pTextureA == NULL)
	{
		return ; 
	}


	IDirect3DSurface9 *screen = NULL;
	IDirect3DSurface9 *target = NULL;

	//gets the screen surface
	if(FAILED(m_pDevice->GetRenderTarget(0,&screen))) {
		return ;
	}
	
	if(FAILED(m_pTextureA->GetSurfaceLevel(0,&target))) 
	{
		return ;
	}

	if(FAILED(m_pDevice->StretchRect(screen,NULL,target,NULL,D3DTEXF_NONE))) {
		return ;
	}

	m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 ); 

	m_pEffect->SetTexture(m_texHandle , m_pTextureA) ; 

	DrawBuffer(m_techHandle , 0 , (float)m_width , (float)m_height) ; 

	target->Release() ; 
	screen->Release() ; 

}


//////////////////////////					ColorMatrix					////////////////////////////////////


CColorMatrix::CColorMatrix(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height ) 
:CPostProcessFilter(pDevice , pEffect) 
{
	if(pEffect != NULL)
	{

		m_techHandle = pEffect->GetTechniqueByName("ColorMatrixTech") ;

		m_texHandle = pEffect->GetParameterBySemantic( NULL, "TEXTURE" );


		m_viewPortSizeHandle = pEffect->GetParameterBySemantic( NULL, "VIEWPORTPIXELSIZE" ); 


		D3DXVECTOR2 viwePortSize((float)width , (float)height) ; 
		pEffect->SetValue(m_viewPortSizeHandle , &viwePortSize , sizeof(D3DXVECTOR2)) ; 


	}

	m_width = width ; 
	m_height = height ; 

	m_pTextureA = LoadResource<IDirect3DTexture9 , ResTextargetObj>("SceneTexture.bmp" , m_width , m_height) ;


}

CColorMatrix::~CColorMatrix()
{

}

void CColorMatrix::SetWorldViewProjectionMat(const D3DXMATRIX &worldMat , const D3DXMATRIX &viewMat , const D3DXMATRIX &projMat)
{
}

void CColorMatrix::SetParameter(const PostPassParameters &params)
{
	if(m_pEffect == NULL)
	{
		return ; 
	}

	m_params = params ; 


	m_brightnessHandle = m_pEffect->GetParameterByName(NULL , "gBrightness") ; 
	m_contrastHandle = m_pEffect->GetParameterByName(NULL , "gContrast") ; 
	m_saturationHandle = m_pEffect->GetParameterByName(NULL , "gSaturation") ; 
	m_hueHandle = m_pEffect->GetParameterByName(NULL , "gHue") ; 

	m_pEffect->SetFloat(m_brightnessHandle , m_params.brightness) ; 
	m_pEffect->SetFloat(m_contrastHandle , m_params.contrast) ; 
	m_pEffect->SetFloat(m_saturationHandle , m_params.saturation) ; 
	m_pEffect->SetFloat(m_hueHandle , m_params.hue) ; 


}


void CColorMatrix::ApplyFiler()
{
	if(m_pTextureA == NULL)
	{
		return ; 
	}


	IDirect3DSurface9 *screen = NULL;
	IDirect3DSurface9 *target = NULL;

	//gets the screen surface
	if(FAILED(m_pDevice->GetRenderTarget(0,&screen))) {
		return ;
	}
	
	if(FAILED(m_pTextureA->GetSurfaceLevel(0,&target))) 
	{
		return ;
	}

	if(FAILED(m_pDevice->StretchRect(screen,NULL,target,NULL,D3DTEXF_NONE))) {
		return ;
	}

	m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 ); 

	m_pEffect->SetTexture(m_texHandle , m_pTextureA) ; 

	DrawBuffer(m_techHandle , 0 , (float)m_width , (float)m_height) ; 

	target->Release() ; 
	screen->Release() ; 

}
void CColorMatrix::OnLostDevice()
{
	ReleaseResource(m_pTextureA) ; 

	m_pEffect->OnLostDevice() ; 

	m_pTextureA = NULL ;
}
void CColorMatrix::OnResetDevice()
{

	m_pEffect->OnResetDevice() ; 

	m_pTextureA = LoadResource<IDirect3DTexture9 , ResTextargetObj>("SceneTexture.bmp" , m_width , m_height) ;
}



//////////////////////////					Glow					////////////////////////////////////



CGlow::CGlow(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height ) 
:CPostProcessFilter(pDevice , pEffect) 
{
	if(pEffect != NULL)
	{
		m_techHandle = pEffect->GetTechniqueByName("GlowTech") ;

		m_texAHandle = pEffect->GetParameterBySemantic( NULL, "GLOWRTA" );
		m_texBHandle = pEffect->GetParameterBySemantic( NULL, "GLOWRTB" );
		m_sceneHandle = pEffect->GetParameterBySemantic( NULL, "SCENERT" );


		m_viewPortSizeHandle = pEffect->GetParameterBySemantic( NULL, "VIEWPORTPIXELSIZE" ); 
		D3DXVECTOR2 viwePortSize((float)width , (float)height) ; 
		pEffect->SetValue(m_viewPortSizeHandle , &viwePortSize , sizeof(D3DXVECTOR2)) ; 


	}

	m_width = width ; 
	m_height = height ; 

	m_pTextureA = LoadResource<IDirect3DTexture9 , ResTextargetObj>("GlowTextureA.bmp" , m_width , m_height) ;
	m_pTextureB = LoadResource<IDirect3DTexture9 , ResTextargetObj>("GlowTextureB.bmp" , m_width , m_height) ;
	m_pSceneTex = LoadResource<IDirect3DTexture9 , ResTextargetObj>("SceneTexture.bmp" , m_width , m_height) ;

}

CGlow::~CGlow()
{

}

void CGlow::SetWorldViewProjectionMat(const D3DXMATRIX &worldMat , const D3DXMATRIX &viewMat , const D3DXMATRIX &projMat)
{
}


void CGlow::SetParameter(const PostPassParameters &params)
{
	if(m_pEffect == NULL)
	{
		return ; 
	}

	m_params = params ; 


	m_glownessHandle = m_pEffect->GetParameterByName(NULL , "gGlowness") ; 
	m_scenenessHandle = m_pEffect->GetParameterByName(NULL , "gSceneness") ; 
	m_glowspanHandle = m_pEffect->GetParameterByName(NULL , "gGlowSpan") ; 

	m_pEffect->SetFloat(m_glownessHandle , m_params.glowness) ; 
	m_pEffect->SetFloat(m_scenenessHandle , m_params.sceneness) ; 
	m_pEffect->SetFloat(m_glowspanHandle , m_params.glowspan) ; 


}

void CGlow::ApplyFiler()
{
	if(m_pTextureA == NULL || m_pTextureB == NULL || m_pSceneTex == NULL)
	{
		return ; 
	}


	IDirect3DSurface9 *screen = NULL;
	IDirect3DSurface9 *target = NULL;


	//////////				copy backbuffer to sceneTexture				////////////

	//gets the screen surface
	if(FAILED(m_pDevice->GetRenderTarget(0,&screen))) {
		return ;
	}
	
	if(FAILED(m_pSceneTex->GetSurfaceLevel(0,&target))) 
	{
		return ;
	}

	if(FAILED(m_pDevice->StretchRect(screen,NULL,target,NULL,D3DTEXF_NONE))) {
		return ;
	}




	// set render target glow texture A
	m_pEffect->SetTexture(m_sceneHandle , m_pSceneTex) ; 

	target->Release() ; 
	target = NULL ; 

	m_pTextureA->GetSurfaceLevel(0,&target);

	m_pDevice->SetRenderTarget(0 , target) ; 

	DrawBuffer(m_techHandle , 0 , (float)m_width , (float)m_height) ; 

	// set render target glow texture B

	m_pEffect->SetTexture(m_texAHandle , m_pTextureA) ; 

	target->Release() ; 
	target = NULL ; 

	m_pTextureB->GetSurfaceLevel(0,&target);

	m_pDevice->SetRenderTarget(0 , target) ; 

	DrawBuffer(m_techHandle , 1 , (float)m_width , (float)m_height) ; 


	// set render target scene texture
	m_pEffect->SetTexture(m_texBHandle , m_pTextureB) ; 

	m_pDevice->SetRenderTarget(0 , screen) ; 

	m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 ); 

	DrawBuffer(m_techHandle , 2 , (float)m_width , (float)m_height) ; 
	

	target->Release() ; 
	screen->Release() ; 

}

void CGlow::OnLostDevice()
{
	ReleaseResource(m_pTextureA) ; 
	ReleaseResource(m_pTextureB) ; 
	ReleaseResource(m_pSceneTex) ; 

	m_pTextureA = NULL ;
	m_pTextureB = NULL ; 
	m_pSceneTex = NULL ; 

	m_pEffect->OnLostDevice() ; 
}
void CGlow::OnResetDevice()
{

	m_pEffect->OnResetDevice() ; 

	m_pTextureA = LoadResource<IDirect3DTexture9 , ResTextargetObj>("GlowTextureA.bmp" , m_width , m_height) ;
	m_pTextureB = LoadResource<IDirect3DTexture9 , ResTextargetObj>("GlowTextureB.bmp" , m_width , m_height) ;
	m_pSceneTex = LoadResource<IDirect3DTexture9 , ResTextargetObj>("SceneTexture.bmp" , m_width , m_height) ;
}


//////////////////////////					Spotlight					////////////////////////////////////


CSpotLight::CSpotLight(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height) 
:CPostProcessFilter(pDevice , pEffect) 
{
	if(pEffect != NULL)
	{

		m_techHandle = pEffect->GetTechniqueByName("SpotlightTech") ;

		m_texHandle = pEffect->GetParameterBySemantic( NULL, "TEXTURE" );

		m_viewPortSizeHandle = pEffect->GetParameterBySemantic( NULL, "VIEWPORTPIXELSIZE" ); 


		D3DXVECTOR2 viwePortSize((float)width , (float)height) ; 
		pEffect->SetValue(m_viewPortSizeHandle , &viwePortSize , sizeof(D3DXVECTOR2)) ; 


	}

	m_width = width ; 
	m_height = height ; 

	m_pTextureA = LoadResource<IDirect3DTexture9 , ResTextargetObj>("SceneTexture.bmp" , m_width , m_height) ;

}

CSpotLight::~CSpotLight()
{

}

void CSpotLight::ApplyFiler() 
{

	if(m_pTextureA == NULL)
	{
		return ; 
	}


	IDirect3DSurface9 *screen = NULL;
	IDirect3DSurface9 *target = NULL;

	//gets the screen surface
	if(FAILED(m_pDevice->GetRenderTarget(0,&screen))) {
		return ;
	}
	
	if(FAILED(m_pTextureA->GetSurfaceLevel(0,&target))) 
	{
		return ;
	}

	if(FAILED(m_pDevice->StretchRect(screen,NULL,target,NULL,D3DTEXF_NONE))) {
		return ;
	}

	m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 ); 

	m_pEffect->SetTexture(m_texHandle , m_pTextureA) ; 

	DrawBuffer(m_techHandle , 0 , (float)m_width , (float)m_height) ; 

	target->Release() ; 
	screen->Release() ; 
}
void CSpotLight::SetParameter(const PostPassParameters &params)
{
	if(m_pEffect == NULL)
	{
		return ; 
	}

	m_params = params ; 



	m_spotColorHandle	= m_pEffect->GetParameterByName(NULL , "SpotColor"); 
	m_ambientHandle		= m_pEffect->GetParameterByName(NULL , "Ambient"); 
	m_angleHandle		= m_pEffect->GetParameterByName(NULL , "Angle"); 
	m_azimuthHandle		= m_pEffect->GetParameterByName(NULL , "Azimuth"); 
	m_centerXHandle		= m_pEffect->GetParameterByName(NULL , "CenterX"); 
	m_centerYHandle		= m_pEffect->GetParameterByName(NULL , "CenterY"); 
	m_distanceHandle	= m_pEffect->GetParameterByName(NULL , "Distance"); 
	m_coneAngleHandle	= m_pEffect->GetParameterByName(NULL , "ConeAngle"); 
	m_intensityHandle	= m_pEffect->GetParameterByName(NULL , "Intensity"); 

	m_pEffect->SetValue(m_spotColorHandle , &m_params.spotColor , sizeof(D3DXVECTOR3)) ; 
	m_pEffect->SetValue(m_ambientHandle , &m_params.ambientColor , sizeof(D3DXVECTOR3)) ; 

	m_pEffect->SetFloat(m_angleHandle , m_params.angle) ; 
	m_pEffect->SetFloat(m_azimuthHandle , m_params.azimuth) ; 
	m_pEffect->SetFloat(m_centerXHandle , m_params.centerX) ; 
	m_pEffect->SetFloat(m_centerYHandle , m_params.centerY) ; 
	m_pEffect->SetFloat(m_distanceHandle , m_params.distance) ; 
	m_pEffect->SetFloat(m_coneAngleHandle , m_params.coneAngle) ; 
	m_pEffect->SetFloat(m_intensityHandle , m_params.intensity) ; 



}
void CSpotLight::OnLostDevice()
{

	ReleaseResource(m_pTextureA) ; 


	m_pTextureA = NULL ;

	m_pEffect->OnLostDevice() ; 

}
void CSpotLight::OnResetDevice()
{
	m_pEffect->OnResetDevice() ; 

	m_pTextureA = LoadResource<IDirect3DTexture9 , ResTextargetObj>("SceneTexture.bmp" , m_width , m_height) ;

}

//////////////////////////					EasyColorMixer					////////////////////////////////////


CEasyColorMixer::CEasyColorMixer(LPDIRECT3DDEVICE9 pDevice , LPD3DXEFFECT pEffect , int width  , int height) 
:CPostProcessFilter(pDevice , pEffect) 
{
	if(pEffect != NULL)
	{

		m_techHandle = pEffect->GetTechniqueByName("EasyColorMixerTech") ;

		m_texHandle = pEffect->GetParameterBySemantic( NULL, "TEXTURE" );

		m_viewPortSizeHandle = pEffect->GetParameterBySemantic( NULL, "VIEWPORTPIXELSIZE" ); 


		D3DXVECTOR2 viwePortSize((float)width , (float)height) ; 
		pEffect->SetValue(m_viewPortSizeHandle , &viwePortSize , sizeof(D3DXVECTOR2)) ; 


	}

	m_width = width ; 
	m_height = height ; 

	m_pTextureA = LoadResource<IDirect3DTexture9 , ResTextargetObj>("SceneTexture.bmp" , m_width , m_height) ;

	m_pDay = new CAllDay() ; 

}

CEasyColorMixer::~CEasyColorMixer()
{

}

void CEasyColorMixer::ApplyFiler() 
{


	if(m_pTextureA == NULL || m_plookupTex == NULL)
	{
		return ; 
	}

	m_pDay->TestUpdate() ; 


	m_pEffect->SetFloat(m_sampleValueHanlde , m_params.sampleValue) ; 



	IDirect3DSurface9 *screen = NULL;
	IDirect3DSurface9 *target = NULL;

	//gets the screen surface
	if(FAILED(m_pDevice->GetRenderTarget(0,&screen))) {
		return ;
	}
	
	if(FAILED(m_pTextureA->GetSurfaceLevel(0,&target))) 
	{
		return ;
	}

	if(FAILED(m_pDevice->StretchRect(screen,NULL,target,NULL,D3DTEXF_NONE))) {
		return ;
	}

	//m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 ); 

	m_pEffect->SetTexture(m_texlookupHanlde , m_plookupTex) ; 

	m_pEffect->SetTexture(m_texHandle , m_pTextureA) ; 

	DrawBuffer(m_techHandle , 0 , m_width , m_height) ; 

	target->Release() ; 
	screen->Release() ; 
}
void CEasyColorMixer::SetParameter(const PostPassParameters &params)
{
	if(m_pEffect == NULL)
	{
		return ; 
	}

	m_params = params ; 



	m_sampleValueHanlde	= m_pEffect->GetParameterByName(NULL , "gSampleValue"); 
	m_texlookupHanlde	= m_pEffect->GetParameterByName(NULL , "gSampleColor"); 

	m_pEffect->SetFloat(m_sampleValueHanlde , m_params.sampleValue) ; 

	m_plookupTex = LoadResource<IDirect3DTexture9 , ResTextrueObj>(m_params.lookupName.c_str()) ; 

	m_pDay->Register(&m_params) ; 

}
void CEasyColorMixer::OnLostDevice()
{

	ReleaseResource(m_pTextureA) ; 
	ReleaseResource(m_plookupTex) ; 


	m_pTextureA = NULL ;
	m_plookupTex = NULL ; 

	m_pEffect->OnLostDevice() ; 

}
void CEasyColorMixer::OnResetDevice()
{
	m_pEffect->OnResetDevice() ; 


	m_plookupTex = LoadResource<IDirect3DTexture9 , ResTextrueObj>(m_params.lookupName.c_str()) ; 
	m_pTextureA = LoadResource<IDirect3DTexture9 , ResTextargetObj>("SceneTexture.bmp" , m_width , m_height) ;

}

CAllDay::CAllDay()
{
	m_pEasyColorMixerParam = NULL ; 
}
CAllDay::~CAllDay()
{
	m_pEasyColorMixerParam = NULL ; 
}
void CAllDay::Register(PostPassParameters *pParam)
{
	if(m_pEasyColorMixerParam != NULL)
		return ; 

	m_pEasyColorMixerParam = pParam ; 


	SYSTEMTIME ti ; 

	GetSystemTime(&ti) ; 
	
	m_startTime	= (ti.wHour + 8 ) * 3600 + ti.wMinute * 60 + ti.wSecond ; 
	m_endTime		= m_startTime + 2 * 3600 ; 

	m_pEasyColorMixerParam->sampleValue = (float)m_startTime / (float)s_oneDayTime ; 


	m_tempTime =  m_pEasyColorMixerParam->sampleValue * (float)(m_endTime - m_startTime) ; 


}

void CAllDay::TestUpdate()
{
	if(m_pEasyColorMixerParam != NULL)
	{
	//	SYSTEMTIME ti ; 

	//	GetSystemTime(&ti) ; 

		m_tempTime += 1 ; 
		
		if(m_startTime + m_tempTime > m_endTime)
		{
			m_tempTime = 0 ;  
		}
		

		float sampValue = (float)(m_tempTime) / (float)(m_endTime - m_startTime); 

		m_pEasyColorMixerParam->sampleValue = sampValue ; 


	}
}

void CAllDay::Update()
{
	if(m_pEasyColorMixerParam != NULL)
	{
		SYSTEMTIME ti ; 

		GetSystemTime(&ti) ; 

		float sampValue = (float)(m_startTime + ti.wHour * 3600 + ti.wMinute * 60 + ti.wSecond) / (float)m_endTime ; 

		m_pEasyColorMixerParam->sampleValue = sampValue ; 


	}
}


//---------------------------------------------------------------------------
}   // namespace FDO







