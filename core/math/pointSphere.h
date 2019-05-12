/*
 *  pointSphere.h
 *  gorse
 *
 *  http://mathworld.wolfram.com/SpherePointPicking.html
 *
 *  2019/5/12
 */

#ifndef ALO_MATH_POINT_SPHERE_H
#define ALO_MATH_POINT_SPHERE_H

#include "miscfuncs.h"

namespace alo {
	
/// uniformly pick point q on sphere of radius
inline void randomPointOnSphere(float *q, const float &radius) 
{
	const float theta = TWOPIF * RandomF01();
	const float u = RandomFn11();
	const float mu = sqrt(1.f - u * u);
	q[0] = cos(theta) * mu * radius;
	q[1] = sin(theta) * mu * radius;
	q[2] = u * radius;
}

} /// end of alo

#endif
