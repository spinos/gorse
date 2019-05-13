#ifndef RAY_CONE_H
#define RAY_CONE_H

#include "line_math.h"
#include "Ray.h"
#include "rayPlane.h"
#include "rayBox.h"

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
		const float *rayD, 
		const Vector3F& p0, const Vector3F& p1, 
		const float& rc, const float &lc, const float &lcrc,
		const Vector3F &vp0p1)
{
/// ignore ray t0
	const Vector3F rayBegin(rayD[0], rayD[1], rayD[2]);
	const Vector3F rayDir(rayD[3], rayD[4], rayD[5]);
	const Vector3F rayEnd = rayBegin + rayDir * t;
	const float tLimit = t;
    
    if(clipRayCapsule(rayBegin, rayEnd, rayDir, p0, p1, rc, vp0p1)) 
        return false;
    
    Vector3F q;
	float tq;
    const Vector3F vp1p0 = vp0p1 * -1.f;

	if(rayPlaneIntersect(tq, rayBegin, rayDir, p0, vp1p0)) {
		
		q = rayBegin + rayDir * tq;//r.travel(tq);
		if(q.distanceTo(p0) <= rc) {
			if(tq > t) return false;
			t = tq;

			hitNormal = vp1p0;
			return true;
		}

/// behind bottom
		if(rayDir.dot(vp0p1) > lc/lcrc) return false;

	}

	q = rayBegin;
	t = 0.f;
	float preStep = 1e10f;
	bool isOnBody;
	Vector3F hitTangent;

	for(int i=0;i<29;++i) {

		float d2c = distanceToCone(hitNormal, hitTangent, isOnBody, 
			q, p0, p1, rc, lc, lcrc, vp0p1, vp1p0);
            
		if(d2c < t * 1.4e-5f) return true;

		if(preStep < d2c) return false;
		preStep = d2c;

		if(isOnBody) {
			float ang = rayDir.dot(hitTangent);
			ang = 1.f / (1e-4f + sqrt(1.f - ang * ang));
			t += d2c * ang;
		}
		else
			t += d2c;

		if(t > tLimit) return false;
		q = rayBegin + rayDir * t;

	}	

	return false;
	
}

inline float distanceToLocalCone(Vector3F &hitTangent, bool &isOnBody,
			const Vector3F &q, 
			const float& radius, const float &height, const float &lcrc)
{
	const float ang = radius/lcrc;
    
    const Vector3F p1(0.f, height, 0.f);

	if((q-p1).normal().y * -1.f < -ang) {
		isOnBody = false;
		return (q-p1).length();
	}

	Vector3F vp(q.x, 0.f, q.z);

	const float &h = q.y;
	
	if(h < 0 && vp.length() < radius) {
		isOnBody = false;
		return -h;
	}

	vp.normalize();
	vp = vp * radius;

	hitTangent = p1 - vp;
	
	vp = q - vp;

	float lvp = vp.length();
	if(lvp < 1e-4f) return 0.f;
	vp = vp / lvp;

	if(-vp.y >= -ang) {
		isOnBody = false;
		return lvp;
	}

	isOnBody = true;
	hitTangent.normalize();

	float r = sqrt(q.length2() - h * h) - radius * (1.f - h / height);
	return r * height/lcrc;
}

inline bool rayLocalConeIntersect(float *rayD, const float &height, const float &radius, const float &lcrc, const float *aabb)
{
    const Vector3F rayBegin(rayD[0], rayD[1], rayD[2]);
	const Vector3F rayDir(rayD[3], rayD[4], rayD[5]);

    if(!rayAabbIntersect(rayD, aabb)) return false;
    
    float &t = rayD[6];
	Vector3F q = rayBegin + rayDir * t;
    
    if(q.y < 1e-3f) {
        if(sqrt(q.x * q.x + q.z * q.z) < radius) return true;
    }

    const float tLimit = rayD[7];
    
    Vector3F hitTangent;

    bool isOnBody;
    

	for(int i=0;i<29;++i) {
		float tq = distanceToLocalCone(hitTangent, isOnBody, q, radius, height, lcrc);

		if(tq < 1e-3f) return true;
        
        if(isOnBody) {
            float ang = rayDir.dot(hitTangent);
            ang = 1.f / (1e-4f + sqrt(1.f - ang * ang));
            t += tq * ang;
        }
        else 
            t += tq;

		if(t > tLimit) return false;

		q = rayBegin + rayDir * t;
	}
    
    return true;
}

}

#endif
