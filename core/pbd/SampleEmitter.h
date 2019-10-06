/*
 *  SampleEmitter.h
 *  pbd
 *
 *  2019/10/7
 */
 
#ifndef ALO_SAMPLE_EMITTER_H
#define ALO_SAMPLE_EMITTER_H

#include "ParticleEmitter.h"

namespace alo {
    
class BoundingBox;
struct SurfaceSample;

namespace sds {
template<typename T>
class SpaceFillingVector;
}

class SampleEmitter : public ParticleEmitter {
    
    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
    
public:

    SampleEmitter();
    
    void setSamples(sds::SpaceFillingVector<SurfaceSample> *pnts);
    void setSampleBox(const BoundingBox &b, const float h);
    
protected:

private:
    
    virtual void coreSetSeed(int x);
    virtual void coreUpdate();
    
};
    
}

#endif