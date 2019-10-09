/*
 *  UniformGridCollisionSolver.h
 *  aloe
 *
 *  2019/10/7
 */
 
#ifndef ALO_UNIFORM_GRID_COLLISION_SOLVER_H
#define ALO_UNIFORM_GRID_COLLISION_SOLVER_H

#include <boost/shared_ptr.hpp>

namespace alo {
    
struct Int2;
class Vector3F;
class BoundingBox;
class ParticleSystem;
    
class UniformGridCollisionSolver {
    
public:

    UniformGridCollisionSolver();
    
    void addParticles(ParticleSystem *x);
    
    void setCellSize(const float x);
    void setGridBox(const BoundingBox &b);
    
    void resolveCollision(const bool lazy = false);
    
    int totalNumParticles() const;
    int numParticleSystems() const;
    
protected:

private:

    ParticleSystem *particles(const int i);
    
    void mapParticles(const int isys, const int offset);
                    
    void collideParticles(const int isys, const int offset);
    void collideParticlesInCells(Vector3F &force,
                    const Vector3F &pos, const Vector3F &vel,
                    const int ind) const;
	void updateVelocities(const int isys);
                    
    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
    
};
    
}

#endif
