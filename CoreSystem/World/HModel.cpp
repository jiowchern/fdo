#include "stdafx.h"
//---------------------------------------------------------------------------
#pragma hdrstop
#include "HModel.h"
#include "JLSkinMesh.h"
#include "SCN.h"
#include "FdoUpdateArgs.h"
#include "FdoSkinMeshHelper.h"
#include "ResourceManager.h"
#include "PhenixD3D9RenderSystem.h"
//---------------------------------------------------------------------------
namespace FDO
{

HModel::HModelFactory HModel::sHModelFactory;
//---------------------------------------------------------------------------
HModel::HModel()
: mCenterCacheOutOfData(true)
, mAnimCtrller(NULL)
, mAlphaValue(ALPHA_OPAQUE)
{
}
//---------------------------------------------------------------------------
HModel::~HModel()
{
    SafeRelease(mAnimCtrller);
}
/**
 *@brief Ã¸¹Ï
 */
bool HModel::Draw()
{
    F_RETURN(mSkinMeshTemp != NULL);

    LPDIRECT3DDEVICE9 pd3dDevice = mSkinMeshTemp->GetDevice();
    F_RETURN(pd3dDevice != NULL);

    LPD3DXFRAME pFrame = mSkinMeshTemp->GetFrameRoot();
    F_RETURN(pFrame != NULL);

    UpdateFrameMatrices(pFrame, &base::GetWorldMatrix());

    pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(1.f, 1.f, 1.f, mAlphaValue));
    DrawFrames(pd3dDevice, pFrame);
    pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(1.f, 1.f, 1.f, 1.f));

    return true;
}
/**
 *
 */
float HModel::GetAlphaDest() const
{
    if (this->IsControlEnabled(MODEL_FLAG::IS_TRANSPARENT))
    {
        return mAlphaDest;
    }
    else
    {
        return ALPHA_OPAQUE;
    }
}
/**
 * @brief
 */
bool HModel::IsRayIntersectBoundingBox(const RAY& ray) const
{
    HitInfo res;
    return mBoundingBox.IsRayIntersect(ray, res);
}
/**
 * @brief
 */
const D3DXVECTOR3& HModel::GetCenter() const
{
    if (mCenterCacheOutOfData)
    {
        D3DXVec3TransformCoord(&mCenterCache, &mSkinMeshTemp->GetBoundingCenter(), &GetWorldMatrix());
        mCenterCacheOutOfData = false;
    }
    return mCenterCache;
}
/**
 * @brief 
 */
float HModel::GetRadius() const
{
    if (mBoundsOutOfData)
    {
        UpdateBounds();
    }
    return mRadius;
}
/**
 * @brief 
 */
float HModel::GetSqDistance() const
{
    if (this->IsControlEnabled(MODEL_FLAG::TERRAIN))
    {
        return 0.f;
    }
    return base::GetSqDistance();
}
//---------------------------------------------------------------------------
void HModel::SetAnimation(LPD3DXANIMATIONCONTROLLER value)
{
    mAnimCtrller = value;
}
//---------------------------------------------------------------------------
void* HModel::operator new (size_t)
{
    return sHModelFactory.Create();
}
//---------------------------------------------------------------------------
void HModel::operator delete (void* p)
{
    if (p != NULL)
    {
        sHModelFactory.Destroy(p);
    }
}
/**
 *
 */
bool HModel::IsRayIntersect(const RAY& ray, HitInfo& res) const
{
    HRESULT hr = S_OK;

    bool hit = false;
    float dist = res.dist;

    V_RETURN
    (
        GetIntersect(mSkinMeshTemp->GetFrameRoot(), &hit, &ray.Orig, &ray.Dir, &dist)
    );

    //res.dist = dist;

    return hit;
}
/**
 * @brief
 */
void HModel::SetSkinMeshTemp(SkinMeshTempPtr value)
{
    mSkinMeshTemp = value;

    if (mSkinMeshTemp != NULL)
    {
        MMBox box;
        mSkinMeshTemp->GetBoundingBox(&box.min, &box.max);
        mBoundingBox = box;
        mBoundsOutOfData = true;
        mCenterCacheOutOfData = true;
    }
}
//---------------------------------------------------------------------------
void HModel::SetAlphaDest(float value)
{
    mAlphaDest = value;
}
/**
 * @brief ability of this object
 */
bool HModel::IsTransparentable() const
{
    return (mAlphaDest < ALPHA_OPAQUE);
}
/**
 * @brief state of this object
 */
bool HModel::IsTransparent() const
{
    T_RETURN(this->IsControlEnabled(MODEL_FLAG::ALWAYS_TRANSPARENT));

    //return (mAlphaValue < Epsilon1);
    return (mAlphaValue < 0.75f);
}
/**
 * @brief 
 */
bool HModel::IsVisible() const
{
    F_RETURN(mVisible);

    //F_RETURN(mAlphaValue > ALPHA_TRANSPARENT);

    return true;
}
/**
 * @brief
 */
void HModel::UpdateBox() const
{
	mBox.min.x = this->GetCenter().x - this->GetRadius();
    mBox.min.y = 0.f;
	mBox.min.z = this->GetCenter().z - this->GetRadius();
	mBox.max.x = this->GetCenter().x + this->GetRadius();
    mBox.max.y = 0.f;
	mBox.max.z = this->GetCenter().z + this->GetRadius();
    mBoxOutOfData = false;
}
/**
 * @brief update
 */
bool HModel::Update(const UpdateArgsPtr &args)
{
	g_RenderSystem.AddModelCount() ; 

    mElapsedTime += args->GetElapsedTime();
    // calc visible
    this->SetVisible
    (
        args->IsInBounding(this->GetMinBox(), this->GetMaxBox()) &&
        args->IsInFieldOfView(this->GetCenter(), this->GetRadius())
    );
    // check viaible
    F_RETURN(this->IsVisible());
    // update animation
    if (mAnimCtrller != NULL)
    {
        mAnimCtrller->AdvanceTime(mElapsedTime, NULL);
    }
    // calc transparent
    if (this->IsTransparentable())
    {
        this->SetControlEnabled
        (
            MODEL_FLAG::IS_TRANSPARENT,
            this->IsRayIntersectBoundingBox(args->GetRay()) &&
            this->IsRayIntersectSkinMesh(args->GetViewMatrix(), args->GetViewDistance(), args->GetRay())
        );
    }
    // calc alpha slope
    if (this->IsAlphaChanging())
    {
        mAlphaValue = Lerp(1.0f - powf(0.8f, 30.f * mElapsedTime), mAlphaValue, this->GetAlphaDest());
    }
    // calc square distance
    mSqDistance = D3DXVec3LengthSq(&(this->GetPosition()-args->GetRay().Orig));
    // clear elapsed time
    mElapsedTime = 0.f;

	g_RenderSystem.AddUpdateModelCount() ; 

    return true;
}
/**
 *
 */
bool HModel::IsAlphaChanging() const
{
    return (fabs(mAlphaValue - this->GetAlphaDest()) > Epsilon0);
}
/**
 *
 */
bool HModel::IsRayIntersectSkinMesh(const D3DXMATRIX& viewMatrix, float maxDist, const RAY& ray) const
{
    //D3DXMATRIX invWorld;
    //D3DXMatrixInverse(&invWorld, NULL, &GetWorldMatrix());

    //D3DXVECTOR3 RayOrig;
    //D3DXVec3TransformCoord(&RayOrig, &ray.Orig, &invWorld);

    //D3DXVECTOR3 RayDir;
    //D3DXVec3Normalize(&RayDir, &ray.Dir);

    D3DXMATRIX worldView(this->GetWorldMatrix() * viewMatrix);
    D3DXMATRIX invWorldView;
    D3DXMatrixInverse(&invWorldView, NULL, &worldView);

    D3DXVECTOR3 RayOrig(invWorldView._41, invWorldView._42, invWorldView._43);

    D3DXVECTOR3 v = D3DXVECTOR3(0.f, 0.02f, 1.f);
    D3DXVECTOR3 RayDir = D3DXVECTOR3
    (
        v.x * invWorldView._11 + v.y * invWorldView._21 + v.z * invWorldView._31,
        v.x * invWorldView._12 + v.y * invWorldView._22 + v.z * invWorldView._32,
        v.x * invWorldView._13 + v.y * invWorldView._23 + v.z * invWorldView._33
    );

    HitInfo res;
    res.dist = maxDist;

    return IsRayIntersect(RAY(RayOrig, RayDir), res);

    //F_RETURN(this->IsRayIntersect(RAY(RayOrig, RayDir), res));
    //F_RETURN(res.dist < maxDist);
    //return true;
}
/**
 *
 */
void HModel::RegisterForDrawing(SCNPtr scn)
{
    if (this->IsVisible() && (mAlphaValue >= 0.13f))
    {
        if (this->IsTransparent())
        {
            scn->RegisterForDrawing(DRAWKIND::TRANSPARENCY, this);
        }
        else
        {
            scn->RegisterForDrawing(DRAWKIND::MODEL, this);
        }
    }
}

void HModel::CleanUp()
{
    base::CleanUp();
}

} // end of namespace FDO