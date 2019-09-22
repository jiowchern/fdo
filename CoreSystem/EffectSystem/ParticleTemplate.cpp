/**
 * @file ParticleTemplate.cpp
 * @author yuming
 */
#include "StdAfx.h"
#include "ParticleTemplate.h"
#include "FDO_ParticleObject.h"

namespace FDO
{

ParticleTemplate::ParticleTemplate(ParticleSystemPtr particleSystem)
: mParticleSystem(particleSystem)
{
}

ParticleTemplate::~ParticleTemplate(void)
{
    mParticleSystem = NULL;
}

//void ParticleTemplate::SetFileName(const fstring& fileName)
//{
//    mFileName = fileName;
//}

//const fstring& ParticleTemplate::GetFileName() const
//{
//    return mFileName;
//}

ParticleSystemPtr ParticleTemplate::GetParticleSystem() const
{
    return mParticleSystem;
}

ParticleInstPtr ParticleTemplate::CreateInstance()
{
    if (mData.empty())
    {
        return NULL;
    }

    ParticleInstPtr Result = new ParticleObject();
    if (Result == NULL)
    {
        return NULL;
    }

    if (!Result->LoadFromFileInMemory(&mData.front(), (UINT)mData.size()))
    {
        delete Result;
        return NULL;
    }

    Result->SetControlEnabled(PARTICLE_FLAG::INTERNAL_UPDATE, (Result->GetPRenderPoolSize() == PRPS_INTERNAL_UPDATE));
    Result->SetControlEnabled(PARTICLE_FLAG::FOG_EFFECT, (Result->GetPRenderPoolSize() == PRPS_FOG_EFFECT));
    Result->SetControlEnabled(PARTICLE_FLAG::FORCE_VISIBLE, (Result->GetPRenderPoolSize() == PRPS_FORCE_VISIBLE));
    Result->SetControlEnabled(PARTICLE_FLAG::FAR_VISIBLE, (Result->GetPRenderPoolSize() == PRPS_FAR_VISIBLE));

    return Result;
}

//bool ParticleTemplate::LoadFromFile(const fstring& fileName)
//{
//    MemPackPtr stream = LoadFromPacket(fileName);
//
//    F_RETURN(stream != NULL);
//
//    F_RETURN(stream->IsValid());
//
//    F_RETURN(this->LoadFromFileInMemory(stream->Data, stream->DataSize));
//
//    this->SetFileName(fileName);
//
//    return true;
//}

bool ParticleTemplate::LoadFromFileInMemory(unsigned char* data, unsigned int size)
{
    F_RETURN(data != NULL);

    F_RETURN(size != 0);

    mData.assign(data, data + size);

    F_RETURN(!mData.empty());

    return true;
}

} // namespace FDO