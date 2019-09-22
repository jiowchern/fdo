//////////////////////////////////////////////////////////////////////////
//CRATFactory.CPP
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CRATFileFactory.h"
#include "RoleResourceManage.h"
#include "CRoleSkinStruct.h"

using namespace FDO;
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
CRATFactory::CRATFactory() 
{
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
CRATFactory::~CRATFactory()
{
}

//////////////////////////////////////////////////////////////////////////
//取得rat檔, 
//注意!! shared_point 傳參考的話，count不會加1。
//////////////////////////////////////////////////////////////////////////
bool CRATFactory::GetRATFile(const char* pFilename, SP_RATFile& spRATFile )
{
	if( pFilename == NULL)
	{
		return false;
	}

    bool bGet = GetRATFileManage()->Get( pFilename, spRATFile );
	
	//沒找到的話就建立
    if (!bGet)
	{
        // TODO: new出RAT檔的記憶體空間
        spRATFile.reset( new SRATFile(), CReleaseRoleRes() );

		CreateRATFile( pFilename, spRATFile );
		Assert(spRATFile.get() , "");
		bGet = GetRATFileManage()->Get( pFilename, spRATFile );
	}

    // TODO: 可以寫成這樣
	/*if( spRATFile == NULL )
	{
		return false ; 
	}
	return true;*/
    return bGet;
}

//////////////////////////////////////////////////////////////////////////
//建立rat檔
//////////////////////////////////////////////////////////////////////////
bool CRATFactory::CreateRATFile( const char* pFilename, SP_RATFile spRATFile )
{
	BYTE* pData = LoadRATPacketFile( pFilename );	
	
	if( pData == NULL )
	{
		return false;
	}
	
	// 讀取標頭檔
	pData += LoadRATHeaderFile( pData, spRATFile );
	
	//設定檔頭
	SetRATFileRootData( pFilename, spRATFile );

	//開始建立rat檔需要的資料
	pData += StartCreateRATAllData( pData, spRATFile );	

	// TODO: 建立完畢，丟入記憶體池管理
	GetRATFileManage()->Insert( pFilename , spRATFile );
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
//讀取rat封裝檔
//////////////////////////////////////////////////////////////////////////
BYTE* CRATFactory::LoadRATPacketFile(const char* pFilename)
{
	memPack* pMem = LoadFromPacket( pFilename );
	return pMem->Data; 
}

//////////////////////////////////////////////////////////////////////////
//讀取rat檔頭
//////////////////////////////////////////////////////////////////////////
int CRATFactory::LoadRATHeaderFile(BYTE* pData, SP_RATFile& spRATFile)
{
	memset( &(spRATFile->RATFileHeader), 0, _RAT_HEADER_FILE_SIZEOF );
	memcpy( &(spRATFile->RATFileHeader), pData, _RAT_HEADER_FILE_SIZEOF );
	return _RAT_HEADER_FILE_SIZEOF;
}

//////////////////////////////////////////////////////////////////////////
//設定檔頭
//////////////////////////////////////////////////////////////////////////
void CRATFactory::SetRATFileRootData(const char* pFilename, SP_RATFile& spRATFile)
{ 
	SRATFileHeader pHeader = spRATFile->RATFileHeader;

	spRATFile->strFilename = pFilename;
	spRATFile->fScaleX = pHeader.fScaleX * 0.92f;
	spRATFile->fScaleY = pHeader.fScaleY * 0.92f;
	spRATFile->fScaleZ = pHeader.fScaleX * 0.92f;
	spRATFile->fShadowSize = pHeader.usShadowSize * 0.15f;
	spRATFile->fShadowOffset = pHeader.usShadowOffset * 0.1f;
	spRATFile->usBoundingBoxSize = pHeader.usBoundingBoxSize;
	spRATFile->usDeadBoundBoxSize = pHeader.usDeadBoundBoxSize;
}

//////////////////////////////////////////////////////////////////////////
//建立出rat檔的動作資料
//////////////////////////////////////////////////////////////////////////
int CRATFactory::StartCreateRATAllData(BYTE* pBeginByte, SP_RATFile& spRATFile) 
{
	BYTE* pLastByte = pBeginByte;
	//動作總數量
	int actCount = spRATFile->RATFileHeader.usActCount;
	
	//裝實體直接resize//採用資源管理要再改
	spRATFile->vRATActList.resize( actCount );
	
	for( int i = 0; i < actCount; ++i)
	{
		//填滿動作資料
		pLastByte += FillRATActFileData( &( spRATFile->vRATActList[i] ), pLastByte );		
		//建立零件資料
		pLastByte += CreateRATComData( &( spRATFile->vRATActList[i] ), pLastByte, spRATFile );
	}

	return pLastByte - pBeginByte ;
}

//////////////////////////////////////////////////////////////////////////
//填滿ACT檔的資料
//////////////////////////////////////////////////////////////////////////
int CRATFactory::FillRATActFileData(SRATActFile* pACTFile, BYTE* pBeginByte)
{
	BYTE* pLastByte = pBeginByte;
	SRATActFileEntry* pDat = (SRATActFileEntry*)pBeginByte;
	int dataSize = _RAT_ACT_SIZSEOF;
	memset( pACTFile, 0, dataSize );
	memcpy( pACTFile, pDat, dataSize);

	pLastByte += dataSize;
	pLastByte += _RAT_KEEP_SIZE;

	return pLastByte - pBeginByte ;
}

//////////////////////////////////////////////////////////////////////////
//建立出rat檔的動作資料
//////////////////////////////////////////////////////////////////////////
int CRATFactory::CreateRATComData(SRATActFile* pRATAct, BYTE* pBeginByte, SP_RATFile& spRATFile)
{
	BYTE* pLastByte = pBeginByte;
	// 取得零件總數
	int  iComponentCount = *(unsigned short*)pBeginByte;
	pLastByte += _US_SHORT_SIZEOF;
	
	pRATAct->vRATComList.resize( iComponentCount );

	for(int i = 0; i < iComponentCount; ++i)
	{
		//設定零件名稱
		pLastByte += SetRATComName( &( pRATAct->vRATComList[i] ), pLastByte);
		//設定方向
		pLastByte += CreateRATDirData( &( pRATAct->vRATComList[i] ), pLastByte, spRATFile );
	}
	
	return pLastByte - pBeginByte ;
}

//////////////////////////////////////////////////////////////////////////
//設定零件名稱
//////////////////////////////////////////////////////////////////////////
int CRATFactory::SetRATComName(SRATComFile* pRATCom, BYTE* pData)
{
	strncpy( pRATCom->szComName, (const char*)pData, _SHORT_FILE_NAME_SIZE -1 );
	return _SHORT_FILE_NAME_SIZE;
}

//////////////////////////////////////////////////////////////////////////
//建立出rat檔的方向資料
//////////////////////////////////////////////////////////////////////////
int CRATFactory::CreateRATDirData( SRATComFile* pRATCom, BYTE* pBeginByte, SP_RATFile& spRATFile)
{
	BYTE* pLastByte = pBeginByte;
	for( int i = 0; i < MAX_DIR; ++i )
	{
		// 設定資料
		SRATDirFile* pNowRATDir = &(pRATCom->RATDir[i]);
		pLastByte += SetDirMaxImageCount( pNowRATDir, pLastByte );
		//設定ratimagedata
		pLastByte += CreateRATImageData( pNowRATDir, pLastByte, spRATFile );
	}
	return pLastByte - pBeginByte ;
}

//////////////////////////////////////////////////////////////////////////
//設定方向畫面最大張數
//////////////////////////////////////////////////////////////////////////
int CRATFactory::SetDirMaxImageCount(SRATDirFile* pRATDir, BYTE* pData )
{
	// 取得資料
	pRATDir->usMaxFrameNum = *(unsigned short*)pData;
	return _US_SHORT_SIZEOF;
}

//////////////////////////////////////////////////////////////////////////
//設定方向畫面最大張數
//////////////////////////////////////////////////////////////////////////
int CRATFactory::CreateRATImageData(SRATDirFile* pRATDir, BYTE* pBeginByte, SP_RATFile& spRATFile)
{
	BYTE* pLastByte = pBeginByte;
	
	// 取得畫面總數
	int iRATImgSize = *(unsigned short*)pBeginByte;
	pLastByte += _US_SHORT_SIZEOF;
	
	//new出來
	pRATDir->vRATImageList.resize( iRATImgSize );
	
	// 建立畫面資料
	for(int i = 0; i < iRATImgSize; ++i)
	{
		pLastByte += FillRATImageData( &(pRATDir->vRATImageList[i]), pLastByte, spRATFile );
		SetImageSizeID( &( pRATDir->vRATImageList[i] ), spRATFile );
	}
	return pLastByte - pBeginByte ;
}

//////////////////////////////////////////////////////////////////////////
//計算圖片張數和設定圖片id
//////////////////////////////////////////////////////////////////////////
void CRATFactory::SetImageSizeID(SRATImageFile* pRATImage, SP_RATFile& spRATFile)
{
	//第一次進來為0，所以初始id為0
    pRATImage->nRATImageID = spRATFile->SRATImageList.size();
    spRATFile->SRATImageList.push_back( pRATImage );
}
//////////////////////////////////////////////////////////////////////////
//填滿rat image資料
//////////////////////////////////////////////////////////////////////////
int CRATFactory::FillRATImageData(SRATImageFile* pRATImage, BYTE* pBeginByte, SP_RATFile& spRATFile)
{

	//一行一行寫入
	BYTE* pLastByte = pBeginByte;

	//pRATImage->usUnitTime = *(unsigned short*)pData;
	pLastByte += sizeof_nShort;
	pRATImage->usPlayTime = *(unsigned short*)pLastByte;
	pLastByte += sizeof_nShort;
	pRATImage->shLayer = *(short*)pLastByte;
	pLastByte += sizeof_nShort;
	pRATImage->vScale.z = *(float*)pLastByte;
	pLastByte += sizeof_float;
	pRATImage->vScale.x = *(float*)pLastByte;
	pLastByte += sizeof_float;
	pRATImage->vScale.y = *(float*)pLastByte;
	pLastByte += sizeof_float;
	pRATImage->vPosition = *(D3DXVECTOR3*)pLastByte;
	pLastByte += sizeof_D3DXVECTOR3;
	pRATImage->bTexInvert = *(bool*)pLastByte;
	pLastByte += sizeof_BOOL;

	//pData += 9;    // 預留欄位
	if( spRATFile->RATFileHeader.usVersion > 1 )
	{
		pRATImage->fPercentX = *(float*)pLastByte;
		pLastByte += sizeof_float;
		pRATImage->fPercentY = *(float*)pLastByte;
		pLastByte += sizeof_float;
        pRATImage->LinearFlag = *(BYTE*)pLastByte;
		pLastByte += 1;
	}
	else
	{
		pRATImage->fPercentX = 50.0f;
		pRATImage->fPercentY = 100.0f;
		pLastByte += 9; // 預留欄位
	}
	
	return pLastByte - pBeginByte;


	//mem cpy 作法
	//檔案空格
	//pLastByte += _US_SHORT_SIZEOF;
	//
	//int nReadSize = 0;
	//int nOffset = 0;

	////區分版本
	//if( spRATFile->RATFileHeader.usVersion > 1 )
	//{
	//	//版本2的大小
	//	nReadSize = _RAT_IMAGE_FILE_SIZEOF_V2;
	//	nOffset = 1;
	//}
	//else
	//{
	//	//版本1的大小
	//	nReadSize = sizeof( _RAT_IMAGE_FILE_SIZEOF_V1 );
	//	nOffset = 9;		
	//	pRATImage->fPercentX = 50.0f;
	//	pRATImage->fPercentY = 100.0f;		
	//}

	//memset( pRATImage, 0, nReadSize );
	//memcpy( pRATImage, pBeginByte, nReadSize );

	//pLastByte += nReadSize;
	//pLastByte += nOffset;

	//return pLastByte - pBeginByte;
}