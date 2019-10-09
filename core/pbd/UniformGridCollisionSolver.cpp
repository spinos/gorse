/*
 *  UniformGridCollisionSolver.cpp
 *  aloe
 *
 *  2019/10/7
 */
 
#include "UniformGridCollisionSolver.h"
#include <sds/UniformGridHash.h>
#include "SphereCollision.h"
#include "ParticleSystem.h"
#include <math/SimpleBuffer.h>
#include <math/BoundingBox.h>
#include <math/Int3.h>
#include <math/Int2.h>
#include <math/Absolute.h>
#include <vector>
#include <boost/thread.hpp>
#include <boost/chrono/include.hpp>

namespace alo {
    
struct UniformGridCollisionSolver::Impl {
    
    std::vector<ParticleSystem *> _parts;
    sds::UniformGridHash _grid;
    SphereCollision _sphereToSphere;
    SimpleBuffer<Vector3F> _pos;
    SimpleBuffer<Vector3F> _vel;
    
};

UniformGridCollisionSolver::UniformGridCollisionSolver() :
m_pimpl(new Impl)
{}

void UniformGridCollisionSolver::addParticles(ParticleSystem *x)
{ m_pimpl->_parts.push_back(x); }

void UniformGridCollisionSolver::setCellSize(const float x)
{ m_pimpl->_grid.setCellSize(x); }

void UniformGridCollisionSolver::setGridBox(const BoundingBox &b)
{ m_pimpl->_grid.setGridBox(b); }

void UniformGridCollisionSolver::resolveCollision(const bool lazy)
{
    sds::UniformGridHash &grid = m_pimpl->_grid;
	const int n = numParticleSystems();
    const int np = totalNumParticles();
    if(m_pimpl->_pos.capacity() < np) {
        m_pimpl->_pos.resetBuffer(np);
        m_pimpl->_vel.resetBuffer(np);
    }
    
    int offset = 0;
    for(int i=0;i<n;++i) {
        const ParticleSystem *parti = particles(i);
        const int &ni = parti->numParticles();
        m_pimpl->_pos.copyFrom(parti->positions(), ni, offset);
        m_pimpl->_vel.copyFrom(parti->velocities(), ni, offset);
        offset += ni;
    }
    
	if(!lazy) {
    grid.setTableSize(np);
    
        offset = 0;
    
    for(int i=0;i<n;++i) {
        mapParticles(i, offset);
        offset += particles(i)->numParticles();
    }
    
    grid.findCellStarts();
	
	}
    
    offset = 0;
    for(int i=0;i<n;++i) {
        collideParticles(i, offset);
        offset += particles(i)->numParticles();
    }
	
	for(int i=0;i<n;++i) {
        updateVelocities(i);
    }
}

void UniformGridCollisionSolver::mapParticles(const int isys,
                                const int offset)
{
    const ParticleSystem *psys = m_pimpl->_parts[isys];
    sds::UniformGridHash &grid = m_pimpl->_grid;
    const int &n = psys->numParticles();
    for(int i=0;i<n;++i) {
        const int ind = offset + i;
        grid.mapPosition(m_pimpl->_pos[ind], ind, ind);
    }
}

void UniformGridCollisionSolver::collideParticles(const int isys,
                                const int offset)
{
    ParticleSystem *psys = m_pimpl->_parts[isys];
    if(!psys->isDynamic()) return;
    
    const sds::UniformGridHash &grid = m_pimpl->_grid;
    
    Vector3F collisionForce;

    const int &n = psys->numParticles();
    for(int i=0;i<n;++i) {
        const int ind = offset + i;
        collideParticlesInCells(collisionForce, m_pimpl->_pos[ind], m_pimpl->_vel[ind], ind);
        psys->setImpulse(collisionForce, i);

    }
}

void UniformGridCollisionSolver::collideParticlesInCells(Vector3F &force,
                    const Vector3F &pos, const Vector3F &vel,
                    const int ind) const
{
    force.setZero();
    const sds::UniformGridHash &grid = m_pimpl->_grid;
    
    const Int3 c = grid.calculateCellCoord(pos);

    for(int j=0;j<27;++j) {
        const Int3 nc = grid.getOffsetCellCoord(c, j);
        const Int2 cellRange = grid.getCellRange(nc);
        if(cellRange.x < 0) continue;
        
        for(int i = cellRange.x;i < cellRange.y;++i) {
            const int &idx = grid.indirection(i);
            if(Absolute<int>::f(idx - ind) < 2) continue;
       
            force += m_pimpl->_sphereToSphere.calculateForce(pos, m_pimpl->_pos[idx], 
                                    vel, m_pimpl->_vel[idx]);
        }
    }
}

void UniformGridCollisionSolver::updateVelocities(const int isys)
{
	ParticleSystem *psys = m_pimpl->_parts[isys];
    if(!psys->isDynamic()) return;
	
	psys->applyImpulse();
}

int UniformGridCollisionSolver::totalNumParticles() const
{
    int c = 0;
    std::vector<ParticleSystem *>::const_iterator it = m_pimpl->_parts.begin();
    for(;it!=m_pimpl->_parts.end();++it) {
        c += (*it)->numParticles();
    }
    return c;
}

int UniformGridCollisionSolver::numParticleSystems() const
{ return m_pimpl->_parts.size(); }

ParticleSystem *UniformGridCollisionSolver::particles(const int i)
{ return m_pimpl->_parts[i]; }
    
}
