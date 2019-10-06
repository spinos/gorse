/*
 *  SampleEmitter.cpp
 *  pbd
 *
 *  2019/10/7
 */
 
#include "SampleEmitter.h"
#include <math/BoundingBox.h>
#include <math/Constant.h>
#include <smp/SurfaceSample.h>
#include <sds/SpaceFillingVector.h>
#include <sds/FZOrder.h>
#include <math/roundi.h>

namespace alo {
    
struct SampleEmitter::Impl {
    
    sds::SpaceFillingVector<SurfaceSample> *_samples;
    sds::FZOrderCurve _sfc;
    int _level;
    
};

SampleEmitter::SampleEmitter() : m_pimpl(new Impl)
{}

void SampleEmitter::setSamples(sds::SpaceFillingVector<SurfaceSample> *pnts)
{ m_pimpl->_samples = pnts; }

void SampleEmitter::setSampleBox(const BoundingBox &b, const float h)
{
    const Vector3F midP = b.center();
    int spanI = b.getLongestDistance();
    
	const float spanL = Round16(spanI);
	m_pimpl->_sfc.setCoord(midP.x, midP.y, midP.z, spanL * .5f);
    
    int l=4;
    for(;l<9;++l) {
        if(h * (1<<l) >= spanL) break;
    }
    m_pimpl->_level = l;
}

void SampleEmitter::coreSetSeed(int x)
{}

void SampleEmitter::coreUpdate()
{
    sds::SpaceFillingVector<SurfaceSample> &pnts = * m_pimpl->_samples;
    pnts.createSFCAtLevel<sds::FZOrderCurve>(m_pimpl->_sfc, m_pimpl->_level);
    
    sds::SpaceFillingVector<SurfaceSample> aggd = pnts.aggregatTo();
    const int n = aggd.size();
            
    createParticles(n);
    
    for(int i=0;i<n;++i) {
        setPosition(aggd[i]._pos, i);
        setVelocity(Vector3F::Zero, i);
        setMass(Constant::VeryHigh, i);
    }
}
    
}
