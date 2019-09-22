#include "StdAfx.h"
#include "FdoUpdateArgs.h"
#include "FOVClip.h"

namespace FDO
{

CUpdateArgs::CUpdateArgs(void)
: mElapsedTime(0.f)
{
}

CUpdateArgs::~CUpdateArgs(void)
{
}

float CUpdateArgs::GetElapsedTime() const
{
    return mElapsedTime;
}

void CUpdateArgs::SetElapsedTime(float value)
{
    mElapsedTime = value;
}

bool CUpdateArgs::IsInBounding(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
{
    return g_FOVClip.MeshInBounding(min, max, mViewRect);
}

bool CUpdateArgs::IsInFieldOfView(const D3DXVECTOR3& center, float radius)
{
    return g_FOVClip.MeshFOVCheck(&center, radius);
}

void CUpdateArgs::SetViewRect(const D3DXVECTOR4& value)
{
    mViewRect = value;
}

void CUpdateArgs::SetRectRange(const D3DXVECTOR4& value)
{
    mRectRange = value;

    //mGoViewRect.x = mRectRange.x - 10.0f;
    //mGoViewRect.y = mRectRange.z + 10.0f;
    //mGoViewRect.z = mRectRange.x + 10.0f;
    //mGoViewRect.w = mRectRange.z - 10.0f;
    mGoViewRect.x = mRectRange.x - 10.0f;
    mGoViewRect.y = mRectRange.y + 10.0f;
    mGoViewRect.z = mRectRange.z + 10.0f;
    mGoViewRect.w = mRectRange.w - 10.0f;
}

bool CUpdateArgs::IsInGoViewRect(float x, float z) const
{
    return
    (
        (mGoViewRect.x < x) &&
        (mGoViewRect.y > z) &&
		(mGoViewRect.z > x) &&
        (mGoViewRect.w < z)
    );
}

bool CUpdateArgs::IsInRectRange(float x, float z) const
{
    return
    (
        (mRectRange.x < x) &&
        (mRectRange.y > z) &&
        (mRectRange.z > x) &&
        (mRectRange.w < z)
    );
}

void CUpdateArgs::SetWidth(float value)
{
    mWidth = value;
}

float CUpdateArgs::GetWidth() const
{
    return mWidth;
}

void CUpdateArgs::SetHeight(float value)
{
    mHeight = value;
}

float CUpdateArgs::GetHeight() const
{
    return mHeight;
}

void CUpdateArgs::SetRay(const RAY& ray)
{
    mRay = ray;
}

const RAY& CUpdateArgs::GetRay() const
{
    return mRay;
}

const D3DXMATRIX& CUpdateArgs::GetViewMatrix() const
{
    return mViewMatrix;
}

void CUpdateArgs::SetViewMatrix(const D3DXMATRIX& value)
{
    mViewMatrix = value;
}

float CUpdateArgs::GetViewDistance() const
{
    return mViewDistance;
}

void CUpdateArgs::SetViewDistance(float value)
{
    mViewDistance = value;
}

//void CUpdateArgs::SetInvViewMatrix(const D3DXMATRIX& value)
//{
//    mInvViewMatrix = value;
//}

//const D3DXMATRIX& CUpdateArgs::GeInvtViewMatrix() const
//{
//    return mInvViewMatrix;
//}

} // namespace FDO