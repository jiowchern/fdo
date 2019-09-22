/**
 * @file ParticleHelper.h
 * @author Yuming Ho
 */
#ifndef ParticleHelperH
#define ParticleHelperH

#include "ParticlePreRequest.h"

namespace FDO
{
    bool RenderParticle(ParticleInstPtr particleInst , unsigned int &outflushedPartCount);
}

#endif // ParticleHelperH