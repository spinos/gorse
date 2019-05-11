#ifndef RAY_CAPSULE_H
#define RAY_CAPSULE_H

#include "line_math.h"
#include "rayPlane.h"
#include "Ray.h"

namespace alo {
    
inline float distanceToCapsule(Vector3F &normal,
    const Vector3F &q, 
	const Vector3F &p0, const Vector3F &p1, const float &rc, const float &lc, const Vector3F &vp0p1)
{
    float h = (q - p0).dot(vp0p1);
    if(h < 0) {
        normal = q - p0;    
    } else if(h > lc) {
        normal = q - p1;
    } else {
        normal = projectPointOnPlane(q, p1, vp0p1) - p1;
        normal.normalize();
        
        return q.distanceTo(p0 + vp0p1 * h + normal * rc);
    }
    
    h = normal.length();
    normal /= h;
    return h - rc;
}    

/// r ray 
/// p0 capsule end0 position
/// p1 capsule end1 position
/// rc capsule radius
/// vp0p1 normalized (p0,p1)
/// t ray distance

inline bool rayCapsuleIntersect(float& t, Vector3F &hitNormal,
		const float *rayD, 
		const Vector3F& p0, const Vector3F& p1, const float& rc, const float &lc, const Vector3F &vp0p1)
{
/// ignore ray t0
	const Vector3F rayBegin(rayD[0], rayD[1], rayD[2]);
    const Vector3F rayDir(rayD[3], rayD[4], rayD[5]);
	const Vector3F rayEnd = rayBegin + rayDir * t;
    
    if(clipRayCapsule(rayBegin, rayEnd, rayDir, p0, p1, rc, vp0p1)) 
        return false;

	const float tLimit = t;

/// ends to ray
	Vector3F q;
	float tq;
    float preStep = 1e10f;

	t = 0.f;
	q = rayBegin;
	for(int i=0;i<19;++i) {
		tq = distanceToCapsule(hitNormal, q, p0, p1, rc, lc, vp0p1);
        
		if(tq < 5e-5f) return true;
        if(preStep < tq) return false;
        preStep = tq;
        
        float ang = rayDir.dot(hitNormal);
        if(ang <0) ang = -ang;
        ang = 1.f / (1e-3f + ang);
            
		t += tq*ang;
		if(t > tLimit) return false;
		q = rayBegin + rayDir * t;
	}

    return false;
}

}

#endif
