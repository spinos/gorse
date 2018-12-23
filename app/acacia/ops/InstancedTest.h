/*
 *  InstancedTest.h
 *  acacia
 *
 */

#ifndef ACA_INSTANCED_TEST_H
#define ACA_INSTANCED_TEST_H

#include "DrawableOps.h"

namespace alo {
class AdaptableMesh;
class GeodesicDistance;
class InstancedTest : public DrawableOps {
    
    AdaptableMesh *m_mesh;
    GeodesicDistance *m_geod;
    
public:
    InstancedTest();
    virtual ~InstancedTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();

};

}

#endif

