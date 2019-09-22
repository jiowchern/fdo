/**
 * @file ParticleTemplate.h
 * @author yuming
 * @brief This file only has memory data, it was not a real particle template.
 * @todo Need someone to fix it!
 */
#ifndef ParticleTemplateH
#define ParticleTemplateH

#include "ParticlePreRequest.h"
#include "FdoString.h"

namespace FDO
{

class ParticleTemplate
{
public:
    ParticleTemplate(ParticleSystemPtr particleSystem);
    ~ParticleTemplate(void);

    //void SetFileName(const fstring& fileName);
    //const fstring& GetFileName() const;
    //bool LoadFromFile(const fstring& fileName);
    bool LoadFromFileInMemory(unsigned char* data, unsigned int size);
    ParticleInstPtr CreateInstance();
    ParticleSystemPtr GetParticleSystem() const;

private:
    //fstring mFileName;
    std::vector<unsigned char> mData;
    ParticleSystemPtr mParticleSystem;
};

} // namespace FDO

#endif // ParticleTemplateH
