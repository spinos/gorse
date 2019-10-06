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
#include <math/Constant.h>

namespace alo {
    
struct ParticleSystem::Impl {
    
    SimpleBuffer<Vector3F> _pos;
    SimpleBuffer<Vector3F> _projPos;
    SimpleBuffer<Vector3F> _vel;
    SimpleBuffer<float> _invMass;
    
};

struct ParticleSystem::AParticle {
    Vector3F _pos;
    Vector3F _vel;
    float _mass;
};

ParticleSystem::ParticleSystem() : m_pimpl(new Impl)
{}

const int &ParticleSystem::numParticles() const
{ return m_pimpl->_pos.count(); }

const Vector3F *ParticleSystem::positions() const
{ return m_pimpl->_pos.c_data(); }

const Vector3F *ParticleSystem::projectedPositions() const
{ return m_pimpl->_projPos.c_data(); }

const Vector3F *ParticleSystem::velocities() const
{ return m_pimpl->_vel.c_data(); }

const float *ParticleSystem::inversedMasses() const
{ return m_pimpl->_invMass.c_data(); }

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
    m_pimpl->_pos << x._pos;
    m_pimpl->_projPos << x._pos;
    m_pimpl->_vel << x._vel;
    m_pimpl->_invMass << (1.f / x._mass);
}

const int &ParticleSystem::capacity() const
{ return m_pimpl->_pos.capacity(); }

void ParticleSystem::addGravity(const Vector3F &g)
{
    Vector3F *u = m_pimpl->_vel.data();
    const float *mi = inversedMasses();
    const int &n = numParticles();
    for(int i=0;i<n;++i) {
        if(mi[i] > Constant::VeryLow) u[i] += g;
    }
}

void ParticleSystem::projectPosition(const float dt)
{
    Vector3F *projX = m_pimpl->_projPos.data();
    const Vector3F *x = positions();
    const Vector3F *u = velocities();
    const int &n = numParticles();
    for(int i=0;i<n;++i) {
        projX[i] = x[i] + u[i] * dt;
    }
}

void ParticleSystem::updateVelocityAndPosition(const float dt)
{
    const float invDt = 1.f / dt;
    const Vector3F *projX = projectedPositions();
    Vector3F *x = m_pimpl->_pos.data();
    Vector3F *u = m_pimpl->_vel.data();
    const int &n = numParticles();
    for(int i=0;i<n;++i) {
        u[i] = (projX[i] - x[i]) * invDt;
    }
    
    m_pimpl->_pos.copyFrom(m_pimpl->_projPos);
}

void ParticleSystem::setProjectedPositions(const Vector3F *x)
{
    m_pimpl->_projPos.copyFrom(x, numParticles());
}

void ParticleSystem::dampVelocity(const float damping)
{
    const float d = 1.f - damping;
    const int &n = numParticles();
    for(int i=0;i<n;++i) {
        m_pimpl->_vel[i] *= d;
    }
}

}
