#include "stdafx.h"
#include "CAvatarBase.h"
#include "CRATFileFactory.h"
#include "CRPCFileFactory.h"
#include "CRoleSkinFactory.h"
#include "CRoleSkinStruct.h"
#include "CRoleRenderer.h"

namespace FDO
{

//////////////////////////////////////////////////////////////////////////
//�إ߯ȫ������
//////////////////////////////////////////////////////////////////////////
bool CAvatarBase::CreateAvatarData()
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool CAvatarBase::LoadRATFile(const char* pFileName, CRATFactory* pRATFactory)
{
	// �Ҧ��ʧ@���浹 RoleRenderer �h���A�o�ˤ@�Ӳ��Ӥ����K
	m_pRoleRenderer->SetRATFactory( pRATFactory );
	if( !m_pRoleRenderer->LoadRATFile( pFileName ) )
	{
		return false;
	}
	SP_RATFile& spRATFile = m_pRoleRenderer->GetRATFile();
	return true;
}

//////////////////////////////////////////////////////////////////////////
//���J�����ɮ�
//////////////////////////////////////////////////////////////////////////
bool CAvatarBase::LoadPartFile(const char* pFilename, CRPCFactory* pRPCFactory)
{
	if( !m_pRoleRenderer->ValidString(pFilename) )
	{
		return false;
	}

	char* pText = NULL ;

	memPack* pMem = LoadFromPacket( pFilename );

	if( pMem->Data == NULL ) return false;

	try
	{
		pText = (char*)(pMem->Data);
	}
	catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what() ) ;
	}


	//cosiann �h�@�ӬO�O��\n
	unsigned int allsize = pMem->DataSize + 1  ;

	//cosiann
	char *ptemp = new char[allsize] ; 
	//cosiann
	ZeroMemory(ptemp , sizeof(char) * allsize) ;
	//cosiann
	memcpy(ptemp , pMem->Data , pMem->DataSize) ;

	//cosiann

	char* pToken = strtok( ptemp, "\n\r" );

	while( pToken )
	{
		this->LoadRPCFile( pToken, pRPCFactory );

		pToken = strtok( 0, "\n\r" );
	}

	//cosiann
	delete[] ptemp ;
	ptemp = NULL ;

	return true;
}

//////////////////////////////////////////////////////////////////////////
//���ó����ɮ�
//////////////////////////////////////////////////////////////////////////
bool CAvatarBase::UnloadPartFile(const char* pFilename, CRPCFactory* pRPCFactory)
{
	if( !m_pRoleRenderer->ValidString(pFilename) )
	{
		return false;
	}
	
	memPack* pMem = LoadFromPacket( pFilename );

	if( pMem->Data == NULL )
	{
		return false;
	}

	//cosiann �h�@�ӬO�O��\n
	unsigned int allsize = pMem->DataSize + 1  ;

	//cosiann
	char *ptemp = new char[allsize] ; 

	//cosiann
	ZeroMemory(ptemp , sizeof(char) * allsize) ;

	//cosiann
	memcpy(ptemp , pMem->Data , pMem->DataSize) ;

	//cosiann
	char* pToken = strtok( ptemp, "\n\r" );

	while( pToken )
	{
		m_pRoleRenderer->UnloadRPCFile( pToken );
		pToken = strtok( 0, "\n\r" );
	}

	//cosiann
	delete[] ptemp ;
	ptemp = NULL ;

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool CAvatarBase::LoadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory)
{
	// �Ҧ��ʧ@���浹 RoleRenderer �h���A�o�ˤ@�Ӳ��Ӥ����K
	m_pRoleRenderer->SetRPCFactory( pRPCFactory );
	return m_pRoleRenderer->LoadRPCFile( pFileName, 1 );
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CAvatarBase::UnloadRPCFile(const char* pFileName, CRPCFactory* pRPCFactory)
{
	m_pRoleRenderer->SetRPCFactory( pRPCFactory );
	m_pRoleRenderer->UnloadRPCFile( pFileName );
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CAvatarBase::UnloadAllRPCFile()
{
	m_pRoleRenderer->UnloadAllRPCFile();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CAvatarBase::PlaySkinAct(const char* szActionName, bool bLoop )
{
	//m_bPlayLoop = bLoop;
	//m_PlayStatus = Status_Play;
	//m_usFrameNum = 1;
	//m_bPlaySound = true;
	////�ˬd����ʧ@�O�_�P�{�b�ʧ@�ۦP
	//if( strcmp( m_szNowActName, szActionName ) == 0 )
	//{
	//	return;
	//}
	////�]�w�ʧ@�W��
	//strcpy( m_szNowActName, szActionName );
	//m_usFrameMax = 1;

	////���o����ʧ@�}�C
	//SRATActFile* pRATAct = m_pRoleRenderer->Play(szActionName, bLoop );

	//m_dwUpdateTime = pRATAct->dwUpdateTime;

	//for(size_t i(0); i < pRATAct->vRATComList.size(); ++i)
	//{
	//	SRATComFile& rRATCom = pRATAct->vRATComList.at(i);

	//	if( m_usFrameMax < rRATCom.RATDir[m_eShowDir].usMaxFrameNum )
	//	{
	//		m_usFrameMax = rRATCom.RATDir[m_eShowDir].usMaxFrameNum;
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CAvatarBase::RenderRoleSkin()
{
	//if( !m_pRoleRenderer->Render( m_pDevice, m_matInvWorld, m_Color, m_eShowDir, m_usFrameNum, m_RoleKind) )
	//{
	//	//���ѴN���O�n�e�D���t��
	//	RenderFramesOriginal();
	//}
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CAvatarBase::RenderFramesOriginal()
{
}


}