#include "stdafx.h"
//#pragma hdrstop
//----------------------------------------------------------------------------
#include "PhenixAssert.h"
//#include "Timer.h"
//#include "Device.h"
//#include "JLCamera.h"
#include "PhenixD3D9RenderSystem.h"
//#include "GameMapManager.h"
#include "PacketReader.h"
#include "DrawPlane.h"
#include "Common.h"
//#include "BinaryTableManager.h"
//#include "JLAnimation2D.h"
#pragma hdrstop
#include "JLAnimation2DManager.h"

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

const float g_fFadeTime = 2500.0f;                    // �H�X�ɶ�
const int gLogNums = 2 ;							  //logo�Ϧ�3�i
const int gLoadingNums = 4 ;						  //loading�Ϧ�5�i

//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("�޲z�Ҧ��Ұ�") 

#include "ResourceManager.h"

#else
#pragma message("�@��Ҧ��Ұ�") 
#endif

////////////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
namespace FDO
{

//----------------------------------------------------------------------------
JLAnimation2DManager::JLAnimation2DManager()
{
    m_pMouseAnimation   = NULL;
    m_pDevice           = NULL;
    m_pLoadingTex       = NULL;
	m_pLoadingTex1		= NULL ; 
	m_pLoadingTex2		= NULL ; 
	m_pLoadingTex3		= NULL ; 
	m_pLoadingTex4		= NULL ; 
	m_pLoadingTex5		= NULL ; 

	m_pLogoTex			= NULL ;


    m_pBackTexture      = NULL;
    m_bShowFadeOut      = false;
    m_bShowBackground   = false;
	m_bCanShowFadeIn    = false;
    m_LoadingImageColor = WHITE;
    m_iFadeOutTime      = 0;
    SetRect( &m_LoadingImageRect, 0, 0, 0, 0 );
}
//----------------------------------------------------------------------------
JLAnimation2DManager::~JLAnimation2DManager()
{
    RemoveAllAnimationRole();

#ifdef RESMGR
	//�]��m_pLoadingTex�Mm_pBackTexture�O�R�A�F�C���ݭnRelease
#else
    SAFE_RELEASE( m_pTexture );
    SAFE_RELEASE( m_pBackTexture );
#endif
}
//----------------------------------------------------------------------------
// �إ߰ʵe
//----------------------------------------------------------------------------
JLAnimation2D* JLAnimation2DManager::CreateAnimationRole( const string& FileName )
{
    JLAnimation2D* pAnimationRole = new JLAnimation2D;
    Assert( pAnimationRole != 0, "Call new failed !!" );

    // Ū���ʵe��
    if( pAnimationRole->LoadFromFileInMemory( FileName ) )
    {
        m_Animation2DList.push_front( pAnimationRole );
    }
    else
    {
        delete pAnimationRole;
        pAnimationRole = NULL;
    }

    return pAnimationRole;
}
//----------------------------------------------------------------------------
// �R���ʵe
//----------------------------------------------------------------------------
bool JLAnimation2DManager::RemoveAnimationRole( JLAnimation2D* pAnimation )
{
    m_Animation2DList.remove( pAnimation );
    return true;
}
//----------------------------------------------------------------------------
// �R���Ҧ��ʵe
//----------------------------------------------------------------------------
void JLAnimation2DManager::RemoveAllAnimationRole()
{
    while( !m_Animation2DList.empty() )
    {
        JLAnimation2D* pAnimation2D = m_Animation2DList.front();
        m_Animation2DList.pop_front();
        delete pAnimation2D;
    }
}
//----------------------------------------------------------------------------
// �إ߷ƹ��ʵe
//----------------------------------------------------------------------------
bool JLAnimation2DManager::CreateMouseAnimation( const string& FileName )  //�즲 ***
{
    m_pMouseAnimation = CreateAnimationRole( FileName );

    if( m_pMouseAnimation != NULL )
    {
        //string str;
        //str.assign(g_BinTable.mCodeInlineTextBin.pFastFindW( 3000, CodeInlineText_Text ));//�`�A���
        m_pMouseAnimation->Play( m_defaultMouseAnim.c_str() );
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
// �إ߷ƹ��ʵe
//----------------------------------------------------------------------------

JLAnimation2D* JLAnimation2DManager::GetMouseAnimation()
{
	static JLAnimation2D gEmptyAnimation2D;
	if(m_pMouseAnimation)
		return m_pMouseAnimation; ////�즲 ***
	else 
		return &gEmptyAnimation2D;
}

//----------------------------------------------------------------------------
// �Q�ζü��H����ܤ@��
//----------------------------------------------------------------------------
void JLAnimation2DManager::RandLoadingTexure(void)
{
    static int iNumber = 0 ;

    if( iNumber == 0 )
		m_pLoadingTex = m_pLoadingTex1 ; 
    else if( iNumber == 1 )
		m_pLoadingTex = m_pLoadingTex2 ; 
    else if( iNumber == 2 )
		m_pLoadingTex = m_pLoadingTex3 ; 
    else if( iNumber == 3 )
		m_pLoadingTex = m_pLoadingTex4 ; 
    else if( iNumber == 4 )
		m_pLoadingTex = m_pLoadingTex5 ; 


	if (m_pLoadingTex == NULL)
	{
		m_pLoadingTex = m_pLoadingTex1 ;  //�@�w�n�T�Om_pLoadingTex1�@�w�n���귽�I
		iNumber = 0 ;
	}

	++iNumber ; 

	if (iNumber > gLoadingNums )
	{
		iNumber = 0 ;
	}
}
//----------------------------------------------------------------------------
// ��s�ʵe
//----------------------------------------------------------------------------
void JLAnimation2DManager::FrameMoveAnimationRole(DWORD dwPassTime)
{
/*
    DWORD dwPassTime  = g_Timer.GetTimeDelta();

    Animation2DList::iterator end( m_Animation2DList.end() );

    for( Animation2DList::iterator i = m_Animation2DList.begin(); i != end; ++i )
        (*i)->FrameMove( dwPassTime );
*/

    if( m_pMouseAnimation )
        m_pMouseAnimation->FrameMove( dwPassTime );

    // Loading�ϲH�X�B�z
    if( m_iFadeOutTime != 0 )
    {

        m_iFadeOutTime -= dwPassTime;

        // �����H�X
        if( m_iFadeOutTime < 0 )
        {
            m_iFadeOutTime = 0;
            m_LoadingImageColor = WHITE;
            m_bShowFadeOut = false;


        }
        else
        {
            // ���ܳz��
            BYTE Alpha = (BYTE)( ( (float)m_iFadeOutTime / g_fFadeTime ) * 255.0f );
            m_LoadingImageColor = D3DCOLOR_ARGB( Alpha, 255, 255, 255 );
			//cosiann ����|�y���L�kFadeOut�A�]�N�O�|�@��false
         // m_bShowFadeOut = (m_LoadingImageColor!=D3DCOLOR_ARGB(Alpha,255,255,255));
			m_bShowFadeOut = true ;

        }
    }


}
//----------------------------------------------------------------------------
// ø�s�ʵe
//----------------------------------------------------------------------------
void JLAnimation2DManager::DrawAnimationRole()
{
/*
    Animation2DList::iterator end( m_Animation2DList.end() );

    for( Animation2DList::iterator i = m_Animation2DList.begin(); i != end; ++i )
        (*i)->Render();
*/

    if( m_pMouseAnimation )
        m_pMouseAnimation->Render();

    // ø�sLoading��
    if( m_bShowFadeOut )
    {
        DWORD dwLighting, dwZEnable;
        m_pDevice->GetRenderState( D3DRS_LIGHTING, &dwLighting );
        m_pDevice->GetRenderState( D3DRS_ZENABLE, &dwZEnable );

        m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		
		DWORD oldAddressU0;
		m_pDevice->GetSamplerState(0, D3DSAMP_ADDRESSU, &oldAddressU0);
		m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);

        m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );




#ifdef RESMGR


		Assert(m_pLoadingTex != NULL , "LoadingTex Error") ;

		m_pDevice->SetTexture( 0, m_pLoadingTex );


#else
        m_pDevice->SetTexture( 0, m_pTexture );
	
#endif

		if(!m_bCanShowFadeIn )
		{
			m_LoadingImageColor = WHITE ;
		}


        DrawFadeOut( m_LoadingImageRect.right, m_LoadingImageRect.bottom, m_LoadingImageColor );

        m_pDevice->SetRenderState( D3DRS_LIGHTING, dwLighting );
        m_pDevice->SetRenderState( D3DRS_ZENABLE, dwZEnable );

		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, oldAddressU0);
       // m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
      //  m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    }
}

//----------------------------------------------------------------------------
void JLAnimation2DManager::DrawBackground()
{
    // ø�slogo�I����
    if( m_bShowBackground )
    {
        m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

        DWORD dwOldColorOP, dwOldColorARG1, dwOldColorARG2;
        DWORD dwOldAlphaOP, dwOldAlphaARG1, dwOldAlphaARG2;

        m_pDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &dwOldColorOP );
        m_pDevice->GetTextureStageState( 0, D3DTSS_COLORARG1, &dwOldColorARG1 );
        m_pDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &dwOldColorARG2 );
        m_pDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP, &dwOldAlphaOP );
        m_pDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &dwOldAlphaARG1 );
        m_pDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &dwOldAlphaARG2 );

		DWORD oldAddressU0;
		m_pDevice->GetSamplerState(0, D3DSAMP_ADDRESSU, &oldAddressU0);
		m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);

		m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

#ifdef RESMGR


		Assert(m_pLogoTex != NULL , "LoadLogo Error") ;

		m_pDevice->SetTexture( 0, m_pLogoTex );

#else
        m_pDevice->SetTexture( 0, m_pTexture );
	
#endif

        DrawFadeOut( m_LoadingImageRect.right, m_LoadingImageRect.bottom, WHITE );

        m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, dwOldColorOP );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, dwOldColorARG1 );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, dwOldColorARG2 );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, dwOldAlphaOP );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, dwOldAlphaARG1 );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, dwOldAlphaARG2 );
		m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, oldAddressU0);

        m_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
        m_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    }
}

//----------------------------------------------------------------------------

void JLAnimation2DManager::LoadingTitleImage() 
{

    m_LoadingImageRect.right  = g_RenderSystem.GetClientWidth();
    m_LoadingImageRect.bottom = g_RenderSystem.GetClientHeight();


	m_pBackTexture = LoadResource<IDirect3DTexture9 , ResTextargetObj>("RTT.bmp" , 
		m_LoadingImageRect.right , m_LoadingImageRect.bottom) ;

	m_pLoadingTex1 = LoadStaticTexture("SkyBox\\Title.dds");

	m_pLoadingTex = m_pLoadingTex1;

}


bool JLAnimation2DManager::InitFadeOut()
{
#ifdef RESMGR


    m_pDevice = g_RenderSystem.GetD3DDevice();

	 //�H�Umark�����A�O�n�O�d

	//Ū���@��RenderTarget Textrure
	//GetResMgr().LockRes("RTT.bmp") ;

	//m_pBackTexture = LoadResource<IDirect3DTexture9 , ResTextargetObj>("RTT.bmp" , 
	//	m_LoadingImageRect.right , m_LoadingImageRect.bottom) ;

    
 

	//////////////////				Loading Texture				//////////////



	//�]���i�J�n�J�e���ɡA�|�I�sRemoveAllDynamicResource()�A�Ӧ�logo����Q�����A
	//�]�����귽�A�i�J��L�C�������һݭn�A�ҥH�b���]�w���R�A�귽�C����C������
	//m_pLoadingTex1 = LoadStaticTexture("SkyBox\\Load1.dds");
	m_pLoadingTex2 = LoadStaticTexture("SkyBox\\Load2.dds");
	m_pLoadingTex3 = LoadStaticTexture("SkyBox\\Load3.dds");
	m_pLoadingTex4 = LoadStaticTexture("SkyBox\\Load4.dds");
	m_pLoadingTex5 = LoadStaticTexture("SkyBox\\Load5.dds");



	m_pLoadingTex = m_pLoadingTex1 ; 


///////////////				Load logo texture			///////////////

   // ��l�ƭI����
    randomize();


    int iNumber = rand()%3;

    string strFileName;


    if( iNumber == 0 )
        strFileName = "SkyBox\\Logo1.dds";
    else if( iNumber == 1 )
        strFileName = "SkyBox\\Logo2.dds";
    else if( iNumber == 2 )
        strFileName = "SkyBox\\Logo3.dds";


	m_pLogoTex = LoadStaticTexture(strFileName);


	return true ;


#else
    m_LoadingImageRect.right  = g_RenderSystem.GetClientWidth();
    m_LoadingImageRect.bottom = g_RenderSystem.GetClientHeight();
    m_pDevice = g_RenderSystem.GetD3DDevice();

    // Ū���ʸ���
    memPack* pMem = LoadFromPacket( "SkyBox\\Scnlogo.tga" );
    if( pMem == NULL )
        return false;

    if( FAILED( D3DXCreateTextureFromFileInMemory( m_pDevice, pMem->Data, pMem->DataSize, &m_pBackTexture ) ) )
        return false;
    
    // ��l�ƭI����
    randomize();
    int iNumber = random(3);

    string strFileName;

    if( iNumber == 0 )
        strFileName = "SkyBox\\Logo1.tga";
    else if( iNumber == 1 )
        strFileName = "SkyBox\\Logo2.tga";
    else if( iNumber == 2 )
        strFileName = "SkyBox\\Logo3.tga";

    // Ū���ʸ���
    pMem = LoadFromPacket( strFileName );
    if( pMem == NULL )
        return false;

    SAFE_RELEASE( m_pTexture );

    if( FAILED( D3DXCreateTextureFromFileInMemoryEx( m_pDevice,
                                                     pMem->Data,
                                                     pMem->DataSize,
                                                     g_RenderSystem.GetClientWidth(),
                                                     g_RenderSystem.GetClientHeight(),
                                                     1,
                                                     D3DUSAGE_RENDERTARGET,
                                                     g_RenderSystem.GetRTTFormat(),
                                                     D3DPOOL_DEFAULT,
                                                     D3DX_DEFAULT ,
                                                     D3DX_DEFAULT ,
                                                     0,
                                                     NULL,
                                                     NULL,
                                                     &m_pTexture ) ) )

    {
        return false;
    }

    return true;

#endif




}
//----------------------------------------------------------------------------
bool JLAnimation2DManager::RestoreDeviceObjects()
{
    m_LoadingImageRect.right  = g_RenderSystem.GetClientWidth();
    m_LoadingImageRect.bottom = g_RenderSystem.GetClientHeight();

    m_pDevice = g_RenderSystem.GetD3DDevice();

#ifdef RESMGR

	m_pBackTexture = LoadResource<IDirect3DTexture9 , ResTextargetObj>("RTT.bmp" , 
		m_LoadingImageRect.right , m_LoadingImageRect.bottom) ;



	return true ;
#else

    SAFE_RELEASE( m_pTexture );


    if( FAILED( m_pDevice->CreateTexture( g_RenderSystem.GetClientWidth(),
                                          g_RenderSystem.GetClientHeight(), 1,
                                          D3DUSAGE_RENDERTARGET, g_RenderSystem.GetRTTFormat(),
                                          D3DPOOL_DEFAULT, &m_pTexture, NULL ) ) )
    {
        return false;
    }

    return true;

#endif



}
//----------------------------------------------------------------------------
bool JLAnimation2DManager::GrabScreenAsTexture(bool bBlend)
{
	m_iFadeOutTime = 0;

	//Need do this here because m_pBackTexture might be destroy while cache size it too small
	m_pBackTexture = LoadResource<IDirect3DTexture9 , ResTextargetObj>("RTT.bmp" , 
	m_LoadingImageRect.right , m_LoadingImageRect.bottom) ;

	//////////////////			���Back Buffer date			/////////////
	LPDIRECT3DSURFACE9 suf = NULL;
	LPDIRECT3DSURFACE9 psurface = NULL ;

	D3DSURFACE_DESC desc;

	//���o��logo��surface
	m_pBackTexture->GetSurfaceLevel(0, &psurface);
	if(psurface)
	{
		//�A���oRender Target suface
		m_pDevice->GetRenderTarget(0, &suf);
		//���o��surface���������
		psurface->GetDesc(&desc);

		RECT rect ; 

		rect.left = 0 ; 
		rect.top = 0 ; 
		rect.right = desc.Width; 
		rect.bottom = desc.Height;
		//copy surface�C����m_pBackTexture��texture�H�g�ORender Target ��texture
		m_pDevice->StretchRect(suf , 0 , psurface , &rect , 
			D3DTEXF_NONE) ; 

		//�O�o�nrelease
		SAFE_RELEASE(suf) ;
		SAFE_RELEASE(psurface) ;

	}

	/////////////		loading textrue  and backbuffer  blend			/////////////

	//�H����@�i
	RandLoadingTexure()  ; 

	Assert(m_pBackTexture != NULL , "backbuffer Error") ;
	Assert(m_pLoadingTex != NULL ,  "Loadingtex Error") ;

	//////////			�}�l��h�V��			/////////////
	m_pDevice->SetTexture( 0,  m_pLoadingTex) ;
	m_pDevice->SetTexture( 1,  m_pBackTexture  );


	DWORD dwLighting, dwZEnable , dwFogEnable ;

	m_pDevice->GetRenderState( D3DRS_LIGHTING, &dwLighting );
	m_pDevice->GetRenderState( D3DRS_ZENABLE, &dwZEnable );
	m_pDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );

	m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	DWORD oldAddressU0;
	m_pDevice->GetSamplerState(0, ::D3DSAMP_ADDRESSU, &oldAddressU0);
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);

	DWORD dwBlendEnable;
	m_pDevice->GetRenderState(::D3DRS_ALPHABLENDENABLE, &dwBlendEnable);
	m_pDevice->SetRenderState(::D3DRS_ALPHABLENDENABLE, FALSE);

	//�Ntexture and diffuse �@alpha�V��  
	m_pDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_MODULATE    ) ;
	m_pDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG1  , D3DTA_TEXTURE ) ;
	m_pDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_DIFFUSE ) ;

	//�Ntexture and diffuse �@color�V��
	m_pDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_SELECTARG1   ) ;
	m_pDevice->SetTextureStageState( 0 , D3DTSS_COLORARG1 , D3DTA_TEXTURE ) ;
	m_pDevice->SetTextureStageState( 0 , D3DTSS_COLORARG2 , D3DTA_DIFFUSE ) ;

	//�@alpha�V��  
	m_pDevice->SetTextureStageState( 1 , D3DTSS_ALPHAARG1  , D3DTA_TEXTURE ) ;
	m_pDevice->SetTextureStageState( 1 , D3DTSS_ALPHAARG2 ,  D3DTA_CURRENT) ;
	m_pDevice->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG1   ) ;

	//�@color�V��
	m_pDevice->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_BLENDCURRENTALPHA ) ;
	m_pDevice->SetTextureStageState( 1 , D3DTSS_COLORARG1 , D3DTA_CURRENT) ;
	m_pDevice->SetTextureStageState( 1 , D3DTSS_COLORARG2 , D3DTA_TEXTURE ) ;

	// Set the texture stages for the second texture unit (image).
	m_pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	if(bBlend)
	{
		//255����]�G���F�C�@��loop��T��]255����alpha control
		const int time = 600;
		const int sleepTime = 10;
		const int count = time/sleepTime;
		for(int i=0;i<count;i++ )
		{	
			g_RenderSystem.BeginFrame() ;

			Sleep(sleepTime);		
			BYTE alpha ;
			alpha = (BYTE)(float)(((float)(i+1) / count) * 255.0f) ; 
			D3DCOLOR color = D3DCOLOR_ARGB( alpha, 255, 255, 255 );
			DrawFadeOut( m_LoadingImageRect.right, m_LoadingImageRect.bottom, color );				
			g_RenderSystem.EndFrame();

		}
	}
	else
	{
		g_RenderSystem.BeginFrame() ;
		D3DCOLOR color = D3DCOLOR_ARGB( 255, 255, 255, 255 );
		DrawFadeOut( m_LoadingImageRect.right, m_LoadingImageRect.bottom, color );				
		g_RenderSystem.EndFrame();

	}
	//restore render state ...
	m_pDevice->SetTexture( 0 , NULL ) ;
	m_pDevice->SetTexture( 1 , NULL ) ;		

	m_pDevice->SetTextureStageState( 1 , D3DTSS_COLOROP ,D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState( 1 , D3DTSS_ALPHAOP ,D3DTOP_DISABLE);
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, oldAddressU0);

	m_pDevice->SetRenderState( D3DRS_LIGHTING, dwLighting );
	m_pDevice->SetRenderState( D3DRS_ZENABLE, dwZEnable );
	m_pDevice->SetRenderState(::D3DRS_ALPHABLENDENABLE, dwBlendEnable);
	return true ;
}
//----------------------------------------------------------------------------
void JLAnimation2DManager::SetShowFadeOut( bool bValue )
{
    m_bShowFadeOut = bValue;

	m_bCanShowFadeIn = false ;

}
//----------------------------------------------------------------------------
void JLAnimation2DManager::SetShowBackground( bool bValue )
{
    m_bShowBackground = bValue;
}
//----------------------------------------------------------------------------
void JLAnimation2DManager::ResetFadeOutTime()
{
    m_iFadeOutTime = (int)g_fFadeTime;
    m_LoadingImageColor = WHITE;

	m_bCanShowFadeIn = true ;
}
//----------------------------------------------------------------------------
void JLAnimation2DManager::InvalidateDeviceObjects()
{
#ifdef RESMGR

	ReleaseResource(m_pBackTexture) ; 

	m_pBackTexture = NULL ;

	GetCatch()->AllClear();


	//�]��m_pLoadingTex�O�R�A�F�A�ҥH���ݭnRelease
#else
    SAFE_RELEASE( m_pTexture );
#endif

}

//---------------------------------------------------------------------------

} // namespace FDO


