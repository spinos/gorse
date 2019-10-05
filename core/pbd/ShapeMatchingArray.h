/*
 *  ShapeMatchingArray.h
 *  aloe
 *  
 *  2019/10/6
 */
 
#ifndef ALO_PBD_SHAPE_MATCHING_ARRAY_H
#define ALO_PBD_SHAPE_MATCHING_ARRAY_H

#include <boost/shared_ptr.hpp>

namespace alo {
    
class Vector3F;
class ParticleSystem;

class ShapeMatchingArray {
    
public:

    ShapeMatchingArray();
    
    void addRegion();
    void addVertex(const int ind);
    void createConstraints(const ParticleSystem &particles);

    const int &numRegions() const;
    const int &numVertices() const;
    int regionIndexCount(const int i) const;
    const int *regionIndices(const int i) const;
    const float &mass(const int i) const;
    const float &regionStiffness(const int i) const;
    
private:

    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
    
};

}

#endif
