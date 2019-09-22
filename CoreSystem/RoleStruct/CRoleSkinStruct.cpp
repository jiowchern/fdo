
#include "stdafx.h"
#include "CRoleSkinStruct.h"
#include "CRATFileStruct.h"
#include "CRPCFileStruct.h"

using namespace FDO;

//////////////////////////////////////////////////////////////////////////
//圖片連結資料
//////////////////////////////////////////////////////////////////////////
// SRoleSkinTexture::SRoleSkinTexture(const char* pFilename) : pTexture(0)
// {
// 	strncpy( szFilename, pFilename, _MAX_PATH );
// }
// 
// SRoleSkinTexture::~SRoleSkinTexture()
// {
// 	if (pTexture)
// 	{
// 		pTexture->Release();
// 		pTexture = 0;
// 	}
// }

//////////////////////////////////////////////////////////////////////////
// 角色skin圖像資料
//////////////////////////////////////////////////////////////////////////
SRoleSkinImage::SRoleSkinImage()
{
}

SRoleSkinImage::~SRoleSkinImage()
{
	//從資源管理處去刪除
	//GetIMGManage()->Release( spImg->szFilename, spImg );
}

SRoleSkinImageNew::SRoleSkinImageNew()
{
}

bool SRoleSkinImageNew::Lerp(float slope, const SRoleSkinImageNew* rhs)
{
    F_RETURN(rhs != NULL);
    F_RETURN(rhs->pRATImg != NULL);
    F_RETURN(rhs->pRPCImg != NULL);

    F_RETURN(pRATImg != NULL);
    F_RETURN(pRPCImg != NULL);

    if (pRATImg->LinearFlag & LINEAR::POSITION)
    {
        pRATImg->vPosition = ::Lerp(slope, pRATImg->vPosition, rhs->pRATImg->vPosition);
    }
    if (pRATImg->LinearFlag & LINEAR::ROTATION)
    {
        pRATImg->vScale.z = ::Lerp(slope, pRATImg->vScale.z, rhs->pRATImg->vScale.z);
    }
    if (pRATImg->LinearFlag & LINEAR::SCALE)
    {
        pRATImg->vScale.x = ::Lerp(slope, pRATImg->vScale.x, rhs->pRATImg->vScale.x);
        pRATImg->vScale.y = ::Lerp(slope, pRATImg->vScale.y, rhs->pRATImg->vScale.y);
    }
    if (pRATImg->LinearFlag & LINEAR::COLOR)
    {
        D3DXCOLOR src(pRPCImg->dwColor);
        D3DXCOLOR dest(rhs->pRPCImg->dwColor);
        D3DXCOLOR result;
        D3DXColorLerp(&result, &src, &dest, slope);
        pRPCImg->dwColor = result;
    }
    if (pRATImg->LinearFlag & LINEAR::SIZE)
    {
        pRPCImg->fWidth  = ::Lerp(slope, pRPCImg->fWidth , rhs->pRPCImg->fWidth );
        pRPCImg->fHeight = ::Lerp(slope, pRPCImg->fHeight, rhs->pRPCImg->fHeight);
    }
    if (pRATImg->LinearFlag & LINEAR::UV)
    {
        pRPCImg->fTU1 = ::Lerp(slope, pRPCImg->fTU1, rhs->pRPCImg->fTU1);
        pRPCImg->fTV1 = ::Lerp(slope, pRPCImg->fTV1, rhs->pRPCImg->fTV1);
        pRPCImg->fTU2 = ::Lerp(slope, pRPCImg->fTU2, rhs->pRPCImg->fTU2);
        pRPCImg->fTV2 = ::Lerp(slope, pRPCImg->fTV2, rhs->pRPCImg->fTV2);
    }

    return true;
}
// 
SRoleSkinImageNew::SRoleSkinImageNew(SRATImageFile* pRATImage)
: pRATImg(pRATImage), pRPCImg(NULL), pSkinTexture(NULL) //, bMatOver(false)
{
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// #include "ElementDef.h"
// typedef ObjFactory<SRoleSkinImageNew> SRoleSkinImageNewFactory;
// 
// void* SRoleSkinImageNew::operator new(size_t size)
// {
// 	if(SRoleSkinImageNewFactory::GetSingletonPtr() == 0)
// 	{
// 		new SRoleSkinImageNewFactory;		
// 	}	
// 	return SRoleSkinImageNewFactory::GetSingletonPtr()->Create();
// }
// //////////////////////////////////////////////////////////////////////////
// void SRoleSkinImageNew::operator delete(void* p)
// {
// 	SRoleSkinImageNewFactory::GetSingletonPtr()->Destroy(p);
// }