/*
 *  HorizonOps.h
 *  vachellia
 *
 */

#ifndef VCHL_HORIZON_OPS_H
#define VCHL_HORIZON_OPS_H

#include "RenderableOps.h"

namespace alo {

class HorizonOps : public RenderableOps {
    
    Vector3F m_center;
    float m_planetRadius;

public:
	enum { Type = 299105 };

    HorizonOps();
    virtual ~HorizonOps();

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual void update() override;
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

protected:
    
private:
    

};

}

#endif

