#ifndef _FDO_RAT_FILE_STRUCT_H_
#define _FDO_RAT_FILE_STRUCT_H_

//////////////////////////////////////////////////////////////////////////
// 這裡的結構是RAT檔底層結構。
// 為了版本控制的問題，所有的typedef都寫在 RoleDef.h 裡
// 請參考RoleDef.h。
// [10/31/2008/by a-wi]
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <d3dx9.h>
#include <vector>
#include <string>
#include "CRoleDef.h"
#include "ElementDef.h"
using std::vector;
using std::string;
//----------------------------------------------------------------------------
namespace FDO
{

#pragma pack( push )
#pragma pack( 1 )
//////////////////////////////////////////////////////////////////////////
// RAT檔的圖片結構
// 一個rat檔的圖片結構包含了以下資訊
// 版本1
//////////////////////////////////////////////////////////////////////////
struct SRATImageFile_v1
{
	unsigned short  usPlayTime; // 畫面播放時間
	short           shLayer;    // 畫面圖層	
	D3DXVECTOR3     vScale;     // 
	D3DXVECTOR3     vPosition;  // 畫面位置座標
	bool            bTexInvert; // 材質反向判斷子
};
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//typedef ObjFactory<SRATImageFile_v2,1000> SRATImageFileFactory;
//////////////////////////////////////////////////////////////////////////
//因應版本問題的作法 版本2
//////////////////////////////////////////////////////////////////////////
struct SRATImageFile_v2 : public SRATImageFile_v1
{
	float           fPercentX;  // X位移百分比
	float           fPercentY;  // Y位移百分比
	
};

//////////////////////////////////////////////////////////////////////////
// 遊戲中使用，存放圖片id
//////////////////////////////////////////////////////////////////////////
struct SRATImageFile_v3 : public SRATImageFile_v2
{
	int				nRATImageID;
    int             LinearFlag;
};

//////////////////////////////////////////////////////////////////////////
// RAT檔方向結構
// 一個rat檔的方向包含了n張圖片資訊
//////////////////////////////////////////////////////////////////////////
struct SRATDirFileEntry
{
	RoleDirect      	eDir;                               // 零件方向(無使用)
	unsigned short  	usMaxFrameNum;                      // 畫面最大張數
};

//////////////////////////////////////////////////////////////////////////
//	取用rat的方向檔
//////////////////////////////////////////////////////////////////////////
struct SRATDirFile : public SRATDirFileEntry
{
	vRATImageFileList 		vRATImageList;	//存放圖片張數
};

//////////////////////////////////////////////////////////////////////////
// RAT檔零件結構
// 一個rat檔的零件包含8個方向結構
//////////////////////////////////////////////////////////////////////////
struct SRATComFile
{
	char            szComName[_SHORT_FILE_NAME_SIZE];
	SRATDirFile		RATDir[MAX_DIR];					//方向只有8個所以寫死

	bool operator == (const char* p) const { return (strcmp(szComName,p)==0); }
};


//////////////////////////////////////////////////////////////////////////
// rat檔動作結構
// 一個rat檔的動作包含資訊、n個零件
//////////////////////////////////////////////////////////////////////////
struct SRATActFileEntry
{
	char            szName[_SHORT_FILE_NAME_SIZE];      	// 角色動作名稱
	bool            bPlayLoop;                          	// 迴圈播放判斷子
	DWORD           dwUpdateTime;                       	// 更新間隔時間
	BYTE            bySoundKind;                        	// 音效種類
	unsigned short  usHitTime1;                         	// 擊中點時間1
	unsigned short  usHitTime2;                         	// 擊中點時間2
	unsigned short  usPlaySoundTime1;                   	// 播放音效時間1
	unsigned short  usPlaySoundTime2;                   	// 播放音效時間2
	unsigned short  usPlaySoundTime3;                   	// 播放音效時間3
	unsigned short  usPlayParticleTime;                 	// 播放分子時間
	char            szSoundFile1[_LONG_FILE_NAME_SIZE];    	// 音效檔名1  //音效 ***
	char            szSoundFile2[_LONG_FILE_NAME_SIZE];    	// 音效檔名2  //音效 ***
	char            szSoundFile3[_LONG_FILE_NAME_SIZE];    	// 音效檔名3  //音效 ***
	char            szParticleFile[_LONG_FILE_NAME_SIZE];  	// 分子檔名
};

//////////////////////////////////////////////////////////////////////////
// rat檔動作取用 遊戲用
//////////////////////////////////////////////////////////////////////////
struct SRATActFile : public SRATActFileEntry
{
	vRATComFileList vRATComList;						// 存放RAT檔零件個數	
	bool operator == (const char* p) const { return ( strcmp( szName, p ) == 0 ); }
};

//////////////////////////////////////////////////////////////////////////
// rat檔頭結構
//////////////////////////////////////////////////////////////////////////
struct SRATFileHeader
{
	char            szFileID[4];						// 檔案辨視ID
	unsigned short  usVersion;          				// 檔案版本
	char            szRoleName[_SHORT_FILE_NAME_SIZE];  // 角色名稱
	float           fScaleX;            				// 角色縮放X
	float           fScaleY;            				// 角色縮放Y
	unsigned short  usShadowSize;       				// 影子大小
	unsigned short  usActCount;         				// 動作數
	unsigned short  usShadowOffset;     				// 影子偏移值
	unsigned short  usBoundingBoxSize;  				// 邊界盒大小
	unsigned short  usDeadBoundBoxSize; 				// 死亡邊界盒大小
	char            szReserved[4];      				// 保留欄位
};

//////////////////////////////////////////////////////////////////////////
// RAT檔的開頭
// 一個rat檔 包含檔頭資訊、n個動作
//////////////////////////////////////////////////////////////////////////
struct SRATFileRootEntry
{
	SRATFileHeader	RATFileHeader;		//檔頭
	string          strFilename;		//檔案名稱
	float           fScaleX;
	float           fScaleY;
	float           fScaleZ;
	float           fShadowSize;
	float           fShadowOffset;
	unsigned short  usBoundingBoxSize;
	unsigned short  usDeadBoundBoxSize;
};

//////////////////////////////////////////////////////////////////////////
//	RAT檔的取用
//////////////////////////////////////////////////////////////////////////
struct SRATFileRoot : public SRATFileRootEntry
{
	vRATActFileList	vRATActList;			//存放動作個數
    vector<SRATImageFile*> SRATImageList;	//圖片id索引用
};

#pragma pack( pop )


	//----------------------------------------------------------------------------
} // end of namespace FDO
//----------------------------------------------------------------------------

#endif // _FDO_RAT_FILE_H_