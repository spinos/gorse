/*
 *  raySphere.h
 *  aloe
 *
 *  Created by jian zhang on 3/28/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_MATH_RAY_SPHERE_H
#define ALO_MATH_RAY_SPHERE_H

#include "Ray.h"
#include <cmath>

namespace alo {
    
/// r ray 
/// p sphere center
/// sr sphere radius
/// solve t^2*A + t*B + C = 0
/// A = d.d
/// B = 2 * (o-p).d
/// C = (o-p).(o-p) - Sr^2
/// t ray distance
inline bool raySphereIntersect(float& t, const Ray& r, const Vector3F& p, const float& sr)
{
	Vector3F op = p - r.origin();
	float b = op.dot(r.direction() );
	float det = b * b - op.dot(op) + sr * sr;
	if(det < 0) return false;
	det = sqrt(det);
	t = b - det;
	if(t > 1e-4f) return true;
	t = b + det;
	if(t > 1e-4f) return true;
	return false;
}

inline bool rayLocalSphereIntersect(float *rayD, const float& sr)
{
	Vector3F op(-rayD[0], -rayD[1], -rayD[2]);
	float b = op.dot(Vector3F(rayD[3], rayD[4], rayD[5]) );
	float det = b * b - op.dot(op) + sr * sr;
	if(det < 0) return false;
	det = sqrt(det);

	float &t = rayD[6];
	t = b - det;
	if(t > 1e-4f) return true;
	t = b + det;
	if(t > 1e-4f) return true;
	return false;
}

}

#endif
