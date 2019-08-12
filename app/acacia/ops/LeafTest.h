/*
 *  LeafTest.h
 *  acacia
 *
 *  sparse grid from surface samples
 *  gradient of geodesic distance as normal direction
 *
 *  2019/8/12
 */

#ifndef ACA_LEAF_TEST_H
#define ACA_LEAF_TEST_H

#include "DrawableOps.h"

namespace alo {

class LeafTest : public DrawableOps {
    
    AdaptableMesh *m_mesh;
    
public:

    enum { Type = 716944 };

    LeafTest();
    virtual ~LeafTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();

};

}

#endif

