/*
 *  ShapeMatchingArray.cpp
 *  aloe
 *  
 *  2019/10/6
 */
 
#include "ShapeMatchingArray.h"
#include "ShapeMatchingConstraint.h"
#include <math/SimpleBuffer.h>
#include <math/Quaternion.h>
#include "ParticleSystem.h"

namespace alo {
    
struct ShapeMatchingArray::Impl {
    
    SimpleBuffer<int> _offset;
    SimpleBuffer<int> _index;
    SimpleBuffer<ShapeMatchingConstraint> _constraints;
    SimpleBuffer<Vector3F> _p;
    SimpleBuffer<Vector3F> _q;
    SimpleBuffer<Vector3F> _g;
    SimpleBuffer<float> _mass;
    
};

ShapeMatchingArray::ShapeMatchingArray() : 
m_pimpl(new Impl)
{}

void ShapeMatchingArray::addRegion()
{ m_pimpl->_offset << m_pimpl->_index.count(); }

void ShapeMatchingArray::addVertex(const int ind)
{ m_pimpl->_index << ind; }

void ShapeMatchingArray::createConstraints(const ParticleSystem &particles)
{
    const int nr = numRegions();
    m_pimpl->_constraints.resetBuffer(nr);
    
    const int ni = m_pimpl->_index.count();
    m_pimpl->_p.resetBuffer(ni);
    m_pimpl->_q.resetBuffer(ni);
    m_pimpl->_g.resetBuffer(ni);
    m_pimpl->_mass.resetBuffer(ni);
    
    std::cout<<"\n ShapeMatchingArray::create n constraints "<<nr
                << " array len " << ni;
                
    const int *vertexInd = m_pimpl->_index.c_data();
    
    for(int i=0;i<nr;++i) {
        
        ShapeMatchingConstraint &ci = m_pimpl->_constraints[i];
        
        ci.setOffset(m_pimpl->_offset[i]);
        const int nv = regionIndexCount(i);
        ci.setNumVertices(nv);

        ci.calculateMass(m_pimpl->_mass.data(),
                        particles.inversedMasses(),
                        vertexInd);
        
        ci.calculateP(m_pimpl->_p.data(), 
                        m_pimpl->_mass.c_data(),
                        particles.positions(),
                        vertexInd);
                        
        m_pimpl->_q.copyFrom(m_pimpl->_p);                
        
        ci.calculateG(m_pimpl->_g.data(), 
                        m_pimpl->_p.c_data(),
                        m_pimpl->_q.c_data());
                        
    }
}

const int &ShapeMatchingArray::numRegions() const
{ return m_pimpl->_offset.count(); }

const int &ShapeMatchingArray::numVertices() const
{ return m_pimpl->_index.count(); }

int ShapeMatchingArray::regionIndexCount(const int i) const
{
    int a = m_pimpl->_offset[i];
    if(i >= numRegions()-1) return numVertices() - a;
    return m_pimpl->_offset[i+1] - a;
}

const int *ShapeMatchingArray::regionIndices(const int i) const
{ return &m_pimpl->_index[m_pimpl->_offset[i]]; }

const float &ShapeMatchingArray::mass(const int i) const
{ return m_pimpl->_mass[i]; }

const float &ShapeMatchingArray::regionStiffness(const int i) const
{ return m_pimpl->_constraints[i].stiffness(); }

}
