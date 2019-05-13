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
								const float &bodyRatio, const float &fuzziness) 
{
	const float theta = TWOPIF * RandomF01();
	const float u = sqrt(RandomF01());
    const float r = u * radius;
    const float fr = 1.f - RandomF01() * fuzziness;

	if(RandomF01() < bodyRatio)	{
        q[0] = cos(theta) * r * fr;
		q[1] = (1.f - u) * height;
        q[2] = sin(theta) * r * fr;
    } else {
        const float h = (radius < height) ? radius : height;
        q[0] = cos(theta) * r;
        q[1] = h * .1f * (1.f - fr);
        q[2] = sin(theta) * r;
    }

}

inline Vector3F normalOnLocalCone(const float *q, 
	const float &radius, const float &height, const float &lcrc)
{
    const float ang = radius/lcrc;
    const Vector3F p1(0.f, height, 0.f);
    
    Vector3F nml = Vector3F(q[0], q[1], q[2]) - p1;
    nml.normalize();
    
    if(nml.y > ang) return nml;
    
    Vector3F vp(q[0], 0.f, q[2]);
	const float &h = q[1];
	if(h < 1e-3f && vp.length() < radius - 1e-3f)
        return Vector3F(0.f, -1.f, 0.f);
    
    vp.normalize();
	
    nml = vp + Vector3F(0.f, ang, 0.f);
    nml.normalize();
	return nml;

}

} /// end of alo

#endif
