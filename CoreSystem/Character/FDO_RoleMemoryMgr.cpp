#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
#include "PhenixAssert.h"
#include <algorithm>
#include <vector>
#include <stdio.h>
#include "RoleResourceManage.h"
#include "ResourceManager.h"
#include "CRPCFileFactory.h"
#include "CRoleRenderer.h"
#include <boost/bind.hpp>
using namespace std;

#pragma hdrstop
#include "FDO_RoleMemoryMgr.h"
#include "GameCommand.h"

//////////////////////								////////////////////////////

#ifdef RESMGR
#pragma message("資源管理模式啟動") 

#include "command.h"

#else
#pragma message("資源一般模式啟動") 
#endif


#ifdef OBJMGR
#pragma message("物件管理模式啟動") 

#else
#pragma message("物件一般模式啟動") 
#endif


////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
namespace FDO
{

	unsigned int g_SkinCount = 1 ; 

//---------------------------------------------------------------------------
// Internal functions forward declarations
//---------------------------------------------------------------------------
void DeleteAction( sRoleAction* p ) { delete p; }
void DeleteComponent( sRoleComponent* p ) { delete p; }
void DeleteFrame( sFrame* p ) { delete p; }

//----------------------------------------------------------------------------
//  MemData Struct
//----------------------------------------------------------------------------
struct sMemData
{
    string                  m_strName;
    unsigned char*          m_pData;
     sMemData() : m_pData(NULL) {}
    ~sMemData() { delete[] m_pData; }
};

typedef map<string, sMemData*> MemDataMap;
typedef pair<string, sMemData*> MemDataPair;
void DelMemData(MemDataPair p) { delete p.second; }


void LoadRPCMemoryBuffer(stRPCThreadStream* pStream) 
{
	static PacketReader PR; 
    boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);

	xt.sec += 1 ; 
    //建立資源。
	pStream->state = THREAD ; 
 
	pStream->pMem = PR.LoadFromFileEx2( pStream->filename );

	if(pStream->pMem == NULL)
	{
		pStream->state = FILEVAILD ;
	}
	else
	{
		pStream->rpc.pData = pStream->pMem->Data ; 

		pStream->state = LOADDONE ;

	}
	

	boost::thread::sleep(xt) ; 
}

void LoadRPCThread(stUnLoadRPC *pRPC)
{
	//設定RPC檔名
	//pRPC->pFileName = pFilename ; 

	CRPCFactory *pFactory = pRPC->pRPCFactory ; 

	//BYTE *pData = pRPC->pData ; 

	// 讀取RPC標頭檔
	pRPC->pData += pFactory->LoadRPCHeaderFile( pRPC->pData, pRPC->spRPCFile );

	//RESIZE
	pFactory->SetRPCActSize(pRPC->spRPCFile);

	//建立出rpc所有資料
	char* pComName1 = pRPC->spRPCFile->RPCFileHeader.szComponentName1;
	char* pComName2 = pRPC->spRPCFile->RPCFileHeader.szComponentName2;


	pRPC->pData += pFactory->CreateRPCFileAllDataThread( pRPC->spRPCFile->vRPCActList1, pComName1, pRPC->spRPCFile, pRPC->spRATFile, pRPC->pData );
	pRPC->pData += pFactory->CreateRPCFileAllDataThread( pRPC->spRPCFile->vRPCActList2, pComName2, pRPC->spRPCFile, pRPC->spRATFile, pRPC->pData );

	pRPC->pData = NULL ; 

	// TODO: 建立完畢，丟入記憶體池管理
	GetRPCFileManage()->Insert( pRPC->pFileName.c_str() , pRPC->spRPCFile ) ;
}

void LoadActionThread(stUnLoadingAction *pUnloadAct)
{
	
	char szTexFile[50];
	D3DXVECTOR3 vScale, vPosition;
	unsigned actCount = pUnloadAct->actCount ; 


    boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);

	for(unsigned int i = 0 ; i < actCount ; ++i)
	{
		xt.nsec += 1000000 * 500;

		sRoleAction* pAction = g_RoleMemoryMgr.GetAction() ; 


		// 取得資料
		strcpy( pAction->szActionName, (char*)pUnloadAct->pData );
		pUnloadAct->pData += 20;
		pAction->bPlayLoop = *(bool*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( bool );
		pAction->dwUpdateTime = *(DWORD*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( DWORD );
		pAction->bySoundKind = *(BYTE*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( BYTE );
		pAction->usHitTime1 = *(unsigned short*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( unsigned short );
		pAction->usHitTime2 = *(unsigned short*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( unsigned short );
		pAction->usPlaySoundTime1 = *(unsigned short*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( unsigned short );
		pAction->usPlaySoundTime2 = *(unsigned short*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( unsigned short );
		pAction->usPlaySoundTime3 = *(unsigned short*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( unsigned short );
		pAction->usPlayParticleTime = *(unsigned short*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( unsigned short );
		strcpy( pAction->szSoundFile1, (char*)pUnloadAct->pData ); //音效 ***
		pUnloadAct->pData += 50;
		strcpy( pAction->szSoundFile2, (char*)pUnloadAct->pData );   //音效 ***
		pUnloadAct->pData += 50;
		strcpy( pAction->szSoundFile3, (char*)pUnloadAct->pData );   //音效 ***
		pUnloadAct->pData += 50;
		strcpy( pAction->szParticleFile, (char*)pUnloadAct->pData );  //音效 ***
		pUnloadAct->pData += 50;
		pUnloadAct->pData += 20;    // 預留欄位

		// 零件總數
		unsigned short usComponentCount = *(unsigned short*)pUnloadAct->pData;
		pUnloadAct->pData += sizeof( unsigned short );

		// 建立零件資料
		for( unsigned short j=0; j<usComponentCount; ++j )
		{
			sRoleComponent* pRoleComponent = g_RoleMemoryMgr.GetComponent();

			pAction->ComponentList->push_back( pRoleComponent );

			// 取得資料
			strcpy( pRoleComponent->szComponentName, (char*)pUnloadAct->pData );
			pUnloadAct->pData += 20;

			for( unsigned short k=0; k<8; ++k )
			{
				// 取得資料
				pRoleComponent->ComponentDirs[k]->usMaxFrameNum = *(unsigned short*)pUnloadAct->pData;
				pUnloadAct->pData += sizeof( unsigned short );

				// 畫面總數
				unsigned short usFrameCount = *(unsigned short*)pUnloadAct->pData;
				pUnloadAct->pData += sizeof( unsigned short );

				// 建立畫面資料
				for( unsigned short l=0; l<usFrameCount; ++l )
				{
					sFrame* pFrame = g_RoleMemoryMgr.GetFrame();

					sComponentDir& scd = *pRoleComponent->ComponentDirs[k];
					sFrameList& sfl = *scd.FrameList;
					sint nJCDebugCode_addr = (sint)scd.FrameList; 
					sfl.push_back(pFrame);
					// 取得資料
					//pFrame->usUnitTime = *(unsigned short*)pData;
					pUnloadAct->pData += sizeof( unsigned short );
					pFrame->usPlayTime = *(unsigned short*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( unsigned short );
					pFrame->shLayer = *(short*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( short );
					vScale.z = *(float*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( float );
					vScale.x = *(float*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( float );
					vScale.y = *(float*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( float );
					vPosition = *(D3DXVECTOR3*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( D3DXVECTOR3 );
					pFrame->bTexInvert = *(bool*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( bool );
					pFrame->fWidth = *(float*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( float );
					pFrame->fHeight = *(float*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( float );
					pFrame->fTU1 = *(float*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( float );
					pFrame->fTV1 = *(float*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( float );
					pFrame->fTU2 = *(float*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( float );
					pFrame->fTV2 = *(float*)pUnloadAct->pData;
					pUnloadAct->pData += sizeof( float );
					strncpy( szTexFile, (char*)pUnloadAct->pData, 50 );
					pUnloadAct->pData += 50;
					if( pUnloadAct->version > 1 )
						pFrame->Color = *(DWORD*)pUnloadAct->pData;
					else
						pFrame->Color = 0xFFFFFFFF;
					pUnloadAct->pData += sizeof( DWORD );

					// 讀取中心點資訊
					if( pUnloadAct->version > 2 )
					{
						pFrame->fPercentX = *(float*)pUnloadAct->pData;
						pUnloadAct->pData += sizeof_float;
						pFrame->fPercentY = *(float*)pUnloadAct->pData;
						pUnloadAct->pData += sizeof_float;
					}
					else
					{
						pFrame->fPercentX = 50.0f;
						pUnloadAct->pData += sizeof_float;
						pFrame->fPercentY = 100.0f;
						pUnloadAct->pData += sizeof_float;
					}
                    // 線性參數
                    pFrame->LinearFlag = *(DWORD*)pUnloadAct->pData;
                    pUnloadAct->pData += sizeof_DWORD;
					pUnloadAct->pData += 4; // 預留欄位

					// 讀取材質
					if( szTexFile[0] == '\0' )
						pFrame->pImage = NULL;
					else
						pFrame->pImage = g_RoleImageManager.GetDynamicImageRes( szTexFile );

					// 更新畫面矩陣
					pFrame->vPosition = vPosition;
					pFrame->vScale = vScale;
					pFrame->RefreshMatrix( vScale, vPosition );
				}
			}
		}

		pUnloadAct->pActionList->push_back(pAction) ; 
		pUnloadAct->actCount-- ; 

		boost::thread::sleep(xt) ; 


	}//end for - i 
	

		
}
//---------------------------------------------------------------------------
RoleMemoryMgr::RoleMemoryMgr() : m_unloadingActVec() , m_roleRendererList() , m_unloadingRPCList() ,m_unloadingStreamList()
{

	//m_pTempFramePool = new boost::pool<>(sizeof(sFrame))  ;

	m_pImageStaticPool = new boost::object_pool<sImage>() ;

	CreatePools() ; 

	srand(time(0)) ; 
	
}
//---------------------------------------------------------------------------
RoleMemoryMgr::~RoleMemoryMgr()
{
	DeletePools() ; 

	delete m_pImageStaticPool ; 
	m_pImageStaticPool = NULL ; 

	
	//delete m_pTempFramePool ; 
//	m_pTempFramePool = NULL ; 
 
	ClearSteamRPCs() ; 
	
	ClearLoadRPCs() ; 


	ClearLoadActions() ; 

    Destory();
}
void RoleMemoryMgr::PushUnLoadingAction(unsigned short	version , sRoleActionList* pActList , BYTE *pData , unsigned int actCount)
{
	stUnLoadingAction unloadAct ; 
	

	unloadAct.pActionList = pActList; 
	unloadAct.pData = pData;
	unloadAct.actCount = actCount;
	unloadAct.version = version ; 

	m_unloadingActVec.push_back(unloadAct) ; 
}

void RoleMemoryMgr::ClearLoadActions()
{
	if(m_pThread != NULL)
	{
	//	m_threads.remove_thread(m_pThread) ; 
	//	delete m_pThread  ; 
	//	m_pThread = NULL ; 
		if(m_pThread->joinable())
			m_pThread->join() ; 
		m_pThread.reset() ; 
	}

	m_unloadingActVec.clear() ; 

}

void RoleMemoryMgr::ClearSteamRPCs()
{
	if(m_pRPCStreamThread != NULL)
	{
		if(m_pRPCStreamThread->joinable())
			m_pRPCStreamThread->join() ; 
		m_pRPCStreamThread.reset() ; 
	}

	m_unloadingStreamList.clear() ; 
}


bool RoleMemoryMgr::LoadActions()
{
	if(m_unloadingActVec.empty())
	{
		return true; 
	}


	if(m_pThread != NULL)
	{
		if(m_pThread->timed_join(boost::posix_time::microseconds(1)))
		{		
			
			if(m_unloadingActVec.front().actCount > 0)
			{
				Assert(0 , "thread error") ; 
			}

			m_pThread.reset() ; 
			
			m_unloadingActVec.pop_front() ; 

			if(m_unloadingActVec.empty())
			{
				return true ; 
			}

			stUnLoadingAction &unloadAct = m_unloadingActVec.front() ; 

			m_pThread.reset(new boost::thread(boost::bind(&LoadActionThread , &unloadAct))) ; 
	
		}
		else
		{
			
		}
	}
	else
	{
		stUnLoadingAction &unloadAct = m_unloadingActVec.front() ; 

		m_pThread.reset(new boost::thread(boost::bind(&LoadActionThread , &unloadAct))) ;

	
	}


	return false ; 

}

bool RoleMemoryMgr::LoadRPCFile()
{

    if( m_unloadingStreamList.empty() )
        return true;


	if(m_pRPCStreamThread != NULL)
	{
		if(m_pRPCStreamThread->timed_join(boost::posix_time::microseconds(1) ))
		{
			//表示已經讀取完畢
			stRPCThreadStream &pLoadingStream = m_unloadingStreamList.front() ;

			if(pLoadingStream.state == FILEVAILD || pLoadingStream.state == THREAD)
			{
				m_pRPCStreamThread.reset() ; 

				std::string head("Loading RPC Memory Error(請找美術要*.RPC檔) :") ; 
				
				head += pLoadingStream.filename ; 

				Assert(0 , head.c_str() ) ; 

				m_pRPCStreamThread.reset() ;

				return false ; 
			}

		

			//複製一份，存於資源管理中
			FDO::memPack * pMem = LoadResource<FDO::memPack , ResMemObj>(pLoadingStream.filename.c_str() ,
				pLoadingStream.pMem) ;

			pLoadingStream.pMem = NULL ; 
		

			m_pRPCStreamThread.reset() ; 
			
			return true ; 
		
		}

	}
	else
	{
		stRPCThreadStream &pUnloadingStream = m_unloadingStreamList.front() ;

		FDO::memPack *pMem = FindResource<FDO::memPack , ResMemObj>(pUnloadingStream.filename.c_str()) ; 
		
		if(pMem == NULL)
		{
			m_pRPCStreamThread.reset(new boost::thread(boost::bind(&LoadRPCMemoryBuffer , &pUnloadingStream))) ; 
		
		}
		else
		{

			std::list<stRoleRenderAsset>::iterator iter(m_roleRendererList.begin()) ; 

			for(; iter != m_roleRendererList.end() ; ++iter)
			{

				if(iter->SkinID != pUnloadingStream.rpc.SkinID)
					continue ; 

				if(iter->pRoleRender != pUnloadingStream.pRoleRender)
				{
					_LOGA(0 , "RAT和RPC會對錯！%s , iter->pRoleRender = %x , pUnloadingStream.pRoleRender = %x" ,pUnloadingStream.filename.c_str() ,iter->pRoleRender , pUnloadingStream.pRoleRender);
					Assert(0 , "RAT和RPC會對錯！") ; 
				}

				pUnloadingStream.rpc.pData = pMem->Data ; 

				PushUnLoadingRPC(pUnloadingStream.pRoleRender , pUnloadingStream.rpc.pRPCFactory , pUnloadingStream.rpc.spRPCFile ,
					pUnloadingStream.rpc.spRATFile , pUnloadingStream.rpc.pData , pUnloadingStream.rpc.RoleID ,
					pUnloadingStream.rpc.SkinID , pUnloadingStream.rpc.pFileName.c_str()) ; 
			
				//RoleID是唯一的，不需要繼續跑
				break ; 
			}

			//拿掉，繼續下一個工作項
			m_unloadingStreamList.pop_front() ; 
		}

	}

	return false ; 
	
}

void RoleMemoryMgr::PushRPCStream(const char *filename , CRoleRenderer *pRoleRenderer , CRPCFactory *pRPCFactory , SP_RPCFile &spRPCFile , SP_RATFile &spRATFile , 
							      uint32 &roleID)
{
	stRPCThreadStream stream ; 

	stream.rpc.spRPCFile = spRPCFile ; 
	stream.rpc.spRATFile = spRATFile ; 
	stream.rpc.pData = NULL ; 
	stream.rpc.RoleID = roleID ; //

	SYSTEMTIME ti ; 

	GetSystemTime(&ti) ; 
	
	DWORD skinID = ti.wHour + ti.wMinute + ti.wSecond + ti.wMilliseconds + roleID + rand() ; 

	while(RegisterLinkRPC(pRoleRenderer , roleID , skinID))
	{
		//重複SkinID的話
		skinID += rand() ; 
	}
	
	stream.rpc.SkinID = skinID ; 
	stream.rpc.pFileName = filename ; 
	stream.rpc.pRPCFactory = pRPCFactory ;  

	roleID = stream.rpc.SkinID ;

	stream.filename = filename ; 
	stream.pRoleRender = pRoleRenderer ; 

	m_unloadingStreamList.push_back(stream) ; 






}

void RoleMemoryMgr::PushUnLoadingRPC(CRoleRenderer *pRoleRenderer , CRPCFactory *pRPCFactory , SP_RPCFile &spRPCFile ,
									 SP_RATFile &spRATFile , BYTE* pData , int RoleID , int SkinID ,  const char* pFileName)
{
	stUnLoadRPC rpc ; 

	rpc.spRPCFile = spRPCFile ; 
	rpc.spRATFile = spRATFile ; 
	rpc.pData = pData ; 
	rpc.RoleID = RoleID ; 
	rpc.SkinID = SkinID ; 
	rpc.pFileName = pFileName ; 
	rpc.pRPCFactory = pRPCFactory ;  
	rpc.pRoleRender = pRoleRenderer ; 

	m_unloadingRPCList.push_back(rpc) ;

	
}


bool RoleMemoryMgr::RegisterLinkRPC(CRoleRenderer *pRoleRenderer , int RoleID , DWORD SkinID)
{
	std::list<stRoleRenderAsset>::iterator iter(m_roleRendererList.begin()) ; 
	
	for(; iter != m_roleRendererList.end() ; ++iter)
	{
		if( iter->SkinID == SkinID)
		{
		//	Assert(0 , "註冊相同的Skin ID") ; 

			return true; 
		}
	}

	stRoleRenderAsset role; 

	//記錄要穿的對象
	role.RoleID = RoleID; 
	role.SkinID = SkinID ; 
	role.pRoleRender = pRoleRenderer ; 

	m_roleRendererList.push_back(role) ;

	return false ; 

}
void RoleMemoryMgr::UnRegisterLinkRPC(DWORD skinID)
{
	std::list<stRoleRenderAsset>::iterator iter(m_roleRendererList.begin()) ; 

	for(; iter != m_roleRendererList.end() ; ++iter)
	{
		if( iter->SkinID != skinID)
			continue ; 

		m_roleRendererList.erase(iter) ;

		break ; 
	}
}

bool RoleMemoryMgr::LoadRPC()
{
	

	if(m_unloadingRPCList.empty())
	{
		return true ; 
	}

	if(m_pRPCThread != NULL)
	{
		if(m_pRPCThread->timed_join(boost::posix_time::microseconds(1) ))
		{
			//跑完，就關閉thread
			m_pRPCThread.reset() ; 

			return true ; 

		}
		else
		{
			
		}
	}
	else
	{
		stUnLoadRPC &unloadRPC = m_unloadingRPCList.front() ; 

		//如果Thread已經跑完了，已經丟入RPC Manage中。所以再一次的檢查
		if(GetRPCFileManage()->Get(unloadRPC.pFileName , unloadRPC.spRPCFile))
		{
			
			//假設成功取得的話，尋找需要Skin Link的RoleRender
			std::list<stRoleRenderAsset>::iterator iter(m_roleRendererList.begin()) ; 

			for(; iter != m_roleRendererList.end() ; ++iter)
			{
				if(iter->SkinID != unloadRPC.SkinID)
					continue ; 

				if(iter->pRoleRender != unloadRPC.pRoleRender)
				{
					
					_LOGA(0 , "RAT和RPC會對錯！%s , iter->pRoleRender = %x , pUnloadingStream.pRoleRender = %x" ,unloadRPC.pFileName.c_str() ,iter->pRoleRender , unloadRPC.pRoleRender);					
					break ; 
				}

				iter->pRoleRender->SkinLinkRPC(unloadRPC.spRPCFile) ; 

				m_roleRendererList.erase(iter) ; 

				//RoleID是唯一的，不需要繼續跑
				break ; 
			}

			//拿掉，繼續下一個工作項
			m_unloadingRPCList.pop_front() ; 

			//不管現在的thread queue是否為空，直接回傳，等到下一個frame作事情。
			return true ; 

		}

		//確定沒有RPC資源，所以去建立。
		m_pRPCThread.reset(new boost::thread(boost::bind(&LoadRPCThread , &unloadRPC))) ;

	
	}



	return false ; 
}

void RoleMemoryMgr::ClearLoadRPCs()
{



	if(m_pRPCThread != NULL)
	{
		if(m_pRPCThread->joinable())
		{
			m_pRPCThread->join() ; 
		}
		m_pRPCThread.reset() ; 

	}

	//m_unloadingActVec.clear() ; 

	m_unloadingRPCList.clear() ; 
	m_roleRendererList.clear() ; 

}


void RoleMemoryMgr::CreatePools()
{

	//m_pComponentDir = new boost::pool<>(sizeof(sComponentDir)) ;
	//m_pRoleComponent = new boost::pool<>(sizeof(sRoleComponent)) ;
	//m_pRoleAction = new boost::pool<>(sizeof(sRoleAction));	
	//m_pImagePool = new boost::pool<>(sizeof(sImage)) ;


	m_pFramePool = new boost::object_pool<sFrame>()  ;

	m_pComponentDir = new boost::object_pool<sComponentDir>()  ;
	m_pRoleComponent = new boost::object_pool<sRoleComponent>() ;


	m_pImagePool = new boost::object_pool<sImage>() ;

	m_pRoleAction = new boost::object_pool<sRoleAction>() ;


	ClearLoadActions() ; 

}
void RoleMemoryMgr::DeletePools() 
{
	ClearLoadActions() ; 

	for (std::list<TdgHandle>::const_iterator it = m_Requesters.begin() ; it != m_Requesters.end() ; ++it)
	{
		TdgHandle hRequester = *it;
		GameCommand::Executer& exr = CommandSystem::fnGetExecuter();
		exr.RunLight(hRequester);
	}
		
	m_Requesters.clear();
	
	delete m_pFramePool ; 
	m_pFramePool = NULL ;


	delete m_pComponentDir ; 
	m_pComponentDir = NULL ; 
	delete m_pRoleComponent ; 
	m_pRoleComponent = NULL ; 
	delete m_pRoleAction ; 
	m_pRoleAction = NULL ; 
	delete m_pImagePool ; 
	m_pImagePool = NULL ;

}
//---------------------------------------------------------------------------

sImage*	RoleMemoryMgr::GetDynamicImage()
{
	sImage *pImage = m_pImagePool->construct() ; 

	//sImage *pImage = (sImage*)m_pImagePool->malloc() ; 


	//ZeroMemory(pImage , sizeof(sImage)) ; 


	return pImage ; 
}

sImage*	RoleMemoryMgr::GetStaticImage()
{
	sImage *pImage = m_pImageStaticPool->construct() ; 
	
	return pImage ; 
}


void RoleMemoryMgr::ResourceRequesterRegister	(TdgHandle hRequester)
{
	if(RS_HANDLEVALID(hRequester))
		m_Requesters.push_back(hRequester);
}

void RoleMemoryMgr::ResourceRequesterUnregister	(TdgHandle hRequester)
{
	if(RS_HANDLEVALID(hRequester))
		m_Requesters.remove(hRequester);
}

sRoleAction* RoleMemoryMgr::GetAction()
{
	
    sRoleAction* pAction = NULL;

	
	//pAction = new(m_pRoleAction->Create()) sRoleAction();
#ifdef OBJMGR


	pAction = m_pRoleAction->construct() ; 

//	CreateObj(&pAction) ; 

#else
    if( m_FreeActionList.empty() )
    {
        pAction = new sRoleAction;
        Assert( pAction != 0, "Call new failed !!" );
    }
    else
    {
        pAction = m_FreeActionList.front();
        m_FreeActionList.pop_front();
    }
#endif




    return pAction;
}
//---------------------------------------------------------------------------
sRoleComponent* RoleMemoryMgr::GetComponent()
{
    sRoleComponent* pComponent = NULL;


#ifdef OBJMGR

	//pComponent = (sRoleComponent*)m_pRoleComponent->malloc() ; 

	pComponent = m_pRoleComponent->construct() ; 

	//ZeroMemory(pComponent , sizeof(sRoleComponent)) ; 


    for( int i=0; i<8; ++i )
    {
		pComponent->ComponentDirs[i] = m_pComponentDir->construct() ; 

        pComponent->ComponentDirs[i]->eDir = (RoleDirect)i;
    } 

	//CreateObj(&pComponent) ;

#else
    if( m_FreeComponentList.empty() )
    {
        pComponent = new sRoleComponent;
        Assert( pComponent != 0, "Call new failed !!" );
    }
    else
    {
        pComponent = m_FreeComponentList.front();
        m_FreeComponentList.pop_front();
    }
#endif





    return pComponent;
}
//---------------------------------------------------------------------------
void RoleMemoryMgr::ReleaseTempFrame(sFrame* pFrame)
{
//	m_pTempFramePool->free(pFrame) ; 

	pFrame->Reset() ; 
	m_FreeFrameList.push_back(pFrame) ; 
}

sFrame*	RoleMemoryMgr::GetTempFrame()
{
	//sFrame* pFrame = NULL;

	//pFrame = (sFrame*)m_pTempFramePool->malloc() ;	

	//Assert( pFrame , "RoleMemoryMgr::GetTempFrame 嚴重錯誤 ...DVO_V1.0.1479.75@20100309-171401crash ") ; 
	//memset(pFrame , 0 , sizeof(sFrame)) ; 

	//return pFrame ; 


	sFrame *pFrame = NULL ; 

    if( m_FreeFrameList.empty() )
    {
        pFrame = new sFrame();
        Assert( pFrame != 0, "Call new failed !!" );
    }
    else
    {
        pFrame = m_FreeFrameList.front();
        m_FreeFrameList.pop_front();
    }

	return pFrame ;
}

sFrame* RoleMemoryMgr::GetFrame()
{
    sFrame* pFrame = NULL;


#ifdef OBJMGR

	pFrame = m_pFramePool->construct() ; 
	//pFrame = (sFrame*)m_pFramePool->malloc() ; 

	//ZeroMemory(pFrame , sizeof(sFrame)) ; 


	//CreateObj(&pFrame) ;

	//pFrame = new sFrame();
#else
    if( m_FreeFrameList.empty() )
    {
        pFrame = new sFrame();
        Assert( pFrame != 0, "Call new failed !!" );
    }
    else
    {
        pFrame = m_FreeFrameList.front();
        m_FreeFrameList.pop_front();
    }
#endif





    return pFrame;
}

//---------------------------------------------------------------------------
void RoleMemoryMgr::FreeAction( sRoleAction* p )
{
// 	p->~sRoleAction();
// 	m_pRoleAction->Destroy(p);
#ifdef OBJMGR

	//先清空，這關係到資源管理部分。很重要！
   // p->Reset();
	//再清除物件
	//DeleteObj(p) ;
	
	

#else
    p->Reset();
    m_FreeActionList.push_back( p );
#endif



}

//---------------------------------------------------------------------------
void RoleMemoryMgr::FreeComponent( sRoleComponent* p )
{

#ifdef OBJMGR
	//先清空，這關係到資源管理部分。很重要！
   // p->Reset();
	//再清除物件
	//DeleteObj(p) ;

	

#else
    p->Reset();
    m_FreeComponentList.push_back( p );
#endif



}

//---------------------------------------------------------------------------
void RoleMemoryMgr::FreeFrame( sFrame* p )
{

#ifdef OBJMGR
	//先清空，這關係到資源管理部分。很重要！
    p->Reset();
	//再清除物件
	DeleteObj(p) ;



#else
    p->Reset();
    m_FreeFrameList.push_back( p );
#endif



}

//---------------------------------------------------------------------------
void RoleMemoryMgr::Destory()
{


#ifdef OBJMGR

	//不需要作任何事情
    GetRPCManage()->Clear();
    GetRATManage()->Clear();

	GetRATFileManage()->Clear();
	GetRPCFileManage()->Clear();

    for_each( m_FreeFrameList.begin(), m_FreeFrameList.end(), DeleteFrame );
    m_FreeFrameList.clear();  

#else
    for_each( m_FreeActionList.begin(), m_FreeActionList.end(), DeleteAction );
    for_each( m_FreeComponentList.begin(), m_FreeComponentList.end(), DeleteComponent );

    m_FreeActionList.clear();
    m_FreeComponentList.clear();
#endif


}

//---------------------------------------------------------------------------
// MemDataMgr Class
//---------------------------------------------------------------------------
MemDataMgr::MemDataMgr()
{
}

//---------------------------------------------------------------------------
MemDataMgr::~MemDataMgr()
{
    RemoveAllMemData();
}

//---------------------------------------------------------------------------
//  取得已儲存記憶體資料
//---------------------------------------------------------------------------
unsigned char* MemDataMgr::GetMemData( const string& strName )
{
    MemDataMap::const_iterator pos( m_MemDataMap.find(strName) );
    if( pos == m_MemDataMap.end() )
        return NULL;
    else
        return (pos->second->m_pData);
}

//---------------------------------------------------------------------------
// 增加記憶體資料
//---------------------------------------------------------------------------
unsigned char* MemDataMgr::AddMemData( const string& strName, unsigned char* pData, unsigned int iSize )
{
    sMemData* pDataTemp = new sMemData;
    Assert( pDataTemp != 0, "Call new failed !!" );

    // 儲存資料
    pDataTemp->m_strName = strName;
    pDataTemp->m_pData = new unsigned char[iSize];
    Assert( pDataTemp->m_pData != 0, "Call new failed !!" );
    memcpy( pDataTemp->m_pData, pData, iSize );
    m_MemDataMap.insert( make_pair( pDataTemp->m_strName, pDataTemp ) );

    return pDataTemp->m_pData;
}

//---------------------------------------------------------------------------
// 刪除記憶體資料
//---------------------------------------------------------------------------
bool MemDataMgr::RemoveMemData( const string& strName )
{
    MemDataMap::iterator pos( m_MemDataMap.find( strName ) );
    if( pos == m_MemDataMap.end() )
    {
        return false;
    }
    else
    {
        delete pos->second;
        m_MemDataMap.erase( pos );
        return true;
    }
}

//---------------------------------------------------------------------------
// 移除所有記憶體資料
//---------------------------------------------------------------------------
void MemDataMgr::RemoveAllMemData()
{
    for_each( m_MemDataMap.begin(), m_MemDataMap.end(), DelMemData );
    m_MemDataMap.clear();
}

//---------------------------------------------------------------------------
// 取得空白記憶體
//---------------------------------------------------------------------------
unsigned char* GetNewMemory( unsigned int iMemorySize )
{
    static unsigned int iMaxDataSize = 0;
    static vector<unsigned char> MemoryData;

    // 紀錄最大使用容量
    if( iMaxDataSize < iMemorySize )
    {
        iMaxDataSize = iMemorySize;

        // 重置新容量
        MemoryData.resize( iMaxDataSize );
    }

     return &MemoryData[0];
}

//---------------------------------------------------------------------------
// 取得空白記憶體2
//---------------------------------------------------------------------------
unsigned char* GetNewMemory2( unsigned int iMemorySize )
{
    static unsigned int iMaxDataSize = 0;
    static vector<unsigned char> MemoryData;

    // 紀錄最大使用容量
    if( iMaxDataSize < iMemorySize )
    {
        iMaxDataSize = iMemorySize;

        // 重置新容量
        MemoryData.resize( iMaxDataSize );
    }

     return &MemoryData[0];
}  
//---------------------------------------------------------------------------


#ifdef RESMGR

//釋放ActionPack物件資源
void FreeActionPack(std::list<std::list<FDO::sRoleAction*> > &actpack) 
{
	std::list<std::list<FDO::sRoleAction*> >::iterator iter(actpack.begin()) ;

	for_each(actpack.begin() , actpack.end() , 
		boost::bind(&FDO::FreeActionList , _1)) ;

}

//釋放ActionList物件資源
void FreeActionList(std::list<FDO::sRoleAction*> &actlist) 
{
	for_each(actlist.begin() , actlist.end() , 
		boost::bind(&FDO::FreeAction , _1)) ;
}

void FreeAction(FDO::sRoleAction *act)
{
	FDO::g_RoleMemoryMgr.FreeAction(act) ;
}

CFreeAction::CFreeAction(void) :m_act() , m_packlist() 
{
	m_maxactnums = 0 ;
	m_maxpacknums = 0;

	m_maxtime = 5000 ;  //最大為五秒
	m_curtime = 0 ; 



	m_tempact = m_act.end() ;
	m_muchact = m_act.end() ;

}
CFreeAction::~CFreeAction(void)
{

	Actmap::iterator iter(m_act.begin()) ;


	while( iter != m_act.end())
	{
		FreeActionPack(iter->second) ;

		iter++ ;

	}


	if(m_packlist.size())
	{
		std::list<PackDate>::iterator packIter(m_packlist.begin()) ;


		while(packIter != m_packlist.end())
		{

			FDO::FreeActionList(packIter->actlist) ; 

			++packIter ;
		}

		
		m_packlist.clear() ; 

	}
	m_act.clear() ; 

}


void CFreeAction::Init(unsigned int packnums , unsigned int actnums ,const DWORD time)
{
	m_maxactnums = actnums ;
	m_maxpacknums = packnums;

	m_maxtime = time ; 
}

void CFreeAction::UpdateTime(DWORD time) 
{

	//判斷是否為空
	if(m_packlist.empty())
	{
		return ; 
	}


	m_curtime += time ; 

	//超過時間的話，進行刪除
	if(m_curtime >= m_maxtime)
	{
		PopCatch() ; 

		m_curtime = 0 ; 
	}
}

void CFreeAction::PopCatch(void)
{
	//刪除最前面的資源
	FDO::FreeActionList( m_packlist.front().actlist) ; 

	//再從packlist移除
	m_packlist.pop_front() ; 

	//空的話，計數器歸0
	if(m_packlist.empty())
	{
		m_curtime = 0 ;
	}

}

//丟入整套的action file
void CFreeAction::PushCatch(const std::string &actname , Actpack &pack) 
{
	Actpack::iterator iter(pack.begin()); 


	while(iter != pack.end())
	{

		PushCatch(actname , *iter) ; 

		++iter ;;
	}

}

//丟入單一的action file
void CFreeAction::PushCatch(const std::string &actname , Actlist &act)
{
	//放置待刪除的快取
	PackDate date ; 

	date.filename = actname ; 
	date.actlist = act ; 

	m_packlist.push_back(date) ;
}

//回傳true：表示已經插入一筆資料，或許還有空間。
//回傳false：表示沒有空間可以塞進去。
bool CFreeAction::Push(const std::string &actname , std::list<FDO::sRoleAction*> &act)
{

	//在檢查是否有此動作封包和此動作封包的數量
	if (!CheackActNums(actname))
	{

		//////////			作排除的動作，讓快取有流動		//////////

		////////////		如果map的某一個action是空太多的話刪除不必要的pack action
		if( m_muchact != m_act.end())
		{
			//表示某一個動作封包必須要砍掉，因為過量的關係
			//移除整個動作封包
		//	FDO::FreeActionPack(m_muchact->second) ;


			//放置待刪除的快取
			PushCatch(actname , m_muchact->second) ;
			

			//從map中移除
			m_act.erase(m_muchact) ;

			//重新初始化
	//		m_muchact = m_act.end() ;

			//return false ;

		}

		////////////	假如在map沒有任何一個空的pack action可刪的話，丟入快取中		
		//放置待刪除的快取
		PushCatch(actname , act) ; 

		//清空它，因為已經放入快取了。預防act做其他動作
		act.clear() ; 

		//重新初始化
		m_muchact = m_act.end() ;

		return false ;
	}

	if ( m_tempact != m_act.end())
	{
		//表示有找到動作封包，並且可以插入到動作封包裡面
		m_tempact->second.push_back(act) ;
	}
	else
	{
		//建立一筆新的動作封包
		Actpack pack ; 
		//並且將此動作丟入
		pack.push_back(act) ;
		//建立一筆新的
		m_act.insert( std::make_pair(actname , pack)) ;
	}

	//不管有沒有插入，要重新初始化
	m_tempact = m_act.end() ;
	
	return true ;
}

void CFreeAction::Get(const std::string &actname , std::list<FDO::sRoleAction*> &act)
{

	/////////			取得資源		/////////////
	Actmap::iterator iter ;

	iter = MapFind(m_act , actname) ; 

	if( iter != m_act.end())
	{

		if(!(iter->second.empty()))
		{
			//進行拷貝。只是拷貝指標。
			act = iter->second.front() ;
			//移除它，確保map裡面都是無使用中的action
			iter->second.pop_front() ;
		}
		else
		{
			//如果已經空的話，移除它
			m_act.erase(iter) ;
		}
	
		return ;
	}

	//////////			在檢查快取中是否擁有資源			/////////////////////
	std::list<PackDate>::iterator PackIter(m_packlist.begin()) ; 

	while(PackIter != m_packlist.end())
	{
		//在快取中尋找資源
		if(PackIter->filename == actname)
		{
			//進行拷貝。只是拷貝指標。
			act = PackIter->actlist ;

			//從快取中移除
			m_packlist.erase(PackIter) ; 

			return ; 
		}
		++PackIter ; 
	}


	
}

bool CFreeAction::CheckPackNums(void) const 
{
	//只要大於動作的封包數量，回傳true
	return m_act.size() >= m_maxpacknums ? true : false ;
}

//回傳true：表示可以插入一筆資料，且m_tempact會有m_act.end()和非m_act.end()的可能性
//回傳false：表示動作封包已經超過容量，或此動作封包的量已經超過了，
//			  所以m_tempact會記錄此過量的動作封包。
bool CFreeAction::CheackActNums(const std::string &actname)
{
	Actmap::iterator iter ;

	iter = MapFind(m_act , actname) ; 

	if ( iter == m_act.end())
	{
		//沒有此動作封包資料。
		//確認是否可以插入一筆新的動作封包
		if(!CheckPackNums())
		{
			//只要沒有超過，回傳true
			m_tempact = m_act.end() ; 

			return true ;
		}
		else
		{
			//記錄哪一個動作封包過量，等待下一個要砍的對象
			CheckActMaxNums() ; 

			//動作封包已經超過容量
			return false ;
		}
	}

	//大於此動作封包的一定數量，回傳false。
	if (iter->second.size() >= m_maxactnums)
	{
		//記錄過量的動作封包
		m_muchact = iter ;

		//此動作封包的量，已經超過了
		return false ;
	}


	//記錄此iterator，等會會利用此作插入動作
	m_tempact = iter ;

	return true ;
}

void CFreeAction::CheckActMaxNums(void)
{
	unsigned int tempnums = 0 ;

	Actmap::iterator iter(m_act.begin()) ;

	while(iter != m_act.end())
	{
		if (tempnums < iter->second.size())
		{
			//得到最大值
			tempnums = iter->second.size() ; 

			//紀錄iterator
			m_muchact = iter ;
		}
		
		iter++ ;
	}

}
#else

#endif

//---------------------------------------------------------------------------
bool FrameSort(sFrame* i, sFrame* j) { return (i->shLayer<j->shLayer); }
//---------------------------------------------------------------------------
} //namespace FDO

