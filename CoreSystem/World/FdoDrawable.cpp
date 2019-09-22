#include "StdAfx.h"
#include "FdoDrawable.h"
#include "SCN.h"

namespace FDO
{

CDrawable::CDrawable(void)
{
    this->CleanUp();
}

CDrawable::~CDrawable(void)
{
}

void CDrawable::UpdateBounds() const
{
    _LOOP(mBoundingBox.count, i)
    {
        D3DXVec3TransformCoord(&mBoundingBox.pts[i], &mBoundingBox.pts[i], &GetWorldMatrix());
    }
    mRadius = mBoundingBox.GetRadius();
    mBoundsOutOfData = false;
}

void CDrawable::CleanUp()
{
    mPosition = VEC3_ZERO;
    mRotation = VEC3_ZERO;
    mScale = VEC3_UNIT_SCALE;
    mOffset = VEC3_ZERO;

    D3DXMatrixIdentity(&mWorldMatrixCache);
    mWorldMatrixOutOfData = false;

    mBoundsOutOfData = false;
    //mMinBox = VEC3_ZERO;
    //mMaxBox = VEC3_ZERO;
    mBoxOutOfData = false;

    mElapsedTime = 0.f;
    mVisible = false;
    mSqDistance = 0.f;
    mRadius = 0.f;
    mFlag = 0;
}

void CDrawable::UpdateBox() const
{
    mBox.min.x = this->GetPosition().x + this->GetOffset().x - this->GetRadius();
    mBox.min.y = 0.f;
	mBox.min.z = this->GetPosition().z + this->GetOffset().z - this->GetRadius();
	mBox.max.x = this->GetPosition().x + this->GetOffset().x + this->GetRadius();
    mBox.max.y = 0.f;
	mBox.max.z = this->GetPosition().z + this->GetOffset().z + this->GetRadius();
    mBoxOutOfData = false;
}

void CDrawable::SetControlEnabled(int which, bool value)
{
    if (value)
    {
        mFlag |= which;
    }
    else
    {
        mFlag &= ~which;
    }
}

void CDrawable::GetBoundingBox(D3DXVECTOR3 * pMin, D3DXVECTOR3 * pMax) const
{
    if (mBoundsOutOfData)
    {
        UpdateBounds();
    }

    if (pMin != NULL) *pMin = mBoundingBox.GetMin();
    if (pMax != NULL) *pMax = mBoundingBox.GetMax();
}

float CDrawable::GetRadius() const
{
    if (mBoundsOutOfData)
    {
        UpdateBounds();
    }
    return mRadius;
}

bool CDrawable::IsControlEnabled(int which) const
{
    return ((mFlag & which) != 0);
}

float CDrawable::GetSqDistance() const
{
    return mSqDistance;
}

bool CDrawable::IsVisible() const
{
    return mVisible;
}
/**
 * @brief 
 */
void CDrawable::SetVisible(bool value)
{
    mVisible = value;
}
/**
 *
 */
const D3DXVECTOR3& CDrawable::GetMinBox() const
{
    if (mBoxOutOfData)
    {
        UpdateBox();
    }
    return mBox.min;
}
/**
 *
 */
const D3DXVECTOR3& CDrawable::GetMaxBox() const
{
    if (mBoxOutOfData)
    {
        UpdateBox();
    }
    return mBox.max;
}
/**
 * @brief 由位置、旋轉、縮放產生矩陣
 */
void CDrawable::UpdateWorldMatrix() const
{
    // 移動
	D3DXMatrixTranslation
    (
        &mWorldMatrixCache,
        mPosition.x + mOffset.x,
        mPosition.y + mOffset.y,
        mPosition.z + mOffset.z
    );
    D3DXMATRIX matTemp;
    // 繞X軸旋轉
	D3DXMatrixRotationX(&matTemp, mRotation.x);
    //D3DXMatrixRotationAxis( &matTemp, &VEC3_UNIT_X, mRotation.x );
    D3DXMatrixMultiply(&mWorldMatrixCache, &matTemp, &mWorldMatrixCache);
    // 繞Y軸旋轉
	D3DXMatrixRotationY(&matTemp, mRotation.y);
    //D3DXMatrixRotationAxis( &matTemp, &VEC3_UNIT_Y, mRotation.y );
    D3DXMatrixMultiply(&mWorldMatrixCache, &matTemp, &mWorldMatrixCache);
    // 繞Z軸旋轉
	D3DXMatrixRotationZ(&matTemp, mRotation.z);
    //D3DXMatrixRotationAxis( &matTemp, &VEC3_UNIT_Z, mRotation.z );
    D3DXMatrixMultiply(&mWorldMatrixCache, &matTemp, &mWorldMatrixCache);
    // 縮放
	D3DXMatrixScaling(&matTemp, mScale.x, mScale.y, mScale.z);
    D3DXMatrixMultiply(&mWorldMatrixCache, &matTemp, &mWorldMatrixCache);
    // clear dirty flag
    mWorldMatrixOutOfData = false;
}
/**
 *
 */
void CDrawable::DrawHelpers(SCNPtr scn)
{
    scn->DrawBox(mBoundingBox, scn->GetSelectionColor());
}
/**
 *
 */
void CDrawable::SetPosition(float x, float y, float z)
{
    mPosition.x = x;
    mPosition.y = y;
    mPosition.z = z;
    mWorldMatrixOutOfData = true;
    mBoundsOutOfData = true;
    mBoxOutOfData = true;
}

void CDrawable::SetPosition(const D3DXVECTOR3& value)
{
    mPosition = value;
    mWorldMatrixOutOfData = true;
    mBoundsOutOfData = true;
    mBoxOutOfData = true;
}
/**
 *
 */
void CDrawable::SetRotation(float x, float y, float z)
{
    mRotation.x = x;
    mRotation.y = y;
    mRotation.z = z;
    mWorldMatrixOutOfData = true;
    mBoundsOutOfData = true;
}

void CDrawable::SetRotation(const D3DXVECTOR3& value)
{
    mRotation = value;
    mWorldMatrixOutOfData = true;
    mBoundsOutOfData = true;
}
/**
 *
 */
void CDrawable::SetScale(float x, float y, float z)
{
    mScale.x = x;
    mScale.y = y;
    mScale.z = z;
    mWorldMatrixOutOfData = true;
    mBoundsOutOfData = true;
    mBoxOutOfData = true;
}

void CDrawable::SetScale(const D3DXVECTOR3& value)
{
    mScale = value;
    mWorldMatrixOutOfData = true;
    mBoundsOutOfData = true;
    mBoxOutOfData = true;
}
/**
 * @brief set this model offset
 */
void CDrawable::SetOffset(float x, float y, float z)
{
    mOffset.x = x;
    mOffset.y = y;
    mOffset.z = z;
    mWorldMatrixOutOfData = true;
    mBoundsOutOfData = true;
    mBoxOutOfData = true;
}

void CDrawable::SetOffset(const D3DXVECTOR3& value)
{
    mOffset = value;
    mWorldMatrixOutOfData = true;
    mBoundsOutOfData = true;
    mBoxOutOfData = true;
}

bool CDrawable::IsRayIntersect(const RAY& ray, HitInfo& res) const
{
    return false;
}

const D3DXMATRIX& CDrawable::GetWorldMatrix() const
{
    if (mWorldMatrixOutOfData)
    {
        UpdateWorldMatrix();
    }
    return mWorldMatrixCache;
}

const D3DXVECTOR3& CDrawable::GetPosition() const
{
    return mPosition;
}

const D3DXVECTOR3& CDrawable::GetRotation() const
{
    return mRotation;
}

const D3DXVECTOR3& CDrawable::GetScale() const
{
    return mScale;
}

const D3DXVECTOR3& CDrawable::GetOffset() const
{
    return mOffset;
}

}