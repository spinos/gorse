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
								const float &bodyRatio, const float &fuzziness) 
{
	const float theta = TWOPIF * RandomF01();
	const float u = RandomF01();
    const float fr = 1.f - RandomF01() * fuzziness;

	if(RandomF01() < bodyRatio) {
		q[0] = cos(theta) * radius * fr;
		q[1] = u * height;
		q[2] = sin(theta) * radius * fr;

	} else {
		const float r = sqrt(u) * radius;
        const float h = (radius < height) ? radius : height;
		q[0] = cos(theta) * r;
		q[1] = h * .1f * (1.f - fr);
		q[2] = sin(theta) * r;

		if(RandomF01() > .5f) q[1] = height - q[1];
	}
}

inline Vector3F normalOnLocalCylinder(const float *q, 
	const float &radius, const float &height)
{
    const float toTop = Absolute(q[1] - height);
    const float toBottom = Absolute(q[1]);
    const float toBody = Absolute(sqrt(q[0] * q[0] + q[2] * q[2]) - radius);
    
	const float &h = q[1];
	if(toBottom < toTop && toBottom < toBody)
        return Vector3F(0.f, -1.f, 0.f);
    
    if(toTop < toBottom && toTop < toBody)
        return Vector3F::YAxis;
    
    Vector3F n(q[0], 0.f, q[2]);
    n.normalize();

	return n;

}

} /// end of alo

#endif
