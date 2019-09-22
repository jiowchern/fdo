/// 有關角色繪圖的部份，全部在這裡完成
/// 包含讀取 RAT 檔、RPC 檔
///
/// 注意：
/// RAT 及 RPC 裡面的所有東西都是用 new 出來的，記憶體尚未刪除
/// 一定要記得處理!!!
///
/// 解決方法：在 RAT、RPC結構裡增加解構式，刪除 new 出來的記憶體空間
///
/// RAT 及 RPC 工廠的 Release 機制尚未完成
///
/// 有關 m_pNewRoleSkin
/// PlaySkinAct 這裡尚有問題
/// SkinTotalSize 這裡尚有問題
///
/// 注意工廠指標傳入的問題!!

#pragma once
#ifndef _ROLE_RENDERER_H_
#define _ROLE_RENDERER_H_

#include "CRoleDef.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

using std::vector;
//////////////////////////////////////////////////////////////////////////
namespace FDO
{
	struct stSkinData
	{
		DWORD		skinID ; 
		std::string rpcName ; 
	};
	class CRenderer ; 
//////////////////////////////////////////////////////////////////////////
    class CRoleRenderer
    {
    private:

        //指向實體的指標 骨架只有一個
	    SP_RATFile	m_spRATFile;

        //記錄角色使用的rpc材質，一個角色使用很多材質，所以用list存
	    SP_RPC_List  m_spRPCFileList;

        //目前動作指標NEW
	    SRATActFile* m_pCurrRATAct;

        // 由建立角色時外部傳進來建立RAT工廠的指標
        CRATFactory* m_pRATFactory;

        // 由建立角色時外部傳進來建立RPC工廠的指標
        CRPCFactory* m_pRPCFactory;

        // 由建立角色時外部傳進來建立矩陣工廠的指標(目前沒有使用)
        //CMTXFactory* m_pMTXFactory;

        // 建立RoleSkin的工廠，裡面是FreeList(目前沒有使用)
        // 當有Matrix工廠時，Skin工廠就不需要了，因為Matrix更改為指標
        //CSkinFactory* m_pSknFactory;

        //角色實體 新作法
        //vector<SRoleSkinImageNew*>	m_vRoleInstance;
		vector<SRoleSkinImageNew>	m_vRoleInstance;
		//SRoleSkinImageNew*	m_pRoleSkinImg;
		//int					m_iRoleSkinImgCount;

        /// 動作循環旗標
        bool m_bPlayLoop;

		std::list<stSkinData>		m_RoleSkinIDVec ;					//當此RPC經由Thread建立的話，會不為-1。

    public:

        // default constructor
        CRoleRenderer();

        // constructor
        CRoleRenderer(CRATFactory* pRATFactory, CRPCFactory* pRPCFactory/*, CMTXFactory* pMTXFactory*/);

        // destructor
        ~CRoleRenderer();

        // 讀取 RAT 檔
        bool LoadRATFile(const char* pFileName);

        // 讀取 RPC 檔
        bool LoadRPCFile(const char* pFileName, int nRoleKind);

		bool LoadRPCFileThread(const char* pFileName, uint32 nRoleID);

		void SkinLinkRPC(SP_RPCFile pRPCFile) ; 

        // 卸載 RPC 檔
        void UnloadRPCFile(const char* pFileName);



        // 卸載目前身上的所有RPC;
        void UnloadAllRPCFile();

        // 播放指定動作
        SRATActFile* Play(const char* szActionName, bool bLoop);

        // 設定目前動作
       // void SetCurrentAction(SRATActFile* pCurrRATAct) { m_pCurrRATAct=pCurrRATAct; }
		SRATActFile*	GetNowAction(){ return  m_pCurrRATAct;}
		SP_RATFile&		GetRATFile(){ return m_spRATFile;}

        bool Render(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX& matInvWorld, 
            D3DCOLOR Color, int iShowDir, unsigned short usFrameNum, int nRoleKind ,
            CRenderer *pRender , float slope , bool divide = false);

        // 設定 RAT 工廠
        void SetRATFactory(CRATFactory* pRATFactory) { m_pRATFactory=pRATFactory; }

        // 設定 RPC 工廠
        void SetRPCFactory(CRPCFactory* pRPCFactory) { m_pRPCFactory=pRPCFactory; }

		static bool ValidString(const char* pString);

    private:

        void InstanceResize(SRoleSkinImageNew* pRoleSkinImg);

        // 取消所有動作的某個零件的連結
        void UnlinkRPC(const char* pComName);

        void UnlinkRPCAct(SRATActFile* pRATAct, const char* pComName);

        void UnlinkRPCCom(SRATComFile* pRATCom);

        void UnlinkRPCDir(SRATDirFile& pRATDir);

        static void UnlinkRPCImg(SRoleSkinImageNew* pRoleInstance);

		void UnRegisterSkin(); 
		void UnRegisterSkin(const char *rpcName); 
		
		
    };


inline bool LayerSort(SRoleSkinImageNew* i, SRoleSkinImageNew* j) ; 

//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#endif