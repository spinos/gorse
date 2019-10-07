/*
 *  UniformGridCollisionSolver.cpp
 *  aloe
 *
 *  2019/10/7
 */
 
#include "UniformGridCollisionSolver.h"
#include <sds/UniformGridHash.h>
#include "ParticleSystem.h"
#include <math/BoundingBox.h>
#include <math/Int3.h>
#include <vector>

namespace alo {
    
struct UniformGridCollisionSolver::Impl {
    
    std::vector<ParticleSystem *> _parts;
    sds::UniformGridHash _grid;
    
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

void UniformGridCollisionSolver::resolveCollision()
{
    sds::UniformGridHash &grid = m_pimpl->_grid;
    grid.setTableSize(totalNumParticles());
    
    int offset = 0;
    const int n = numParticleSystems();
    for(int i=0;i<n;++i) {
        mapParticles(i, offset);
        offset += particles(i)->numParticles();
    }
    
    grid.findCellStarts();
    
    offset = 0;
    for(int i=0;i<n;++i) {
        collideParticles(i, offset);
        offset += particles(i)->numParticles();
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

void UniformGridCollisionSolver::collideParticles(const int isys,
                                const int offset)
{
    ParticleSystem *psys = m_pimpl->_parts[isys];
    if(!psys->isDynamic()) return;
    
    const sds::UniformGridHash &grid = m_pimpl->_grid;
    const Vector3F *pos = psys->positions();
    const Vector3F *vel = psys->velocities();
    const int &n = psys->numParticles();
    for(int i=0;i<n;++i) {
        Vector3F collideForce(0.f, 0.f, 0.f);
        const Int3 c = grid.calculateCellCoord(pos[i]);
        for(int j=0;j<27;++j) {
            const Int3 nc = grid.getOffsetCellCoord(c, j);
            const int cellStart = grid.getCellStart(nc);
            if(cellStart < 0) continue;
            collideParticlesInCell(collideForce, pos[i], vel[i], cellStart, offset + i);
        }
    }
}

void UniformGridCollisionSolver::collideParticlesInCell(Vector3F &force,
                    const Vector3F &pos, const Vector3F &vel,
                    const int cellStart, const int tableIdx)
{
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

int UniformGridCollisionSolver::encodeParticle(const int sys, const int i)
{ return (sys << 20) | i; }

void UniformGridCollisionSolver::decodeParticle(int &sys, int &i, const int x)
{
    i = x & 1048575;
    sys = x >> 20;
}
    
}
