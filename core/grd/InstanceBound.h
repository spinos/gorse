/*
 *  InstanceBound.h
 *
 *  2019/4/26
 */

#ifndef INSTANCE_BOUND_H
#define INSTANCE_BOUND_H

#include <math/Matrix44F.h>
#include <math/pointBox.h>

namespace alo {

struct InstanceBound {

    int _instanceId;
    const float *_aabb;
    Matrix44F *_tm;
    Matrix44F *_invTm;
    
    float mapDistance(const float *q) const
    {
        float a[3];
        memcpy(a, q, 12);
        _invTm->transformPoint(a);
        
        if(isPointOutsideBox(a, _aabb))
            return distanceOutsidePointToBox(a, _aabb);

        return -distanceInsidePointToBox(a, _aabb);
    }
    
};

}

#endif
