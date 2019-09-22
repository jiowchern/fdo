/**
 * @file ParticlePreRequest.h
 * @author Yuming Ho
 */

#ifndef ParticlePreRequestH
#define ParticlePreRequestH

#include <boost\shared_ptr.hpp>
#include <d3d9.h>
#include <d3dx9.h>
#include <list>

class R_3DInterface;
class R_EmitterPlayer;
class R_EmitterManager;
class R_ParticleRender;
class R_ParticleEmitter;
class R_Particle;
class R_PRenderManager;

//typedef boost::shared_ptr<R_EmitterPlayer> R_EmitterPlayerPtr;
typedef R_EmitterPlayer *R_EmitterPlayerPtr;
typedef R_3DInterface *R_3DInterfacePtr;
typedef R_EmitterManager *R_EmitterManagerPtr;
typedef R_ParticleRender *R_ParticleRenderPtr;
typedef R_ParticleEmitter *R_ParticleEmitterPtr;
typedef R_Particle *R_ParticlePtr;
typedef R_PRenderManager* R_PRenderManagerPtr;

namespace FDO
{
    class ParticleSystem;
    class ParticleObject;
    class ParticleTemplate;

    typedef ParticleSystem* ParticleSystemPtr;
    typedef ParticleTemplate ParticleTemp, *ParticleTempPtr;
    typedef ParticleObject ParticleInst, *ParticleInstPtr;

    typedef std::list<ParticleInstPtr> ParticleList;
    typedef ParticleList::iterator ParticleListIterator;
    typedef ParticleList::const_iterator ConstParticleListIterator;
    // particle render pool size
    const unsigned short PRPS_INTERNAL_UPDATE = 23;
    const unsigned short PRPS_FORCE_VISIBLE = 24;
    const unsigned short PRPS_FAR_VISIBLE = 25;
    const unsigned short PRPS_FOG_EFFECT = 26;

    const float PARTICLE_RADIUS = 10.f;
    //const unsigned int MAX_PARTICLE_COUNT = 80;
    //const D3DXVECTOR3 g_ParticleScale(2.0f, 2.0f, 2.0f);
    //const D3DXVECTOR3 g_vNegativeZDir(0.0f, 0.0f, -1.0f);
    //const D3DXVECTOR3 g_vPositiveZDir(0.0f, 0.0f, 1.0f);
    namespace PARTICLE_FLAG
    {
        enum E
        {
            FORCE_VISIBLE   = 0x00000001,
            INTERNAL_UPDATE = 0x00000002,
            FAR_VISIBLE     = 0x00000004,
            FOG_EFFECT      = 0x00000008,
            STATIC_RESOURCE = 0x00000010
        };
    }

    const char PARTICLE_SOUND_PATH[] = "sound\\";

    typedef std::map<fstring, ParticleTempPtr> ParticleTempMap;
    typedef ParticleTempMap::iterator ParticleTempMapIterator;
    typedef ParticleTempMap::const_iterator ConstParticleTempMapIterator;

    typedef std::map<fstring, LPDIRECT3DTEXTURE9> TextureMap;
    typedef TextureMap::iterator TextureMapIterator;
    typedef TextureMap::const_iterator ConstTextureMapIterator;

    typedef std::map<fstring, LPD3DXMESH> MeshMap;
    typedef MeshMap::iterator MeshMapIterator;
    typedef MeshMap::const_iterator ConstMeshMapIterator;
} // namespace FDO

#endif // ParticlePreRequestH