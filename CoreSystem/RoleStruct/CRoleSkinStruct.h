#ifndef ROLE_SKIN_STRUCT_H
#define ROLE_SKIN_STRUCT_H
//////////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <d3dx9.h>
#include "CRoleDef.h"
#include "FDO_RoleMemoryMgr.h"



//////////////////////////////////////////////////////////////////////////
//前置宣告
//////////////////////////////////////////////////////////////////////////
namespace FDO
{

#pragma pack( push )
#pragma pack( 1 )
//////////////////////////////////////////////////////////////////////////
//運算完後的圖片資料 就是材質
//////////////////////////////////////////////////////////////////////////
// struct SRoleSkinTexture
// {
// 	SRoleSkinTexture(const char* pFilename);
// 	~SRoleSkinTexture();
// 
// 	char				szFilename[_MAX_PATH];
// 	LPDIRECT3DTEXTURE9	pTexture;
// 
// 	bool operator==(const char* sz) { return !strcmp(szFilename,sz); }
// };

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// struct SRoleSkinTexture
// {
// 	char                szTexFile[_LONG_FILE_NAME_SIZE];          // 畫面材質檔名
// 	LPDIRECT3DTEXTURE9  pTexture;               // 畫面材質
// 
// 	SRoleSkinTexture();
// 	~SRoleSkinTexture(); 
// 
// 	void Clear();
// };

 
//////////////////////////////////////////////////////////////////////////
// 角色skin圖像資料
//////////////////////////////////////////////////////////////////////////


typedef struct SRoleSkinImageNew
{
	SRATImageFile*		pRATImg;		//骨架
	SRPCImageFile*		pRPCImg;		//皮膚
	
    //SMatrix*            pMatrix;        //連結骨架皮膚算出的貼圖位置矩陣
    //D3DXMATRIX		 	vMatLocal;	    //連結骨架皮膚算出的貼圖位置矩陣
	sImage*	            pSkinTexture;   //要連結到載入的檔案
//	bool				bMatOver;
	//void* operator new(size_t size);
	//////////////////////////////////////////////////////////////////////////
	//void operator delete(void* p);
    SRoleSkinImageNew();
	SRoleSkinImageNew( SRATImageFile* pRATImage );
    bool Lerp(float slope, const SRoleSkinImageNew* rhs);
} RoleSkinImageNew, *RoleSkinImageNewPtr;



//////////////////////////////////////////////////////////////////////////
// 角色skin圖像資料
//////////////////////////////////////////////////////////////////////////
struct SRoleSkinImage
{
	SRoleSkinImage();
	~SRoleSkinImage();

	SRATImageFile*		pRATImg;		//骨架
	SRPCImageFile*		pRPCImg;		//皮膚

	D3DXMATRIX      	matLocal;		//連結骨架皮膚算出的貼圖位置矩陣
	
	sImage*	pSkinTexture;	//要連結到載入的檔案

	//boost::shared_ptr<SRoleSkinTexture> spSkinTexture; // share pointer
};

typedef vector<SRoleSkinImage> vRoleSkinImageList;
//////////////////////////////////////////////////////////////////////////
//角色skin方向資料
//////////////////////////////////////////////////////////////////////////
struct SRoleSkinDir
{
	SRoleSkinImage*		pRoleSkinImg;
	int					nSkinImgSize;

	vector<SRoleSkinImage> vRoleImgList;
};

//////////////////////////////////////////////////////////////////////////
//角色skin零件資料
//////////////////////////////////////////////////////////////////////////
struct SRoleSkinCom
{
	char            szComName[_SHORT_FILE_NAME_SIZE];
	bool            bVisible;
	SRoleSkinDir    skinDirs[MAX_DIR];

	bool operator==(const char* sz) { return !strcmp(szComName,sz); }
};



//////////////////////////////////////////////////////////////////////////
//角色skin動作資料
//////////////////////////////////////////////////////////////////////////
struct SRoleSkinAct
{
	char				szName[_SHORT_FILE_NAME_SIZE];
	
	SRoleSkinCom*		pRoleSkinCom;
	int					nSkinComSize;
	
	vRoleSkinComList	vSkinComList;

	bool operator==(const char* sz) { return !strcmp(szName,sz); }
};



//////////////////////////////////////////////////////////////////////////
//角色skin開頭
//////////////////////////////////////////////////////////////////////////
struct SRoleSkin
{
	SRoleSkinAct*	pRoleSkinAct;
	int				nSkinActSize;

	vRoleSkinActList vSkinActList;
	
};

#pragma pack( pop )


}	//end of FDO
#endif