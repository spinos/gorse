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
    void addGravity(const Vector3F &g);
    void projectPosition(const float dt);
    void updateVelocityAndPosition(const float dt);
    void dampVelocity(const float damping);
    
    void setProjectedPositions(const Vector3F *x);
    
    const int &numParticles() const;
    const Vector3F *positions() const;
    const Vector3F *projectedPositions() const;
    const Vector3F *velocities() const;
    const float *inversedMasses() const;
    const int &capacity() const;
    
protected:

private:

    struct AParticle;
    void addParticle(const AParticle &x);
    
};
    
}

#endif
