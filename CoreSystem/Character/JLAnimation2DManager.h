//----------------------------------------------------------------------------
#ifndef _JLAnimation2DManager_H_
#define _JLAnimation2DManager_H_

#include "singleton.h"
#include "JLAnimation2D.h"

//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("管理模式啟動") 

#include "resourceMgr.h"


#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////

#define g_Animation2DManager JLAnimation2DManager::GetSingleton()



//----------------------------------------------------------------------------

namespace FDO
{
class JLAnimation2D;
typedef list<JLAnimation2D*> Animation2DList;

class JLAnimation2DManager : public Singleton<JLAnimation2DManager>
{
private:
    Animation2DList m_Animation2DList;

    // 滑鼠動畫角色指標
    JLAnimation2D*     m_pMouseAnimation;

    // Loading圖 變數
    LPDIRECT3DDEVICE9  m_pDevice;

#ifdef RESMGR

	//////////			Loading Textrure			////////////
	/*stLogtexture	   *m_pLoadingTex ;
	stLogtexture	   *m_pLoadingTex1 ;
	stLogtexture	   *m_pLoadingTex2 ;
	stLogtexture	   *m_pLoadingTex3;
	stLogtexture	   *m_pLoadingTex4;
	stLogtexture	   *m_pLoadingTex5;*/

	IDirect3DTexture9	   *m_pLoadingTex ;
	IDirect3DTexture9	   *m_pLoadingTex1 ;
	IDirect3DTexture9	   *m_pLoadingTex2 ;
	IDirect3DTexture9	   *m_pLoadingTex3;
	IDirect3DTexture9	   *m_pLoadingTex4;
	IDirect3DTexture9	   *m_pLoadingTex5;



	//////////////		logo Textrure				///////////
	IDirect3DTexture9	   *m_pLogoTex ;


#else
	IDirect3DTexture9* m_pTexture;
#endif


    
    IDirect3DTexture9* m_pBackTexture;   //存放back buffer

    bool               m_bShowFadeOut;
    bool               m_bShowBackground;
    DWORD              m_LoadingImageColor;
    int                m_iFadeOutTime;
    RECT               m_LoadingImageRect;
    bool			   m_bCanShowFadeIn;
	//<gino>
	std::string m_defaultMouseAnim; ///常態游標
	//</gino>
public:
    JLAnimation2DManager();
    ~JLAnimation2DManager();

    // 建立與刪除動畫函式
    JLAnimation2D*  CreateAnimationRole( const string& FileName );
    bool            RemoveAnimationRole( JLAnimation2D* pAnimation );
    void            RemoveAllAnimationRole();

    // 滑鼠動畫函式
    bool            CreateMouseAnimation( const string& FileName );
    JLAnimation2D*  GetMouseAnimation();

	//隨機選擇loading tex
	void			RandLoadingTexure(void) ;



    // 更新函式
    void            FrameMoveAnimationRole(DWORD dwPassTime);

    // 繪圖函式
    void            DrawAnimationRole();
    void            DrawBackground();

    // 初始化與顯示淡出
    bool            InitFadeOut();
    bool            GrabScreenAsTexture(bool bBlend = true);
    void            SetShowFadeOut( bool bValue );
    void            SetShowBackground( bool bValue );
    void            ResetFadeOutTime();
    bool            RestoreDeviceObjects();
    void            InvalidateDeviceObjects();
	void			LoadingTitleImage() ; 

	//<gino>
	void SetDefaultMouseAnimName(const char *str){ m_defaultMouseAnim = str;} 
	//</gino>
};

} // namespace FDO


//----------------------------------------------------------------------------
#endif // ROLE_H