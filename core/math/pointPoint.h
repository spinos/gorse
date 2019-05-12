/*
 *  pointPoint.h
 *  gorse
 *
 *  2019/5/12
 */

#ifndef ALO_MATH_POINT_POINT_H
#define ALO_MATH_POINT_POINT_H

#include "miscfuncs.h"

namespace alo {

inline float vectorLength(const float &x, const float &y, const float &z)
{
	return sqrt(x * x + y * y + z * z);
}

inline float vectorLength(const float *q)
{
	return vectorLength(q[0], q[1], q[2]);
}

inline float distancePointToPoint(const float *a, const float *b)
{
    float dx = a[0] - b[0];
    float dy = a[1] - b[1];
    float dz = a[2] - b[2];
    return vectorLength(dx, dy, dz);
}

} /// end of alo

#endif
