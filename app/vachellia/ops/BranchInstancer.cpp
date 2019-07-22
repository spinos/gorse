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
#include <math/RandomSelect.h>

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
    
    RandomSelect branchSel;
    countBranches(branchSel);
    RandomSelect trunkSel;
    countTrunks(trunkSel);
    
    int sd = 87654321;
    int nsel = 200;
    
    Uniform<Lehmer> lmlcg(sd);
    
    int t = trunkSel.select<Uniform<Lehmer> >(&lmlcg);
    const RenderableOps *top = inputRenderable(t);
    const smp::SampleFilter<SurfaceGeodesicSample> *filter = top->getGeodesicSamples();
    if(!filter) {
        std::cout << "\n ERROR not a trunk ";
        return;
    }
    
    typedef GeodesicSampleRule<SurfaceGeodesicSample, Uniform<Lehmer> > GeodRuleTyp;
    GeodRuleTyp rule;
    rule.setRng(&lmlcg);
    rule.setMaxNumSelected(nsel);
    const int ns = filter->numSamples();
    rule.setAcceptRatio(filter->c_samples(), ns);
    
    SimpleBuffer<SurfaceGeodesicSample> subset;
    filter->drawSamples<GeodRuleTyp>(subset, rule);
    
    std::map<int, int> instanceCounter;

    const int n = subset.count();
	std::cout << "\n n subset " << n;
    createInstances(n);
	Matrix44F tm;
	float pitch, roll;
    for(int i=0;i<n;++i) {
        int j = branchSel.select<Uniform<Lehmer> >(&lmlcg);
        instanceCounter[j] = 0;
        
        grd::TestInstance &inst = instance(i);
        inst.setObjectId(j);

        const SurfaceGeodesicSample &si = subset[i];
		
        tm.setTranslation(si._pos);
		
		rule.getPitchAndRoll(pitch, roll, si);
		Matrix33F r = getBranchRotation(si._grad, si._nml, pitch, roll);
		tm.setRotation(r);
        inst.setSpace(tm);
        
    }
    
    const float instancerSize = getMeanBranchSize();
    setInstancedObjectCountAndSize(instanceCounter.size(), instancerSize);
}

void BranchInstancer::countBranches(RandomSelect &selector)
{
    int b = 0;
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(!r->hasGeodesicSamples()) {
            b++;
        }
    }
    selector.create(b);
    
    b = 0;
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(!r->hasGeodesicSamples()) {
            selector.setValue(i, b);
            selector.setWeight(1.f, b);
            b++;
        }
    }
    selector.normalize();
    
}

void BranchInstancer::countTrunks(RandomSelect &selector)
{
    int b = 0;
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(r->hasGeodesicSamples()) {
            b++;
        }
    }
    selector.create(b);
    
    b = 0;
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(r->hasGeodesicSamples()) {
            selector.setValue(i, b);
            selector.setWeight(1.f, b);
            b++;
        }
    }
    selector.normalize();
}

float BranchInstancer::getMeanBranchSize() const
{
    float bx[6];
    float s = 0.f;
    int b = 0;
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(!r->hasGeodesicSamples()) {
            r->getAabb(bx);
            s += BoundingBox(bx).getLongestDistance();
            b++;
        }
    }
    return s *= 1.f / (float)b;
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

