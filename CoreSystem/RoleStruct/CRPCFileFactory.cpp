#include "stdafx.h"
#include "CRPCFileFactory.h"
#include "RoleResourceManage.h"
#include <algorithm>

using namespace FDO;

struct CRPCFactoryException
{
	friend class CRPCFactory; 
	typedef std::string Description;
	CRPCFactoryException	(const Description& description) : mDescription(description) {	}
	~CRPCFactoryException	(){}
private:
	uint nReadSize;
	Description	mDescription;
};

CRPCFactory::CRPCFactory()
{
}

CRPCFactory::~CRPCFactory()
{

}

//////////////////////////////////////////////////////////////////////////
//取得rpc檔 對外唯一的介面
//////////////////////////////////////////////////////////////////////////
bool CRPCFactory::GetRPCFile(const char* pFilename, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile)
{
	if( pFilename == NULL)
	{
		return false;
	}

	bool bState = true;

	//沒找到的話就建立
	if( !GetRPCFileManage()->Get( pFilename, spRPCFile ) )
	{
		//沒找到再new
		spRPCFile.reset( new SRPCFile, CReleaseRoleRes() ); 
		bState = StartCreateRPCFile( pFilename, spRPCFile, spRATFile );
		
		GetRPCFileManage()->Get( pFilename, spRPCFile );
	}
	//釋放
	return bState;
}

bool CRPCFactory::GetRPCFileThread(const char* pFilename, CRoleRenderer *pRoleRenderer,  SP_RPCFile& spRPCFile, 
								   SP_RATFile& spRATFile , uint32 &nRoleID)
{
	if( pFilename == NULL)
	{
		return false;
	}


	//沒找到的話就建立
	if( !GetRPCFileManage()->Get( pFilename, spRPCFile ) )
	{
		
		//沒找到再new
		spRPCFile.reset( new SRPCFile, CReleaseRoleRes() ); 

		//設定RPC檔名
		spRPCFile->strFilename = pFilename;


		g_RoleMemoryMgr.PushRPCStream(pFilename , pRoleRenderer , this , spRPCFile , spRATFile , nRoleID) ; 


		//BYTE* pData;
		//pData = LoadRPCPacketFile( pFilename );

		//if( pData == NULL )
		//{
		//	return false;
		//}

		////設定RPC檔名
		//spRPCFile->strFilename = pFilename;

		//g_RoleMemoryMgr.PushUnLoadingRPC(pRoleRenderer , this , spRPCFile , spRATFile , pData , nRoleID , pFilename) ; 

		//並不需要thread的話，就不需要設定RoleID
		return true ; 
	}

	//並不需要thread的話，就不需要設定RoleID
	return false ;

}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool CRPCFactory::StartCreateRPCFile( const char* pFilename, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile )
{
	BYTE* pData;
	pData = LoadRPCPacketFile( pFilename );
	if( pData == NULL )
	{
		return false;
	}

	//設定RPC檔名
	spRPCFile->strFilename = pFilename;

	// 讀取RPC標頭檔
	pData += LoadRPCHeaderFile( pData, spRPCFile );

	//設定檔頭
//	SetRPCHeaderFileData();

	//RESIZE
	SetRPCActSize(spRPCFile);

	//建立出rpc所有資料
	char* pComName1 = spRPCFile->RPCFileHeader.szComponentName1;
	char* pComName2 = spRPCFile->RPCFileHeader.szComponentName2;
	pData += CreateRPCFileAllData( spRPCFile->vRPCActList1, pComName1, spRPCFile, spRATFile, pData );
	pData += CreateRPCFileAllData( spRPCFile->vRPCActList2, pComName2, spRPCFile, spRATFile, pData );

	// TODO: 建立完畢，丟入記憶體池管理
	GetRPCFileManage()->Insert( pFilename , spRPCFile ) ;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//讀取RPC封裝檔
///////////	///////////////////////////////////////////////////////////////
BYTE* CRPCFactory::LoadRPCPacketFile( const char* pFilename )
{
	memPack* pMem = LoadFromPacket( pFilename );
	return pMem->Data; 
}

//////////////////////////////////////////////////////////////////////////
//讀取RPC檔頭
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::LoadRPCHeaderFile(BYTE* pData, SP_RPCFile& spRPCFile)
{
	memset( &(spRPCFile->RPCFileHeader), 0, _RPC_HEADER_FILE_SIZEOF );
	memcpy( &(spRPCFile->RPCFileHeader), pData, _RPC_HEADER_FILE_SIZEOF );
	return _RPC_HEADER_FILE_SIZEOF;
}

//////////////////////////////////////////////////////////////////////////
//設定rpc檔動作數量
//////////////////////////////////////////////////////////////////////////
void CRPCFactory::SetRPCActSize(SP_RPCFile& spRPCFile)
{
	//resize
	spRPCFile->vRPCActList1.resize( spRPCFile->RPCFileHeader.usActCount1 );
	spRPCFile->vRPCActList2.resize( spRPCFile->RPCFileHeader.usActCount2 );
}

//////////////////////////////////////////////////////////////////////////
//建立RPC檔資料
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::CreateRPCFileAllData( vRPCActFileList& vRPCActList, const char* pComName, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile, BYTE* pBeginByte )
{
	BYTE* pLastByte = pBeginByte;

	for( size_t i(0); i < vRPCActList.size(); ++i )
	{	
		try
		{
			//取出RPC動作
			SRPCActFile& rRPCAct = vRPCActList.at(i);
			//設定動作名稱
			pLastByte += SetRPCActFileName( rRPCAct, pLastByte );
			//沒找到相同動作就繼續
			SRATActFile* pRATAct = FindRATAct( rRPCAct.szName, spRATFile );
			if(pRATAct == NULL)
			{
				continue;
			}

			//取得RPC零件
			SRPCComFile& rRPCCom = rRPCAct.RPCCom;
			//設定RPC零件名稱
			strncpy( rRPCCom.szName, pComName, _SHORT_FILE_NAME_SIZE - 1 );
			//沒找到相同零件就繼續
			SRATComFile* pRATCom = FindRATCom( pComName, pRATAct );
			if(pRATCom == NULL)
			{
				continue;
			}

			//建立方向資料
			pLastByte += CreateRPCDirData( rRPCCom, *pRATCom, pLastByte, spRPCFile );
		}
		catch (const CRPCFactoryException& e)
		{
			std::string logstring;
			StringFormat8(logstring , "CreateRPCFileAllData RPC File %s ,RAT File %s \n",spRPCFile->strFilename.c_str() , spRATFile->strFilename.c_str() , e.mDescription.c_str());
			_LOGA(0 , logstring.c_str());
			Assert(0 , logstring.c_str());
		}				
	}
	return pLastByte - pBeginByte;
}

int CRPCFactory::CreateRPCFileAllDataThread( vRPCActFileList& vRPCActList, const char* pComName, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile, BYTE* pBeginByte )
{
	BYTE* pLastByte = pBeginByte;

    boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);

	for( size_t i(0); i < vRPCActList.size(); ++i )
	{

		xt.nsec += 1000000 * 10;
		try
		{
			//取出RPC動作
			SRPCActFile& rRPCAct = vRPCActList.at(i);
			//設定動作名稱
			pLastByte += SetRPCActFileName( rRPCAct, pLastByte );
			//沒找到相同動作就繼續
			SRATActFile* pRATAct = FindRATAct( rRPCAct.szName, spRATFile );
			if(pRATAct == NULL)
			{
				continue;
			}

			//取得RPC零件
			SRPCComFile& rRPCCom = rRPCAct.RPCCom;
			//設定RPC零件名稱
			strncpy( rRPCCom.szName, pComName, _SHORT_FILE_NAME_SIZE - 1 );
			//沒找到相同零件就繼續
			SRATComFile* pRATCom = FindRATCom( pComName, pRATAct );
			if(pRATCom == NULL)
			{
				continue;
			}

			//建立方向資料
			pLastByte += CreateRPCDirData( rRPCCom, *pRATCom, pLastByte, spRPCFile );
		}
		catch (const CRPCFactoryException& e)
		{
			std::string logstring;
			StringFormat8(logstring , "CreateRPCFileAllDataThread RPC File %s ,RAT File %s \n %s",spRPCFile->strFilename.c_str() , spRATFile->strFilename.c_str() , e.mDescription.c_str());
			_LOGA(0 , logstring.c_str());
			Assert(0 , logstring.c_str());
		}	
		boost::thread::sleep(xt) ; 

	}
	return pLastByte - pBeginByte;
}

//////////////////////////////////////////////////////////////////////////
//設定動作名稱
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::SetRPCActFileName(SRPCActFile& rRPCAct, BYTE* pData)
{
	strncpy( rRPCAct.szName, (char*)pData, _SHORT_FILE_NAME_SIZE - 1 );
	return _SHORT_FILE_NAME_SIZE;
}

//////////////////////////////////////////////////////////////////////////
//比對rat檔 動作名稱
//////////////////////////////////////////////////////////////////////////
SRATActFile* CRPCFactory::FindRATAct(const char* pActName, SP_RATFile& spRATFile)
{

	//搜尋rat檔動作
	vRATActFileList::iterator actPos(
		std::find( spRATFile->vRATActList.begin(), spRATFile->vRATActList.end(), pActName ) );
	if( actPos == spRATFile->vRATActList.end() )
	{
		//沒找到名稱相同的動作
		return NULL;
	}
	return &(*actPos);
}

//////////////////////////////////////////////////////////////////////////
//比對rat動作檔 零件名稱
//////////////////////////////////////////////////////////////////////////
SRATComFile* CRPCFactory::FindRATCom(const char* pComName, SRATActFile* pRATAct)
{


	//搜尋rat檔零件
	vRATComFileList::iterator comPos (
		std::find( pRATAct->vRATComList.begin(), pRATAct->vRATComList.end(), pComName ) );

	if( comPos == pRATAct->vRATComList.end() )
	{
		//沒找到名稱相同的零件
		return NULL;
	}
	return &(*comPos);
}

//////////////////////////////////////////////////////////////////////////
//建立方向資料
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::CreateRPCDirData( SRPCComFile& rRPCCom, SRATComFile& SRATCom, BYTE* pBeginByte, SP_RPCFile& spRPCFile )
{
	BYTE* pLastByte = pBeginByte;
	
	for( int i = 0; i < MAX_DIR; ++i )
	{
		//取出目前RPC索引方向
		SRPCDirFile& rNowRPCDir = rRPCCom.RPCDir[i];
		//
		SRATDirFile& rNowRATDir = SRATCom.RATDir[i];

		//重設每個方向裡的圖片數量
		rNowRPCDir.vRPCImgList.resize( *(unsigned short*)pLastByte );
		//增加位移量			
		pLastByte += _US_SHORT_SIZEOF;
		//建立圖片資料
		
		pLastByte += CreateRPCImageData( rNowRPCDir.vRPCImgList, rNowRATDir.vRATImageList, pLastByte, spRPCFile );
		
		
	}
	return pLastByte - pBeginByte;
}

//////////////////////////////////////////////////////////////////////////
//建立圖片資料
//////////////////////////////////////////////////////////////////////////

int CRPCFactory::CreateRPCImageData( vRPCImgFileList& RPCImgList, vRATImageFileList& RATImgList, BYTE* pBeginByte, SP_RPCFile& spRPCFile )
{
	//rat跟rpc圖片數量不同，表示檔案有問題
	
	
	size_t count = RPCImgList.size() ; 
	

	if(RPCImgList.size() != RATImgList.size())
	{
		std::string description;	
		StringFormat8(description , "rat跟rpc圖片數量不同，表示檔案有問題 \nRPC Image size %d, RAT Image size %d" , RPCImgList.size() , RATImgList.size());		
		throw CRPCFactoryException(description.c_str());
		if(RPCImgList.size() > RATImgList.size())
		{
			count = RATImgList.size() ; 
		}
		else
		{
			count = RPCImgList.size() ; 
		}
	}

	BYTE* pLastByte = pBeginByte;
	for( size_t i(0); i < count ; ++i )
	{
		//拿出rpc的img
		SRPCImageFile& rRPCImg = RPCImgList.at(i);
		//拿出rat的img
		SRATImageFile& rRATImage = RATImgList.at(i);
		
		pLastByte += FillRPCImageData( rRPCImg, pLastByte, spRPCFile );
		//設定rpc的id
		SetRPCImgID( rRPCImg, rRATImage, spRPCFile );
	}	
		
	return pLastByte - pBeginByte;
}

//////////////////////////////////////////////////////////////////////////
//設定rpc img id
//////////////////////////////////////////////////////////////////////////
void CRPCFactory::SetRPCImgID( SRPCImageFile& rRPCImg, SRATImageFile& rRATImg, SP_RPCFile& spRPCFile )
{
	rRPCImg.nRPCImgID = rRATImg.nRATImageID;
	spRPCFile->vRPCImgPointList1.push_back( &rRPCImg );
}

//////////////////////////////////////////////////////////////////////////
//填滿rpc圖檔的資料
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::FillRPCImageData(SRPCImageFile& pRPCImage, BYTE* pBeginByte, SP_RPCFile& spRPCFile )
{
	//BYTE* pLastByte = pBeginByte;
	////檔案空格
	//pLastByte += _BOOL_SIZEOF;
	//
	//int nReadSize = 0;
	//int nOffset = 0;

	//if( spRPCFile->RPCFileHeader.usVersion > 1 )
	//{
	//	nReadSize = _RPC_IMAGE_FILE_SIZEOF_NEW;
	//	nOffset = 6;	//保留欄位
	//}
	//else
	//{
	//	nReadSize = _RPC_IMAGE_FILE_SIZEOF_OLD;
	//	nOffset = 10;	//舊版保留欄位
	//	pRPCImage.dwColor = 0xFFFFFFFF; // 白色
	//}

	////先清空
	//memset( &pRPCImage, 0, nReadSize );
	//memcpy( &pRPCImage, pLastByte, nReadSize  );

	//pLastByte += nReadSize;
	//pLastByte += nOffset;

	//return pLastByte - pBeginByte;


	
//舊的
	BYTE* pLastByte = pBeginByte;
	//SFrm.bTexInvert = *(bool*)pData;
	pLastByte += sizeof_BOOL;

	pRPCImage.fWidth = *(float*)pLastByte;
	pLastByte += sizeof_float;

	pRPCImage.fHeight = *(float*)pLastByte;
	pLastByte += sizeof_float;

	pRPCImage.fTU1 = *(float*)pLastByte;
	pLastByte += sizeof_float;

	pRPCImage.fTV1 = *(float*)pLastByte;
	pLastByte += sizeof_float;

	pRPCImage.fTU2 = *(float*)pLastByte;
	pLastByte += sizeof_float;

	pRPCImage.fTV2 = *(float*)pLastByte;
	pLastByte += sizeof_float;

// <yuming> modify 
	//memset( pRPCImage.szTexFile, 0, LENGTHOF_FILENAME );
	//memcpy( pRPCImage.szTexFile, pLastByte, LENGTHOF_FILENAME-1 );
	//assert(pRPCImage.szTexFile[0] != 0);

    char  szTexFile[_LONG_FILE_NAME_SIZE];
    memset(szTexFile, 0, LENGTHOF_FILENAME);
    memcpy(szTexFile, pLastByte, LENGTHOF_FILENAME-1);
    pRPCImage.nTextureNameID = this->GetStringID(szTexFile);    
// </yuming>

	pLastByte += LENGTHOF_FILENAME;

	//控制版本
	if( spRPCFile->RPCFileHeader.usVersion > 1 )
	{
		pRPCImage.dwColor = *(DWORD*)pLastByte;
		pLastByte += sizeof_DWORD;
		pLastByte += 6;
	}
	else
	{
		pRPCImage.dwColor = 0xFFFFFFFF; // 白色
		pLastByte += 10;    // 預留欄位
	}

	return pLastByte - pBeginByte;
}

/**
 * @modifier yuming
 * @brief Generate texture id
 * @in texture file name
 * @out index of texture file name
 * @note this way don't clear texture string until program terminate,
 *       but can save memory ASAP
 */
unsigned int CRPCFactory::GetStringID(const char TextureName[])
{
    unsigned int Result = mTextureStringList.size();

    for (unsigned int i = 0; i < mTextureStringList.size(); ++i)
    {
        if (mTextureStringList.at(i) == TextureName)
        {
            Result = i;
            break;
        }
    }

    if (Result == mTextureStringList.size())
    {
        mTextureStringList.push_back(TextureName);
    }

    return Result;
}

/**
 * @modifier yuming
 * @brief get texture string
 * @in index of texture string list
 * @out texture string
 */
const char* CRPCFactory::GetTextureStringAt(unsigned int index)
{
    return mTextureStringList[index].c_str();
}