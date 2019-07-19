/*
 *  BranchInstancer.cpp
 *  vachellia
 *
 *  2019/7/19
 */
 
#include "BranchInstancer.h"
#include <grd/ObjectInstancer.h>

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

    const int n = 100;
    createInstances(n);
    for(int i=0;i<n;++i) {
        int j = selectABranch();
        int t = selectATrunk();
        
        grd::TestInstance &inst = instance(i);
        inst.setObjectId(j);
        Matrix44F tm;
        tm.setTranslation(Vector3F(19.f * i, 0.f, 0.f));
        inst.setSpace(tm);
        
    }
    
    setInstancedObjectCountAndSize(1, 9.f);
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

