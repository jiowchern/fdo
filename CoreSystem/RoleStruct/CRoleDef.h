#ifndef ROLE_DEF_H
#define ROLE_DEF_H
#include <boost\shared_ptr.hpp>
//////////////////////////////////////////////////////////////////////////
// 將RAT檔結構、RPC檔結構、角色SKIN結構的
// 型別定義 typedef 全集中在這裡。
// 要使用的類別只要在h檔include這隻檔案就行，
// 只在cpp include實體。
// [10/31/2008: by a-wi]
//////////////////////////////////////////////////////////////////////////

namespace FDO
{

//////////////////////////////////////////////////////////////////////////
// 角色方向定義
//////////////////////////////////////////////////////////////////////////
enum RoleDir
{
	RID_South,
	RID_ESouth,
	RID_East,
	RID_ENorth,
	RID_North,
	RID_WNorth,
	RID_West,
	RID_WSouth,
	MAX_DIR
};

typedef struct SRoleSkinImageNew SRoleSkinImageNew;
//////////////////////////////////////////////////////////////////////////
//RAT檔的typedef前置宣告
//////////////////////////////////////////////////////////////////////////
typedef struct SRATFileRoot SRATFile;				//整個rat檔

typedef struct SRATFileHeader SRATFileHeader;		//rat檔頭	 

typedef struct SRATActFile SRATActFile;				//rat檔的動作
typedef vector<SRATActFile> vRATActFileList;		//rat檔的動作陣列

typedef struct SRATComFile SRATComFile;				//RAT檔的零件
typedef vector<SRATComFile> vRATComFileList;		//rat檔的零件陣列

typedef struct SRATDirFile SRATDirFile;				//RAT檔的方向
//typedef SRATDirFile[MAX_DIR] vRATDirArray;			//rat檔的方向陣列，因方向固定為8方，所以寫死!!

typedef struct SRATImageFile_v1 SRATImageFile_v1;
typedef struct SRATImageFile_v2 SRATImageFile_v2;		//RAT檔的圖片資料v2指版本

typedef struct SRATImageFile_v3 SRATImageFile;		

typedef vector<SRATImageFile> vRATImageFileList;		//RAT檔的圖片資料陣列



typedef boost::shared_ptr<SRATFile>	 SP_RATFile;	

//////////////////////////////////////////////////////////////////////////
//定義出rat檔裡 結構sizeof的大小
//////////////////////////////////////////////////////////////////////////
#define  _RAT_HEADER_FILE_SIZEOF 	sizeof(SRATFileHeader)
#define  _RAT_FILE_SIZEOF			sizeof(SRATFileRootEntry)
#define  _RAT_ACT_SIZSEOF			sizeof(SRATActFileEntry)	//rat檔動作檔的大小



#define  _RAT_IMAGE_FILE_SIZEOF_V2		sizeof(SRATImageFile_v2)
#define  _RAT_IMAGE_FILE_SIZEOF_V1		sizeof(SRATImageFile_v1)




//////////////////////////////////////////////////////////////////////////
//RPC檔的前置宣告
//////////////////////////////////////////////////////////////////////////
typedef struct SRPCFileRoot SRPCFile;				//整個rpc檔

typedef struct SRPCFileHeader SRPCFileHeader;		//rpc檔頭

typedef struct SRPCActFile SRPCActFile;				//RPC動作檔資料
typedef vector<SRPCActFile> vRPCActFileList;		//RPC動作檔資料陣列

typedef struct SRPCComFile SRPCComFile;				//RPC零件檔資料
typedef vector<SRPCComFile> vRPCComFileList;		//RPC零件檔資料陣列

typedef struct SRPCDirFile SRPCDirFile;				//rpc方向檔資料
//typedef SRPCDirFile[MAX_DIR] vRATDirArray;			//rpc檔的方向陣列，因方向固定為8方，所以寫死!!

typedef struct SRPCImageFile_v1 SRPCImageFile_v1;
typedef struct SRPCImageFile_v2 SRPCImageFile_v2;
typedef struct SRPCImageFile_v3 SRPCImageFile;

typedef vector<SRPCImageFile>	vRPCImgFileList;	

typedef vector<SRPCImageFile*>	vRPCImgPointList;	//RPC檔的圖片資料索引用陣列


typedef boost::shared_ptr<SRPCFile>  SP_RPCFile;

typedef list<SP_RPCFile>	SP_RPC_List;

//////////////////////////////////////////////////////////////////////////
//定義出RPC檔裡 結構sizeof的大小
//////////////////////////////////////////////////////////////////////////
#define _RPC_HEADER_FILE_SIZEOF 		sizeof(SRPCFileHeader)

#define _RPC_IMAGE_FILE_SIZEOF_V2		sizeof(SRPCImageFile_v2)

#define _RPC_IMAGE_FILE_SIZEOF_V1		sizeof(SRPCImageFile_v1)



//////////////////////////////////////////////////////////////////////////
//RoleSkinStruct的前置宣告
//////////////////////////////////////////////////////////////////////////
typedef struct SRoleSkin SRoleSkin;

typedef struct SRoleSkinAct SRoleSkinAct;
typedef vector<SRoleSkinAct> vRoleSkinActList;

typedef struct SRoleSkinCom SRoleSkinCom;
typedef vector<SRoleSkinCom> vRoleSkinComList;

typedef struct SRoleSkinDir SRoleSkinDir;
//typedef SRoleSkinDir    skinDirs[MAX_DIR];

typedef struct SRoleSkinImage SRoleSkinImage;
typedef vector<SRoleSkinImage> vSkinImgList;

typedef struct SRoleSkinTexture SRoleSkinTexture; 


//////////////////////////////////////////////////////////////////////////
// 檔案的class 前置宣告
//////////////////////////////////////////////////////////////////////////
typedef class CRATFactory CRATFactory;
typedef class CRPCFactory CRPCFactory;
typedef class CRoleSkinFactory CRoleSkinFactory;
typedef class CRoleRenderer CRoleRenderer;

//////////////////////////////////////////////////////////////////////////
//變數定義
//////////////////////////////////////////////////////////////////////////
#define  _SHORT_FILE_NAME_SIZE		20
#define  _LONG_FILE_NAME_SIZE		50
#define  _RAT_KEEP_SIZE				20		//保留欄位
#define  _DWORD_SIZEOF				sizeof( DWORD )
#define  _US_SHORT_SIZEOF			sizeof(unsigned short)
#define  _BOOL_SIZEOF				sizeof(bool)


//////////////////////////////////////////////////////////////////////////
//變數定義
//////////////////////////////////////////////////////////////////////////
const unsigned char LENOF_ID = 4;
const unsigned char LENOF_NAME = 20;
const unsigned char LENOF_FILENAME = 50;
const unsigned char LENOF_RESERVE_4 = 4;

const char LENGTHOF_FILENAME = 50;
const char LENGTHOF_NAME = 20;
const int sizeof_DWORD = sizeof( DWORD );

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

// const int sizeof_DirHeader = sizeof(sDirHeader);
// //const int sizeof_EmoHeader = sizeof(sEmoHeader);
// const int sizeof_Image = sizeof(sImage);
// const int sizeof_Frame = sizeof(sFrame);
// const int sizeof_ComponentDir = sizeof(sComponentDir);
// const int sizeof_RoleComponent = sizeof(sRoleComponent);
// const int sizeof_RoleAction = sizeof(sRoleAction);
// const int sizeof_nShort = sizeof(unsigned short);
// const int sizeof_short = sizeof(short);
// const int sizeof_float = sizeof(float);
// const int sizeof_D3DXVECTOR3 = sizeof(D3DXVECTOR3);
// const int sizeof_BOOL = sizeof(bool);
// const int sizeof_ComHeader = sizeof(sComHeader);
// const int sizeof_RoleHeader = sizeof(sRoleHeader);




}	// end of ROLE_DEF_H
#endif 