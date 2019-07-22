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
    
    int nsel = 200;
    int sd = 87654321;
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
	Matrix44F tm;
	float pitch, roll;
    for(int i=0;i<n;++i) {
        int j = selectABranch();
        
        grd::TestInstance &inst = instance(i);
        inst.setObjectId(j);

        const SurfaceGeodesicSample &si = subset[i];
		
        tm.setTranslation(si._pos);
		
		rule.getPitchAndRoll(pitch, roll, si);
		Matrix33F r = getBranchRotation(si._grad, si._nml, pitch, roll);
		tm.setRotation(r);
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

Matrix33F BranchInstancer::getBranchRotation(const Vector3F &binormal, const Vector3F &normal,
					const float &pitch, const float &rollAngle) const
{
	Vector3F up = binormal * (1.f - pitch) + normal * pitch; up.normalize();
	Vector3F front = normal.cross(up); front.normalize();
	Vector3F side = up.cross(front); side.normalize();
	
	Matrix33F r;
	r.fill(side, up, front);
	
	Quaternion rq(rollAngle, up);
	Matrix33F roll(rq);
	return r * roll;
}
    
}

