#ifndef RAY_CONE_H
#define RAY_CONE_H

#include "line_math.h"
#include "Ray.h"
#include "rayPlane.h"

namespace alo {

/// distance to q
inline float distanceToCone(Vector3F &nml, const Vector3F &q, 
			const Vector3F& p0, const Vector3F& p1, const float& rc, const float &lc, const Vector3F &vp0p1)
{
	const float ang = rc/sqrt(lc*lc + rc*rc);

	if((q-p1).normal().dot(vp0p1 * -1.f) < -ang) {
		nml = q - p1;
		return (q-p1).length();
	}

	Vector3F vp = projectPointOnPlane(q, p0, vp0p1) - p0;
	vp.normalize(); 
	nml = vp + vp0p1 * ang;
	vp = p0 + vp * rc;
	vp = q - vp;

	float lvp = vp.length();
	if(lvp < 1e-3f) return 0.f;
	vp = vp / lvp;

	if(vp.dot(vp0p1 * -1.f) >= -ang)
		return lvp;

	float h = (q - p0).dot(vp0p1);
	float r = sqrt((q - p0).length2() - h * h) - rc * (1.f - h / lc);
	return r * lc/sqrt(lc*lc + rc*rc);
}

inline bool rayConeIntersect(float& t, Vector3F &hitNormal,
		const Ray& r, 
		const Vector3F& p0, const Vector3F& p1, const float& rc, const float &lc, const Vector3F &vp0p1)
{
/// ignore ray t0
	const Vector3F &rayBegin = r.origin();
	const Vector3F rayEnd = r.travel(t);
	const float tLimit = t;

/// line to ray
	if(distanceBetweenLines(rayBegin, rayEnd, p0, p1) >= rc) return false;

	Vector3F q;
	float tq, lp2r;
	lp2r = distancePointLine(p0, rayBegin, rayEnd);
	projectPointLineSegment(q, tq, lp2r, p0, rayBegin, rayEnd, r.direction());

	if((q - p0).dot(vp0p1) < 0 ) {
		if(lp2r > rc) return false;
	}

	lp2r = distancePointLine(p1, rayBegin, rayEnd);
	projectPointLineSegment(q, tq, lp2r, p1, rayBegin, rayEnd, r.direction());

	if((q - p1).dot(vp0p1) > 0 ) {
		if(lp2r > rc) return false;
	}

	const Vector3F vp1p0 = vp0p1 * -1.f;

	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p0, vp0p1)) {
		q = r.travel(tq);
		if(q.distanceTo(p0) > rc && (q - p0).dot(r.direction()) < 0) 
			return false;
	}	


	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p1, vp1p0)) {
		q = r.travel(tq);
		if(q.distanceTo(p1) > rc && (q - p1).dot(r.direction()) < 0) 
			return false;
	}

	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p0, vp1p0)) {
		
		q = r.travel(tq);
		if(q.distanceTo(p0) <= rc) {
			if(tq > t) return false;
			t = tq;

			hitNormal = vp1p0;
			return true;
		}

		if((q - p0).dot(r.direction()) > 0) return false;

/// behind bottom
		if(r.direction().dot(vp0p1) > lc/sqrt(lc*lc + rc*rc)) return false;

	}

	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p1, vp0p1)) {
		q = r.travel(tq);
		if(q.distanceTo(p1) > rc && (q - p1).dot(r.direction()) > 0) 
			return false;
	}


	q = rayBegin;
	t = 0.f;
	float preStep = 1e10f;

	for(int i=0;i<19;++i) {

		float d2c = distanceToCone(hitNormal, q, p0, p1, rc, lc, vp0p1);

		if(d2c < 5e-3f) break;

		if(preStep < d2c) return false;

		preStep = d2c;

		t += d2c;
		if(t > tLimit) return false;
		q = r.travel(t);

	}	
	
	hitNormal.normalize();
	return true;
}

}

#endif
