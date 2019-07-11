/*
 *  BranchTest.h
 *  acacia
 *
 *  sparse grid from surface samples
 *  gradient of geodesic distance as normal direction
 *
 *  2019/7/11
 */

#ifndef ACA_BRANCH_TEST_H
#define ACA_BRANCH_TEST_H

#include "DrawableOps.h"

namespace alo {

class BranchTest : public DrawableOps {
    
    AdaptableMesh *m_mesh;
    
public:

    enum { Type = 716943 };

    BranchTest();
    virtual ~BranchTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();

};

}

#endif

