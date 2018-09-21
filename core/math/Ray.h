/*
 *  Ray.h
 *  aloe
 *
 *  Created by jian zhang on 3/16/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ALO_RAY_H
#define ALO_RAY_H

#include "Vector3F.h"

namespace alo {

class Ray {
public:
	Ray();
	Ray(const Vector3F& pfrom, const Vector3F& vdir, float tmin, float tmax);
	Ray(const Vector3F& pfrom, const Vector3F& pto);
/// v[8] as (origin, direction, t0, t1)
	void set(const float* v);
	void operator=(const Ray& b);
	Vector3F travel(const float & t) const;
	
	Vector3F& origin();
	Vector3F& direction();
	const Vector3F& origin() const;
	const Vector3F& direction() const;
	Vector3F destination() const;
	float length() const;
	const Vector3F closestPointOnRay(const Vector3F & p, float * t = NULL) const;
	
    Vector3F m_origin;
	float m_tmin;
	Vector3F m_dir;
	float m_tmax;
	
	const std::string str() const;
	
	friend std::ostream& operator<<(std::ostream &output, const Ray & p) {
        output << "(" << p.m_origin <<","<<p.m_dir
                << "(" << p.m_tmin <<", "<<p.m_tmax<<")";
        return output;
    }
	
/// project q on line, return t
/// if return -1 out of range
	float projectP(const Vector3F & q, Vector3F & pproj) const;
	
	float distanceToPoint(const Vector3F & q) const;
	
};

}
#endif