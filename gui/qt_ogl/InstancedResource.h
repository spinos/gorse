/*
 *  InstancedResource.h
 *  aloe
 * 
 *  to draw multiple instances
 *
 */
 
#ifndef ALO_INSTANCED_RESOURCE_H
#define ALO_INSTANCED_RESOURCE_H

#include "DrawableResource.h"
#include <math/Matrix44F.h>

namespace alo {

class InstancedResource : public DrawableResource {

    SimpleBuffer<Matrix44F> m_tms;
    int m_numInstances;
    
public:
    InstancedResource();
    virtual ~InstancedResource();
    
    virtual void attachToDrawable(DrawableObject *object) override;
    
    const SimpleBuffer<Matrix44F> &transformBuffer() const;
    SimpleBuffer<Matrix44F> &transformBuffer();
    
    const int &numInstances() const;
    void setNumInstances(int x);
    
protected:

private:

};


}

#endif
