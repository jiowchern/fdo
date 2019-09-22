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
//���orat��, 
//�`�N!! shared_point �ǰѦҪ��ܡAcount���|�[1�C
//////////////////////////////////////////////////////////////////////////
bool CRATFactory::GetRATFile(const char* pFilename, SP_RATFile& spRATFile )
{
	if( pFilename == NULL)
	{
		return false;
	}

    bool bGet = GetRATFileManage()->Get( pFilename, spRATFile );
	
	//�S��쪺�ܴN�إ�
    if (!bGet)
	{
        // TODO: new�XRAT�ɪ��O����Ŷ�
        spRATFile.reset( new SRATFile(), CReleaseRoleRes() );

		CreateRATFile( pFilename, spRATFile );
		Assert(spRATFile.get() , "");
		bGet = GetRATFileManage()->Get( pFilename, spRATFile );
	}

    // TODO: �i�H�g���o��
	/*if( spRATFile == NULL )
	{
		return false ; 
	}
	return true;*/
    return bGet;
}

//////////////////////////////////////////////////////////////////////////
//�إ�rat��
//////////////////////////////////////////////////////////////////////////
bool CRATFactory::CreateRATFile( const char* pFilename, SP_RATFile spRATFile )
{
	BYTE* pData = LoadRATPacketFile( pFilename );	
	
	if( pData == NULL )
	{
		return false;
	}
	
	// Ū�����Y��
	pData += LoadRATHeaderFile( pData, spRATFile );
	
	//�]�w���Y
	SetRATFileRootData( pFilename, spRATFile );

	//�}�l�إ�rat�ɻݭn�����
	pData += StartCreateRATAllData( pData, spRATFile );	

	// TODO: �إߧ����A��J�O������޲z
	GetRATFileManage()->Insert( pFilename , spRATFile );
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
//Ū��rat�ʸ���
//////////////////////////////////////////////////////////////////////////
BYTE* CRATFactory::LoadRATPacketFile(const char* pFilename)
{
	memPack* pMem = LoadFromPacket( pFilename );
	return pMem->Data; 
}

//////////////////////////////////////////////////////////////////////////
//Ū��rat���Y
//////////////////////////////////////////////////////////////////////////
int CRATFactory::LoadRATHeaderFile(BYTE* pData, SP_RATFile& spRATFile)
{
	memset( &(spRATFile->RATFileHeader), 0, _RAT_HEADER_FILE_SIZEOF );
	memcpy( &(spRATFile->RATFileHeader), pData, _RAT_HEADER_FILE_SIZEOF );
	return _RAT_HEADER_FILE_SIZEOF;
}

//////////////////////////////////////////////////////////////////////////
//�]�w���Y
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
//�إߥXrat�ɪ��ʧ@���
//////////////////////////////////////////////////////////////////////////
int CRATFactory::StartCreateRATAllData(BYTE* pBeginByte, SP_RATFile& spRATFile) 
{
	BYTE* pLastByte = pBeginByte;
	//�ʧ@�`�ƶq
	int actCount = spRATFile->RATFileHeader.usActCount;
	
	//�˹��骽��resize//�ĥθ귽�޲z�n�A��
	spRATFile->vRATActList.resize( actCount );
	
	for( int i = 0; i < actCount; ++i)
	{
		//�񺡰ʧ@���
		pLastByte += FillRATActFileData( &( spRATFile->vRATActList[i] ), pLastByte );		
		//�إ߹s����
		pLastByte += CreateRATComData( &( spRATFile->vRATActList[i] ), pLastByte, spRATFile );
	}

	return pLastByte - pBeginByte ;
}

//////////////////////////////////////////////////////////////////////////
//��ACT�ɪ����
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
//�إߥXrat�ɪ��ʧ@���
//////////////////////////////////////////////////////////////////////////
int CRATFactory::CreateRATComData(SRATActFile* pRATAct, BYTE* pBeginByte, SP_RATFile& spRATFile)
{
	BYTE* pLastByte = pBeginByte;
	// ���o�s���`��
	int  iComponentCount = *(unsigned short*)pBeginByte;
	pLastByte += _US_SHORT_SIZEOF;
	
	pRATAct->vRATComList.resize( iComponentCount );

	for(int i = 0; i < iComponentCount; ++i)
	{
		//�]�w�s��W��
		pLastByte += SetRATComName( &( pRATAct->vRATComList[i] ), pLastByte);
		//�]�w��V
		pLastByte += CreateRATDirData( &( pRATAct->vRATComList[i] ), pLastByte, spRATFile );
	}
	
	return pLastByte - pBeginByte ;
}

//////////////////////////////////////////////////////////////////////////
//�]�w�s��W��
//////////////////////////////////////////////////////////////////////////
int CRATFactory::SetRATComName(SRATComFile* pRATCom, BYTE* pData)
{
	strncpy( pRATCom->szComName, (const char*)pData, _SHORT_FILE_NAME_SIZE -1 );
	return _SHORT_FILE_NAME_SIZE;
}

//////////////////////////////////////////////////////////////////////////
//�إߥXrat�ɪ���V���
//////////////////////////////////////////////////////////////////////////
int CRATFactory::CreateRATDirData( SRATComFile* pRATCom, BYTE* pBeginByte, SP_RATFile& spRATFile)
{
	BYTE* pLastByte = pBeginByte;
	for( int i = 0; i < MAX_DIR; ++i )
	{
		// �]�w���
		SRATDirFile* pNowRATDir = &(pRATCom->RATDir[i]);
		pLastByte += SetDirMaxImageCount( pNowRATDir, pLastByte );
		//�]�wratimagedata
		pLastByte += CreateRATImageData( pNowRATDir, pLastByte, spRATFile );
	}
	return pLastByte - pBeginByte ;
}

//////////////////////////////////////////////////////////////////////////
//�]�w��V�e���̤j�i��
//////////////////////////////////////////////////////////////////////////
int CRATFactory::SetDirMaxImageCount(SRATDirFile* pRATDir, BYTE* pData )
{
	// ���o���
	pRATDir->usMaxFrameNum = *(unsigned short*)pData;
	return _US_SHORT_SIZEOF;
}

//////////////////////////////////////////////////////////////////////////
//�]�w��V�e���̤j�i��
//////////////////////////////////////////////////////////////////////////
int CRATFactory::CreateRATImageData(SRATDirFile* pRATDir, BYTE* pBeginByte, SP_RATFile& spRATFile)
{
	BYTE* pLastByte = pBeginByte;
	
	// ���o�e���`��
	int iRATImgSize = *(unsigned short*)pBeginByte;
	pLastByte += _US_SHORT_SIZEOF;
	
	//new�X��
	pRATDir->vRATImageList.resize( iRATImgSize );
	
	// �إߵe�����
	for(int i = 0; i < iRATImgSize; ++i)
	{
		pLastByte += FillRATImageData( &(pRATDir->vRATImageList[i]), pLastByte, spRATFile );
		SetImageSizeID( &( pRATDir->vRATImageList[i] ), spRATFile );
	}
	return pLastByte - pBeginByte ;
}

//////////////////////////////////////////////////////////////////////////
//�p��Ϥ��i�ƩM�]�w�Ϥ�id
//////////////////////////////////////////////////////////////////////////
void CRATFactory::SetImageSizeID(SRATImageFile* pRATImage, SP_RATFile& spRATFile)
{
	//�Ĥ@���i�Ӭ�0�A�ҥH��lid��0
    pRATImage->nRATImageID = spRATFile->SRATImageList.size();
    spRATFile->SRATImageList.push_back( pRATImage );
}
//////////////////////////////////////////////////////////////////////////
//��rat image���
//////////////////////////////////////////////////////////////////////////
int CRATFactory::FillRATImageData(SRATImageFile* pRATImage, BYTE* pBeginByte, SP_RATFile& spRATFile)
{

	//�@��@��g�J
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

	//pData += 9;    // �w�d���
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
		pLastByte += 9; // �w�d���
	}
	
	return pLastByte - pBeginByte;


	//mem cpy �@�k
	//�ɮתŮ�
	//pLastByte += _US_SHORT_SIZEOF;
	//
	//int nReadSize = 0;
	//int nOffset = 0;

	////�Ϥ�����
	//if( spRATFile->RATFileHeader.usVersion > 1 )
	//{
	//	//����2���j�p
	//	nReadSize = _RAT_IMAGE_FILE_SIZEOF_V2;
	//	nOffset = 1;
	//}
	//else
	//{
	//	//����1���j�p
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