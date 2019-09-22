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
		nHelmet = BeginField,		//���U			#0      41
		nAdom,						//����			#1 �ӥ� 43
		nArmor,						//�ҭH�W*		#3      45
		nGlove,						//��M*			#10     47
		nPants,						//�ҭH�U*		#4 �@�L 49
		nShoes,						//�c�u*			#13     51	
		nNecklace,					//�V��*			#2      53
		nCloak,						//�ܭ�			#9 �I�� 55
		nWeapon_1,					//�Z�� 1		#5      57
		nShield_1,					//�޵P 1		#6      59
		nR_Finger,					//�k���*		#11     61
		nL_Finger,					//�����*		#12 �y�a63

		EQPART_MAX,					//�˳��`�ƶq20
	};

	//////////////////////////////////////////////////////////////////////////
	//�����T
	//////////////////////////////////////////////////////////////////////////
	struct SPartData
	{
		int nEQPart;	//�˳Ƴ���
		int nItemID;	//�D��id
	};

public:
	SPartData PDArray[EQPART_MAX];

private:
// �ݩ��ܼ�--------------------------------------------------------------------
    LPDIRECT3DDEVICE9       m_pd3dDevice;                                       //D3D�˸m
    bool                    m_bVisible;                                         //��ܧP�_�l
    RoleDirect              m_eShowDir;                                         //��ܤ�V

//�˳��ܼ�--------------------------------------------------------------------
    sRoleAction*            m_pNowAction;                                       //�ثe�ϥΤ��ʧ@
	std::vector<sRoleAction*>	m_actionVec ;		
    BYTE                    m_byClassID;                                        //¾�~
    
     int                     m_iShieldID;                                        //�޵P
     int m_iHelmetID;	//���U			#1
 	int m_iAdomID;		//����			#2
 	int m_iArmorID;		//�ҭH�W*		#4
 	int m_iPantsID;		//�ҭH�U*		#5
 	int m_iWeaponID;	//�Z�� 1		#6
 	int m_iCloakID;		//�ܭ�			#10
 	int m_iGloveID;		//��M*			#11
 	int m_iShoesID;		//�c�u*			#14
	

// ��m�ܼ�--------------------------------------------------------------------
    D3DXVECTOR2             m_vPosition;                                        //��m
    D3DXVECTOR2             m_vScale;                                           //�Y��
    float                   m_fTransRate;                                       //�ഫ���

//�˳ƨ禡--------------------------------------------------------------------
    void                    RemoveAllAction();                                  //���������ʧ@
    bool                    LoadComponent( const string& strFileName );         //���J�s��

//ø�s�禡--------------------------------------------------------------------

public:
    DisplayRole();
    virtual ~DisplayRole();

    void                    Reset();
	void					ReleaseRoleRes();
// �إߨ禡
    //bool                    CreateRole( BYTE byClassID, short shFaceID );       //�إߨ���
    //bool                    CreateRole( const S_RoleSelection* pRole );         //�إߨ���
    //bool                    CreateRole( const sHuman_MainRole_Init* pRole );    //�إߨ���

// ø�s�禡--------------------------------------------------------------------
	//void RenderRole(const D3DXMATRIX& matInv);	//ø�s�D��
	void RenderRole();	//ø�s�D��

// �]�w�禡--------------------------------------------------------------------
    void                    SetPosition( const D3DXVECTOR2& vPos );             //�]�w��m
    const D3DXVECTOR2&      GetPosition() const;                                //���o��m
    const D3DXVECTOR2&      GetScale() const;                                   //���o�Y��
    void                    SetVisible( bool bValue ) { m_bVisible = bValue; }  //�]�w��ܧP�_�l
    bool                    GetVisible() { return m_bVisible; }                 //���o��ܧP�_�l
    void                    SetDirection( RoleDirect bValue ) { m_eShowDir = bValue; } //�]�w��ܤ�V
    RoleDirect              GetDirection() { return m_eShowDir; }               //���o��ܤ�V
    void                    NextDirection();                                    //�U�Ӥ�V
    void                    LastDirection();                                    //�W�Ӥ�V
	void					PushActionStack(const char *actName) ; 
	//void					SetAction(sRoleAction *pAct) { m_pNowAction = pAct ; } 
	// <yuming> --------------------------------------------------------------
public:
	void LockView() ; 
	void UnLockView(void) ; 
	void InitLockParts(const std::vector<std::string> &partVec) ; 
	void InitLockParts() ; //��w�Ҧ���parts
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

	int		GetDesignatePartItemID(int part);	//���o���w���쪺���

	void	SetDesignatePartItemID(int part, int itemID);	//�]�w���w���쪺���


    void  SetClassID(BYTE byClassID)  { m_byClassID=byClassID; }	//¾�~

 	void  SetWeaponID(int weaponID)  { m_iWeaponID = weaponID; }	//�Z��
 	void  SetHelmetID(int helmetID)  { m_iHelmetID = helmetID; }	//���U
 	void  SetAdomID(int adomID)      { m_iAdomID = adomID;    }		//����
 	void  SetCloakID(int cloakID)    { m_iCloakID = cloakID;  }		//�ܭ�
 	void  SetArmorID(int armorID)	{ m_iArmorID = armorID;  }		//�ҭH�W
 	void  SetPantsID(int pantsID)	{ m_iPantsID = pantsID;  }  	//�ҭH�U
 	void  SetGloveID(int gloveID)	{ m_iGloveID = gloveID;  }		//��M
 	void  SetShoesID(int shoesId)	{ m_iShoesID = shoesId;  }		//�c
 	void  SetShieldID(int shieldID) { m_iShieldID = shieldID; }
	
	void  SetEQItemID(int EQpartNum, int itemID);
	void  ClearDisplayRoleAllInfo();

    // Move from private
    bool LoadImage( const string& strFileName );    //���J�ϧ�
    void HideComponent( const char* pComName );     //���ùs��
	void HideComponents();							//���ðʧ@�U�Ҧ��s��
	bool LoadMonster(const string& strFileName );
	bool LoadMonsterEx(const string& strFileName );

    bool LoadRoleAll( const string& strFileName );  //���J��Ө���
    bool LoadAction( const string& strFileName );   //���J�ʧ@
	bool LoadPart(const string& strFileName );		//���J�����ɮ�
	bool UnLoadPart(const string& strFileName );	//�M�������ɮ�
    bool HidePart(const char* pFilename);
	void SwapWeapon(WORD oldWeaponID, WORD newWeaponID, short gender ); //�洫��ܪ��Z��

//	void SetOnePartOrder(const Human_Equip_Change* pEquip);	//�]�w��@����˳�

    bool LoadActionEx(const char* pFileName);
    bool LoadImageEx(const char* pFileName);        //���J�ϧ�

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
    // �`���v�檺�e��
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
	void UnLoadImageEx(const char* pFileName);   //�M�����J�ϧ�
	void UnLoadImage(const char* pFileName);   //�M�����J�ϧ�
	void UnLoadComponentTexture( sRoleComponent* pRoleComponent );

	void FilterAction() ; 
	sRoleAction* GetAction(const char *actionName) ; 
	bool m_bDynamicRender ; // 
	// </yuming> -------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	//Ū�����[�ֽ�
	//////////////////////////////////////////////////////////////////////////
	// �H�U���F��������� RoleRenderer ��
	// �����O���������A�ҲդơA��������ø�ϡBŪ�� RAT�BRPC
	// �������N�����浹 RoleRenderer �h�B�z

	//////////////////////////////////////////////////////////////////////////
	// �H�U�O�s���@�k
	//////////////////////////////////////////////////////////////////////////
	CRPCFactory*			m_pRPCFactory;
	CRATFactory*			m_pRATFactory;
	//CRoleRenderer           m_RoleRenderer;

	//////////////////////////////////////////////////////////////////////////
	//���J��Ƴ���
	//////////////////////////////////////////////////////////////////////////
	bool				    LoadRATFile(const char* pFileName, CRATFactory* pRATFactory);
	
	bool                    LoadPartFile(const char* pFilename, CRPCFactory* pRPCFactory);                    //���J�����ɮ�
	bool                    UnloadPartFile(const char* pFilename, CRPCFactory* pRPCFactory);                    //���ó����ɮ�
	
	bool				    LoadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
	void                    UnloadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
	
	void                    UnloadAllRPCFile();
	
	//////////////////////////////////////////////////////////////////////////
	//ø�s����
	//////////////////////////////////////////////////////////////////////////
	void				    PlaySkinAct(const char* szActionName, bool bLoop );

	void				    RenderRoleSkin();
	void					RenderFramesOriginal();
	//////////////////////////////////////////////////////////////////////////
	// �H�W�O�s���@�k
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
