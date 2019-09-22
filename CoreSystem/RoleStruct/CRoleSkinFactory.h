#ifndef _ROLESKINFACTORY_
#define _ROLESKINFACTORY_


#include <stdlib.h>
#include <string.h>
#include <vector>
#include <d3dx9.h>
#include "CRoleDef.h"

//#include "CRoleSkinStruct.h"

namespace FDO
{

class CRoleSkinFactory
{

public:

// 	CRoleSkinFactory();
// 	~CRoleSkinFactory();
	
	//�s���@�k
	static void LinkRoleSkinRATFile(vector<SRoleSkinImageNew>& vRoleInstanceList, SP_RATFile& pRATFile);	//�s�����[
	//static void LinkRoleSkinRATFile(SRoleSkinImageNew* pRoleInstanceList, int nRoleSkinImgCount, SP_RATFile& pRATFile);
    // TODO: �H�U�����|�Ψ�...
	static void LinkRoleSkinRATAct(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRATActFile& RATAct);
	static void LinkRoleSkinRATCom(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRATComFile& RATCom );
	static void LinkRoleSkinRATDir(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRATDirFile& RATDir);
	static void LinkRoleSkinRATImage(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRATImageFile& RATImg);
	
	//RPC����
	static void LinkRoleSkinRPCFile(vector<SRoleSkinImageNew>& vRoleInstanceList, SP_RPCFile& spRPCFile );
	//
	static void LinkRoleSkinRPCActList( vector<SRoleSkinImageNew*>& vRoleInstanceList, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile, int nRoleKind );
	static void LinkRoleSkinRPCAct( vector<SRoleSkinImageNew*>& vRoleInstanceList, SRPCComFile& RPCCom, SRATActFile& RATAct, int nRoleKind );
	static void LinkRoleSkinRPCCom( vector<SRoleSkinImageNew*>& vRoleInstanceList, SRPCComFile& RPCCom, SRATComFile& RATCom, int nRoleKind );
	static void LinkRoleSkinRPCDir(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRPCDirFile& RPCDir, SRATDirFile& RATDir, int nRoleKind);
	static void LinkRoleSkinRPCImage(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRPCImageFile& RPCImg, SRATImageFile& RATImg, int nRoleKind );

	

	static bool RoleSkinImageSort(SRoleSkinImage* i, SRoleSkinImage* j);	//�Ƨǹs��ϼh



//	//////////////////////////////////////////////////////////////////////////
//	//��~��api
//	//////////////////////////////////////////////////////////////////////////
//	static void LinkRoleSkinRATFile(SRoleSkin* pRoleSkin, SP_RATFile pRATFile);		//�s�����[
//	static void LinkRoleSkinRPCFile(SRoleSkin* pRoleSkin, SP_RPCFile spRPCFile, int nRoleKind );		//�s���ֽ�
//	static bool RoleSkinImageSort(SRoleSkinImage* i, SRoleSkinImage* j);	//�Ƨǹs��ϼh
//	
//private:
//	//////////////////////////////////////////////////////////////////////////
//	//rat�ɪ�����
//	//////////////////////////////////////////////////////////////////////////
//	static void LinRoleSkinRATAct(SRoleSkinAct& roleSkinAct, SRATActFile& RATAct);
//	static void LinkRoleSkinRATCom(SRoleSkinCom& roleSkinCom, SRATComFile& RATCom );
//	static void LinkRoleSkinRATDir(SRoleSkinDir& roleSkinDir, SRATDirFile& RATDir);
//	static void LinkRoleSkinRATImage(SRoleSkinImage& roleSkinImg, SRATImageFile& RATImg);
//
//	//////////////////////////////////////////////////////////////////////////
//	//rpc�ɪ�����
//	//////////////////////////////////////////////////////////////////////////
//	
//	static void LinkRoleSkinRPCActList( SRoleSkin* pRoleSkin, SRPCActFile* pRPCAct, int nRPCActSize, int nRoleKind );
//
//	static void LinkRoleSkinRPCAct( SRoleSkinAct& pRoleSkinAct, SRPCActFile& RPCAct, int nRoleKind );
//	
//	
//	static void LinkRoleSkinRPCCom(SRoleSkinCom& roleSkinCom, SRPCComFile& RPCCom, int nRoleKind);
//	static void LinkRoleSkinRPCDir(SRoleSkinDir& roleSkinDir, SRPCDirFile& RPCDir, int nRoleKind);
//	static void LinkRoleSkinRPCImage(SRoleSkinImage& roleSkinImg, SRPCImageFile& RPCImg, int nRoleKind);

	static void UpdateLocalMatrix(D3DXMATRIX* pMatLocal, const SRATImageFile* pRATImg, const SRPCImageFile* pRPCImg);

private:
//	SRoleSkin* m_pRoleSkin;
};

}
#endif