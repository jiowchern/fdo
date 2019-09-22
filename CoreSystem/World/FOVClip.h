//---------------------------------------------------------------------------
// field of view clip
//---------------------------------------------------------------------------
#ifndef _FOV_CLIP_H_
#define _FOV_CLIP_H_
//---------------------------------------------------------------------------
#include "singleton.h"
#include <d3dx9math.h>
//---------------------------------------------------------------------------
#define g_FOVClip FOVClip::GetSingleton()
//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
class FOVClip : public Singleton<FOVClip>
{
private:

    float       FrontClip;
    float       BackClip;

    D3DXMATRIX  matView;
    D3DXPLANE	FOVLeftPlane;
    D3DXPLANE	FOVRightPlane;
    D3DXPLANE	FOVTopPlane;
    D3DXPLANE	FOVBottomPlane;

public:

    void SetupFOVClipPlanes( const float Angle, const float Aspect,
        const float zn, const float zf );

    void SetViewMatrix( const D3DMATRIX *pMatrix );

    bool MeshFOVCheck( const D3DXVECTOR3 *pBSpherePos,
        const float BSphereRadius );

	bool MeshInBounding(const D3DXVECTOR3 &minBox , const D3DXVECTOR3 &maxBox , const D3DXVECTOR4 &range) ; 
	

};
//---------------------------------------------------------------------------
} // namespace FDO
//---------------------------------------------------------------------------
#endif
