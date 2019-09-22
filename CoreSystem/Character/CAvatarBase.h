#ifndef _AVATAR_H_
#define _AVATAR_H_

#include "CRoleDef.h"
namespace FDO
{
	class CAvatarBase
	{
	private:
		
		//////////////////////////////////////////////////////////////////////////
		//�ݩ��ܼ�
		//////////////////////////////////////////////////////////////////////////
		LPDIRECT3DDEVICE9       m_pd3dDevice;                                       //D3D�˸m
		bool                    m_bVisible;                                         //��ܧP�_�l
		RoleDirect              m_eShowDir;                                         //��ܤ�V

		//////////////////////////////////////////////////////////////////////////
		// ��m�ܼ�
		//////////////////////////////////////////////////////////////////////////
		D3DXVECTOR2             m_vPosition;                                        //��m
		D3DXVECTOR2             m_vScale;                                           //�Y��
		float                   m_fTransRate;                                       //�ഫ���

		//////////////////////////////////////////////////////////////////////////
		//ø�ϻݭn������
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
		// �`���v�檺�e��
		sFrameList m_RenderFrames;


	public:
		CAvatarBase();
		~CAvatarBase();

		bool CreateAvatarData();
				
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
		CRoleRenderer*          m_pRoleRenderer;

		bool				    LoadRATFile(const char* pFileName, CRATFactory* pRATFactory);
		bool                    LoadPartFile(const char* pFilename, CRPCFactory* pRPCFactory);                    //���J�����ɮ�
		bool                    UnloadPartFile(const char* pFilename, CRPCFactory* pRPCFactory);                    //���ó����ɮ�
		
		bool				    LoadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
		void                    UnloadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory);
		void                    UnloadAllRPCFile();
		
		void				    PlaySkinAct(const char* szActionName, bool bLoop );
		void				    RenderRoleSkin();
		void					RenderFramesOriginal();
		//////////////////////////////////////////////////////////////////////////
		// �H�W�O�s���@�k
		//////////////////////////////////////////////////////////////////////////
	};
}
#endif