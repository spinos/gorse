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
    
    float m_planetRadius;

public:
	enum { Type = 299105 };

    HorizonOps();
    virtual ~HorizonOps();

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual void update() override;

protected:
    
private:
    

};

}

#endif

