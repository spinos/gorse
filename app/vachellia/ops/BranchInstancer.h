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

namespace alo {
	
class Matrix33F;
class RandomSelect;

class BranchInstancer : public InstancerBase {

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
    float getBranchCollisionDistance() const;
    
};

}

#endif

