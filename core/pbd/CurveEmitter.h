/*
 *  CurveEmitter.h
 *  pbd
 *
 *  2019/10/6
 */
 
#ifndef ALO_CURVE_EMITTER_H
#define ALO_CURVE_EMITTER_H

#include "ParticleEmitter.h"
#include "RegionEmitter.h"

namespace alo {

class CurveEmitter : public ParticleEmitter, public RegionEmitter {
    
    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
    
public:

    CurveEmitter();
    
    void begin();
    void end();
    void operator<<(const Vector3F &x);

protected:

private:
    
    virtual void coreSetSeed(int x);
    virtual void coreUpdate();
    
};
    
}

#endif