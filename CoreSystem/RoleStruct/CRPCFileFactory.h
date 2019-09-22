#ifndef _RPCFACTORY_H
#define _RPCFACTORY_H

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "CRoleDef.h"

namespace FDO
{
	class CRoleRenderer ; 

	
	

class CRPCFactory
{
	//////////////////////////////////////////////////////////////////////////
	//對外唯一介面
	//////////////////////////////////////////////////////////////////////////
public:
	CRPCFactory();
	~CRPCFactory();
	bool GetRPCFile(const char* pFilename, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile);
	bool GetRPCFileThread(const char* pFilename, CRoleRenderer *pRoleRenderer , SP_RPCFile& spRPCFile, SP_RATFile& spRATFile , uint32 &nRoleID);
	
private:
	bool			StartCreateRPCFile(const char* pFilename, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile);

	BYTE*			LoadRPCPacketFile( const char* pFilename );
	int 			SetRPCActFileName(SRPCActFile& rRPCAct, BYTE* pData);
	SRATActFile* 	FindRATAct(const char* pActName, SP_RATFile& spRATFile);
	SRATComFile* 	FindRATCom(const char* pComName, SRATActFile* pRATAct);
	int				CreateRPCDirData( SRPCComFile& rRPCCom, SRATComFile& SRATCom, BYTE* pBeginByte, SP_RPCFile& spRPCFile );
	int 			GetDirImageListCount(vRPCImgPointList& vRPCImageList, BYTE* pData);
	int				CreateRPCImageData( vRPCImgFileList& RPCImgList, vRATImageFileList& RATImgList, BYTE* pBeginByte, SP_RPCFile& spRPCFile );
	int 			FillRPCImageData(SRPCImageFile& pRPCImage, BYTE* pData, SP_RPCFile& spRPCFile);
	void			SetRPCImgID( SRPCImageFile& rRPCImage, SRATImageFile& rRATImage, SP_RPCFile& spRPCFile );
public:
	int 			LoadRPCHeaderFile(BYTE* pData, SP_RPCFile& spRPCFile);
	void 			SetRPCActSize(SP_RPCFile& spRPCFile);
	int 			CreateRPCFileAllData(vRPCActFileList& vRPCActList, const char* pComName, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile, BYTE* pData );
	int 			CreateRPCFileAllDataThread(vRPCActFileList& vRPCActList, const char* pComName, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile, BYTE* pData );
// <yuming>
private:
    typedef std::vector<std::string> StringArray;
    StringArray mTextureStringList;
    unsigned int GetStringID(const char TextureName[]);
public:
    const char* GetTextureStringAt(unsigned int index);
// </yuming>
};

}

#endif