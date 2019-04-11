#ifndef RAY_CYLINDER_H
#define RAY_CYLINDER_H

#include "line_math.h"
#include "rayPlane.h"
#include "Ray.h"

namespace alo {

inline bool rayCylinderIntersect(float& t, Vector3F &hitNormal,
		const Ray& r, 
		const Vector3F& p0, const Vector3F& p1, const float& rc, const Vector3F &vp0p1)
{
/// ignore ray t0
	const Vector3F &rayBegin = r.origin();
	const Vector3F rayEnd = r.travel(t);
	const float tLimit = t;

/// line to ray
	if(distanceBetweenLines(rayBegin, rayEnd, p0, p1) >= rc) return false;

	const Vector3F vp1p0 = vp0p1 * -1.f;

	Vector3F q, c;
	float tq, lp2r;

	lp2r = distancePointLine(p0, rayBegin, rayEnd);
	projectPointLineSegment(q, tq, lp2r, p0, rayBegin, rayEnd, r.direction());

	if((q - p0).dot(vp1p0) > 0 && (q - p0).length() > rc) return false;

	lp2r = distancePointLine(p1, rayBegin, rayEnd);
	projectPointLineSegment(q, tq, lp2r, p1, rayBegin, rayEnd, r.direction());

	if((q - p1).dot(vp0p1) > 0 && (q - p1).length() > rc) return false;

/// caps
	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p0, vp1p0)) {
		
		q = r.travel(tq);
		if(q.distanceTo(p0) <= rc) {
			if(tq > t) return false;
			t = tq;

			hitNormal = vp1p0;
			return true;
		}

		if((q - p0).dot(r.direction()) > 0) return false;
	}	

	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p0, vp0p1)) {
		q = r.travel(tq);
		if(q.distanceTo(p0) > rc && (q - p0).dot(r.direction()) < 0) 
			return false;
	}

	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p1, vp0p1)) {
		
		q = r.travel(tq);
		if(q.distanceTo(p1) <= rc) {
			if(tq > t) return false;

			t = tq;
			hitNormal = vp0p1;
			return true;
		}

		if((q - p1).dot(r.direction()) > 0) return false;
	}

	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p1, vp1p0)) {
		q = r.travel(tq);
		if(q.distanceTo(p1) > rc && (q - p1).dot(r.direction()) < 0) 
			return false;
	}
	
/// body
	t = 0.f;
	q = rayBegin;
	for(int i=0;i<19;++i) {
		distancePointLineSegment(tq, q, p0, p1);
		if(tq - rc < 4e-5f) break;
		t += tq - rc;
		if(t > tLimit) return false;
		q = r.travel(t);

		if(r.direction().dot(vp0p1) > 0 && (q - p1).dot(vp0p1) > 0) return false;
		if(r.direction().dot(vp1p0) > 0 && (q - p0).dot(vp1p0) > 0) return false;
	}


	Vector3F pol;
	projectPointLine(pol, tq, q, p0, p1, vp0p1);

	hitNormal = q - pol;
	hitNormal.normalize();

	return true;
}

}

#endif
