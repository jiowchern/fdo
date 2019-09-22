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
#pragma message("資源管理模式啟動") 

#else
#pragma message("資源一般模式啟動") 
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
//  角色種類
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
// 角色動作 New Version for 天地劫
//---------------------------------------------------------------------------
enum
{
	ACT_None = -1,	// 無
	ACT_Atk01 = 3,	// 1號普攻
	ACT_Atk02,		// 2號普攻
	ACT_Atk03,		// 3號普攻
	ACT_Atk04,		// 4號普攻
	ACT_Atk05,		// 5號普攻
	ACT_Atk06,		// 6號普攻
	ACT_Atk07,		// 7號普攻
	ACT_Atk08,		// 8號普攻
	ACT_Atk09,		// 9號普攻
	ACT_Atk10,		// 10號普攻
	ACT_Stand,		// 待機
	ACT_Guard,		// 警戒
	ACT_Injury,		// 受擊
	ACT_Walk,		// 步行
	ACT_Sit,		// 坐下
	ACT_Fall,		// 倒地
	ACT_Dead,		// 死亡
	ACT_Ghost,		// 鬼魂
	ACT_Charge,		// 聚氣
	ACT_Cast,		// 法術
	ACT_Loot,		// 拾取
	ACT_Critical,	// 爆擊
	ACT_Dodge,		// 閃躲 (敵人沒打中你)
	ACT_Miss,		// 失手 (沒打中敵人)
	ACT_Parry,		// 格檔
	ACT_Change,		// 擺攤變身
	ACT_Market,		// 攤販
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
		pShadowSize			= NULL;                                      //影子大小
		pShadowOffset		= NULL;                                    //影子偏移值
		pBoundingBoxSize	= NULL;                                //邊界盒大小
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
		this->pShadowSize			= pShadowSize;                                      //影子大小
		this->pShadowOffset			= pShadowOffset;                                    //影子偏移值
		this->pBoundingBoxSize		= pBoundingBoxSize;                                //邊界盒大小
		this->pDeadBoundBoxSize		= pDeadBoundBoxSize; 
		this->pScale				= pScale ; 
		this->bThreadOk				= bThreadOk ; 
	}
	float			        *pShadowSize;                                      //影子大小
    float		            *pShadowOffset;                                    //影子偏移值
    unsigned short          *pBoundingBoxSize;                                //邊界盒大小
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
	//bool                    m_bVisible;                                         //顯示判斷子
protected:
//裝備變數-----------------------------------------------------------------
    sRoleActionList         m_ActionList;                                       //動作陣列
    sRoleAction*            m_pNowAction;                                       //目前使用之動作
	SRATActFile* 			m_pCurrRATAct;										//目前動作指標NEW

#ifdef RESMGR
	std::string				m_actname ; 
#else

#endif
// 位置變數----------------------------------------------------------------
    D3DXVECTOR3             m_vPosition;                                        //位置
    bool                    m_bMatrixChange;                                    //世界矩陣改變判斷子
	D3DXVECTOR3				m_shakeDir ;										//震動方向
// 播放控制變數-----------------------------------------------------------
    unsigned short          m_usFrameNum;                                       //顯示張數
    bool                    m_bPlayLoop;                                        //迴圈播放
    DWORD                   m_dwPassTime;                                       //經過時間
    RoleDirect              m_eShowDir;                                         //顯示方向
    bool                    m_bPlaySound;                                       //播放聲音判斷子
	bool					m_bShake;		//震動開啟
	bool					m_bDrop ; 
// 裝備函式----------------------------------------------------------------
    void                    RemoveAllAction();                                  //移除全部動作  

// 稱號變數----------------------------------------------------------------
    int                     m_NameWidth;                                        //稱號寬度
    wstring                 m_NameTitle;                                        //稱號字串
    RECT                    m_NameRect;                                         //稱號大小

    bool                    m_bShowShadow;                                      //顯示影子判斷子
	bool					m_bDivide ;											//是否產生殘影
	bool					m_bRage	;											//殺氣設定

// 事件狀態變數-----------------------------------------------------------
    int                     m_IDWidth;
    RECT                    m_IDRect;                                         //稱號大小
	D3DXVECTOR3             m_NpcEventScreenPos;							  //事件提示在螢幕上的位置


// 對話匡變數--------------------------------------------------------------
	unsigned int			m_TalkPassTime;
	unsigned int			m_TalkShowTime;										//對話匡顯示時間			
    bool					m_bShowTalk;
	bool					m_bUseList;
	RECT					m_TalkRect;
	wstring					m_TextString;
	CTextObject*			m_pTextObject;
	CDXUTListBox*			m_pListBox;
	unsigned int			m_iTalkWaitTime;									//自言自語的間隔時間
	unsigned int			m_iTalkWaitNowTime;									//自言自語的已經等待時間
	bool					m_bNpcTalkSelf;										//是否自言自語
	D3DXVECTOR3             m_NpcTalkScreenPos;									//對話在螢幕上的位置

protected:
	sRoleAllRes			    *m_BaseData ; 
    sRoleActionList*        m_pActionList;                                      //裝備動作陣列指標
    D3DXVECTOR3             m_vScale;                                           //縮放
    D3DXVECTOR3             m_vPosOffset;
    float                   m_fShadowSize;                                      //影子大小
    float                   m_fShadowOffset;                                    //影子偏移值
    LPDIRECT3DDEVICE9       m_pDevice;                                          //D3D裝置
	D3DXMATRIX				m_matShake;											//震動matrix 
    D3DXMATRIX              m_matInv;
    D3DXMATRIX              m_matWorld;                                         //世界矩陣
    D3DXMATRIX              m_matInvWorld;                                      //反向世界矩陣
    D3DXMATRIX              m_matPos;                                           //位置矩陣
    D3DXMATRIX              m_matInvPos;                                        //反向位置矩陣
    DWORD                   m_dwUpdateTime;                                     //更新時間
    RoleDirect              m_eRoleDir;                                         //人物方向
    D3DCOLOR                m_Color;                                            //顯示顏色
	D3DCOLOR				m_pickColor; 
    bool                    m_bVisible;                                         //顯示判斷子
    bool                    m_bShowName;                                        //稱號顯示判斷子
    HDC                     m_hDC;                                              //Device Context
    RoleKind                m_RoleKind;                                         //角色種類
    unsigned short          m_usBoundingBoxSize;                                //邊界盒大小
    unsigned short          m_usDeadBoundBoxSize;                               //死亡邊界盒大小
    D3DXVECTOR3             m_ScreenPos;                                        //螢幕上的位置
	RECT					m_rcBody;
    D3DCOLOR                m_NameColor;                                        //稱號顏色
    PlayStatus              m_PlayStatus;                                       //播放狀態
    unsigned short          m_usFrameMax;                                       //顯示最大張數
    char                    m_szNowActName[20];                                 //現在動作名稱
    int                     m_iCellPosX;                                        //相對橫向格子數
    int                     m_iCellPosY;                                        //相對縱向格子數
    int                     m_iNowAction;                                       //目前人物動作
	float                   m_fDistance;

	RoleID&					m_RoleId;											//角色ClientID
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
	//讀取骨架皮膚
	//////////////////////////////////////////////////////////////////////////
    // 以下的東西全部移至 RoleRenderer 裡
    // 為的是全部集中，模組化，有關角色繪圖、讀取 RAT、RPC
    // 的部份就全部交給 RoleRenderer 去處理
    //////////////////////////////////////////////////////////////////////////
    // 以下是新的作法
    /////////////////////////////////////////////////////////////////////////
	CRPCFactory*			m_pRPCFactory;
	CRATFactory*			m_pRATFactory;
    CRoleRenderer*          m_pRoleRenderer;

	const int				GetBodyID() { return m_BodyID; }

	inline void					SetVidide(bool divide) { m_bDivide = divide; } 
	inline void					SetRage(bool bRage) { m_bRage = bRage; SetVidide(m_bRage); } 
	bool                    LoadPart(const char* pFilename, CRPCFactory* pRPCFactory);                    //載入部位檔案
    bool                    UnloadPart(const char* pFilename, CRPCFactory* pRPCFactory);                    //隱藏部位檔案
	bool				    LoadRATFile(const char* pFileName, CRATFactory* pRATFactory);
	bool				    LoadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
    void                    UnloadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
    void                    UnloadAllRPCFile();
	void				    PlaySkinAct(const char* szActionName, bool bLoop );
	void				    RenderRoleSkin(RoleBase *pRole , CRenderer *pRender);
	void					RenderFramesOriginal(RoleBase *pRole , CRenderer *pRender);
	void					FlushFrameOriginal() ; 
    //////////////////////////////////////////////////////////////////////////
    // 以上是新的作法
    //////////////////////////////////////////////////////////////////////////

	// 讀檔函式
    bool                    LoadRoleAll( const string& strFileName );           //載入整個角色
    bool                    LoadComponent( const string& strFileName );         //載入零件
    sRoleAction*            GetAction( const char* pActName );                  //取得動作指標
	//更新角色
	void					FrameMoveRole	(const DWORD dwPassTime, const D3DXMATRIX& matInv,
                                const D3DXVECTOR3& vDir, const float fScale,
                                bool bAngleChange, const D3DXVECTOR3& vPosOffset , 
								const D3DXVECTOR4 &rectRange);

	//更新角色 其他需要顯示的2D介面相關位置
	virtual	void			FrameMove2DInfo	(const DWORD dwPassTime ,  const D3DXVECTOR4 &rectRange);
	// 稱號函式------------------------------------------------------------
    void                    FrameMoveNameTitle();									//更新稱號
	// 更新螢幕位置函式--------------------------------------------------
    virtual void            FrameMoveScreenPos	( const D3DXMATRIX& matInv );		//更新螢幕位置函式	

	// 繪製函式-------------------------------------------------------------
    void                    RenderRole		();
	virtual void			RenderOther		();
	virtual	const int		GetRoleID		() const = 0;

    //short                   GetRenderFrame( sFrame* ppFrameArray[], short shSize );
    void                    RenderShadow(CRenderer *prender , const float fOffsetX, const float fOffsetY, const float fOffsetZ);

// 設定函式---------------------------------------------------------------
	void					SetPickColor(bool bPick , DWORD Color ) ; 
	bool					IsPickColor() {return m_bPickColor ; } 
    void                    SetPosition( const D3DXVECTOR3& vPos );             //設定位置
    void                    SetScale( const D3DXVECTOR3& vScale );              //設定縮放
    const D3DXVECTOR3&      GetPosition() const;                                //取得位置
    const D3DXVECTOR3&      GetScale() const;                                   //取得縮放
    void                    SetColor(DWORD Color);                              //設定顏色
    const D3DCOLOR&         GetColor();                                         //取得顏色
    virtual void            SetVisible(bool bValue );                            //設定顯示判斷子
    bool                    GetVisible();                                       //取得顯示判斷子
    void                    SetDirection( RoleDirect eDirect );                 //設定方向 
    RoleDirect              GetDirection();                                     //取得方向
    //void                    SetPositoinWithOffset( const D3DXVECTOR3& vPos );   //設定角色位置(含偏移量)
    D3DXVECTOR3             GetPositoinWithoutOffset();                         //取得角色位置(減掉偏移量)
    D3DXVECTOR3             GetCenterPosition();                                //取得角色中心點位置
    RoleKind                GetRoleKind();                                      //取得角色種類
    void                    SetUpdateTime( const DWORD& dwUpdateTime );         //設定更新時間  

    void                    SetShowShadow( bool bValue );                       //設定顯示影子判斷子
    bool                    GetShowShadow();                                    //取得顯示影子判斷子  
	void					SetRoleData(sRoleAllRes *pRoleData);
	bool					IsRoleData() { return (m_BaseData != NULL) ; }
//播放控制函式------------------------------------------------------------
    void                    Play( const char* szActionName, bool bLoop );       //播放
    void                    SharePlay( const char* szActionName, bool bLoop, bool bIsNPC = false );  //共享模式播放
    void                    Stop();                                             //停止(after Stop position goto top)
    void                    Pause( unsigned short usFrameNum );                 //暫停   

//稱號函式-----------------------------------------------------------------
    void                    SetNameTitle( const char16* szName );                 //設定稱號
	void                    SetNameTitleHV( const WCHAR* szName );              //設定稱號
    void                    SetCondition( int SP );                             //設定事件狀態
	int						NPCDifference;										//FDO和HVO的NPC的高度差
    LPCWSTR                 GetNameTitle() { return m_NameTitle.c_str(); }      //取得稱號
    void                    SetShowNameTitle( bool bShow );                     //設定顯示稱號
    void                    SetNameTitleColor( D3DCOLOR clrColor );             //設定稱號顏色
    void                    RenderNameTitle();                                  //顯示稱號

// NPC對話函式-------------------------------------------------------------
    void                    SetNPCTalkText( const char* szText );               //設定對話訊息
    void                    FrameMoveNPCTalk( const DWORD& dwPassTime );        //更新顯示對話
    void                    RenderNPCTalk();                                    //顯示對話訊息
	void					AddMessage( const string &Message );				//未完成
	void					SetNPCTalkList();									//未完成
	void					AddList();											//未完成
	void					SetTalkPassTime( int iTime ){ m_TalkPassTime = iTime; }	
	void					SetNpcTalk( DWORD dwPassTime );									//NPC自言自語
	void					SetNpcTalkSelf( bool bIstalk ){ m_bNpcTalkSelf = bIstalk; }		//是否自言自語
	bool					GetShowTalk(){ return m_bShowTalk; }					
	unsigned int			GetTalkWaitTime(){ return m_iTalkWaitTime; }					//取得自言自語間隔時間（秒X1000）
	unsigned int			GetTalkWaitNowTime(){ return m_iTalkWaitNowTime; }				//取得自言自語間隔已過時間（秒X1000）
	void					SetTalkWaitTime( int iTime){ m_iTalkWaitTime = iTime; }			//設定自言自語間隔時間（秒X1000）
	void					SetTalkWaitNowTime( int iTime){ m_iTalkWaitNowTime = iTime; }	//設定自言自語間隔已過時間（秒X1000）
	void					SetTalkshowTime( int iTime ){ m_TalkShowTime = iTime; }			//設定自言自語顯示時間（秒X1000）

//點擊偵測函式
    bool                    PickRole( const int& iX, const int& iY );
	bool					PickRole(D3DXVECTOR3 &vPickRayDir , D3DXVECTOR3 &vPickRayOrig) ; 
    float                   GetBoundingBoxTop();

//角色相對格子位置函式
    void                    GetCellPosition( int& iPosX, int& iPosY );        //取得相對格子位置
    void                    SetCellPosition( const int& iPosX, const int& iPosY );  //設定相對格子位置

    const D3DXVECTOR3&      GetScreenPos() const;                                     //取得螢幕上的位置

	const RECT&				GetScreenRect() const ;                                     //取得螢幕上的RECT

	//const RoleID&			GetClientID	() const ;

    const D3DXMATRIX&       GetInvWorld() { return m_matInvWorld; }             //取得反向世界矩陣

    void                    PlayActionSound( RoleKind iRoleKind = Role_None );  //播放動作聲音

	int						GetNowAction() { return m_iNowAction; }				//取得目前動作

	D3DCOLOR				GetNameColor() { return m_NameColor; }				// 取得名字顏色

    // 取消…更改為使用 RenderRoleSkin
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

	D3DXMATRIX	m_matDrop;											//震動matrix 

    void OnFrameMove(); // 影格移動時的事件
};



//----------------------------------------------------------------------------
}
//----------------------------------------------------------------------------
#endif // _FDO_ROLEBASE_H_
