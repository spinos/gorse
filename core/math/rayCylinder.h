#ifndef RAY_CYLINDER_H
#define RAY_CYLINDER_H

#include "line_math.h"
#include "rayPlane.h"
#include "rayBox.h"
#include "Ray.h"

namespace alo {
    
inline float distanceToCylinder(Vector3F &normal, bool &isOnBody,
	const Vector3F &q, 
	const Vector3F &p0, const Vector3F &p1, const float &rc, const float &lc, const Vector3F &vp0p1)
{
    isOnBody = true;
        
    Vector3F p, v;
	float h = (q - p0).dot(vp0p1);
	if(h < 0) {
        normal = vp0p1 * -1.f;
        p = projectPointOnPlane(q, p0, vp0p1);
        if(p.distanceTo(p0) > rc) {
            isOnBody = false;
            v = p - p0;
            v.normalize();
            return q.distanceTo(p0 + v * rc);
        } 
        return -h;
	}
    
    p = projectPointOnPlane(q, p1, vp0p1);
	
    if(h > lc) {
        normal = vp0p1;
        if(p.distanceTo(p1) > rc) {
            isOnBody = false;
            v = p - p1;
            v.normalize();
            return q.distanceTo(p1 + v * rc);
        }
        return h - lc;
	} 
    
    normal = p - p1;
	normal.normalize();

	return q.distanceTo(p0 + vp0p1 * h) - rc;

}

inline bool rayCylinderIntersect(float& t, Vector3F &hitNormal,
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
    
	Vector3F q;
	float tq;
/*
/// caps
	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p0, vp1p0)) {
		
		q = r.travel(tq);
		if(q.distanceTo(p0) <= rc) {
			if(tq > tLimit) return false;
			t = tq;

			hitNormal = vp1p0;
			return true;
		}
	}	

	if(rayPlaneIntersect(tq, rayBegin, r.direction(), p1, vp0p1)) {
		
		q = r.travel(tq);
		if(q.distanceTo(p1) <= rc) {
			if(tq > tLimit) return false;

			t = tq;
			hitNormal = vp0p1;
			return true;
		}
	}
*/
	bool isOnBody;
	float preStep = 1e10f;
	t = 0.f;
	q = rayBegin;
	for(int i=0;i<29;++i) {
		tq = distanceToCylinder(hitNormal, isOnBody, q, p0, p1, rc, lc, vp0p1);
        if(hitNormal.dot(rayDir) > 0) return false;

		if(tq < t * 1.4e-5f) return true;
        if(preStep < tq) return false;
		preStep = tq;
        
        if(isOnBody) {
            float ang = rayDir.dot(hitNormal);
            if(ang <0) ang = -ang;
            ang = 1.f / (1e-3f + ang);
            tq *= ang;
        }
        
        t += tq;

		if(t > tLimit) return false;

		q = rayBegin + rayDir * t;
	}

	return false;
}

inline float distanceToLocalCylinder(Vector3F &normal, bool &isOnBody,
	const Vector3F &q, 
	const float &radius, const float &height)
{
    isOnBody = true;
        
    Vector3F p, v;
	const float h = q.y;
	if(h < 0) {
        normal.set(0.f, -1.f, 0.f);
        p.set(q.x, 0.f, q.z);
        if(p.length() > radius) {
            isOnBody = false;
            v = p;
            v.normalize();
            return q.distanceTo(v * radius);
        } 
        return -h;
	}
    
    const Vector3F p1(0.f, height, 0.f);
    p.set(q.x, height, q.z);
	
    if(h > height) {
        normal = Vector3F::YAxis;
        if(p.distanceTo(p1) > radius) {
            isOnBody = false;
            v = p - p1;
            v.normalize();
            return q.distanceTo(p1 + v * radius);
        }
        return h - height;
	} 
    
    normal = p - p1;
    float nl = normal.length();
	normal /= nl;

	return nl - radius;

}

inline bool rayLocalCylinderIntersect(float *rayD, const float &height, const float &radius, const float *aabb)
{
    const Vector3F rayBegin(rayD[0], rayD[1], rayD[2]);
	const Vector3F rayDir(rayD[3], rayD[4], rayD[5]);

    if(!rayAabbIntersect(rayD, aabb)) return false;

    const float tLimit = rayD[7];
    
	Vector3F q;
    Vector3F hitNormal;

    bool isOnBody;
    float &t = rayD[6];
	q = rayBegin + rayDir * t;
	for(int i=0;i<29;++i) {
		float tq = distanceToLocalCylinder(hitNormal, isOnBody, q, radius, height);

		if(tq < 1e-3f) return true;
        
        if(isOnBody) {
            float ang = rayDir.dot(hitNormal);
            if(ang <0) ang = -ang;
            ang = 1.f / (1e-3f + ang);
            tq *= ang;
        }
        
        t += tq;

		if(t > tLimit) return false;

		q = rayBegin + rayDir * t;
	}
    
    return true;
}

}

#endif
