#include "stdafx.h"

#include "CRoleSkinFactory.h"
#include "CRoleSkinStruct.h"
#include "CRATFileStruct.h"
#include "CRPCFileStruct.h"
#include "ResourceManager.h"


using namespace FDO;
//////////////////////////////////////////////////////////////////////////
//�s��rat�ɨ쨤��skin��
//////////////////////////////////////////////////////////////////////////
// CRoleSkinFactory::CRoleSkinFactory()
// {
// }
// 
// CRoleSkinFactory::~CRoleSkinFactory()
// {
// }


//////////////////////////////////////////////////////////////////////////
// �Ƨǹs��ϼh
//////////////////////////////////////////////////////////////////////////
bool CRoleSkinFactory::RoleSkinImageSort(SRoleSkinImage* i, SRoleSkinImage* j)	
{
	return ( i->pRATImg->shLayer < j->pRATImg->shLayer );
//	return (i->pFrame->shLayer<j->pFrame->shLayer);
}


//////////////////////////////////////////////////////////////////////////
//�s�@�k
//////////////////////////////////////////////////////////////////////////
void CRoleSkinFactory::LinkRoleSkinRATFile(vector<SRoleSkinImageNew>& vRoleInstanceList, SP_RATFile& pRATFile)	//�s�����[
{
	// TODO:�]�wskin���ʧ@�ƶq
	vRoleInstanceList.assign( pRATFile->SRATImageList.begin(), pRATFile->SRATImageList.end() );
}
////////////////////////////////////////////////////////////////////////////
////link skin act of rat file 
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRATAct(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRATActFile& RATAct)
//{
//	for (int i = 0; i < RATAct.nSRATComSize; ++i)
//	{
//		LinkRoleSkinRATCom( vRoleInstanceList, RATAct.pSRATCom[i] );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////link skin com of rat file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRATCom(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRATComFile& RATCom )
//{
//	// �]�w�s�󬰤��i��
//	//roleSkinCom.bVisible = false;
//
//	for (int i = RID_South; i < MAX_DIR; ++i)
//	{
//		LinkRoleSkinRATDir( vRoleInstanceList, RATCom.RATDir[i] );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
//// link skin dir of rat file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRATDir(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRATDirFile& RATDir)
//{
//	for (int i=0; i < RATDir.nSRATImageSize; ++i)
//	{
//		LinkRoleSkinRATImage( vRoleInstanceList, RATDir.pSRATImage[i] );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////  link skin image of rat file
//// RenderRole
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRATImage(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRATImageFile& RATImg)
//{
//	SRoleSkinImageNew* skinImg = vRoleInstanceList.at( RATImg.nRATImageID );
//	skinImg->pRATImg = &RATImg;
//}

//////////////////////////////////////////////////////////////////////////
//�s��rpc�ɪ�����
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//�s��rpc�ɨ쨤��skin �s�@�k
//////////////////////////////////////////////////////////////////////////
void CRoleSkinFactory::LinkRoleSkinRPCFile(vector<SRoleSkinImageNew>& vRoleInstanceList, SP_RPCFile& spRPCFile )
{
	
	
	//�Ĥ@��
	for(size_t i(0); i < spRPCFile->vRPCImgPointList1.size(); ++i)
	{
		SRPCImageFile* pRPCImg = spRPCFile->vRPCImgPointList1.at(i);

		if((int)vRoleInstanceList.size() < pRPCImg->nRPCImgID)
		{
			Assert(0 , "RAT�MRPC���") ; 
		}

		if(vRoleInstanceList.at( pRPCImg->nRPCImgID ).pRPCImg == NULL)
			vRoleInstanceList.at( pRPCImg->nRPCImgID ).pRPCImg = pRPCImg;
		else
		{			
			std::string assertout;			
			StringFormat8(assertout , "�ɦW %s : ���ƥhload part" ,spRPCFile->strFilename.c_str() );			
			_LOGA( 0 , assertout.c_str() );
		}
		
		//SRATImageFile* pRATImg = vRoleInstanceList.at( pRPCImg->nRPCImgID ).pRATImg;
		//D3DXMATRIX* pMatLocal = &vRoleInstanceList.at( pRPCImg->nRPCImgID ).vMatLocal;
		
		//UpdateLocalMatrix(pMatLocal, pRATImg, pRPCImg);
	}

	


	//�ĤG��
	for(size_t i(0); i < spRPCFile->vRPCImgPointList2.size(); ++i)
	{
		SRPCImageFile*	pRPCImg = spRPCFile->vRPCImgPointList2.at(i);
		vRoleInstanceList.at( pRPCImg->nRPCImgID ).pRPCImg = pRPCImg;
	}
}

////////////////////////////////////////////////////////////////////////////
////�s��rpc���ʧ@list�쨤��skin�ʧ@��list
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCActList( vector<SRoleSkinImageNew*>& vRoleInstanceList, SP_RPCFile& spRPCFile, SP_RATFile& spRATFile, int nRoleKind )
//{
//	for (int i = 0; i < spRATFile->nSRATActSize; ++i)
//	{
//		SRPCComFile& RPCCom = spRPCFile->pRPCAct1[i].RPCCom;
//		SRATActFile& RATAct = spRATFile->pSRATAct[i];
//		LinkRoleSkinRPCAct( vRoleInstanceList, RPCCom, RATAct, nRoleKind );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////link role skin Act of rpc file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCAct( vector<SRoleSkinImageNew*>& vRoleInstanceList, SRPCComFile& RPCCom, SRATActFile& RATAct, int nRoleKind )
//{
//	for(int i=0; i < RATAct.nSRATComSize; ++i)
//	{
//		if(strcmp(RATAct.pSRATCom[i].szComName, RPCCom.szName) != 0)
//		{
//			continue;
//		}
//		
//		SRATComFile& RATCom = RATAct.pSRATCom[i];
//		LinkRoleSkinRPCCom( vRoleInstanceList, RPCCom, RATCom, nRoleKind );
//
//        // TODO: �o�̥[�Wbreak�A�����ڭ̷Q�n�����ƴN���X�h
//        //       �o�˴N�����]���Ҧ����j��
//        break;
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////link role skin com of rpc file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCCom( vector<SRoleSkinImageNew*>& vRoleInstanceList, SRPCComFile& RPCCom, SRATComFile& RATCom, int nRoleKind )
//{
//	// �]�w�s�󬰥i��
//	//roleSkinCom.bVisible = true;
//
//	for (int i = RID_South; i < MAX_DIR; ++i)
//	{
//		LinkRoleSkinRPCDir( vRoleInstanceList, RPCCom.RPCDir[i], RATCom.RATDir[i], nRoleKind );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////link role skin dir of rpc file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCDir(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRPCDirFile& RPCDir, SRATDirFile& RATDir, int nRoleKind)
//{
//	for (int i=0; i < RATDir.nSRATImageSize; ++i)
//	{
//		LinkRoleSkinRPCImage(vRoleInstanceList, RPCDir.pRPCImage[i], RATDir.pSRATImage[i], nRoleKind );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
//// link role skin image of rpc file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCImage(vector<SRoleSkinImageNew*>& vRoleInstanceList, SRPCImageFile& RPCImg, SRATImageFile& RATImg, int nRoleKind )
//{
//	//�إ߳s��
//	SRoleSkinImageNew* skinImg = vRoleInstanceList.at( RATImg.nRATImageID );
//	skinImg->pRPCImg = &RPCImg;
//
//	////���@�Ӥ��s�b�N���}
//	//if( ! roleSkinImg.pRATImg || roleSkinImg.pRPCImg )
//	//{
//	//	return ;
//	//}
//
//    // TODO: �o�̪��ʧ@����ø�Ϯɥh��
//	//if( RPCImg.szTexFile[0] == '\0')
//	//{
//	//	skinImg->pSkinTexture = NULL;
//	//}
//	//else
//	//{
//	//	//�D���O1 
//	//	if( nRoleKind == 1 )
//	//	{
//	//		skinImg->pSkinTexture = g_RoleImageManager.GetStaticImageRes(RPCImg.szTexFile );
//	//	}
//	//	else
//	//	{
//	//		skinImg->pSkinTexture = g_RoleImageManager.GetDynamicImageRes(RPCImg.szTexFile );
//	//	}
//	//}
//
//    // TODO: �p�G Matrix �u�t�����A�o�u�@�]�i�H����ø�Ϯɰ�
//	//UpdateLocalMatrix( &skinImg->vMatLocal, skinImg->pRATImg, skinImg->pRPCImg );
//}

//////////////////////////////////////////////////////////////////////////
//�U���O�§@�k 
//////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////
////
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRATFile(SRoleSkin* pRoleSkin, SP_RATFile pRATFile)
//{
//	//�]�wskin���ʧ@�ƶq
//	pRoleSkin->nSkinActSize = pRATFile->nSRATActSize;
//	pRoleSkin->pRoleSkinAct = new SRoleSkinAct[pRoleSkin->nSkinActSize];
//	
//	for( int i = 0; i < pRoleSkin->nSkinActSize; ++i )
//	{
//		LinRoleSkinRATAct( pRoleSkin->pRoleSkinAct[i], pRATFile->pSRATAct[i] );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////link skin act of rat file 
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinRoleSkinRATAct(SRoleSkinAct& roleSkinAct, SRATActFile& RATAct)
//{
//	//�]�w�W��
//	strncpy( roleSkinAct.szName, RATAct.szName, LENGTHOF_NAME );
//	//�]�w�s��ƶq
//	roleSkinAct.nSkinComSize = RATAct.nSRATComSize;
//	roleSkinAct.pRoleSkinCom = new SRoleSkinCom[roleSkinAct.nSkinComSize];
//
//	for (int i = 0; i < roleSkinAct.nSkinComSize; ++i)
//	{
//		//LinkRoleSkinRATCom( roleSkinAct.vSkinComList.at(i), *( RATAct.vRATComList.at(i) ) );
//		LinkRoleSkinRATCom( roleSkinAct.pRoleSkinCom[i], RATAct.pSRATCom[i] );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////link skin com of rat file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRATCom(SRoleSkinCom& roleSkinCom, SRATComFile& RATCom )
//{
//	//�]�w�W��
//	strncpy( roleSkinCom.szComName, RATCom.szComName, LENGTHOF_NAME );
//	// �]�w�s�󬰤��i��
//	roleSkinCom.bVisible = false;
//	
//	for (int i = RID_South; i < MAX_DIR; ++i)
//	{
//		LinkRoleSkinRATDir( roleSkinCom.skinDirs[i], RATCom.RATDir[i] );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
//// link skin dir of rat file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRATDir(SRoleSkinDir& roleSkinDir, SRATDirFile& RATDir)
//{
//	//�]�w�Ϥ��ƶq
//	roleSkinDir.nSkinImgSize = RATDir.nSRATImageSize;
//	roleSkinDir.pRoleSkinImg = new SRoleSkinImage[RATDir.nSRATImageSize];
//
//	for (int i=0; i < roleSkinDir.nSkinImgSize ; ++i)
//	{
//		LinkRoleSkinRATImage( roleSkinDir.pRoleSkinImg[i], RATDir.pSRATImage[i] );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////  link skin image of rat file
//// RenderRole
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRATImage(SRoleSkinImage& roleSkinImg, SRATImageFile& RATImg)
//{
//	roleSkinImg.pRATImg = &RATImg;
//}
//
//
////////////////////////////////////////////////////////////////////////////
////�s��rpc�ɪ�����
////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////
////�s��rpc�ɨ쨤��skin
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCFile(SRoleSkin* pRoleSkin, SP_RPCFile spRPCFile, int nRoleKind)
//{
//	//rpc�ɸ̦�����ʧ@
//	LinkRoleSkinRPCActList( pRoleSkin, spRPCFile->pRPCAct1, spRPCFile->nRPCAct1Size, nRoleKind );
//
//	//if(spRPCFile->nRPCAct2Size > 0 )
//	{
//		LinkRoleSkinRPCActList( pRoleSkin, spRPCFile->pRPCAct2, spRPCFile->nRPCAct2Size, nRoleKind );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////�s��rpc���ʧ@list�쨤��skin�ʧ@��list
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCActList( SRoleSkin* pRoleSkin, SRPCActFile* pRPCAct, int nRPCActSize, int nRoleKind )
//{
//	for (int i = 0; i < pRoleSkin->nSkinActSize ; ++i)
//	{
//		if(strcmp(pRoleSkin->pRoleSkinAct[i].szName ,pRPCAct[i].szName) != 0)
//		{
//			continue;
//		}
//		// 
//		string skinActName = pRoleSkin->pRoleSkinAct[i].szName;
//		string skinComName = pRoleSkin->pRoleSkinAct[i].pRoleSkinCom[0].szComName;
//		string RpcActName = pRPCAct[i].szName;
//		string RpcComName = pRPCAct[i].RPCCom.szName;
//		LinkRoleSkinRPCAct( pRoleSkin->pRoleSkinAct[i], pRPCAct[i], nRoleKind );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////link role skin Act of rpc file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCAct( SRoleSkinAct& roleSkinAct, SRPCActFile& RPCAct, int nRoleKind )
//{
//	for(int i=0; i < roleSkinAct.nSkinComSize; ++i)
//	{
//		if(strcmp(roleSkinAct.pRoleSkinCom[i].szComName, RPCAct.RPCCom.szName) != 0)
//		{
//				continue;
//		}
//		LinkRoleSkinRPCCom( roleSkinAct.pRoleSkinCom[i], RPCAct.RPCCom, nRoleKind );
//		break;
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////link role skin com of rpc file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCCom(SRoleSkinCom& roleSkinCom, SRPCComFile& RPCCom, int nRoleKind )
//{
//	// �]�w�s�󬰥i��
//	roleSkinCom.bVisible = true;
//
//	for (int i = RID_South; i < MAX_DIR; ++i)
//	{
//		LinkRoleSkinRPCDir( roleSkinCom.skinDirs[i], RPCCom.RPCDir[i], nRoleKind );
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
////link role skin dir of rpc file
////////////////////////////////////////////////////////////////////////////
//void CRoleSkinFactory::LinkRoleSkinRPCDir(SRoleSkinDir& roleSkinDir, SRPCDirFile& RPCDir, int nRoleKind)
//{
//	for (int i=0; i < roleSkinDir.nSkinImgSize; ++i)
//	{
//		LinkRoleSkinRPCImage(roleSkinDir.pRoleSkinImg[i], RPCDir.pRPCImage[i], nRoleKind );
//	}
//}

//////////////////////////////////////////////////////////////////////////
// link role skin image of rpc file
//////////////////////////////////////////////////////////////////////////

void CRoleSkinFactory::UpdateLocalMatrix(D3DXMATRIX* pMatLocal, const SRATImageFile* pRATImg, const SRPCImageFile* pRPCImg)
{
	//���p���Y��
	D3DXMatrixScaling( pMatLocal, pRATImg->vScale.x, pRATImg->vScale.y, 1.0f );

	// �p�����
	float fShiftX = 0.0f;
	float fShiftY = -( pRATImg->vScale.y * pRPCImg->fHeight * (100.0f - pRATImg->fPercentY ) * 0.01f );

	//�ϦV����
	if( pRATImg->bTexInvert )
	{
		fShiftX = -( pRATImg->vScale.x * pRPCImg->fWidth * ( pRATImg->fPercentX - 50.0f ) * 0.01f * 2.0f );
	}
	else
	{
		fShiftX = - (pRATImg->vScale.x * pRPCImg->fWidth * ( 50.0f - pRATImg->fPercentX ) * 0.01f * 2.0f );
	}

	D3DXMATRIX matTemp;
	D3DXMatrixTranslation( &matTemp, fShiftX, fShiftY, 0 );
	D3DXMatrixMultiply( pMatLocal, pMatLocal, &matTemp );
	D3DXMatrixRotationZ( &matTemp, pRATImg->vScale.z );
	D3DXMatrixMultiply( pMatLocal, pMatLocal, &matTemp );
	D3DXMatrixTranslation( &matTemp, -fShiftX, -fShiftY, 0 );
	D3DXMatrixMultiply( pMatLocal, pMatLocal, &matTemp );

	// �p��첾
	D3DXMatrixTranslation( &matTemp, pRATImg->vPosition.x, pRATImg->vPosition.y, pRATImg->vPosition.z );
	D3DXMatrixMultiply( pMatLocal, pMatLocal, &matTemp );
}