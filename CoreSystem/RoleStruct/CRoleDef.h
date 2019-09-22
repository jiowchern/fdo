#ifndef ROLE_DEF_H
#define ROLE_DEF_H
#include <boost\shared_ptr.hpp>
//////////////////////////////////////////////////////////////////////////
// �NRAT�ɵ��c�BRPC�ɵ��c�B����SKIN���c��
// ���O�w�q typedef �������b�o�̡C
// �n�ϥΪ����O�u�n�bh��include�o���ɮ״N��A
// �u�bcpp include����C
// [10/31/2008: by a-wi]
//////////////////////////////////////////////////////////////////////////

namespace FDO
{

//////////////////////////////////////////////////////////////////////////
// �����V�w�q
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
//RAT�ɪ�typedef�e�m�ŧi
//////////////////////////////////////////////////////////////////////////
typedef struct SRATFileRoot SRATFile;				//���rat��

typedef struct SRATFileHeader SRATFileHeader;		//rat���Y	 

typedef struct SRATActFile SRATActFile;				//rat�ɪ��ʧ@
typedef vector<SRATActFile> vRATActFileList;		//rat�ɪ��ʧ@�}�C

typedef struct SRATComFile SRATComFile;				//RAT�ɪ��s��
typedef vector<SRATComFile> vRATComFileList;		//rat�ɪ��s��}�C

typedef struct SRATDirFile SRATDirFile;				//RAT�ɪ���V
//typedef SRATDirFile[MAX_DIR] vRATDirArray;			//rat�ɪ���V�}�C�A�]��V�T�w��8��A�ҥH�g��!!

typedef struct SRATImageFile_v1 SRATImageFile_v1;
typedef struct SRATImageFile_v2 SRATImageFile_v2;		//RAT�ɪ��Ϥ����v2������

typedef struct SRATImageFile_v3 SRATImageFile;		

typedef vector<SRATImageFile> vRATImageFileList;		//RAT�ɪ��Ϥ���ư}�C



typedef boost::shared_ptr<SRATFile>	 SP_RATFile;	

//////////////////////////////////////////////////////////////////////////
//�w�q�Xrat�ɸ� ���csizeof���j�p
//////////////////////////////////////////////////////////////////////////
#define  _RAT_HEADER_FILE_SIZEOF 	sizeof(SRATFileHeader)
#define  _RAT_FILE_SIZEOF			sizeof(SRATFileRootEntry)
#define  _RAT_ACT_SIZSEOF			sizeof(SRATActFileEntry)	//rat�ɰʧ@�ɪ��j�p



#define  _RAT_IMAGE_FILE_SIZEOF_V2		sizeof(SRATImageFile_v2)
#define  _RAT_IMAGE_FILE_SIZEOF_V1		sizeof(SRATImageFile_v1)




//////////////////////////////////////////////////////////////////////////
//RPC�ɪ��e�m�ŧi
//////////////////////////////////////////////////////////////////////////
typedef struct SRPCFileRoot SRPCFile;				//���rpc��

typedef struct SRPCFileHeader SRPCFileHeader;		//rpc���Y

typedef struct SRPCActFile SRPCActFile;				//RPC�ʧ@�ɸ��
typedef vector<SRPCActFile> vRPCActFileList;		//RPC�ʧ@�ɸ�ư}�C

typedef struct SRPCComFile SRPCComFile;				//RPC�s���ɸ��
typedef vector<SRPCComFile> vRPCComFileList;		//RPC�s���ɸ�ư}�C

typedef struct SRPCDirFile SRPCDirFile;				//rpc��V�ɸ��
//typedef SRPCDirFile[MAX_DIR] vRATDirArray;			//rpc�ɪ���V�}�C�A�]��V�T�w��8��A�ҥH�g��!!

typedef struct SRPCImageFile_v1 SRPCImageFile_v1;
typedef struct SRPCImageFile_v2 SRPCImageFile_v2;
typedef struct SRPCImageFile_v3 SRPCImageFile;

typedef vector<SRPCImageFile>	vRPCImgFileList;	

typedef vector<SRPCImageFile*>	vRPCImgPointList;	//RPC�ɪ��Ϥ���Ư��ޥΰ}�C


typedef boost::shared_ptr<SRPCFile>  SP_RPCFile;

typedef list<SP_RPCFile>	SP_RPC_List;

//////////////////////////////////////////////////////////////////////////
//�w�q�XRPC�ɸ� ���csizeof���j�p
//////////////////////////////////////////////////////////////////////////
#define _RPC_HEADER_FILE_SIZEOF 		sizeof(SRPCFileHeader)

#define _RPC_IMAGE_FILE_SIZEOF_V2		sizeof(SRPCImageFile_v2)

#define _RPC_IMAGE_FILE_SIZEOF_V1		sizeof(SRPCImageFile_v1)



//////////////////////////////////////////////////////////////////////////
//RoleSkinStruct���e�m�ŧi
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
// �ɮת�class �e�m�ŧi
//////////////////////////////////////////////////////////////////////////
typedef class CRATFactory CRATFactory;
typedef class CRPCFactory CRPCFactory;
typedef class CRoleSkinFactory CRoleSkinFactory;
typedef class CRoleRenderer CRoleRenderer;

//////////////////////////////////////////////////////////////////////////
//�ܼƩw�q
//////////////////////////////////////////////////////////////////////////
#define  _SHORT_FILE_NAME_SIZE		20
#define  _LONG_FILE_NAME_SIZE		50
#define  _RAT_KEEP_SIZE				20		//�O�d���
#define  _DWORD_SIZEOF				sizeof( DWORD )
#define  _US_SHORT_SIZEOF			sizeof(unsigned short)
#define  _BOOL_SIZEOF				sizeof(bool)


//////////////////////////////////////////////////////////////////////////
//�ܼƩw�q
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