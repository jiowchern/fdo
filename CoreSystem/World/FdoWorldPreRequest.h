/**
 * @file FdoWorldPreRequest.h
 * @author yuming
 * @brief This file was include by another header file
 */
#ifndef FdoWorldPreRequestH
#define FdoWorldPreRequestH

#include <boost\shared_ptr.hpp>
#include <vector>
#include <list>
#include <d3d9.h>
#include <d3dx9.h>
#include "FdoString.h"
#include "FdoMath.h"

namespace FDO
{
    namespace DRAWKIND
    {
        enum E
        {
            TERRAIN,
            MODEL,
            PARTICLE,
            OCEAN,
            TRANSPARENCY,
            FOG_PARTICLE,
            COUNT
        };
    }

    class SCN;
    class CDrawable;
    class CUpdateArgs;
    /// This is skin mesh template
    class JLSkinMesh;
    /// This is skin mesh instance
    class HModel;
    class Terrain;
    class JLSkyBox;
    class Ocean;
    class CCollisionMap;
    class CAltitudeMap;

    typedef JLSkyBox    SKYBOX;
    typedef JLSkinMesh  SKINMESHTEMP;
    typedef HModel      SKINMESHINST;
    typedef Terrain     TERRAIN;
    typedef Ocean       OCEAN;
    //typedef boost::shared_ptr<SKINMESHTEMP> SkinMeshTempPtr;
    //typedef boost::shared_ptr<SKINMESHINST> SkinMeshInstPtr;
    typedef SKINMESHTEMP *SkinMeshTempPtr;
    typedef SKINMESHINST *SkinMeshInstPtr;
    typedef boost::shared_ptr<CUpdateArgs>  UpdateArgsPtr;
    //typedef boost::shared_ptr<CDrawable>    DrawablePtr;
    typedef boost::shared_ptr<SKYBOX>       SkyBoxPtr;
    //typedef boost::shared_ptr<TERRAIN>      TerrainPtr;
    typedef TERRAIN *TerrainPtr;
    typedef CDrawable *DrawablePtr;
    typedef SCN *SCNPtr;
    typedef OCEAN *OceanPtr;
    typedef boost::shared_ptr<CCollisionMap> CollisionMapPtr;
    typedef boost::shared_ptr<CAltitudeMap> AltitudeMapPtr;

    typedef std::vector<DrawablePtr>         DRAWLIST;
    typedef std::vector<SCNPtr>               SCNLIST;

    const int CELL_SPACING = 8;

    namespace MODEL_FLAG
    {
        enum E
        {
            TERRAIN            = 0x00000001,
            IS_TRANSPARENT     = 0x00000002,
            ALWAYS_TRANSPARENT = 0x00000004,
            WITHOUT_ALPHATEST  = 0x00000008
        };
    }

    typedef struct SImmediateVertex
    {
        D3DXVECTOR3 position;
        DWORD color;
        D3DXVECTOR2 uv;
    } ImmediateVertex, *ImmediateVertexPtr;
    const DWORD fvfof_ImmediateVertex = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
    const unsigned int sizeof_ImmediateVertex = sizeof(ImmediateVertex);
    typedef std::vector<ImmediateVertex> ImmediateVertexList;
} // namespace FDO

#endif // FdoWorldPreRequestH