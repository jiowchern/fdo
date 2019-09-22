
#ifndef _RATFILEFACTORY_H
#define _RATFILEFACTORY_H

#include "CRoleDef.h"

namespace FDO
{


class CRATFactory
{
	//////////////////////////////////////////////////////////////////////
	//對外使用的api
	//////////////////////////////////////////////////////////////////////
public:
	CRATFactory();
	~CRATFactory();
	bool GetRATFile(const char* pFilename, SP_RATFile& spRATFile );


	//////////////////////////////////////////////////////////////////////
	//建立的部份
	//////////////////////////////////////////////////////////////////////
private:
	bool	CreateRATFile(const char* pFilename, SP_RATFile spRATFile);

	BYTE*	LoadRATPacketFile(const char* pFilename);
	int 	LoadRATHeaderFile(BYTE* pData, SP_RATFile& spRATFile);
	void 	SetRATFileRootData(const char* pFilename, SP_RATFile& spRATFile);
	
	int 	StartCreateRATAllData(BYTE* pData, SP_RATFile& spRATFile);
	int		FillRATActFileData(SRATActFile* pACTFile, BYTE* pData);
	int		CreateRATComData(SRATActFile* pRATAct, BYTE* pData, SP_RATFile& spRATFile);
	int		SetRATComName(SRATComFile* pRATCom, BYTE* pData);
	int		CreateRATDirData( SRATComFile* pRATCom, BYTE* pData, SP_RATFile& spRATFile);
	int		SetDirMaxImageCount(SRATDirFile* pRATDir, BYTE* pData );
	int		CreateRATImageData(SRATDirFile* pRATDir, BYTE* pData, SP_RATFile& spRATFile);
	
	void	SetImageSizeID(SRATImageFile* pRATImage, SP_RATFile& spRATFile);
	int		FillRATImageData(SRATImageFile* pRATImage, BYTE* pData, SP_RATFile& spRATFile);
	void	CreateSkinImgArray(SP_RATFile& spRATFile, vector<SRoleSkinImageNew*>& vRoleInstance);

};

}

#endif
