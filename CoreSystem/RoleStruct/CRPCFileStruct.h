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
// RPC圖片檔
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
//  RPC方向檔
//////////////////////////////////////////////////////////////////////////
struct SRPCDirFile
{
	//eDir;                                 // 零件方向
	vRPCImgFileList		vRPCImgList;
};

//////////////////////////////////////////////////////////////////////////
//  RPC零件檔
//////////////////////////////////////////////////////////////////////////
struct SRPCComFile
{
	char            szName[_SHORT_FILE_NAME_SIZE];
	SRPCDirFile     RPCDir[MAX_DIR];
};

//////////////////////////////////////////////////////////////////////////
//  RPC動作檔
//////////////////////////////////////////////////////////////////////////
struct SRPCActFile
{
	char            szName[_SHORT_FILE_NAME_SIZE];
	SRPCComFile     RPCCom;
};


//////////////////////////////////////////////////////////////////////////
//  RPC檔頭
//////////////////////////////////////////////////////////////////////////
struct SRPCFileHeader
{
	char            szFileID[4];                                                // 檔案辨視ID
	unsigned short  usVersion;                                                  // 檔案版本
	unsigned short  usActCount1;                                                // 動作數1
	unsigned short  usActCount2;                                                // 動作數2
	char            szComponentName1[20];                                       // 零件名稱1
	char            szComponentName2[20];                                       // 零件名稱2
	char            szReserved[10];                                             // 保留欄位
};

//////////////////////////////////////////////////////////////////////////
//  RPC檔
//////////////////////////////////////////////////////////////////////////
struct SRPCFileRootEntry
{
	SRPCFileHeader	RPCFileHeader;
	string 			strFilename; // 紀錄RPC檔名
};

//////////////////////////////////////////////////////////////////////////
//rpc檔
//////////////////////////////////////////////////////////////////////////
struct SRPCFileRoot : public SRPCFileRootEntry
{
	vRPCActFileList		vRPCActList1;		//動作個數
	vRPCActFileList		vRPCActList2;
	
	vRPCImgPointList	vRPCImgPointList1;	//圖片id索引
	vRPCImgPointList	vRPCImgPointList2;
};


#pragma pack( pop )


//----------------------------------------------------------------------------
} // end of namespace FDO
//----------------------------------------------------------------------------

#endif // _FDO_RPC_FILE_H_