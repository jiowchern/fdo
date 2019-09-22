#ifndef _FDO_RPC_FILE_H_
#define _FDO_RPC_FILE_H_

#include "stdafx.h"
#include <d3dx9.h>
#include <vector>
#include <string>
#include "CRoleDef.h"
using std::vector;
using std::string;
//----------------------------------------------------------------------------
namespace FDO
{

#pragma pack( push )
#pragma pack( 1 )
//////////////////////////////////////////////////////////////////////////
// RPC�Ϥ���
//////////////////////////////////////////////////////////////////////////
struct SRPCImageFile_v1
{
	//bool  bNouse;
	float fWidth;
	float fHeight;
	float fTU1;
	float fTV1;
	float fTU2;
	float fTV2;

    // <yuming> close texture file name in order to save memory
	//char  szTexFile[_LONG_FILE_NAME_SIZE];
    // </yuming>
};

struct SRPCImageFile_v2 : public SRPCImageFile_v1
{	
	DWORD dwColor;
	//char  space[10];       
};

struct SRPCImageFile_v3 : public SRPCImageFile_v2
{
	int nRPCImgID;

    // <yuming> add texture name id
    unsigned int nTextureNameID;
    // </yuming>
};

//////////////////////////////////////////////////////////////////////////
//  RPC��V��
//////////////////////////////////////////////////////////////////////////
struct SRPCDirFile
{
	//eDir;                                 // �s���V
	vRPCImgFileList		vRPCImgList;
};

//////////////////////////////////////////////////////////////////////////
//  RPC�s����
//////////////////////////////////////////////////////////////////////////
struct SRPCComFile
{
	char            szName[_SHORT_FILE_NAME_SIZE];
	SRPCDirFile     RPCDir[MAX_DIR];
};

//////////////////////////////////////////////////////////////////////////
//  RPC�ʧ@��
//////////////////////////////////////////////////////////////////////////
struct SRPCActFile
{
	char            szName[_SHORT_FILE_NAME_SIZE];
	SRPCComFile     RPCCom;
};


//////////////////////////////////////////////////////////////////////////
//  RPC���Y
//////////////////////////////////////////////////////////////////////////
struct SRPCFileHeader
{
	char            szFileID[4];                                                // �ɮ׿��ID
	unsigned short  usVersion;                                                  // �ɮת���
	unsigned short  usActCount1;                                                // �ʧ@��1
	unsigned short  usActCount2;                                                // �ʧ@��2
	char            szComponentName1[20];                                       // �s��W��1
	char            szComponentName2[20];                                       // �s��W��2
	char            szReserved[10];                                             // �O�d���
};

//////////////////////////////////////////////////////////////////////////
//  RPC��
//////////////////////////////////////////////////////////////////////////
struct SRPCFileRootEntry
{
	SRPCFileHeader	RPCFileHeader;
	string 			strFilename; // ����RPC�ɦW
};

//////////////////////////////////////////////////////////////////////////
//rpc��
//////////////////////////////////////////////////////////////////////////
struct SRPCFileRoot : public SRPCFileRootEntry
{
	vRPCActFileList		vRPCActList1;		//�ʧ@�Ӽ�
	vRPCActFileList		vRPCActList2;
	
	vRPCImgPointList	vRPCImgPointList1;	//�Ϥ�id����
	vRPCImgPointList	vRPCImgPointList2;
};


#pragma pack( pop )


//----------------------------------------------------------------------------
} // end of namespace FDO
//----------------------------------------------------------------------------

#endif // _FDO_RPC_FILE_H_