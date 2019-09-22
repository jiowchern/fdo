#include "stdafx.h"
#include "FOVClip.h"
//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
void FOVClip::SetViewMatrix( const D3DMATRIX *pMatrix )
{
    matView = *pMatrix;
}

//---------------------------------------------------------------------------
// 建立視野平面
//---------------------------------------------------------------------------
void FOVClip::SetupFOVClipPlanes( const float Angle, const float Aspect,
      const float zn, const float zf )
{
    FrontClip = zn;
    BackClip  = zf;

    D3DXVECTOR3	P0 , P1 , P2;

//	Calculate left plane using 3 points
	P0.x =  0.0f;
	P0.y =  0.0f;
	P0.z =  0.0f;
	P1.x = -BackClip * ( (float)tan( Angle * 0.5f ) / Aspect );
	P1.y = -BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z =  BackClip;
	P2.x =  P1.x;
	P2.y = -P1.y;
	P2.z =  P1.z;
	D3DXPlaneFromPoints( &FOVLeftPlane, &P0, &P1, &P2 );

//	Calculate right plane using 3 points
	P0.x =  0.0f;
	P0.y =  0.0f;
	P0.z =  0.0f;
	P1.x =  BackClip * ( (float)tan( Angle * 0.5f ) / Aspect);
	P1.y =  BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z =  BackClip;
	P2.x =  P1.x;
	P2.y = -P1.y;
	P2.z =  P1.z;
	D3DXPlaneFromPoints( &FOVRightPlane, &P0, &P1, &P2 );

//	Calculate top plane using 3 points
	P0.x =  0.0f;
	P0.y =  0.0f;
	P0.z =  0.0f;
	P1.x = -BackClip * ( (float)tan( Angle * 0.5f ) / Aspect);
	P1.y =  BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z =  BackClip;
	P2.x = -P1.x;
	P2.y =  P1.y;
	P2.z =  P1.z;
	D3DXPlaneFromPoints( &FOVTopPlane, &P0, &P1, &P2 );

//	Calculate bottom plane using 3 points
	P0.x =  0.0f;
	P0.y =  0.0f;
	P0.z =  0.0f;
	P1.x =  BackClip * ( (float)tan( Angle * 0.5f ) / Aspect);
	P1.y = -BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z =  BackClip;
	P2.x = -P1.x;
	P2.y =  P1.y;
	P2.z =  P1.z;
	D3DXPlaneFromPoints( &FOVBottomPlane, &P0, &P1, &P2 );
}

//---------------------------------------------------------------------------
// 確認模型是否在視野範圍內
//---------------------------------------------------------------------------
bool FOVClip::MeshInBounding(const D3DXVECTOR3 &minBox , const D3DXVECTOR3 &maxBox, const D3DXVECTOR4 &range)
{
	if(minBox.x < range.z && range.x < maxBox.x &&
       minBox.z < range.y && range.w < maxBox.z)
	{
		return true ; 
	}

	return false ; 
}

bool FOVClip::MeshFOVCheck( const D3DXVECTOR3 *pBSpherePos,
      const float BSphereRadius )
{
	float       Dist;
    D3DXVECTOR3 ViewPos;

//	Transform Z into view space
	ViewPos.z = matView._13 * pBSpherePos->x +
		matView._23 * pBSpherePos->y +
		matView._33 * pBSpherePos->z + matView._43;

//	Behind front clip plane?
	if( ( ViewPos.z + BSphereRadius ) < FrontClip )
		return false;

//	Beyond the back clip plane?
	if( ( ViewPos.z - BSphereRadius ) > BackClip )
		return false;

//	Transform X into view space
	ViewPos.x = matView._11 * pBSpherePos->x +
		matView._21 * pBSpherePos->y +
		matView._31 * pBSpherePos->z + matView._41;

//	Test against left clip plane
	Dist = ( ViewPos.x * FOVLeftPlane.a ) + ( ViewPos.z * FOVLeftPlane.c );
	if( Dist > BSphereRadius )
		return false;

//	Test against right clip plane
	Dist = ( ViewPos.x * FOVRightPlane.a ) + ( ViewPos.z * FOVRightPlane.c );
	if( Dist > BSphereRadius )
		return false;

//	Transform Y into view space
	ViewPos.y = matView._12 * pBSpherePos->x +
		matView._22 * pBSpherePos->y +
		matView._32 * pBSpherePos->z + matView._42;

//	Test against top clip plane
	Dist = ( ViewPos.y * FOVTopPlane.b ) + ( ViewPos.z * FOVTopPlane.c );
	if( Dist > BSphereRadius )
		return false;

//	Test against bottom plane
	Dist = ( ViewPos.y * FOVBottomPlane.b ) + ( ViewPos.z * FOVBottomPlane.c);
	if( Dist > BSphereRadius )
		return false;

//	Mesh is inside the field of view
	return true;
}
//---------------------------------------------------------------------------
} // namespace FDO
