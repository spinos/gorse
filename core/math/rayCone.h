#ifndef RAY_CONE_H
#define RAY_CONE_H

#include "line_math.h"
#include "Ray.h"
#include "rayPlane.h"

namespace alo {

/// distance to q
inline float distanceToCone(Vector3F &nml, Vector3F &hitTangent, bool &isOnBody,
			const Vector3F &q, 
			const Vector3F& p0, const Vector3F& p1, 
			const float& rc, const float &lc, const float &lcrc,
			const Vector3F &vp0p1, const Vector3F &vp1p0)
{
	const float ang = rc/lcrc;

	if((q-p1).normal().dot(vp1p0) < -ang) {
		nml = q - p1;
		isOnBody = false;
		return (q-p1).length();
	}

	Vector3F vp = projectPointOnPlane(q, p0, vp0p1) - p0;

	float h = (q - p0).dot(vp0p1);
	
	if(h < 0 && vp.length() < rc) {
		nml = vp1p0;
		isOnBody = false;
		return -h;
	}

	vp.normalize();
	nml = vp + vp0p1 * ang;
	vp = p0 + vp * rc;

	hitTangent = p1 - vp;
	
	vp = q - vp;

	float lvp = vp.length();
	if(lvp < 1e-4f) return 0.f;
	vp = vp / lvp;

	if(vp.dot(vp1p0) >= -ang) {
		isOnBody = false;
		return lvp;
	}

	isOnBody = true;
	hitTangent.normalize();

	float r = sqrt((q - p0).length2() - h * h) - rc * (1.f - h / lc);
	return r * lc/lcrc;
}

inline bool rayConeIntersect(float& t, Vector3F &hitNormal,
		const Ray& r, 
		const Vector3F& p0, const Vector3F& p1, 
		const float& rc, const float &lc, const float &lcrc,
		const Vector3F &vp0p1)
{
/// ignore ray t0
	const Vector3F &rayBegin = r.origin();
	const Vector3F rayEnd = r.travel(t);
	const float tLimit = t;
    
    if(clipRayCapsule(rayBegin, rayEnd, r.direction(), p0, p1, rc, vp0p1)) 
        return false;
    
    Vector3F q;
	float tq;
    const Vector3F vp1p0 = vp0p1 * -1.f;

	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p0, vp1p0)) {
		
		q = r.travel(tq);
		if(q.distanceTo(p0) <= rc) {
			if(tq > t) return false;
			t = tq;

			hitNormal = vp1p0;
			return true;
		}

/// behind bottom
		if(r.direction().dot(vp0p1) > lc/lcrc) return false;

	}

	q = rayBegin;
	t = 0.f;
	float preStep = 1e10f;
	bool isOnBody;
	Vector3F hitTangent;

	for(int i=0;i<29;++i) {

		float d2c = distanceToCone(hitNormal, hitTangent, isOnBody, 
			q, p0, p1, rc, lc, lcrc, vp0p1, vp1p0);
            
		if(d2c < 5e-3f) return true;

		if(preStep < d2c) return false;
		preStep = d2c;

		if(isOnBody) {
			float ang = (r.direction()).dot(hitTangent);
			ang = 1.f / (1e-4f + sqrt(1.f - ang * ang));
			t += d2c * ang;
		}
		else
			t += d2c;

		if(t > tLimit) return false;
		q = r.travel(t);

	}	

	return false;
	
}

}

#endif
