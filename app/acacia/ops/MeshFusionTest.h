/*
 *  MeshFusionTest.h
 *  acacia
 *
 */

#ifndef ACA_MESH_FUSION_H
#define ACA_MESH_FUSION_H

#include "DrawableOps.h"

namespace alo {

class MeshFusionTest : public DrawableOps {
    
    int m_level;
    float m_radius;
    bool m_shoUV;

public:

    enum { Type = 716942 };
    
    MeshFusionTest();
    virtual ~MeshFusionTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();

};

}

#endif

