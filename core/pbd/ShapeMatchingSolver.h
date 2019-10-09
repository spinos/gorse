/*
 *  ShapeMatchingSolver.h
 *  aloe
 *
 *  2019/10/6
 */
 
#ifndef ALO_SHAPE_MATCHING_SOLVER_H
#define ALO_SHAPE_MATCHING_SOLVER_H

#include <boost/shared_ptr.hpp>

namespace alo {
    
class Vector3F;
class BoundingBox;
class RegionEmitter;
class ParticleSystem;
    
class ShapeMatchingSolver {
  
public:

    ShapeMatchingSolver();
    
    void clearRegions();
    void addRegions(const RegionEmitter &emitter, const int &offset);
    
    void setParticles(ParticleSystem *particles);
    void createConstraints();
    void setStiffness(const float &x);
    
    void applyPositionConstraint();
    void applyGravity(const Vector3F &x);
    void applyDamping(const float &x);
    void projectPosition(const float &dt);
    void integrate(const float &dt);
    
    const BoundingBox &aabb() const;
    
protected:
    
    ParticleSystem *particles();
    
private:

    int systemDimension() const;
    void addStiffness(const int v1, const int v2);

    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
};

}

#endif
