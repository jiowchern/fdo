//----------------------------------------------------------------------------
#ifndef _FDO_ROLEBASE_H_
#define _FDO_ROLEBASE_H_

#include "FDO_RoleMemoryMgr.h"
#include "JLAnimation.h"
#include "TextObject.h"
#include <d3dx9math.h>
#include <boost\shared_ptr.hpp>
#include "IGDRole.h"

#include "RoleBaseData.h"

// STL
#include <string>
#include <list>

// using STL
using std::wstring;
using std::list;
//---------------------------------------------------------------------------
#ifdef RESMGR
#pragma message("�귽�޲z�Ҧ��Ұ�") 

#else
#pragma message("�귽�@��Ҧ��Ұ�") 
#endif




#include "CRoleDef.h"

//////////////////////////////////////////////////////////
namespace FDO
{
	class RoleBase ; 
	class CRenderer ; 

	bool ActorSort(RoleBase* i, RoleBase* j) ; 

	

struct SkinRPC;
struct SkinRAT;
struct SkinAct;
struct RoleSkin;
//---------------------------------------------------------------------------
//  �������
//---------------------------------------------------------------------------
enum RoleKind
{
    Role_None		= 0,
    Role_Leader		= 1,
    Role_Costar		= 2,
    Role_Monster	= 3,
    Role_NPC		= 4,
    Role_Item		= 5,
    Role_Pet		= 6,
	Role_TeamMember	= 7,
};

//---------------------------------------------------------------------------
// ����ʧ@ New Version for �Ѧa�T
//---------------------------------------------------------------------------
enum
{
	ACT_None = -1,	// �L
	ACT_Atk01 = 3,	// 1������
	ACT_Atk02,		// 2������
	ACT_Atk03,		// 3������
	ACT_Atk04,		// 4������
	ACT_Atk05,		// 5������
	ACT_Atk06,		// 6������
	ACT_Atk07,		// 7������
	ACT_Atk08,		// 8������
	ACT_Atk09,		// 9������
	ACT_Atk10,		// 10������
	ACT_Stand,		// �ݾ�
	ACT_Guard,		// ĵ��
	ACT_Injury,		// ����
	ACT_Walk,		// �B��
	ACT_Sit,		// ���U
	ACT_Fall,		// �˦a
	ACT_Dead,		// ���`
	ACT_Ghost,		// ����
	ACT_Charge,		// �E��
	ACT_Cast,		// �k�N
	ACT_Loot,		// �B��
	ACT_Critical,	// �z��
	ACT_Dodge,		// �{�� (�ĤH�S�����A)
	ACT_Miss,		// ���� (�S�����ĤH)
	ACT_Parry,		// ����
	ACT_Change,		// �\�u�ܨ�
	ACT_Market,		// �u�c
};

//---------------------------------------------------------------------------
// Class RoleBase
//---------------------------------------------------------------------------
struct sRoleAllRes
{
public:

    float fScale;
    float usShadowSize;
    float usShadowOffset;
    unsigned short  usBoundingBoxSize;
    unsigned short  usDeadBoundBoxSize;
    sRoleActionList mActionList;
	bool			bThreadOk ; 
};


struct stRoleBaseData
{
public:
	void Clear()
	{
		pShadowSize			= NULL;                                      //�v�l�j�p
		pShadowOffset		= NULL;                                    //�v�l������
		pBoundingBoxSize	= NULL;                                //��ɲ��j�p
		pDeadBoundBoxSize	= NULL; 
		pScale				= NULL ; 
		pActionList			= NULL ; 
		showSizeUnit		= 0.0f ; 
		showOffsetUnit		= 0.0f ; 
		bThreadOk			= NULL ; 
	}
	void Set(sRoleActionList *pActionList , float *pScale , float *pShadowSize , float *pShadowOffset , 
		unsigned short *pBoundingBoxSize , unsigned short *pDeadBoundBoxSize , bool *bThreadOk)
	{
		this->pActionList			= pActionList ;
		this->pShadowSize			= pShadowSize;                                      //�v�l�j�p
		this->pShadowOffset			= pShadowOffset;                                    //�v�l������
		this->pBoundingBoxSize		= pBoundingBoxSize;                                //��ɲ��j�p
		this->pDeadBoundBoxSize		= pDeadBoundBoxSize; 
		this->pScale				= pScale ; 
		this->bThreadOk				= bThreadOk ; 
	}
	float			        *pShadowSize;                                      //�v�l�j�p
    float		            *pShadowOffset;                                    //�v�l������
    unsigned short          *pBoundingBoxSize;                                //��ɲ��j�p
    unsigned short          *pDeadBoundBoxSize; 
	float					*pScale ; 
	sRoleActionList			*pActionList ; 
	float					showSizeUnit ; 
	float					showOffsetUnit ; 
	bool					*bThreadOk ; 
};

class RoleBase : public RoleBaseData
{
	friend bool ActorSort(RoleBase* i, RoleBase* j)  ; 

private:
	bool						m_shareCatch ; 
	//bool                    m_bVisible;                                         //��ܧP�_�l
protected:
//�˳��ܼ�-----------------------------------------------------------------
    sRoleActionList         m_ActionList;                                       //�ʧ@�}�C
    sRoleAction*            m_pNowAction;                                       //�ثe�ϥΤ��ʧ@
	SRATActFile* 			m_pCurrRATAct;										//�ثe�ʧ@����NEW

#ifdef RESMGR
	std::string				m_actname ; 
#else

#endif
// ��m�ܼ�----------------------------------------------------------------
    D3DXVECTOR3             m_vPosition;                                        //��m
    bool                    m_bMatrixChange;                                    //�@�ɯx�}���ܧP�_�l
	D3DXVECTOR3				m_shakeDir ;										//�_�ʤ�V
// ���񱱨��ܼ�-----------------------------------------------------------
    unsigned short          m_usFrameNum;                                       //��ܱi��
    bool                    m_bPlayLoop;                                        //�j�鼽��
    DWORD                   m_dwPassTime;                                       //�g�L�ɶ�
    RoleDirect              m_eShowDir;                                         //��ܤ�V
    bool                    m_bPlaySound;                                       //�����n���P�_�l
	bool					m_bShake;		//�_�ʶ}��
	bool					m_bDrop ; 
// �˳ƨ禡----------------------------------------------------------------
    void                    RemoveAllAction();                                  //���������ʧ@  

// �ٸ��ܼ�----------------------------------------------------------------
    int                     m_NameWidth;                                        //�ٸ��e��
    wstring                 m_NameTitle;                                        //�ٸ��r��
    RECT                    m_NameRect;                                         //�ٸ��j�p

    bool                    m_bShowShadow;                                      //��ܼv�l�P�_�l
	bool					m_bDivide ;											//�O�_���ʹݼv
	bool					m_bRage	;											//����]�w

// �ƥ󪬺A�ܼ�-----------------------------------------------------------
    int                     m_IDWidth;
    RECT                    m_IDRect;                                         //�ٸ��j�p
	D3DXVECTOR3             m_NpcEventScreenPos;							  //�ƥ󴣥ܦb�ù��W����m


// ��ܦJ�ܼ�--------------------------------------------------------------
	unsigned int			m_TalkPassTime;
	unsigned int			m_TalkShowTime;										//��ܦJ��ܮɶ�			
    bool					m_bShowTalk;
	bool					m_bUseList;
	RECT					m_TalkRect;
	wstring					m_TextString;
	CTextObject*			m_pTextObject;
	CDXUTListBox*			m_pListBox;
	unsigned int			m_iTalkWaitTime;									//�ۨ��ۻy�����j�ɶ�
	unsigned int			m_iTalkWaitNowTime;									//�ۨ��ۻy���w�g���ݮɶ�
	bool					m_bNpcTalkSelf;										//�O�_�ۨ��ۻy
	D3DXVECTOR3             m_NpcTalkScreenPos;									//��ܦb�ù��W����m

protected:
	sRoleAllRes			    *m_BaseData ; 
    sRoleActionList*        m_pActionList;                                      //�˳ưʧ@�}�C����
    D3DXVECTOR3             m_vScale;                                           //�Y��
    D3DXVECTOR3             m_vPosOffset;
    float                   m_fShadowSize;                                      //�v�l�j�p
    float                   m_fShadowOffset;                                    //�v�l������
    LPDIRECT3DDEVICE9       m_pDevice;                                          //D3D�˸m
	D3DXMATRIX				m_matShake;											//�_��matrix 
    D3DXMATRIX              m_matInv;
    D3DXMATRIX              m_matWorld;                                         //�@�ɯx�}
    D3DXMATRIX              m_matInvWorld;                                      //�ϦV�@�ɯx�}
    D3DXMATRIX              m_matPos;                                           //��m�x�}
    D3DXMATRIX              m_matInvPos;                                        //�ϦV��m�x�}
    DWORD                   m_dwUpdateTime;                                     //��s�ɶ�
    RoleDirect              m_eRoleDir;                                         //�H����V
    D3DCOLOR                m_Color;                                            //����C��
	D3DCOLOR				m_pickColor; 
    bool                    m_bVisible;                                         //��ܧP�_�l
    bool                    m_bShowName;                                        //�ٸ���ܧP�_�l
    HDC                     m_hDC;                                              //Device Context
    RoleKind                m_RoleKind;                                         //�������
    unsigned short          m_usBoundingBoxSize;                                //��ɲ��j�p
    unsigned short          m_usDeadBoundBoxSize;                               //���`��ɲ��j�p
    D3DXVECTOR3             m_ScreenPos;                                        //�ù��W����m
	RECT					m_rcBody;
    D3DCOLOR                m_NameColor;                                        //�ٸ��C��
    PlayStatus              m_PlayStatus;                                       //���񪬺A
    unsigned short          m_usFrameMax;                                       //��̤ܳj�i��
    char                    m_szNowActName[20];                                 //�{�b�ʧ@�W��
    int                     m_iCellPosX;                                        //�۹��V��l��
    int                     m_iCellPosY;                                        //�۹��a�V��l��
    int                     m_iNowAction;                                       //�ثe�H���ʧ@
	float                   m_fDistance;

	RoleID&					m_RoleId;											//����ClientID
	int						m_BodyID;
	const int				GetBodyID(int nRaceID, int nClassID){ return nRaceID * 20 + nClassID; }
	D3DXVECTOR3				m_dropDir ; 



    
    CTextObject             m_TextObjectNameTitle;
    CTextObject             m_TextObjectIDTitle;

public:
    RoleBase(uint nType , PropertyPtr pBase , PropertyPtr pStatus);
    virtual ~RoleBase();

    HDC                     GetHDC() { return m_hDC; }
    void                    ResetBase();
	MessageResult			BtnResult;
	void SetShake(bool bShake , const D3DXVECTOR3 &shakeDir) { m_bShake = bShake ; m_shakeDir = shakeDir ;} 
	void SetDrop(bool bDrop) { m_bDrop = bDrop; }
	bool GetShake(void) { return m_bShake ; }
	bool GetDrop(void) {return m_bDrop ; } 
	//////////////////////////////////////////////////////////////////////////
	//Ū�����[�ֽ�
	//////////////////////////////////////////////////////////////////////////
    // �H�U���F��������� RoleRenderer ��
    // �����O���������A�ҲդơA��������ø�ϡBŪ�� RAT�BRPC
    // �������N�����浹 RoleRenderer �h�B�z
    //////////////////////////////////////////////////////////////////////////
    // �H�U�O�s���@�k
    /////////////////////////////////////////////////////////////////////////
	CRPCFactory*			m_pRPCFactory;
	CRATFactory*			m_pRATFactory;
    CRoleRenderer*          m_pRoleRenderer;

	const int				GetBodyID() { return m_BodyID; }

	inline void					SetVidide(bool divide) { m_bDivide = divide; } 
	inline void					SetRage(bool bRage) { m_bRage = bRage; SetVidide(m_bRage); } 
	bool                    LoadPart(const char* pFilename, CRPCFactory* pRPCFactory);                    //���J�����ɮ�
    bool                    UnloadPart(const char* pFilename, CRPCFactory* pRPCFactory);                    //���ó����ɮ�
	bool				    LoadRATFile(const char* pFileName, CRATFactory* pRATFactory);
	bool				    LoadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
    void                    UnloadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
    void                    UnloadAllRPCFile();
	void				    PlaySkinAct(const char* szActionName, bool bLoop );
	void				    RenderRoleSkin(RoleBase *pRole , CRenderer *pRender);
	void					RenderFramesOriginal(RoleBase *pRole , CRenderer *pRender);
	void					FlushFrameOriginal() ; 
    //////////////////////////////////////////////////////////////////////////
    // �H�W�O�s���@�k
    //////////////////////////////////////////////////////////////////////////

	// Ū�ɨ禡
    bool                    LoadRoleAll( const string& strFileName );           //���J��Ө���
    bool                    LoadComponent( const string& strFileName );         //���J�s��
    sRoleAction*            GetAction( const char* pActName );                  //���o�ʧ@����
	//��s����
	void					FrameMoveRole	(const DWORD dwPassTime, const D3DXMATRIX& matInv,
                                const D3DXVECTOR3& vDir, const float fScale,
                                bool bAngleChange, const D3DXVECTOR3& vPosOffset , 
								const D3DXVECTOR4 &rectRange);

	//��s���� ��L�ݭn��ܪ�2D����������m
	virtual	void			FrameMove2DInfo	(const DWORD dwPassTime ,  const D3DXVECTOR4 &rectRange);
	// �ٸ��禡------------------------------------------------------------
    void                    FrameMoveNameTitle();									//��s�ٸ�
	// ��s�ù���m�禡--------------------------------------------------
    virtual void            FrameMoveScreenPos	( const D3DXMATRIX& matInv );		//��s�ù���m�禡	

	// ø�s�禡-------------------------------------------------------------
    void                    RenderRole		();
	virtual void			RenderOther		();
	virtual	const int		GetRoleID		() const = 0;

    //short                   GetRenderFrame( sFrame* ppFrameArray[], short shSize );
    void                    RenderShadow(CRenderer *prender , const float fOffsetX, const float fOffsetY, const float fOffsetZ);

// �]�w�禡---------------------------------------------------------------
	void					SetPickColor(bool bPick , DWORD Color ) ; 
	bool					IsPickColor() {return m_bPickColor ; } 
    void                    SetPosition( const D3DXVECTOR3& vPos );             //�]�w��m
    void                    SetScale( const D3DXVECTOR3& vScale );              //�]�w�Y��
    const D3DXVECTOR3&      GetPosition() const;                                //���o��m
    const D3DXVECTOR3&      GetScale() const;                                   //���o�Y��
    void                    SetColor(DWORD Color);                              //�]�w�C��
    const D3DCOLOR&         GetColor();                                         //���o�C��
    virtual void            SetVisible(bool bValue );                            //�]�w��ܧP�_�l
    bool                    GetVisible();                                       //���o��ܧP�_�l
    void                    SetDirection( RoleDirect eDirect );                 //�]�w��V 
    RoleDirect              GetDirection();                                     //���o��V
    //void                    SetPositoinWithOffset( const D3DXVECTOR3& vPos );   //�]�w�����m(�t�����q)
    D3DXVECTOR3             GetPositoinWithoutOffset();                         //���o�����m(������q)
    D3DXVECTOR3             GetCenterPosition();                                //���o���⤤���I��m
    RoleKind                GetRoleKind();                                      //���o�������
    void                    SetUpdateTime( const DWORD& dwUpdateTime );         //�]�w��s�ɶ�  

    void                    SetShowShadow( bool bValue );                       //�]�w��ܼv�l�P�_�l
    bool                    GetShowShadow();                                    //���o��ܼv�l�P�_�l  
	void					SetRoleData(sRoleAllRes *pRoleData);
	bool					IsRoleData() { return (m_BaseData != NULL) ; }
//���񱱨�禡------------------------------------------------------------
    void                    Play( const char* szActionName, bool bLoop );       //����
    void                    SharePlay( const char* szActionName, bool bLoop, bool bIsNPC = false );  //�@�ɼҦ�����
    void                    Stop();                                             //����(after Stop position goto top)
    void                    Pause( unsigned short usFrameNum );                 //�Ȱ�   

//�ٸ��禡-----------------------------------------------------------------
    void                    SetNameTitle( const char16* szName );                 //�]�w�ٸ�
	void                    SetNameTitleHV( const WCHAR* szName );              //�]�w�ٸ�
    void                    SetCondition( int SP );                             //�]�w�ƥ󪬺A
	int						NPCDifference;										//FDO�MHVO��NPC�����׮t
    LPCWSTR                 GetNameTitle() { return m_NameTitle.c_str(); }      //���o�ٸ�
    void                    SetShowNameTitle( bool bShow );                     //�]�w��ܺٸ�
    void                    SetNameTitleColor( D3DCOLOR clrColor );             //�]�w�ٸ��C��
    void                    RenderNameTitle();                                  //��ܺٸ�

// NPC��ܨ禡-------------------------------------------------------------
    void                    SetNPCTalkText( const char* szText );               //�]�w��ܰT��
    void                    FrameMoveNPCTalk( const DWORD& dwPassTime );        //��s��ܹ��
    void                    RenderNPCTalk();                                    //��ܹ�ܰT��
	void					AddMessage( const string &Message );				//������
	void					SetNPCTalkList();									//������
	void					AddList();											//������
	void					SetTalkPassTime( int iTime ){ m_TalkPassTime = iTime; }	
	void					SetNpcTalk( DWORD dwPassTime );									//NPC�ۨ��ۻy
	void					SetNpcTalkSelf( bool bIstalk ){ m_bNpcTalkSelf = bIstalk; }		//�O�_�ۨ��ۻy
	bool					GetShowTalk(){ return m_bShowTalk; }					
	unsigned int			GetTalkWaitTime(){ return m_iTalkWaitTime; }					//���o�ۨ��ۻy���j�ɶ��]��X1000�^
	unsigned int			GetTalkWaitNowTime(){ return m_iTalkWaitNowTime; }				//���o�ۨ��ۻy���j�w�L�ɶ��]��X1000�^
	void					SetTalkWaitTime( int iTime){ m_iTalkWaitTime = iTime; }			//�]�w�ۨ��ۻy���j�ɶ��]��X1000�^
	void					SetTalkWaitNowTime( int iTime){ m_iTalkWaitNowTime = iTime; }	//�]�w�ۨ��ۻy���j�w�L�ɶ��]��X1000�^
	void					SetTalkshowTime( int iTime ){ m_TalkShowTime = iTime; }			//�]�w�ۨ��ۻy��ܮɶ��]��X1000�^

//�I�������禡
    bool                    PickRole( const int& iX, const int& iY );
	bool					PickRole(D3DXVECTOR3 &vPickRayDir , D3DXVECTOR3 &vPickRayOrig) ; 
    float                   GetBoundingBoxTop();

//����۹��l��m�禡
    void                    GetCellPosition( int& iPosX, int& iPosY );        //���o�۹��l��m
    void                    SetCellPosition( const int& iPosX, const int& iPosY );  //�]�w�۹��l��m

    const D3DXVECTOR3&      GetScreenPos() const;                                     //���o�ù��W����m

	const RECT&				GetScreenRect() const ;                                     //���o�ù��W��RECT

	//const RoleID&			GetClientID	() const ;

    const D3DXMATRIX&       GetInvWorld() { return m_matInvWorld; }             //���o�ϦV�@�ɯx�}

    void                    PlayActionSound( RoleKind iRoleKind = Role_None );  //����ʧ@�n��

	int						GetNowAction() { return m_iNowAction; }				//���o�ثe�ʧ@

	D3DCOLOR				GetNameColor() { return m_NameColor; }				// ���o�W�r�C��

    // �����K��אּ�ϥ� RenderRoleSkin
    //void RenderFrames();

    inline float GetDistance() { return m_fDistance; }
    inline void SetDistance(float fDistance) { m_fDistance=fDistance; }

	inline sRoleAction*		GetCurAction	() {return m_pNowAction; } 
	
private:
    bool mFrameListCacheOutOfData;
	bool		m_bPickColor ; 
    sFrameList mFrameListCache;
	float		m_angleLoop ; 
	float		m_DropDownLoop ; 
	float		m_DropScale ; 
	float		m_DropRoation ; 

	D3DXMATRIX	m_matDrop;											//�_��matrix 

    void OnFrameMove(); // �v�沾�ʮɪ��ƥ�
};



//----------------------------------------------------------------------------
}
//----------------------------------------------------------------------------
#endif // _FDO_ROLEBASE_H_
