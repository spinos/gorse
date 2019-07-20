/*
 *  BranchInstancer.cpp
 *  vachellia
 *
 *  2019/7/19
 */
 
#include "BranchInstancer.h"
#include <grd/ObjectInstancer.h>
#include <smp/SurfaceGeodesicSample.h>
#include <smp/SampleFilter.h>
#include <smp/GeodesicSampleRule.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>

namespace alo {
    
BranchInstancer::BranchInstancer()
{}
    
BranchInstancer::~BranchInstancer()
{} 

bool BranchInstancer::isInstancerReady() const
{
    const int n = numInputRenderables();
    if(n < 2) return false;
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(r->hasGeodesicSamples()) return true;
    }
    return false;
}

void BranchInstancer::synthesizeBranchInstances()
{
    std::cout << "\n BranchInstancer::synthesizeBranchInstances ";
    
    int t = selectATrunk();
    const RenderableOps *top = inputRenderable(t);
    const smp::SampleFilter<SurfaceGeodesicSample> *filter = top->getGeodesicSamples();
    if(!filter) {
        std::cout << "\n ERROR not a trunk ";
        return;
    }
    
    int nsel = 400;
    int sd = 7654321;
    Uniform<Lehmer> lmlcg(sd);
    
    typedef GeodesicSampleRule<SurfaceGeodesicSample, Uniform<Lehmer> > GeodRuleTyp;
    GeodRuleTyp rule;
    rule.setRng(&lmlcg);
    rule.setMaxNumSelected(nsel);
    const int ns = filter->numSamples();
    rule.setAcceptRatio(filter->c_samples(), ns);
    
    SimpleBuffer<SurfaceGeodesicSample> subset;
    filter->drawSamples<GeodRuleTyp>(subset, rule);

    const int n = subset.count();
	std::cout << "\n n subset " << n;
    createInstances(n);
    for(int i=0;i<n;++i) {
        int j = selectABranch();
        
        grd::TestInstance &inst = instance(i);
        inst.setObjectId(j);

        const SurfaceGeodesicSample &si = subset[i];
		
		Matrix44F tm;
		Vector3F up = si._grad;
		Vector3F front = si._nml;
		Vector3F side = up.cross(front); side.normalize();
		up = front.cross(side); up.normalize();
		
        tm.setTranslation(si._pos);
		tm.setOrientations(side, up, front);
        inst.setSpace(tm);
        
    }
    
    setInstancedObjectCountAndSize(1, 10.f);
}

int BranchInstancer::selectABranch()
{
    int b = 0;
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(!r->hasGeodesicSamples()) {
            b = i;
// randomly?
        }
    }
    return b;
}

int BranchInstancer::selectATrunk()
{
    int b = 0;
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(r->hasGeodesicSamples()) {
            b = i;
// randomly?
        }
    }
    return b;
}
    
}

