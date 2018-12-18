/*
 *  SphereOps.h
 *  acacia
 *
 */

#ifndef ACA_SPHERE_OPS_H
#define ACA_SPHERE_OPS_H

#include "DrawableOps.h"

namespace alo {

class SphereOps : public DrawableOps {
    
    int m_level;
    float m_radius;
    bool m_shoUV;

public:
    SphereOps();
    virtual ~SphereOps();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();

};

}

#endif

