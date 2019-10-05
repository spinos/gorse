/*
 *  SphereEmitter.cpp
 *  pbd
 *
 *  2019/9/30
 */
 
#include "SphereEmitter.h"
#include <math/Vector3F.h>
#include <rng/Uniform.h>
#include <rng/Lehmer.h>
#include <rng/Halton.h>
#include <math/Constant.h>
#include <math/Spherical.h>

namespace alo {
    
struct SphereEmitter::Impl {
    
    boost::shared_ptr<Uniform<Lehmer> > _rng;
    Vector3F _center;
    float _radius;
    int _minNum;
    int _maxNum;
    
    Impl() : _rng(new Uniform<Lehmer>(654321)),
    _center(0.f, 20.f, 0.f),
    _radius(10.f),
    _minNum(200),
    _maxNum(300)
    {}
    
    int randNum()
    { return _minNum + (_maxNum - _minNum) * _rng->randf1(); }
    
    float randR()
    { return _radius * _rng->randf(.1f, 1.f, 1024); }
    
};

SphereEmitter::SphereEmitter() : m_pimpl(new Impl)
{}

void SphereEmitter::coreSetSeed(int x)
{ m_pimpl->_rng.reset(new Uniform<Lehmer>(x)); }

void SphereEmitter::coreUpdate()
{
    const int n = m_pimpl->randNum();
            
    createParticles(n);
    
    for(int i=0;i<n;++i) {
        const float* h2 = Halton::Rand2<Uniform<Lehmer> >(m_pimpl->_rng.get());
        const float &phi = h2[0] * Constant::PI;
        const float &theta = h2[1] * Constant::PI2;
        const float r = m_pimpl->randR();
        const Vector3F v = Spherical::toCartesian(r, phi, theta);
        setPosition(m_pimpl->_center + v, i);
        setVelocity(v.normal(), i);
        setMass(1.f, i);
    }
}
    
}
