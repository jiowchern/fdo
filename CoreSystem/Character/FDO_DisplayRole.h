//----------------------------------------------------------------------------
#ifndef _FDO_DISPLAYROLE_H_
#define _FDO_DISPLAYROLE_H_
//----------------------------------------------------------------------------
#include <stack>
namespace FDO
{

	class ParticleObject ; 

	class CNumber : public IDXUTRender
	{
	public:
		CNumber();
		virtual ~CNumber() {}
	public:
		void RenderPipeline();
		void Update(float fElapsedTime);
		void SetViewPosition(float left , float top) ;
		void SetViewSize(unsigned int width , unsigned int height);
		void SetResetValue(int value) { m_resetValue = value; } 
		void SetCurrentValue(int value , D3DCOLOR color) { m_currValue = value ; m_color = color ; }
		bool IsZero() { return m_bZero ; } 
		void Play(){if(m_bZero){ m_bZero = false ;}}
		void SetScale(float scale){m_disScale = scale ; }
	private:
		D3DVIEWPORT9	m_ViewPort;
		D3DXMATRIX		m_matView;
		D3DXMATRIX		m_matProj;


		D3DCOLOR		m_color ; 
		float			m_disScale ; 
		int				m_currValue ;
		int				m_resetValue ;
		bool			m_bZero ; 

		static float	s_fixCount ;
		static D3DVIEWPORT9 s_ViewPort;
		static D3DXMATRIX s_matProj;
		static D3DXMATRIX s_matView;

		LPDIRECT3DDEVICE9       m_pd3dDevice;
	};


//----------------------------------------------------------------------------
class DisplayRole : public IDXUTRender
{

public:

	enum
	{	
		BeginField,
		nHelmet = BeginField,		//盔帽			#0      41
		nAdom,						//面飾			#1 肩甲 43
		nArmor,						//甲胄上*		#3      45
		nGlove,						//手套*			#10     47
		nPants,						//甲胄下*		#4 護腿 49
		nShoes,						//鞋靴*			#13     51	
		nNecklace,					//頸飾*			#2      53
		nCloak,						//披風			#9 背飾 55
		nWeapon_1,					//武器 1		#5      57
		nShield_1,					//盾牌 1		#6      59
		nR_Finger,					//右手指*		#11     61
		nL_Finger,					//左手指*		#12 腰帶63

		EQPART_MAX,					//裝備總數量20
	};

	//////////////////////////////////////////////////////////////////////////
	//部位資訊
	//////////////////////////////////////////////////////////////////////////
	struct SPartData
	{
		int nEQPart;	//裝備部位
		int nItemID;	//道具id
	};

public:
	SPartData PDArray[EQPART_MAX];

private:
// 屬性變數--------------------------------------------------------------------
    LPDIRECT3DDEVICE9       m_pd3dDevice;                                       //D3D裝置
    bool                    m_bVisible;                                         //顯示判斷子
    RoleDirect              m_eShowDir;                                         //顯示方向

//裝備變數--------------------------------------------------------------------
    sRoleAction*            m_pNowAction;                                       //目前使用之動作
	std::vector<sRoleAction*>	m_actionVec ;		
    BYTE                    m_byClassID;                                        //職業
    
     int                     m_iShieldID;                                        //盾牌
     int m_iHelmetID;	//盔帽			#1
 	int m_iAdomID;		//面飾			#2
 	int m_iArmorID;		//甲胄上*		#4
 	int m_iPantsID;		//甲胄下*		#5
 	int m_iWeaponID;	//武器 1		#6
 	int m_iCloakID;		//披風			#10
 	int m_iGloveID;		//手套*			#11
 	int m_iShoesID;		//鞋靴*			#14
	

// 位置變數--------------------------------------------------------------------
    D3DXVECTOR2             m_vPosition;                                        //位置
    D3DXVECTOR2             m_vScale;                                           //縮放
    float                   m_fTransRate;                                       //轉換比例

//裝備函式--------------------------------------------------------------------
    void                    RemoveAllAction();                                  //移除全部動作
    bool                    LoadComponent( const string& strFileName );         //載入零件

//繪製函式--------------------------------------------------------------------

public:
    DisplayRole();
    virtual ~DisplayRole();

    void                    Reset();
	void					ReleaseRoleRes();
// 建立函式
    //bool                    CreateRole( BYTE byClassID, short shFaceID );       //建立角色
    //bool                    CreateRole( const S_RoleSelection* pRole );         //建立角色
    //bool                    CreateRole( const sHuman_MainRole_Init* pRole );    //建立角色

// 繪製函式--------------------------------------------------------------------
	//void RenderRole(const D3DXMATRIX& matInv);	//繪製主角
	void RenderRole();	//繪製主角

// 設定函式--------------------------------------------------------------------
    void                    SetPosition( const D3DXVECTOR2& vPos );             //設定位置
    const D3DXVECTOR2&      GetPosition() const;                                //取得位置
    const D3DXVECTOR2&      GetScale() const;                                   //取得縮放
    void                    SetVisible( bool bValue ) { m_bVisible = bValue; }  //設定顯示判斷子
    bool                    GetVisible() { return m_bVisible; }                 //取得顯示判斷子
    void                    SetDirection( RoleDirect bValue ) { m_eShowDir = bValue; } //設定顯示方向
    RoleDirect              GetDirection() { return m_eShowDir; }               //取得顯示方向
    void                    NextDirection();                                    //下個方向
    void                    LastDirection();                                    //上個方向
	void					PushActionStack(const char *actName) ; 
	//void					SetAction(sRoleAction *pAct) { m_pNowAction = pAct ; } 
	// <yuming> --------------------------------------------------------------
public:
	void LockView() ; 
	void UnLockView(void) ; 
	void InitLockParts(const std::vector<std::string> &partVec) ; 
	void InitLockParts() ; //鎖定所有的parts
	void SetDisplayScale(float scale) ; 
public:
	bool IsGM() { return ( (m_byClassID==62) || (m_byClassID==63) ); }
    void SetSize(DWORD Width, DWORD Height);


    BYTE GetClassID()   { return m_byClassID; }
	
 	int  GetWeaponID()  { return m_iWeaponID; }
 	int  GetHelmetID()  { return m_iHelmetID; }
     int  GetAdomID()    { return m_iAdomID;   }
     int  GetCloakID()   { return m_iCloakID;  }
 	int  GetArmorID()	{ return m_iArmorID;  }
 	int  GetPantsID()	{ return m_iPantsID;  }
 	int  GetGloveID()	{ return m_iGloveID;  }
 	int  GetShoesID()	{ return m_iShoesID;  }
 	int  GetShieldID()	{ return m_iShieldID; }

	int	GetBodyID(int nRaceID, int nClassID) { return nRaceID * 20 + nClassID; }
	
//	const int  GetItemID(const int EQpartNum);

	int		GetDesignatePartItemID(int part);	//取得指定部位的資料

	void	SetDesignatePartItemID(int part, int itemID);	//設定指定部位的資料


    void  SetClassID(BYTE byClassID)  { m_byClassID=byClassID; }	//職業

 	void  SetWeaponID(int weaponID)  { m_iWeaponID = weaponID; }	//武器
 	void  SetHelmetID(int helmetID)  { m_iHelmetID = helmetID; }	//盔帽
 	void  SetAdomID(int adomID)      { m_iAdomID = adomID;    }		//面飾
 	void  SetCloakID(int cloakID)    { m_iCloakID = cloakID;  }		//披風
 	void  SetArmorID(int armorID)	{ m_iArmorID = armorID;  }		//甲胄上
 	void  SetPantsID(int pantsID)	{ m_iPantsID = pantsID;  }  	//甲胄下
 	void  SetGloveID(int gloveID)	{ m_iGloveID = gloveID;  }		//手套
 	void  SetShoesID(int shoesId)	{ m_iShoesID = shoesId;  }		//鞋
 	void  SetShieldID(int shieldID) { m_iShieldID = shieldID; }
	
	void  SetEQItemID(int EQpartNum, int itemID);
	void  ClearDisplayRoleAllInfo();

    // Move from private
    bool LoadImage( const string& strFileName );    //載入圖形
    void HideComponent( const char* pComName );     //隱藏零件
	void HideComponents();							//隱藏動作下所有零件
	bool LoadMonster(const string& strFileName );
	bool LoadMonsterEx(const string& strFileName );

    bool LoadRoleAll( const string& strFileName );  //載入整個角色
    bool LoadAction( const string& strFileName );   //載入動作
	bool LoadPart(const string& strFileName );		//載入部位檔案
	bool UnLoadPart(const string& strFileName );	//清除部位檔案
    bool HidePart(const char* pFilename);
	void SwapWeapon(WORD oldWeaponID, WORD newWeaponID, short gender ); //交換顯示的武器

//	void SetOnePartOrder(const Human_Equip_Change* pEquip);	//設定單一部位裝備

    bool LoadActionEx(const char* pFileName);
    bool LoadImageEx(const char* pFileName);        //載入圖形

	inline void	SetDynamicRender	(bool bOK)	{ m_bDynamicRender = bOK; }

	void RenderPipeline() ; 
	void Update(float fElapsedTime) ; 
	void SetViewPosition(float left , float top)  ; 
	void SetViewSize(unsigned int width , unsigned int height)  ; 
	void SetAction(const char* actName) ; 
	void PushParticle(const char *emtName , bool isStatic) ; 
private:
    //
    D3DVIEWPORT9 m_ViewPort;
    D3DXMATRIX m_matView;
    D3DXMATRIX m_matProj;
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matInvWorld;
    //
    static D3DVIEWPORT9 s_ViewPort;
    static D3DXMATRIX s_matProj;
    static D3DXMATRIX s_matView;
    // 蒐集影格的容器
    sFrameList m_RenderFrames;

	TdgHandle m_hRoleResRequester;
    //
    bool BeginRender();
    bool EndRender();
    //
    bool GetRenderFrames(float fElapsedTime);
    void RenderFrame(sFrame* pFrame);
//    void RenderFrame3D(sFrame* pFrame);
    void HideImage(const char* pFilename);
	void UnLoadImageEx(const char* pFileName);   //清除載入圖形
	void UnLoadImage(const char* pFileName);   //清除載入圖形
	void UnLoadComponentTexture( sRoleComponent* pRoleComponent );

	void FilterAction() ; 
	sRoleAction* GetAction(const char *actionName) ; 
	bool m_bDynamicRender ; // 
	// </yuming> -------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	//讀取骨架皮膚
	//////////////////////////////////////////////////////////////////////////
	// 以下的東西全部移至 RoleRenderer 裡
	// 為的是全部集中，模組化，有關角色繪圖、讀取 RAT、RPC
	// 的部份就全部交給 RoleRenderer 去處理

	//////////////////////////////////////////////////////////////////////////
	// 以下是新的作法
	//////////////////////////////////////////////////////////////////////////
	CRPCFactory*			m_pRPCFactory;
	CRATFactory*			m_pRATFactory;
	//CRoleRenderer           m_RoleRenderer;

	//////////////////////////////////////////////////////////////////////////
	//載入資料部份
	//////////////////////////////////////////////////////////////////////////
	bool				    LoadRATFile(const char* pFileName, CRATFactory* pRATFactory);
	
	bool                    LoadPartFile(const char* pFilename, CRPCFactory* pRPCFactory);                    //載入部位檔案
	bool                    UnloadPartFile(const char* pFilename, CRPCFactory* pRPCFactory);                    //隱藏部位檔案
	
	bool				    LoadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
	void                    UnloadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
	
	void                    UnloadAllRPCFile();
	
	//////////////////////////////////////////////////////////////////////////
	//繪製部份
	//////////////////////////////////////////////////////////////////////////
	void				    PlaySkinAct(const char* szActionName, bool bLoop );

	void				    RenderRoleSkin();
	void					RenderFramesOriginal();
	//////////////////////////////////////////////////////////////////////////
	// 以上是新的作法
	//////////////////////////////////////////////////////////////////////////
	std::vector<sFrame*>			m_lockPartVec ; 
	float							m_disScale ; 
//	float							m_frameCenter; 
//	D3DXVECTOR2						m_viewOffset ; 

	std::stack<const char*>				m_playStack ; 

	float							m_passTime  ; 
	unsigned short					m_usFrameMax   ; 
	unsigned short					m_usFrameNum   ; 

	ParticleObject					*m_pParticle ; 
	DWORD							m_oldCullState;
};


//----------------------------------------------------------------------------
}
//----------------------------------------------------------------------------
#endif // _FDO_DISPLAYROLE_H_
