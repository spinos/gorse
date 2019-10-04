/*
 *  ParticleSystem.h
 *  pbd
 *
 *  2019/9/30
 */
 
#ifndef ALO_PARTICLE_SYSTEM_H
#define ALO_PARTICLE_SYSTEM_H

#include <boost/shared_ptr.hpp>

namespace alo {
    
class Vector3F;
class ParticleEmitter;
    
class ParticleSystem {
    
    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
  
public:

    ParticleSystem();
    
    void create(const ParticleEmitter &emitter);
    
    const int &numParticles() const;
    const Vector3F *positions() const;
    const Vector3F *velocities() const;
    
protected:

private:

    struct AParticle;
    void addParticle(const AParticle &x);
    
};
    
}

#endif
