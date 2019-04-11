#ifndef RAY_CAPSULE_H
#define RAY_CAPSULE_H

#include "line_math.h"
#include "Ray.h"

namespace alo {

/// r ray 
/// p0 capsule end0 position
/// p1 capsule end1 position
/// rc capsule radius
/// vp0p1 normalized (p0,p1)
/// t ray distance

inline bool rayCapsuleIntersect(float& t, Vector3F &hitNormal,
		const Ray& r, 
		const Vector3F& p0, const Vector3F& p1, const float& rc, const float &lc, const Vector3F &vp0p1)
{
/// ignore ray t0
	const Vector3F &rayBegin = r.origin();
	const Vector3F rayEnd = r.travel(t);
	
/// line to ray
	if(distanceBetweenLines(rayBegin, rayEnd, p0, p1) >= rc) return false;

	const float tLimit = t;

/// ends to ray
	Vector3F q;
	float tq;
	float lp2r = distancePointLine(p0, rayBegin, rayEnd);
	projectPointLineSegment(q, tq, lp2r, p0, rayBegin, rayEnd, r.direction());

	if((q - p0).dot(vp0p1) < 0 ) {
		if(lp2r > rc) return false;
	}

	lp2r = distancePointLine(p1, rayBegin, rayEnd);
	projectPointLineSegment(q, tq, lp2r, p1, rayBegin, rayEnd, r.direction());

	if((q - p1).dot(vp0p1) > 0 ) {
		if(lp2r > rc) return false;
	}

	t = 0.f;
	q = rayBegin;
	for(int i=0;i<19;++i) {
		distancePointLineSegment(tq, q, p0, p1);
		if(tq - rc < 4e-5f) break;
		t += tq - rc;
		if(t > tLimit) return false;
		q = r.travel(t);
	}

	Vector3F pol;
	projectPointLine(pol, tq, q, p0, p1, vp0p1);
	if(tq <= 0)
		hitNormal = q - p0;
	else if(tq >= lc)
		hitNormal = q - p1;
	else
		hitNormal = q - pol;

	hitNormal.normalize();

	return true;
}

}

#endif
