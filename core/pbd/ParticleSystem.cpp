/*
 *  ParticleSystem.cpp
 *  pbd
 *
 *  2019/9/30
 */
 
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

namespace alo {
    
struct ParticleSystem::Impl {
    
    int _numParticles;
    SimpleBuffer<Vector3F> _pos;
    SimpleBuffer<Vector3F> _vel;
    SimpleBuffer<float> _invMass;
    
    Impl() : _numParticles(0) {}
    
};

struct ParticleSystem::AParticle {
    Vector3F _pos;
    Vector3F _vel;
    float _mass;
};

ParticleSystem::ParticleSystem() : m_pimpl(new Impl)
{}

const int &ParticleSystem::numParticles() const
{ return m_pimpl->_numParticles; }

const Vector3F *ParticleSystem::positions() const
{ return m_pimpl->_pos.c_data(); }

const Vector3F *ParticleSystem::velocities() const
{ return m_pimpl->_vel.c_data(); }

void ParticleSystem::create(const ParticleEmitter &emitter)
{
    AParticle a;
    for(int i=0;i<emitter.numParticles();++i) {
        a._pos = emitter.positions()[i];
        a._vel = emitter.velocities()[i];
        a._mass = emitter.masses()[i];
        addParticle(a);
    }
}

void ParticleSystem::addParticle(const AParticle &x)
{
    m_pimpl->_numParticles++;
    m_pimpl->_pos << x._pos;
    m_pimpl->_vel << x._vel;
    m_pimpl->_invMass << (1.f / x._mass);
}

}
