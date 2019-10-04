/*
 *  ParticleEmitter.h
 *  pbd
 *
 *  2019/9/30
 */
 
#ifndef ALO_PARTICLE_EMITTER_H
#define ALO_PARTICLE_EMITTER_H

#include <boost/shared_ptr.hpp>

namespace alo {
    
class Vector3F;

class ParticleEmitter {
    
    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
  
public:

    void setSeed(int x);
    void update();
    
    const int &numParticles() const;
    const Vector3F *positions() const;
    const Vector3F *velocities() const;
    const float *masses() const;
    
protected:

    ParticleEmitter();
    virtual ~ParticleEmitter();

    void createParticles(int n);
    void setPosition(const Vector3F &x, const int &i);
    void setVelocity(const Vector3F &x, const int &i);
    void setMass(const float &x, const int &i);
    
private:
    
    virtual void coreSetSeed(int x) = 0;
    virtual void coreUpdate() = 0;
    
};
    
}

#endif
