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
//建立紙娃娃資料
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
	// 所有動作都交給 RoleRenderer 去做，這樣一來移植比較方便
	m_pRoleRenderer->SetRATFactory( pRATFactory );
	if( !m_pRoleRenderer->LoadRATFile( pFileName ) )
	{
		return false;
	}
	SP_RATFile& spRATFile = m_pRoleRenderer->GetRATFile();
	return true;
}

//////////////////////////////////////////////////////////////////////////
//載入部位檔案
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


	//cosiann 多一個是記錄\n
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
//隱藏部位檔案
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

	//cosiann 多一個是記錄\n
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
	// 所有動作都交給 RoleRenderer 去做，這樣一來移植比較方便
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
	////檢查播放動作是否與現在動作相同
	//if( strcmp( m_szNowActName, szActionName ) == 0 )
	//{
	//	return;
	//}
	////設定動作名稱
	//strcpy( m_szNowActName, szActionName );
	//m_usFrameMax = 1;

	////取得角色動作陣列
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
	//	//失敗就不是要畫主角配角
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