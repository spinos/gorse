/*
 *  ShapeMatchingSolver.cpp
 *  aloe
 *
 *  2019/10/6
 */
 
#include "ShapeMatchingSolver.h"
#include "RegionEmitter.h"
#include "ParticleSystem.h"
#include <math/SimpleBuffer.h>
#include "ShapeMatchingArray.h"
#include "ShapeMatchingConstraint.h"
#include <math/ConjugateGradient.h>

namespace alo {
    
struct ShapeMatchingSolver::Impl {
    
    ShapeMatchingArray _sma;
    SparseMatrix<float> _lhsMat;
    boost::shared_ptr<ConjugateGradient<float> > _cg;
    ParticleSystem *_particles;
    SimpleBuffer<float> _particleMass;
    
};

ShapeMatchingSolver::ShapeMatchingSolver() : 
m_pimpl(new Impl)
{}

void ShapeMatchingSolver::clearRegions()
{
    m_pimpl.reset(new Impl);
}

void ShapeMatchingSolver::addRegions(const RegionEmitter &emitter, const int &offset)
{    
    ShapeMatchingArray &sma = m_pimpl->_sma;
    const int nr = emitter.numRegions();
    for(int i=0;i<nr;++i) {
        
        sma.addRegion();
        
        const int nv = emitter.regionIndexCount(i);
        const int *rv = emitter.regionIndices(i);
        for(int j=0;j<nv;++j) {

            sma.addVertex(rv[j] + offset);
        }
    }
}

void ShapeMatchingSolver::setParticles(ParticleSystem *particles)
{ m_pimpl->_particles = particles; }

void ShapeMatchingSolver::createConstraints()
{
    ParticleSystem *particleSys = particles();
    
    const int &np = particleSys->numParticles();
    
    m_pimpl->_particleMass.resetBuffer(np);
    for(int i=0;i<np;++i) {
        m_pimpl->_particleMass[i] = ShapeMatchingConstraint::getMass(particleSys->inversedMasses()[i]);
    }
    
    const int dim = systemDimension();
    
    SparseMatrix<float> &lhsMat = m_pimpl->_lhsMat;
    lhsMat.create(dim, dim, true);
    
    static const float oneovert2 = 900.f;
    for(int i=0;i<np;++i) {

		const float movert2 = m_pimpl->_particleMass[i] * oneovert2;
		
        lhsMat.set(i * 3, i * 3, movert2);
		lhsMat.set(i * 3 + 1, i * 3 + 1, movert2);
		lhsMat.set(i * 3 + 2, i * 3 + 2, movert2);
	}
    
    ShapeMatchingArray &sma = m_pimpl->_sma;
    sma.createConstraints(*particleSys);
    
    const int &nr = sma.numRegions();
    for(int i=0;i<nr;++i) {
        const int ne = sma.regionIndexCount(i) - 1;
        const int *rind = sma.regionIndices(i);
        for(int j=0;j<ne;++j) {
            int va = rind[j];
            int vb = rind[j+1];
            addStiffness(va, vb);
        }
    }
    
    m_pimpl->_cg.reset(new ConjugateGradient<float>(&m_pimpl->_lhsMat) );
    
}

void ShapeMatchingSolver::addStiffness(const int v1, const int v2)
{
    const float &w = m_pimpl->_sma.stiffness();
    SparseMatrix<float> &lhsMat = m_pimpl->_lhsMat;
    lhsMat.add(v1 * 3    , v1 * 3    , w);
    lhsMat.add(v1 * 3 + 1, v1 * 3 + 1, w);
	lhsMat.add(v1 * 3 + 2, v1 * 3 + 2, w);
	lhsMat.add(v2 * 3    , v2 * 3    , w);
	lhsMat.add(v2 * 3 + 1, v2 * 3 + 1, w);
	lhsMat.add(v2 * 3 + 2, v2 * 3 + 2, w);

	lhsMat.add(v1 * 3    , v2 * 3    , -w);
	lhsMat.add(v1 * 3 + 1, v2 * 3 + 1, -w);
    lhsMat.add(v1 * 3 + 2, v2 * 3 + 2, -w);
	lhsMat.add(v2 * 3    , v1 * 3    , -w);
	lhsMat.add(v2 * 3 + 1, v1 * 3 + 1, -w);
	lhsMat.add(v2 * 3 + 2, v1 * 3 + 2, -w);
}

void ShapeMatchingSolver::applyPositionConstraint()
{
    ParticleSystem *particleSys = particles();
    
    const int &np = particleSys->numParticles();
    
    const int dim = systemDimension();
    
    DenseVector<float> q_n1;
	q_n1.create(dim);
	q_n1.copyData((const float*)particleSys->projectedPositions() );
    
    DenseVector<float> s_n; 
	s_n.copy(q_n1);
    
    
    static const float oneovert2 = 900.f;
    for(int i=0;i<np;++i) {

		const float movert2 = m_pimpl->_particleMass[i] * oneovert2;
		
        s_n[i * 3] *= movert2;
		s_n[i * 3 + 1] *= movert2;
		s_n[i * 3 + 2] *= movert2;
	}
    
	DenseVector<float> b;
	
    ShapeMatchingArray &sma = m_pimpl->_sma;
    
    const int &nr = sma.numRegions();
    
	//for(int k=0;k<1;++k) {
	
		b.copy(s_n);
	
		for(int i=0;i<nr;++i) {
            sma.applyRegionConstraint(b.v(), q_n1.v(), i);
		}
        
		m_pimpl->_cg->solve(q_n1, b);
	
	//}
    
    particleSys->setProjectedPositions((const Vector3F *)q_n1.v());
}

ParticleSystem *ShapeMatchingSolver::particles()
{ return m_pimpl->_particles; }

int ShapeMatchingSolver::systemDimension() const
{ return m_pimpl->_particles->numParticles() * 3; }

}
