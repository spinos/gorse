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

public:

    BranchInstancer();
    virtual ~BranchInstancer();

protected:
/// has branch and trunk
    virtual bool isInstancerReady() const override;

    void synthesizeBranchInstances();
    
private:

    void countBranches(RandomSelect &selector);
    void countTrunks(RandomSelect &selector);
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

