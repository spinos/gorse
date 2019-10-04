/*
 *  ParticleEmitter.cpp
 *  pbd
 *
 *  2019/9/30
 */
 
#include "ParticleEmitter.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

namespace alo {
    
struct ParticleEmitter::Impl {
    
    int _numParticles;
    SimpleBuffer<Vector3F> _pos;
    SimpleBuffer<Vector3F> _vel;
    SimpleBuffer<float> _mass;
    
    Impl() : _numParticles(0)
    {}
    
};
    
ParticleEmitter::ParticleEmitter() : m_pimpl(new Impl)
{}

ParticleEmitter::~ParticleEmitter()
{}

void ParticleEmitter::setSeed(int x)
{ coreSetSeed(x); }

void ParticleEmitter::update()
{ coreUpdate(); }

const int &ParticleEmitter::numParticles() const
{ return m_pimpl->_numParticles; }

const Vector3F *ParticleEmitter::positions() const
{ return m_pimpl->_pos.c_data(); }

const Vector3F *ParticleEmitter::velocities() const
{ return m_pimpl->_vel.c_data(); }

const float *ParticleEmitter::masses() const
{ return m_pimpl->_mass.c_data();}

void ParticleEmitter::createParticles(int n)
{
    m_pimpl->_numParticles = n;
    m_pimpl->_pos.resetBuffer(n);
    m_pimpl->_vel.resetBuffer(n);
    m_pimpl->_mass.resetBuffer(n);
}

void ParticleEmitter::setPosition(const Vector3F &x, const int &i)
{ m_pimpl->_pos[i] = x; }

void ParticleEmitter::setVelocity(const Vector3F &x, const int &i)
{ m_pimpl->_vel[i] = x; }
    
void ParticleEmitter::setMass(const float &x, const int &i)
{ m_pimpl->_mass[i] = x; }

}
