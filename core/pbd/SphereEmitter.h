/*
 *  SphereEmitter.h
 *  pbd
 *
 *  2019/9/30
 */
 
#ifndef ALO_SPHERE_EMITTER_H
#define ALO_SPHERE_EMITTER_H

#include "ParticleEmitter.h"

namespace alo {

class SphereEmitter : public ParticleEmitter {
    
    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
    
public:

    SphereEmitter();

protected:

private:
    
    virtual void coreSetSeed(int x);
    virtual void coreUpdate();
    
};
    
}

#endif