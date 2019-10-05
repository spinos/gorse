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
#include <math/ConjugateGradient.h>

namespace alo {
    
struct ShapeMatchingSolver::Impl {
    
    ShapeMatchingArray _sma;
    SparseMatrix<float> _lhsMat;
    boost::shared_ptr<ConjugateGradient<float> > _cg;
      
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

void ShapeMatchingSolver::createConstraints(const ParticleSystem &particles)
{
    ShapeMatchingArray &sma = m_pimpl->_sma;
    sma.createConstraints(particles);
    
    const int &np = particles.numParticles();
    const int dim = np * 3;
    
    SparseMatrix<float> &lhsMat = m_pimpl->_lhsMat;
    lhsMat.create(dim, dim, true);
    
    static const float oneovert2 = 900.f;
    for(int i=0;i<np;++i) {

		const float movert2 = sma.mass(i) * oneovert2;
		
        lhsMat.set(i * 3, i * 3, movert2);
		lhsMat.set(i * 3 + 1, i * 3 + 1, movert2);
		lhsMat.set(i * 3 + 2, i * 3 + 2, movert2);
	}
    
    const int &nr = sma.numRegions();
    for(int i=0;i<nr;++i) {
        const int ne = sma.regionIndexCount(i) - 1;
        const int *rind = sma.regionIndices(i);
        for(int j=0;j<ne;++j) {
            int va = rind[j];
            int vb = rind[j+1];
            addStiffness(va, vb, sma.regionStiffness(i));
        }
    }
    
    m_pimpl->_cg.reset(new ConjugateGradient<float>(&m_pimpl->_lhsMat) );
}

void ShapeMatchingSolver::addStiffness(const int v1, const int v2, const float &stiffness)
{
    SparseMatrix<float> &lhsMat = m_pimpl->_lhsMat;
    lhsMat.add(v1 * 3    , v1 * 3    , stiffness);
    lhsMat.add(v1 * 3 + 1, v1 * 3 + 1, stiffness);
	lhsMat.add(v1 * 3 + 2, v1 * 3 + 2, stiffness);
	lhsMat.add(v2 * 3    , v2 * 3    , stiffness);
	lhsMat.add(v2 * 3 + 1, v2 * 3 + 1, stiffness);
	lhsMat.add(v2 * 3 + 2, v2 * 3 + 2, stiffness);

	lhsMat.add(v1 * 3    , v2 * 3    , -stiffness);
	lhsMat.add(v1 * 3 + 1, v2 * 3 + 1, -stiffness);
    lhsMat.add(v1 * 3 + 2, v2 * 3 + 2, -stiffness);
	lhsMat.add(v2 * 3    , v1 * 3    , -stiffness);
	lhsMat.add(v2 * 3 + 1, v1 * 3 + 1, -stiffness);
	lhsMat.add(v2 * 3 + 2, v1 * 3 + 2, -stiffness);
}

}
