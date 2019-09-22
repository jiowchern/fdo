/// ��������ø�Ϫ������A�����b�o�̧���
/// �]�tŪ�� RAT �ɡBRPC ��
///
/// �`�N�G
/// RAT �� RPC �̭����Ҧ��F�賣�O�� new �X�Ӫ��A�O����|���R��
/// �@�w�n�O�o�B�z!!!
///
/// �ѨM��k�G�b RAT�BRPC���c�̼W�[�Ѻc���A�R�� new �X�Ӫ��O����Ŷ�
///
/// RAT �� RPC �u�t�� Release ����|������
///
/// ���� m_pNewRoleSkin
/// PlaySkinAct �o�̩|�����D
/// SkinTotalSize �o�̩|�����D
///
/// �`�N�u�t���жǤJ�����D!!

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

        //���V���骺���� ���[�u���@��
	    SP_RATFile	m_spRATFile;

        //�O������ϥΪ�rpc����A�@�Ө���ϥΫܦh����A�ҥH��list�s
	    SP_RPC_List  m_spRPCFileList;

        //�ثe�ʧ@����NEW
	    SRATActFile* m_pCurrRATAct;

        // �ѫإߨ���ɥ~���Ƕi�ӫإ�RAT�u�t������
        CRATFactory* m_pRATFactory;

        // �ѫإߨ���ɥ~���Ƕi�ӫإ�RPC�u�t������
        CRPCFactory* m_pRPCFactory;

        // �ѫإߨ���ɥ~���Ƕi�ӫإ߯x�}�u�t������(�ثe�S���ϥ�)
        //CMTXFactory* m_pMTXFactory;

        // �إ�RoleSkin���u�t�A�̭��OFreeList(�ثe�S���ϥ�)
        // ��Matrix�u�t�ɡASkin�u�t�N���ݭn�F�A�]��Matrix��אּ����
        //CSkinFactory* m_pSknFactory;

        //������� �s�@�k
        //vector<SRoleSkinImageNew*>	m_vRoleInstance;
		vector<SRoleSkinImageNew>	m_vRoleInstance;
		//SRoleSkinImageNew*	m_pRoleSkinImg;
		//int					m_iRoleSkinImgCount;

        /// �ʧ@�`���X��
        bool m_bPlayLoop;

		std::list<stSkinData>		m_RoleSkinIDVec ;					//��RPC�g��Thread�إߪ��ܡA�|����-1�C

    public:

        // default constructor
        CRoleRenderer();

        // constructor
        CRoleRenderer(CRATFactory* pRATFactory, CRPCFactory* pRPCFactory/*, CMTXFactory* pMTXFactory*/);

        // destructor
        ~CRoleRenderer();

        // Ū�� RAT ��
        bool LoadRATFile(const char* pFileName);

        // Ū�� RPC ��
        bool LoadRPCFile(const char* pFileName, int nRoleKind);

		bool LoadRPCFileThread(const char* pFileName, uint32 nRoleID);

		void SkinLinkRPC(SP_RPCFile pRPCFile) ; 

        // ���� RPC ��
        void UnloadRPCFile(const char* pFileName);



        // �����ثe���W���Ҧ�RPC;
        void UnloadAllRPCFile();

        // ������w�ʧ@
        SRATActFile* Play(const char* szActionName, bool bLoop);

        // �]�w�ثe�ʧ@
       // void SetCurrentAction(SRATActFile* pCurrRATAct) { m_pCurrRATAct=pCurrRATAct; }
		SRATActFile*	GetNowAction(){ return  m_pCurrRATAct;}
		SP_RATFile&		GetRATFile(){ return m_spRATFile;}

        bool Render(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX& matInvWorld, 
            D3DCOLOR Color, int iShowDir, unsigned short usFrameNum, int nRoleKind ,
            CRenderer *pRender , float slope , bool divide = false);

        // �]�w RAT �u�t
        void SetRATFactory(CRATFactory* pRATFactory) { m_pRATFactory=pRATFactory; }

        // �]�w RPC �u�t
        void SetRPCFactory(CRPCFactory* pRPCFactory) { m_pRPCFactory=pRPCFactory; }

		static bool ValidString(const char* pString);

    private:

        void InstanceResize(SRoleSkinImageNew* pRoleSkinImg);

        // �����Ҧ��ʧ@���Y�ӹs�󪺳s��
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