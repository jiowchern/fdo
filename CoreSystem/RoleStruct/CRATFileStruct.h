#ifndef _FDO_RAT_FILE_STRUCT_H_
#define _FDO_RAT_FILE_STRUCT_H_

//////////////////////////////////////////////////////////////////////////
// �o�̪����c�ORAT�ɩ��h���c�C
// ���F����������D�A�Ҧ���typedef���g�b RoleDef.h ��
// �аѦ�RoleDef.h�C
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
// RAT�ɪ��Ϥ����c
// �@��rat�ɪ��Ϥ����c�]�t�F�H�U��T
// ����1
//////////////////////////////////////////////////////////////////////////
struct SRATImageFile_v1
{
	unsigned short  usPlayTime; // �e������ɶ�
	short           shLayer;    // �e���ϼh	
	D3DXVECTOR3     vScale;     // 
	D3DXVECTOR3     vPosition;  // �e����m�y��
	bool            bTexInvert; // ����ϦV�P�_�l
};
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//typedef ObjFactory<SRATImageFile_v2,1000> SRATImageFileFactory;
//////////////////////////////////////////////////////////////////////////
//�]���������D���@�k ����2
//////////////////////////////////////////////////////////////////////////
struct SRATImageFile_v2 : public SRATImageFile_v1
{
	float           fPercentX;  // X�첾�ʤ���
	float           fPercentY;  // Y�첾�ʤ���
	
};

//////////////////////////////////////////////////////////////////////////
// �C�����ϥΡA�s��Ϥ�id
//////////////////////////////////////////////////////////////////////////
struct SRATImageFile_v3 : public SRATImageFile_v2
{
	int				nRATImageID;
    int             LinearFlag;
};

//////////////////////////////////////////////////////////////////////////
// RAT�ɤ�V���c
// �@��rat�ɪ���V�]�t�Fn�i�Ϥ���T
//////////////////////////////////////////////////////////////////////////
struct SRATDirFileEntry
{
	RoleDirect      	eDir;                               // �s���V(�L�ϥ�)
	unsigned short  	usMaxFrameNum;                      // �e���̤j�i��
};

//////////////////////////////////////////////////////////////////////////
//	����rat����V��
//////////////////////////////////////////////////////////////////////////
struct SRATDirFile : public SRATDirFileEntry
{
	vRATImageFileList 		vRATImageList;	//�s��Ϥ��i��
};

//////////////////////////////////////////////////////////////////////////
// RAT�ɹs�󵲺c
// �@��rat�ɪ��s��]�t8�Ӥ�V���c
//////////////////////////////////////////////////////////////////////////
struct SRATComFile
{
	char            szComName[_SHORT_FILE_NAME_SIZE];
	SRATDirFile		RATDir[MAX_DIR];					//��V�u��8�өҥH�g��

	bool operator == (const char* p) const { return (strcmp(szComName,p)==0); }
};


//////////////////////////////////////////////////////////////////////////
// rat�ɰʧ@���c
// �@��rat�ɪ��ʧ@�]�t��T�Bn�ӹs��
//////////////////////////////////////////////////////////////////////////
struct SRATActFileEntry
{
	char            szName[_SHORT_FILE_NAME_SIZE];      	// ����ʧ@�W��
	bool            bPlayLoop;                          	// �j�鼽��P�_�l
	DWORD           dwUpdateTime;                       	// ��s���j�ɶ�
	BYTE            bySoundKind;                        	// ���ĺ���
	unsigned short  usHitTime1;                         	// �����I�ɶ�1
	unsigned short  usHitTime2;                         	// �����I�ɶ�2
	unsigned short  usPlaySoundTime1;                   	// ���񭵮Įɶ�1
	unsigned short  usPlaySoundTime2;                   	// ���񭵮Įɶ�2
	unsigned short  usPlaySoundTime3;                   	// ���񭵮Įɶ�3
	unsigned short  usPlayParticleTime;                 	// ������l�ɶ�
	char            szSoundFile1[_LONG_FILE_NAME_SIZE];    	// �����ɦW1  //���� ***
	char            szSoundFile2[_LONG_FILE_NAME_SIZE];    	// �����ɦW2  //���� ***
	char            szSoundFile3[_LONG_FILE_NAME_SIZE];    	// �����ɦW3  //���� ***
	char            szParticleFile[_LONG_FILE_NAME_SIZE];  	// ���l�ɦW
};

//////////////////////////////////////////////////////////////////////////
// rat�ɰʧ@���� �C����
//////////////////////////////////////////////////////////////////////////
struct SRATActFile : public SRATActFileEntry
{
	vRATComFileList vRATComList;						// �s��RAT�ɹs��Ӽ�	
	bool operator == (const char* p) const { return ( strcmp( szName, p ) == 0 ); }
};

//////////////////////////////////////////////////////////////////////////
// rat���Y���c
//////////////////////////////////////////////////////////////////////////
struct SRATFileHeader
{
	char            szFileID[4];						// �ɮ׿��ID
	unsigned short  usVersion;          				// �ɮת���
	char            szRoleName[_SHORT_FILE_NAME_SIZE];  // ����W��
	float           fScaleX;            				// �����Y��X
	float           fScaleY;            				// �����Y��Y
	unsigned short  usShadowSize;       				// �v�l�j�p
	unsigned short  usActCount;         				// �ʧ@��
	unsigned short  usShadowOffset;     				// �v�l������
	unsigned short  usBoundingBoxSize;  				// ��ɲ��j�p
	unsigned short  usDeadBoundBoxSize; 				// ���`��ɲ��j�p
	char            szReserved[4];      				// �O�d���
};

//////////////////////////////////////////////////////////////////////////
// RAT�ɪ��}�Y
// �@��rat�� �]�t���Y��T�Bn�Ӱʧ@
//////////////////////////////////////////////////////////////////////////
struct SRATFileRootEntry
{
	SRATFileHeader	RATFileHeader;		//���Y
	string          strFilename;		//�ɮצW��
	float           fScaleX;
	float           fScaleY;
	float           fScaleZ;
	float           fShadowSize;
	float           fShadowOffset;
	unsigned short  usBoundingBoxSize;
	unsigned short  usDeadBoundBoxSize;
};

//////////////////////////////////////////////////////////////////////////
//	RAT�ɪ�����
//////////////////////////////////////////////////////////////////////////
struct SRATFileRoot : public SRATFileRootEntry
{
	vRATActFileList	vRATActList;			//�s��ʧ@�Ӽ�
    vector<SRATImageFile*> SRATImageList;	//�Ϥ�id���ޥ�
};

#pragma pack( pop )


	//----------------------------------------------------------------------------
} // end of namespace FDO
//----------------------------------------------------------------------------

#endif // _FDO_RAT_FILE_H_