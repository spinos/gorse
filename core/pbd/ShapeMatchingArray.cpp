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
    float _stiffness;
    
    Impl() : _stiffness(30000.f)
    {}
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
                        
        ci.setQ(m_pimpl->_q.data(),
                m_pimpl->_p.c_data(),
                vertexInd);
        
    }
}

void ShapeMatchingArray::setStiffness(const float &x)
{ m_pimpl->_stiffness = x; }

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

const float &ShapeMatchingArray::stiffness() const
{ return m_pimpl->_stiffness; }

void ShapeMatchingArray::applyRegionConstraint(float *b, const float *q_n1, const int iregion)
{
    ShapeMatchingConstraint &ci = m_pimpl->_constraints[iregion];
    
    ci.calculateP(m_pimpl->_p.data(), 
                        m_pimpl->_mass.c_data(),
                        (const Vector3F *)q_n1,
                        m_pimpl->_index.c_data());
                        
    ci.calculateG(m_pimpl->_g.data(), 
                        m_pimpl->_p.c_data(),
                        m_pimpl->_q.c_data());
                                          
    const int ne = regionIndexCount(iregion) - 1;
    const int *rind = regionIndices(iregion);
    
    const Vector3F *goals = m_pimpl->_g.c_data();
    Vector3F pSpring[2];
    for(int i=0;i<ne;++i) {
        const int &v1 = rind[i];
        const int &v2 = rind[i+1];
        
        ci.solvePosition(pSpring[0], q_n1, v1, i, goals);
		ci.solvePosition(pSpring[1], q_n1, v2, i+1, goals);
        
        pSpring[0] -= pSpring[1];
        pSpring[1] = pSpring[0] * -1.f;
        
        pSpring[0] *= m_pimpl->_stiffness;
        pSpring[1] *= m_pimpl->_stiffness;
			
        b[v1 * 3    ] += pSpring[0].x;
        b[v1 * 3 + 1] += pSpring[0].y;
        b[v1 * 3 + 2] += pSpring[0].z;
        
        b[v2 * 3    ] += pSpring[1].x;
        b[v2 * 3 + 1] += pSpring[1].y;
        b[v2 * 3 + 2] += pSpring[1].z;
    }
    
}

}
