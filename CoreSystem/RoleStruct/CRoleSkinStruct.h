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
//�e�m�ŧi
//////////////////////////////////////////////////////////////////////////
namespace FDO
{

#pragma pack( push )
#pragma pack( 1 )
//////////////////////////////////////////////////////////////////////////
//�B�⧹�᪺�Ϥ���� �N�O����
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
// 	char                szTexFile[_LONG_FILE_NAME_SIZE];          // �e�������ɦW
// 	LPDIRECT3DTEXTURE9  pTexture;               // �e������
// 
// 	SRoleSkinTexture();
// 	~SRoleSkinTexture(); 
// 
// 	void Clear();
// };

 
//////////////////////////////////////////////////////////////////////////
// ����skin�Ϲ����
//////////////////////////////////////////////////////////////////////////


typedef struct SRoleSkinImageNew
{
	SRATImageFile*		pRATImg;		//���[
	SRPCImageFile*		pRPCImg;		//�ֽ�
	
    //SMatrix*            pMatrix;        //�s�����[�ֽ���X���K�Ϧ�m�x�}
    //D3DXMATRIX		 	vMatLocal;	    //�s�����[�ֽ���X���K�Ϧ�m�x�}
	sImage*	            pSkinTexture;   //�n�s������J���ɮ�
//	bool				bMatOver;
	//void* operator new(size_t size);
	//////////////////////////////////////////////////////////////////////////
	//void operator delete(void* p);
    SRoleSkinImageNew();
	SRoleSkinImageNew( SRATImageFile* pRATImage );
    bool Lerp(float slope, const SRoleSkinImageNew* rhs);
} RoleSkinImageNew, *RoleSkinImageNewPtr;



//////////////////////////////////////////////////////////////////////////
// ����skin�Ϲ����
//////////////////////////////////////////////////////////////////////////
struct SRoleSkinImage
{
	SRoleSkinImage();
	~SRoleSkinImage();

	SRATImageFile*		pRATImg;		//���[
	SRPCImageFile*		pRPCImg;		//�ֽ�

	D3DXMATRIX      	matLocal;		//�s�����[�ֽ���X���K�Ϧ�m�x�}
	
	sImage*	pSkinTexture;	//�n�s������J���ɮ�

	//boost::shared_ptr<SRoleSkinTexture> spSkinTexture; // share pointer
};

typedef vector<SRoleSkinImage> vRoleSkinImageList;
//////////////////////////////////////////////////////////////////////////
//����skin��V���
//////////////////////////////////////////////////////////////////////////
struct SRoleSkinDir
{
	SRoleSkinImage*		pRoleSkinImg;
	int					nSkinImgSize;

	vector<SRoleSkinImage> vRoleImgList;
};

//////////////////////////////////////////////////////////////////////////
//����skin�s����
//////////////////////////////////////////////////////////////////////////
struct SRoleSkinCom
{
	char            szComName[_SHORT_FILE_NAME_SIZE];
	bool            bVisible;
	SRoleSkinDir    skinDirs[MAX_DIR];

	bool operator==(const char* sz) { return !strcmp(szComName,sz); }
};



//////////////////////////////////////////////////////////////////////////
//����skin�ʧ@���
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
//����skin�}�Y
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