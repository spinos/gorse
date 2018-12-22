/*
 *  GeodesicDistanceTest.h
 *  acacia
 *
 */

#ifndef ACA_GEODESIC_DSITANCE_TEST_H
#define ACA_GEODESIC_DSITANCE_TEST_H

#include "DrawableOps.h"

namespace alo {
class GeodesicDistance;
class GeodesicDistanceTest : public DrawableOps {
    
    float m_displacement[3];
    AdaptableMesh *m_mesh;
    GeodesicDistance *m_geod;
    
public:
    GeodesicDistanceTest();
    virtual ~GeodesicDistanceTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();

};

}

#endif

