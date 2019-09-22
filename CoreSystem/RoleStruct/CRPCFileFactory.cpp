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
//���orpc�� ��~�ߤ@������
//////////////////////////////////////////////////////////////////////////
bool CRPCFactory::GetRPCFile(const char* pFilename, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile)
{
	if( pFilename == NULL)
	{
		return false;
	}

	bool bState = true;

	//�S��쪺�ܴN�إ�
	if( !GetRPCFileManage()->Get( pFilename, spRPCFile ) )
	{
		//�S���Anew
		spRPCFile.reset( new SRPCFile, CReleaseRoleRes() ); 
		bState = StartCreateRPCFile( pFilename, spRPCFile, spRATFile );
		
		GetRPCFileManage()->Get( pFilename, spRPCFile );
	}
	//����
	return bState;
}

bool CRPCFactory::GetRPCFileThread(const char* pFilename, CRoleRenderer *pRoleRenderer,  SP_RPCFile& spRPCFile, 
								   SP_RATFile& spRATFile , uint32 &nRoleID)
{
	if( pFilename == NULL)
	{
		return false;
	}


	//�S��쪺�ܴN�إ�
	if( !GetRPCFileManage()->Get( pFilename, spRPCFile ) )
	{
		
		//�S���Anew
		spRPCFile.reset( new SRPCFile, CReleaseRoleRes() ); 

		//�]�wRPC�ɦW
		spRPCFile->strFilename = pFilename;


		g_RoleMemoryMgr.PushRPCStream(pFilename , pRoleRenderer , this , spRPCFile , spRATFile , nRoleID) ; 


		//BYTE* pData;
		//pData = LoadRPCPacketFile( pFilename );

		//if( pData == NULL )
		//{
		//	return false;
		//}

		////�]�wRPC�ɦW
		//spRPCFile->strFilename = pFilename;

		//g_RoleMemoryMgr.PushUnLoadingRPC(pRoleRenderer , this , spRPCFile , spRATFile , pData , nRoleID , pFilename) ; 

		//�ä��ݭnthread���ܡA�N���ݭn�]�wRoleID
		return true ; 
	}

	//�ä��ݭnthread���ܡA�N���ݭn�]�wRoleID
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

	//�]�wRPC�ɦW
	spRPCFile->strFilename = pFilename;

	// Ū��RPC���Y��
	pData += LoadRPCHeaderFile( pData, spRPCFile );

	//�]�w���Y
//	SetRPCHeaderFileData();

	//RESIZE
	SetRPCActSize(spRPCFile);

	//�إߥXrpc�Ҧ����
	char* pComName1 = spRPCFile->RPCFileHeader.szComponentName1;
	char* pComName2 = spRPCFile->RPCFileHeader.szComponentName2;
	pData += CreateRPCFileAllData( spRPCFile->vRPCActList1, pComName1, spRPCFile, spRATFile, pData );
	pData += CreateRPCFileAllData( spRPCFile->vRPCActList2, pComName2, spRPCFile, spRATFile, pData );

	// TODO: �إߧ����A��J�O������޲z
	GetRPCFileManage()->Insert( pFilename , spRPCFile ) ;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//Ū��RPC�ʸ���
///////////	///////////////////////////////////////////////////////////////
BYTE* CRPCFactory::LoadRPCPacketFile( const char* pFilename )
{
	memPack* pMem = LoadFromPacket( pFilename );
	return pMem->Data; 
}

//////////////////////////////////////////////////////////////////////////
//Ū��RPC���Y
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::LoadRPCHeaderFile(BYTE* pData, SP_RPCFile& spRPCFile)
{
	memset( &(spRPCFile->RPCFileHeader), 0, _RPC_HEADER_FILE_SIZEOF );
	memcpy( &(spRPCFile->RPCFileHeader), pData, _RPC_HEADER_FILE_SIZEOF );
	return _RPC_HEADER_FILE_SIZEOF;
}

//////////////////////////////////////////////////////////////////////////
//�]�wrpc�ɰʧ@�ƶq
//////////////////////////////////////////////////////////////////////////
void CRPCFactory::SetRPCActSize(SP_RPCFile& spRPCFile)
{
	//resize
	spRPCFile->vRPCActList1.resize( spRPCFile->RPCFileHeader.usActCount1 );
	spRPCFile->vRPCActList2.resize( spRPCFile->RPCFileHeader.usActCount2 );
}

//////////////////////////////////////////////////////////////////////////
//�إ�RPC�ɸ��
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::CreateRPCFileAllData( vRPCActFileList& vRPCActList, const char* pComName, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile, BYTE* pBeginByte )
{
	BYTE* pLastByte = pBeginByte;

	for( size_t i(0); i < vRPCActList.size(); ++i )
	{	
		try
		{
			//���XRPC�ʧ@
			SRPCActFile& rRPCAct = vRPCActList.at(i);
			//�]�w�ʧ@�W��
			pLastByte += SetRPCActFileName( rRPCAct, pLastByte );
			//�S���ۦP�ʧ@�N�~��
			SRATActFile* pRATAct = FindRATAct( rRPCAct.szName, spRATFile );
			if(pRATAct == NULL)
			{
				continue;
			}

			//���oRPC�s��
			SRPCComFile& rRPCCom = rRPCAct.RPCCom;
			//�]�wRPC�s��W��
			strncpy( rRPCCom.szName, pComName, _SHORT_FILE_NAME_SIZE - 1 );
			//�S���ۦP�s��N�~��
			SRATComFile* pRATCom = FindRATCom( pComName, pRATAct );
			if(pRATCom == NULL)
			{
				continue;
			}

			//�إߤ�V���
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
			//���XRPC�ʧ@
			SRPCActFile& rRPCAct = vRPCActList.at(i);
			//�]�w�ʧ@�W��
			pLastByte += SetRPCActFileName( rRPCAct, pLastByte );
			//�S���ۦP�ʧ@�N�~��
			SRATActFile* pRATAct = FindRATAct( rRPCAct.szName, spRATFile );
			if(pRATAct == NULL)
			{
				continue;
			}

			//���oRPC�s��
			SRPCComFile& rRPCCom = rRPCAct.RPCCom;
			//�]�wRPC�s��W��
			strncpy( rRPCCom.szName, pComName, _SHORT_FILE_NAME_SIZE - 1 );
			//�S���ۦP�s��N�~��
			SRATComFile* pRATCom = FindRATCom( pComName, pRATAct );
			if(pRATCom == NULL)
			{
				continue;
			}

			//�إߤ�V���
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
//�]�w�ʧ@�W��
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::SetRPCActFileName(SRPCActFile& rRPCAct, BYTE* pData)
{
	strncpy( rRPCAct.szName, (char*)pData, _SHORT_FILE_NAME_SIZE - 1 );
	return _SHORT_FILE_NAME_SIZE;
}

//////////////////////////////////////////////////////////////////////////
//���rat�� �ʧ@�W��
//////////////////////////////////////////////////////////////////////////
SRATActFile* CRPCFactory::FindRATAct(const char* pActName, SP_RATFile& spRATFile)
{

	//�j�Mrat�ɰʧ@
	vRATActFileList::iterator actPos(
		std::find( spRATFile->vRATActList.begin(), spRATFile->vRATActList.end(), pActName ) );
	if( actPos == spRATFile->vRATActList.end() )
	{
		//�S���W�٬ۦP���ʧ@
		return NULL;
	}
	return &(*actPos);
}

//////////////////////////////////////////////////////////////////////////
//���rat�ʧ@�� �s��W��
//////////////////////////////////////////////////////////////////////////
SRATComFile* CRPCFactory::FindRATCom(const char* pComName, SRATActFile* pRATAct)
{


	//�j�Mrat�ɹs��
	vRATComFileList::iterator comPos (
		std::find( pRATAct->vRATComList.begin(), pRATAct->vRATComList.end(), pComName ) );

	if( comPos == pRATAct->vRATComList.end() )
	{
		//�S���W�٬ۦP���s��
		return NULL;
	}
	return &(*comPos);
}

//////////////////////////////////////////////////////////////////////////
//�إߤ�V���
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::CreateRPCDirData( SRPCComFile& rRPCCom, SRATComFile& SRATCom, BYTE* pBeginByte, SP_RPCFile& spRPCFile )
{
	BYTE* pLastByte = pBeginByte;
	
	for( int i = 0; i < MAX_DIR; ++i )
	{
		//���X�ثeRPC���ޤ�V
		SRPCDirFile& rNowRPCDir = rRPCCom.RPCDir[i];
		//
		SRATDirFile& rNowRATDir = SRATCom.RATDir[i];

		//���]�C�Ӥ�V�̪��Ϥ��ƶq
		rNowRPCDir.vRPCImgList.resize( *(unsigned short*)pLastByte );
		//�W�[�첾�q			
		pLastByte += _US_SHORT_SIZEOF;
		//�إ߹Ϥ����
		
		pLastByte += CreateRPCImageData( rNowRPCDir.vRPCImgList, rNowRATDir.vRATImageList, pLastByte, spRPCFile );
		
		
	}
	return pLastByte - pBeginByte;
}

//////////////////////////////////////////////////////////////////////////
//�إ߹Ϥ����
//////////////////////////////////////////////////////////////////////////

int CRPCFactory::CreateRPCImageData( vRPCImgFileList& RPCImgList, vRATImageFileList& RATImgList, BYTE* pBeginByte, SP_RPCFile& spRPCFile )
{
	//rat��rpc�Ϥ��ƶq���P�A����ɮצ����D
	
	
	size_t count = RPCImgList.size() ; 
	

	if(RPCImgList.size() != RATImgList.size())
	{
		std::string description;	
		StringFormat8(description , "rat��rpc�Ϥ��ƶq���P�A����ɮצ����D \nRPC Image size %d, RAT Image size %d" , RPCImgList.size() , RATImgList.size());		
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
		//���Xrpc��img
		SRPCImageFile& rRPCImg = RPCImgList.at(i);
		//���Xrat��img
		SRATImageFile& rRATImage = RATImgList.at(i);
		
		pLastByte += FillRPCImageData( rRPCImg, pLastByte, spRPCFile );
		//�]�wrpc��id
		SetRPCImgID( rRPCImg, rRATImage, spRPCFile );
	}	
		
	return pLastByte - pBeginByte;
}

//////////////////////////////////////////////////////////////////////////
//�]�wrpc img id
//////////////////////////////////////////////////////////////////////////
void CRPCFactory::SetRPCImgID( SRPCImageFile& rRPCImg, SRATImageFile& rRATImg, SP_RPCFile& spRPCFile )
{
	rRPCImg.nRPCImgID = rRATImg.nRATImageID;
	spRPCFile->vRPCImgPointList1.push_back( &rRPCImg );
}

//////////////////////////////////////////////////////////////////////////
//��rpc���ɪ����
//////////////////////////////////////////////////////////////////////////
int CRPCFactory::FillRPCImageData(SRPCImageFile& pRPCImage, BYTE* pBeginByte, SP_RPCFile& spRPCFile )
{
	//BYTE* pLastByte = pBeginByte;
	////�ɮתŮ�
	//pLastByte += _BOOL_SIZEOF;
	//
	//int nReadSize = 0;
	//int nOffset = 0;

	//if( spRPCFile->RPCFileHeader.usVersion > 1 )
	//{
	//	nReadSize = _RPC_IMAGE_FILE_SIZEOF_NEW;
	//	nOffset = 6;	//�O�d���
	//}
	//else
	//{
	//	nReadSize = _RPC_IMAGE_FILE_SIZEOF_OLD;
	//	nOffset = 10;	//�ª��O�d���
	//	pRPCImage.dwColor = 0xFFFFFFFF; // �զ�
	//}

	////���M��
	//memset( &pRPCImage, 0, nReadSize );
	//memcpy( &pRPCImage, pLastByte, nReadSize  );

	//pLastByte += nReadSize;
	//pLastByte += nOffset;

	//return pLastByte - pBeginByte;


	
//�ª�
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

	//�����
	if( spRPCFile->RPCFileHeader.usVersion > 1 )
	{
		pRPCImage.dwColor = *(DWORD*)pLastByte;
		pLastByte += sizeof_DWORD;
		pLastByte += 6;
	}
	else
	{
		pRPCImage.dwColor = 0xFFFFFFFF; // �զ�
		pLastByte += 10;    // �w�d���
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