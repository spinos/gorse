/*
 *  pointCylinder.h
 *  gorse
 *
 *  2019/5/12
 */

#ifndef ALO_MATH_POINT_CYLINDER_H
#define ALO_MATH_POINT_CYLINDER_H

#include "miscfuncs.h"

namespace alo {

/// distance moved
inline float movePointOntoLocalCylinder(Vector3F &q, 
	const float &radius, const float &height) 
{
    const float &h = q.y;
    Vector3F vq(q.x, 0.f, q.z);
    const float lvq = vq.length();
    
    float d;
    if(h < 1e-3f) {
        d = -h;
        if(lvq < radius) {
            q.y = 0.f;
            return d;
        }
        vq *= radius / lvq;
        d = q.distanceTo(vq);
        q = vq;
        return d;
    }
    
    if(h > height - 1e-3f) {
        d = h - height;
        if(lvq < radius) {
            q.y = height;
            return d;
        }
        vq = vq * (radius / lvq) + Vector3F(0.f, height, 0.f);
        d = q.distanceTo(vq);
        q = vq;
        return d;
    }
    
    d = lvq - radius;
    
    vq *= radius / lvq;
    q.x = vq.x;
    q.z = vq.z;
    
    return d;
}
	
/// on body or either of the caps
inline void randomPointOnCylinder(float *q, const float &radius, const float &height,
								const float &bodyRatio) 
{
	const float theta = TWOPIF * RandomF01();
	const float u = RandomF01();

	if(RandomF01() < bodyRatio) {
		q[0] = cos(theta) * radius;
		q[1] = u * height;
		q[2] = sin(theta) * radius;

	} else {
		const float r = sqrt(u) * radius;
		q[0] = cos(theta) * r;
		q[1] = 0.f;
		q[2] = sin(theta) * r;

		if(RandomF01() > .5f) q[1] = height;
	}
}

} /// end of alo

#endif
