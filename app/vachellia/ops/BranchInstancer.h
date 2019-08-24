/*
 *  BranchInstancer.h
 *  vachellia
 *
 *  scattering on trunk
 *
 *  2019/7/19
 */
 
#ifndef VCHL_BRANCH_INSTANCER_H
#define VCHL_BRANCH_INSTANCER_H

#include "InstancerBase.h"
#include <rng/Lehmer.h>

namespace alo {

template<typename T>
class Uniform;
template<typename T>
class SimpleBuffer;
class Matrix44F;
class RandomSelect;
struct SurfaceGeodesicSample;
template<typename T1, typename T2>
class GeodesicSampleRule;

class BranchInstancer : public InstancerBase {
    
    typedef GeodesicSampleRule<SurfaceGeodesicSample, Uniform<Lehmer> > GeodRuleTyp;
/// limit of n trees
    int m_synthesizeNumTrunk;
/// limit of n branch per tree
    int m_synthesizeNumBranch;
    int m_randomSeed;
/// spacing between trees relative to mean tree size
    float m_relativeSpacing;
    
public:

    BranchInstancer();
    virtual ~BranchInstancer();

protected:
/// has at least one branch and one trunk
    virtual bool isInstancerReady() const override;
    
    void setSynthesizeNumTrunk(const int &x);
    void setSynthesizeNumBranch(const int &x);
    void setRandomSeed(const int &x);
    void setRelativeSpacing(const float &x);

    void synthesizeBranchInstances();
    
private:

    void countBranches(RandomSelect &selector);
    void countTrunks(RandomSelect &selector);
    bool isBranch(const RenderableOps *r) const;
    bool isTrunk(const RenderableOps *r) const;
    bool isTerrain(const RenderableOps *r) const;
    float getMeanBranchSize() const;
    float getMeanTrunkSize() const;
    float getBranchCollisionDistance() const;
    float getTrunkCollisionDistance() const;
    const RenderableOps *getTerrain(int &ind) const;
    void synthesizeSingleBranchInstances();
    void synthesizeBranchInstancesOnTerrain(const smp::SampleFilter<SurfaceSample> *filter);
    
};

}

#endif

