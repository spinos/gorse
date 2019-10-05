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
    
class RegionEmitter;
class ParticleSystem;
    
class ShapeMatchingSolver {
  
public:

    ShapeMatchingSolver();
    
    void clearRegions();
    void addRegions(const RegionEmitter &emitter, const int &offset);
    
    void createConstraints(const ParticleSystem &particles);
    
protected:
    
private:

    void addStiffness(const int v1, const int v2, const float &stiffness);

    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
};

}

#endif
