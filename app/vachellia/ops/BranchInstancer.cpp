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
#include <smp/SpatialSampleRule.h>
#include <smp/GeodesicSampleRule.h>
#include <rng/Uniform.h>
#include <sds/FZOrder.h>
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

const RenderableOps *BranchInstancer::getTerrain(int &ind) const
{
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(r->hasSurfaceSamples()) {
            ind = i;
            return r;
        }
    }
    return nullptr;
}

void BranchInstancer::synthesizeBranchInstances()
{
    int terrainObjI;
    const RenderableOps *terrain = getTerrain(terrainObjI);
    if(terrain) {
        const smp::SampleFilter<SurfaceSample> *flt = terrain->getSurfaceSamples();
        synthesizeBranchInstancesOnTerrain(flt);
    } else {
        synthesizeSingleBranchInstances();
    }
}

void BranchInstancer::synthesizeSingleBranchInstances()
{
    RandomSelect branchSel;
    countBranches(branchSel);
    RandomSelect trunkSel;
    countTrunks(trunkSel);
    
    int sd = 87654321;
    int nsel = 300;
    
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
    float collisionDistance = getBranchCollisionDistance();
    rule.setMinDistance(collisionDistance);
    const int ns = filter->numSamples();
    rule.calculateMaxGeod(filter->c_samples(), ns);
    
    SimpleBuffer<SurfaceGeodesicSample> subset;
    filter->drawSamples<GeodRuleTyp>(subset, rule);
    
    std::map<int, int> instanceCounter;

    const int n = subset.count();
	std::cout << "\n n subset " << n;
    createInstances(n);
	Matrix44F tm;
    for(int i=0;i<n;++i) {
        int j = branchSel.select<Uniform<Lehmer> >(&lmlcg);
        instanceCounter[j] = 0;
        
        grd::TestInstance &inst = instance(i);
        inst.setObjectId(j);

        const SurfaceGeodesicSample &si = subset[i];
		
        tm.setTranslation(si._pos);
		
		Matrix33F r = rule.getRotation(si);
		Matrix33F::rotateUpToAlignLimited(r, Vector3F(0.f, 1.f, 0.f), .7f);
		tm.setRotation(r);
        inst.setSpace(tm);
        
    }
    
    const float instancerSize = getMeanBranchSize();
    setInstancedObjectCountAndSize(instanceCounter, instancerSize);
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
        if(!r->hasGeodesicSamples() && !r->hasSurfaceSamples()) {
            r->getAabb(bx);
            s += BoundingBox(bx).getLongestDistance();
            b++;
        }
    }
    return s *= 1.f / (float)b;
}

float BranchInstancer::getMeanTrunkSize() const
{
    float bx[6];
    float s = 0.f;
    int b = 0;
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(r->hasGeodesicSamples()) {
            r->getAabb(bx);
            s += BoundingBox(bx).getLongestDistance();
            b++;
        }
    }
    return s *= 1.f / (float)b;
}

float BranchInstancer::getBranchCollisionDistance() const
{
    float bx[6];
    float s = 0.f;
    int b = 0;
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(!r->hasGeodesicSamples() && !r->hasSurfaceSamples()) {
            r->getAabb(bx);
            s += BoundingBox(bx).distance(0) + BoundingBox(bx).distance(2);
            b+=2;
        }
    }
    return s *= 1.f / (float)b;
}

float BranchInstancer::getTrunkCollisionDistance() const
{
    float bx[6];
    float s = 0.f;
    int b = 0;
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(r->hasGeodesicSamples()) {
            r->getAabb(bx);
            s += BoundingBox(bx).distance(0) + BoundingBox(bx).distance(2);
            b+=2;
        }
    }
    return s *= 1.f / (float)b;
}

void BranchInstancer::synthesizeBranchInstancesOnTerrain(const smp::SampleFilter<SurfaceSample> *filter)
{
    RandomSelect branchSel;
    countBranches(branchSel);
    RandomSelect trunkSel;
    countTrunks(trunkSel);
    
    int sd = 87654321;
    int nsel = 20000;
    
    Uniform<Lehmer> lmlcg(sd);
    sds::FZOrderCurve sfc;
    typedef SpatialSampleRule<Uniform<Lehmer>, sds::FZOrderCurve > SurfaceRuleTyp;
    SurfaceRuleTyp trunkRule(&sfc);
    trunkRule.setRng(&lmlcg);
    trunkRule.setMaxNumSelected(nsel);
    float collisionDistance = getTrunkCollisionDistance();
    const int ns = filter->numSamples();
    trunkRule.createGrid<SurfaceSample>(filter->c_samples(), ns, collisionDistance);
    
    SimpleBuffer<SurfaceSample> subset;
    filter->drawSamples<SurfaceRuleTyp>(subset, trunkRule);
    
    std::map<int, int> instanceCounter;

    const int ntrunk = subset.count();
	std::cout << "\n n trunk " << ntrunk;
    
    SimpleBuffer<int> trunkObjectIds;
    SimpleBuffer<Matrix44F> trunkTms;
    
    Matrix44F tm;
    for(int i=0;i<ntrunk;++i) {
        int j = trunkSel.select<Uniform<Lehmer> >(&lmlcg);
        
        trunkObjectIds << j;
        
        instanceCounter[j] = 0;

        const SurfaceSample &si = subset[i];
		
        tm.setTranslation(si._pos);
		
		Matrix33F r = trunkRule.getRotation<SurfaceSample>(si);
        tm.setRotation(r);
        
        trunkTms << tm;
    }
    
    std::map<int, GeodRuleTyp *> geodRuleMap;
    
    float branchCollisionDistance = getBranchCollisionDistance();
        
    std::map<int, int>::const_iterator trunkIt = instanceCounter.begin();
    for(;trunkIt!=instanceCounter.end();++trunkIt) {
        GeodRuleTyp *rule = new GeodRuleTyp;
        rule->setRng(&lmlcg);
        rule->setMaxNumSelected(300);
        rule->setMinDistance(branchCollisionDistance);
        
        const RenderableOps *top = inputRenderable(trunkIt->first);
        const smp::SampleFilter<SurfaceGeodesicSample> *filter = top->getGeodesicSamples();
        
        const int ns = filter->numSamples();
        rule->calculateMaxGeod(filter->c_samples(), ns);
    
        geodRuleMap[trunkIt->first] = rule;
    }
    
    SimpleBuffer<int> branchObjectIds;
    SimpleBuffer<Matrix44F> branchTms;
    
    for(int i=0;i<ntrunk;++i) {
        GeodRuleTyp *rule = geodRuleMap[trunkObjectIds[i]];
        rule->setTransform(trunkTms[i]);
        
        const RenderableOps *top = inputRenderable(trunkObjectIds[i]);
        const smp::SampleFilter<SurfaceGeodesicSample> *filter = top->getGeodesicSamples();
        
        synthesizeBranchInstancesOnTrunk(branchObjectIds, branchTms, &lmlcg, filter, branchSel, trunkTms[i], rule);
    }
    
    std::map<int, GeodRuleTyp *>::iterator trunkRuleIt = geodRuleMap.begin();
    for(;trunkRuleIt != geodRuleMap.end();++trunkRuleIt) {
        delete trunkRuleIt->second;
    }
    
    const int nbranch = branchObjectIds.count();
    std::cout << "\n n branch " << nbranch;
    
    const int n = ntrunk + nbranch;
    createInstances(n);
	
    for(int i=0;i<ntrunk;++i) {
        int j = trunkObjectIds[i];
        
        grd::TestInstance &inst = instance(i);
        inst.setObjectId(j);
        inst.setSpace(trunkTms[i]);
        
    }
    
    for(int i=0;i<nbranch;++i) {
        int j = branchObjectIds[i];
        
        grd::TestInstance &inst = instance(i + ntrunk);
        inst.setObjectId(j);
        inst.setSpace(branchTms[i]);
        
    }
    
    const float instancerSize = getMeanTrunkSize();
    setInstancedObjectCountAndSize(instanceCounter, instancerSize);
}

void BranchInstancer::synthesizeBranchInstancesOnTrunk(SimpleBuffer<int> &branchObjectIds,
                    SimpleBuffer<Matrix44F> &branchTms,
                    Uniform<Lehmer> *lmlcg, 
                    const smp::SampleFilter<SurfaceGeodesicSample> *filter,
                    RandomSelect &branchSel,
                    const Matrix44F &trunkTm, GeodRuleTyp *trunkRule)
{
    SimpleBuffer<SurfaceGeodesicSample> subset;
    filter->drawSamples<GeodRuleTyp>(subset, *trunkRule);
    
    const int n = subset.count();

	Matrix44F tm;
    for(int i=0;i<n;++i) {
        int j = branchSel.select<Uniform<Lehmer> >(lmlcg);
        
        branchObjectIds << j;

        const SurfaceGeodesicSample &si = subset[i];
		
        tm.setTranslation(trunkTm.transform(si._pos));
		
		Matrix33F r = trunkRule->getRotation(si);
		Matrix33F::rotateUpToAlignLimited(r, Vector3F(0.f, 1.f, 0.f), .7f);
		tm.setRotation(r);
        
        branchTms << tm;
        
    }
}
    
}
