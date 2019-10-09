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
#include <math/BoundingBox.h>
#include <math/Int3.h>
#include <math/Int2.h>
#include <vector>

namespace alo {
    
struct UniformGridCollisionSolver::Impl {
    
    std::vector<ParticleSystem *> _parts;
    sds::UniformGridHash _grid;
    SphereCollision _sphereToSphere;
    
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
	if(!lazy) {
    grid.setTableSize(totalNumParticles());
    
    int offset = 0;
    
    for(int i=0;i<n;++i) {
        mapParticles(i, offset);
        offset += particles(i)->numParticles();
    }
    
    grid.findCellStarts();
	
	}
    
    for(int i=0;i<n;++i) {
        collideParticles(i);
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
    const Vector3F *pos = psys->positions();
    const int &n = psys->numParticles();
    for(int i=0;i<n;++i) {
        const int ind = encodeParticle(isys, i);
        grid.mapPosition(pos[i], ind, offset + i);
    }
}

void UniformGridCollisionSolver::collideParticles(const int isys)
{
    ParticleSystem *psys = m_pimpl->_parts[isys];
    if(!psys->isDynamic()) return;
    
    const sds::UniformGridHash &grid = m_pimpl->_grid;
    const Vector3F *pos = psys->positions();
    const Vector3F *vel = psys->velocities();
    const int &n = psys->numParticles();
    for(int i=0;i<n;++i) {
        const int ind = encodeParticle(isys, i);
        Vector3F collisionForce(0.f, 0.f, 0.f);
        const Int3 c = grid.calculateCellCoord(pos[i]);
        for(int j=0;j<27;++j) {
            const Int3 nc = grid.getOffsetCellCoord(c, j);
            const Int2 cellR = grid.getCellRange(nc);
            if(cellR.x < 0) continue;
            collideParticlesInCell(collisionForce, pos[i], vel[i], cellR, ind);
        }
        
        psys->setImpulse(collisionForce, i);
    }
}

void UniformGridCollisionSolver::collideParticlesInCell(Vector3F &force,
                    const Vector3F &pos, const Vector3F &vel,
                    const Int2 &cellRange, const int ind) const
{
    const sds::UniformGridHash &grid = m_pimpl->_grid;
    Vector3F pos2, vel2;
    int sys2, i2;
    for(int i = cellRange.x;i < cellRange.y;++i) {
        const int &idx = grid.indirection(i);
        if(idx==ind) continue;
        
        decodeParticle(sys2, i2, idx);
        
        readParticle(pos2, vel2, sys2, i2);
        
        force += m_pimpl->_sphereToSphere.calculateForce(pos, pos2, vel, vel2);
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

void UniformGridCollisionSolver::readParticle(Vector3F &pos, Vector3F &vel,
                    const int isys, const int i) const
{
    const ParticleSystem *psys = m_pimpl->_parts[isys];
    pos = psys->positions()[i];
    vel = psys->velocities()[i];
}

int UniformGridCollisionSolver::encodeParticle(const int sys, const int i)
{ return (sys << 21) | i; }

void UniformGridCollisionSolver::decodeParticle(int &sys, int &i, const int x)
{
    i = x & 2097151;
    sys = x >> 21;
}
    
}
