/*
 *  pointCone.h
 *  gorse
 *
 *  2019/5/12
 */

#ifndef ALO_MATH_POINT_CONE_H
#define ALO_MATH_POINT_CONE_H

#include "miscfuncs.h"

namespace alo {

/// distance moved
inline float movePointOntoLocalCone(Vector3F &q, const float &height, const float &radius, const float &lcrc)
{
    const float ang = radius/lcrc;
    const Vector3F p1(0.f, height, 0.f);
    
    Vector3F nml = q - p1;
    float d = nml.length();
    nml /= d;
    
    if(nml.y > ang) {
        q = p1;
        return d;
    }
    
    Vector3F vq(q.x, 0.f, q.z);
    const float lvq = vq.length();
    
    const float &h = q.y;
    if(h < 9e-3f) {
        d = -h;
        if(lvq < radius) {
            q.y = 0.f;
            return d;
        }
        vq *= radius / lvq;
        d = q.distanceTo(vq);
        if(d > Absolute(h)) {
            q = vq;
            return d;
        }
    }
    
    vq *= radius / lvq;
    
    nml = q - vq;
    d = nml.length();
    nml /= d;
    
    if(nml.y < ang) {
        q = vq;
        return d;
    }
    
    d = sqrt(q.x * q.x + q.z * q.z) - radius * (1.f - h / height);
    d *= height/lcrc;
    
    nml.y = ang;
    nml.normalize();
    q -= nml * d;
    
    return d;
}
	
/// on body or either of the caps
inline void randomPointOnCone(float *q, const float &radius, const float &height,
								const float &bodyRatio) 
{
	const float theta = TWOPIF * RandomF01();
	const float u = sqrt(RandomF01());
    const float r = u * radius;

	if(RandomF01() < bodyRatio)	{
        q[0] = cos(theta) * r;
		q[1] = (1.f - u) * height;
        q[2] = sin(theta) * r;
    } else {
        q[0] = cos(theta) * r;
        q[1] = 0.f;
        q[2] = sin(theta) * r;
    }

}

} /// end of alo

#endif
