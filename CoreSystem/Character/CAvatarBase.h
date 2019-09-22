#ifndef _AVATAR_H_
#define _AVATAR_H_

#include "CRoleDef.h"
namespace FDO
{
	class CAvatarBase
	{
	private:
		
		//////////////////////////////////////////////////////////////////////////
		//屬性變數
		//////////////////////////////////////////////////////////////////////////
		LPDIRECT3DDEVICE9       m_pd3dDevice;                                       //D3D裝置
		bool                    m_bVisible;                                         //顯示判斷子
		RoleDirect              m_eShowDir;                                         //顯示方向

		//////////////////////////////////////////////////////////////////////////
		// 位置變數
		//////////////////////////////////////////////////////////////////////////
		D3DXVECTOR2             m_vPosition;                                        //位置
		D3DXVECTOR2             m_vScale;                                           //縮放
		float                   m_fTransRate;                                       //轉換比例

		//////////////////////////////////////////////////////////////////////////
		//繪圖需要的物件
		//////////////////////////////////////////////////////////////////////////
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


	public:
		CAvatarBase();
		~CAvatarBase();

		bool CreateAvatarData();
				
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
		CRoleRenderer*          m_pRoleRenderer;

		bool				    LoadRATFile(const char* pFileName, CRATFactory* pRATFactory);
		bool                    LoadPartFile(const char* pFilename, CRPCFactory* pRPCFactory);                    //載入部位檔案
		bool                    UnloadPartFile(const char* pFilename, CRPCFactory* pRPCFactory);                    //隱藏部位檔案
		
		bool				    LoadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
		void                    UnloadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
		void                    UnloadAllRPCFile();
		
		void				    PlaySkinAct(const char* szActionName, bool bLoop );
		void				    RenderRoleSkin();
		void					RenderFramesOriginal();
		//////////////////////////////////////////////////////////////////////////
		// 以上是新的作法
		//////////////////////////////////////////////////////////////////////////
	};
}
#endif