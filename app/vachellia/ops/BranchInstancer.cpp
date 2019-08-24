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
#include <scatter/SynthesisBundle.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono/include.hpp>

namespace alo {
    
BranchInstancer::BranchInstancer() : m_synthesizeNumTrunk(100),
m_synthesizeNumBranch(200),
m_randomSeed(7654321),
m_relativeSpacing(1.f),
m_relativeBranchSpacing(.5f)
{}
    
BranchInstancer::~BranchInstancer()
{} 

void BranchInstancer::setSynthesizeNumTrunk(const int &x)
{ m_synthesizeNumTrunk = x; }

void BranchInstancer::setSynthesizeNumBranch(const int &x)
{ m_synthesizeNumBranch = x; }

void BranchInstancer::setRandomSeed(const int &x)
{ m_randomSeed = x; }

void BranchInstancer::setRelativeSpacing(const float &x)
{ m_relativeSpacing = x; }

void BranchInstancer::setRelativeBranchSpacing(const float &x)
{ m_relativeBranchSpacing = x; }

bool BranchInstancer::isInstancerReady() const
{
    const int n = numInputRenderables();
    if(n < 2) return false;
    bool hasBranch = false;
    bool hasTrunk = false;
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(isTrunk(r)) hasTrunk = true;
        if(isBranch(r)) hasBranch = true;
    }
    return hasBranch && hasTrunk;
}

const RenderableOps *BranchInstancer::getTerrain(int &ind) const
{
    const int n = numInputRenderables();
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(isTerrain(r)) {
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
        float bx[6];
        terrain->getAabb(bx);
        setWorldCenter(bx);
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
    
    Uniform<Lehmer> lmlcg(m_randomSeed);
    
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
    rule.setMaxNumSelected(m_synthesizeNumBranch);
    float collisionDistance = getBranchCollisionDistance() * m_relativeBranchSpacing;
	std::cout << "\n branch coll dist " << collisionDistance<< " rel " << m_relativeBranchSpacing;
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
		Matrix33F::rotateUpToAlignLimited(r, Vector3F(0.f, 1.f, 0.f), .5f);
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
        if(isBranch(r)) {
            b++;
        }
    }
    selector.create(b);
    
    b = 0;
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(isBranch(r)) {
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
        if(isTrunk(r)) {
            b++;
        }
    }
    selector.create(b);
    
    b = 0;
    for(int i=0;i<n;++i) {
        const RenderableOps *r = inputRenderable(i);
        if(isTrunk(r)) {
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
        if(isBranch(r)) {
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
        if(isTrunk(r)) {
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
        if(isBranch(r)) {
            r->getAabb(bx);
            s += BoundingBox(bx).distance(1) + BoundingBox(bx).distance(2);
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
        if(isTrunk(r)) {
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
    
    Uniform<Lehmer> *lmlcgs[8];
    for(int i=0;i<8;++i) lmlcgs[i] = new Uniform<Lehmer>(m_randomSeed + i);
    
    sds::FZOrderCurve sfc;
    typedef SpatialSampleRule<Uniform<Lehmer>, sds::FZOrderCurve > SurfaceRuleTyp;
    SurfaceRuleTyp trunkRule(&sfc);
    trunkRule.setRng(lmlcgs[0]);
    trunkRule.setMaxNumSelected(m_synthesizeNumTrunk);
    float collisionDistance = getTrunkCollisionDistance() * m_relativeSpacing;
    const int ns = filter->numSamples();
    trunkRule.createGrid<SurfaceSample>(filter->c_samples(), ns, collisionDistance);
    
    SimpleBuffer<SurfaceSample> subset;
    filter->drawSamples<SurfaceRuleTyp>(subset, trunkRule);
    
    std::map<int, int> instanceCounter;

    const int ntrunk = subset.count();
	std::cout << "\n n trunk " << ntrunk;
    
    boost::chrono::system_clock::time_point t0 = boost::chrono::system_clock::now();
    
    SimpleBuffer<int> trunkObjectIds;
    SimpleBuffer<Matrix44F> trunkTms;
    
    Matrix44F tm;
    for(int i=0;i<ntrunk;++i) {
        int j = trunkSel.select<Uniform<Lehmer> >(lmlcgs[0]);
        
        trunkObjectIds << j;
        
        instanceCounter[j] = 0;

        const SurfaceSample &si = subset[i];
		
        tm.setTranslation(si._pos);
		
		Matrix33F r = trunkRule.getRotation<SurfaceSample>(si);
        tm.setRotation(r);
        
        trunkTms << tm;
    }
    
    typedef sca::SynthesisBundle<64> SynthBudleTyp;
    
    const float branchCollisionDistance = getBranchCollisionDistance() * m_relativeBranchSpacing;
    
    std::map<int, SynthBudleTyp *> trunkBundleMap;
       
    std::map<int, int>::const_iterator trunkIt = instanceCounter.begin();
    for(;trunkIt!=instanceCounter.end();++trunkIt) {
        GeodRuleTyp rule;
        rule.setRng(lmlcgs[0]);
        rule.setMaxNumSelected(m_synthesizeNumBranch);
        rule.setMinDistance(branchCollisionDistance);
        
        const RenderableOps *top = inputRenderable(trunkIt->first);
        const smp::SampleFilter<SurfaceGeodesicSample> *filter = top->getGeodesicSamples();
        
        const int ns = filter->numSamples();
        rule.calculateMaxGeod(filter->c_samples(), ns);
    
        SynthBudleTyp *bnd = new SynthBudleTyp;
        
        bnd->synthesize<SurfaceGeodesicSample, GeodRuleTyp, Uniform<Lehmer> >(filter, rule, branchSel, lmlcgs[0]);
        
        trunkBundleMap[trunkIt->first] = bnd;
    }
    
    clearClusters();
    
    SimpleBuffer<int> branchObjectIds;
    SimpleBuffer<Matrix44F> branchTms;
    
    SimpleBuffer<int> thObjectIds[8];
    SimpleBuffer<Matrix44F> thTms[8];
    boost::thread transformThread[8];
    
    int threadCount = 0;
    for(int i=0;i<ntrunk;++i) {
        SynthBudleTyp *bnd = trunkBundleMap[trunkObjectIds[i]];

        transformThread[threadCount] = boost::thread(boost::bind(
        &SynthBudleTyp::transformABundle, bnd, 
                &thObjectIds[threadCount], &thTms[threadCount], &trunkTms[i], lmlcgs[threadCount])
        );
        threadCount++;
        
        if(threadCount == 8) {
            for(int t=0;t<threadCount;++t) {
                transformThread[t].join();
            }
            
            for(int t=0;t<threadCount;++t) {
                clusterBegin(ntrunk + branchObjectIds.count());
                
                branchObjectIds.append(thObjectIds[t]);
                thObjectIds[t].purgeBuffer();
                branchTms.append(thTms[t]);
                thTms[t].purgeBuffer();
                
                clusterEnd(ntrunk + branchObjectIds.count());
            }
            threadCount = 0;
        }
    }
    
    if(threadCount > 0) {
        for(int t=0;t<threadCount;++t) {
            transformThread[t].join();
        }
        
        for(int t=0;t<threadCount;++t) {
            clusterBegin(ntrunk + branchObjectIds.count());
            
            branchObjectIds.append(thObjectIds[t]);
            thObjectIds[t].purgeBuffer();
            branchTms.append(thTms[t]);
            thTms[t].purgeBuffer();
            
            clusterEnd(ntrunk + branchObjectIds.count());
        }

    }
    
    std::map<int, SynthBudleTyp *>::iterator bundleIt = trunkBundleMap.begin();
    for(;bundleIt != trunkBundleMap.end();++bundleIt) {
        delete bundleIt->second;
    }
    
    for(int i=0;i<8;++i) delete lmlcgs[i];
    
    const int nbranch = branchObjectIds.count();
    std::cout << "\n n branch " << nbranch;
    
    boost::chrono::system_clock::time_point t1 = boost::chrono::system_clock::now();
    
    boost::chrono::duration<double> sec = t1 - t0;
    std::cout << "\n finished in " << sec.count() << " seconds ";
    
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

bool BranchInstancer::isBranch(const RenderableOps *r) const
{ return !r->hasGeodesicSamples() && !r->hasSurfaceSamples(); }

bool BranchInstancer::isTrunk(const RenderableOps *r) const
{ return r->hasGeodesicSamples(); }

bool BranchInstancer::isTerrain(const RenderableOps *r) const
{ return r->hasSurfaceSamples(); }
    
}
