/*
 *  RenderableCone.cpp
 *  vachellia
 *
 */

#include "RenderableCone.h"
#include <math/rayCone.h>
#include "IntersectResult.h"

namespace alo {

RenderableCone::RenderableCone() :
m_p0(0.f, 0.f, 0.f),
m_r(2.f),
m_p1(7.f, 0.f, 0.f),
m_l(7.f),
m_dir(1.f, 0.f, 0.f),
m_lcrc(7.28010989f)
{}

void RenderableCone::setPoints(const Vector3F &p0, const Vector3F &p1)
{
	m_p0 = p0; m_p1 = p1;
	m_dir = p1 - p0; 
	m_l = m_dir.length();
	m_dir /= m_l;
	m_lcrc = sqrt(m_r*m_r + m_l*m_l);
}

void RenderableCone::setRadius(float x)
{ 
	m_r = x; 
	m_lcrc = sqrt(m_r*m_r + m_l*m_l);
}

bool RenderableCone::intersectRay(const Ray& aray, IntersectResult& result) const
{
	float tt = result.rayDistance();
	Vector3F tn;
	if(!rayConeIntersect(tt, tn, aray, m_p0, m_p1, m_r, m_l, m_lcrc, m_dir) )
		return false;

	return result.updateRayDistance(tt, tn);
}

}
