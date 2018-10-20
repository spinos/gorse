#ifndef ACA_ROOT_OPS_H
#define ACA_ROOT_OPS_H

#include "DrawableOps.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

namespace alo {

class RootOps : public DrawableOps {
    
    SimpleBuffer<Vector3F> posarr;
    SimpleBuffer<Vector3F> nmlarr;
    SimpleBuffer<Vector3F> baryc;
    
public:
    RootOps();
    
    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
};

}

#endif

