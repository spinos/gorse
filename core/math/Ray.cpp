/*
 *  Ray.cpp
 *  lapl
 *
 *  Created by jian zhang on 3/16/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "Ray.h"
//#include "line_math.h"

namespace alo {

Ray::Ray() {}

Ray::Ray(const Vector3F& pfrom, const Vector3F& vdir, float tmin, float tmax)
{
	m_origin = pfrom;
	m_dir = vdir;
	m_tmin = tmin;
	m_tmax = tmax;
	if(m_tmax > 1e7f) {
		std::cout<<"\n truncate ray max to 1e7f";
		m_tmax = 1e7f;
	}
}

Ray::Ray(const Vector3F& pfrom, const Vector3F& pto) 
{
	m_origin = pfrom;
	m_dir = pto - pfrom;
	m_tmin = 0.f;
	m_tmax = m_dir.length();
	if(m_tmax > 1e7f) {
		std::cout<<"\n truncate ray max to 1e7f";
		m_tmax = 1e7f;
	}
	m_dir.normalize();
}

void Ray::set(const float* v)
{
	memcpy(&m_origin, v, 12);
	memcpy(&m_dir, &v[3], 12);
	m_tmin = v[6];
	m_tmax = v[7];
}

void Ray::get(float* v) const
{
	memcpy(v, &m_origin, 12);
	memcpy(&v[3], &m_dir, 12);
	v[6] = m_tmin;
	v[7] = m_tmax;
}

void Ray::operator=(const Ray& b)
{ 
	m_origin = b.m_origin;
	m_tmin = b.m_tmin;
	m_dir = b.m_dir;
	m_tmax = b.m_tmax;
}

Vector3F Ray::travel(const float & t) const
{ return m_origin + m_dir * t; }

Vector3F Ray::destination() const
{ return m_origin + m_dir * m_tmax; }

const Vector3F Ray::closestPointOnRay(const Vector3F & p, float * t) const
{
	float tt = (p - m_origin).dot(m_dir);
	if(t) *t = tt;
	return m_origin + m_dir * tt;
}

Vector3F& Ray::origin()
{ return m_origin; }

Vector3F& Ray::direction()
{ return m_dir; }

const Vector3F& Ray::origin() const
{ return m_origin; }

const Vector3F& Ray::direction() const
{ return m_dir; }

float Ray::length() const
{ return m_tmax - m_tmin; }

float Ray::projectP(const Vector3F & q, Vector3F & pproj) const
{ 
	Vector3F voq = q - m_origin;
	float voqdotdir = voq.dot(m_dir);
	if(voqdotdir < m_tmin) {
		pproj = travel(m_tmin);
		return -1.f;
	}
		
	if(voqdotdir >= m_tmax) {
		pproj = travel(m_tmax);
		return -1.f;
	}
		
	pproj = travel(voqdotdir);
	return voqdotdir;
	
}

float Ray::distanceToPoint(const Vector3F & q) const
{
	Vector3F pol;
	projectP(q, pol);
	return q.distanceTo(pol);
}

}