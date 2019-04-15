/*
 *  RenderableCylinder.cpp
 *  vachellia
 *
 */

#include "RenderableCylinder.h"
#include <math/rayCylinder.h>
#include "IntersectResult.h"

namespace alo {

RenderableCylinder::RenderableCylinder() :
m_p0(0.f, -2.f, 0.f),
m_r(4.5f),
m_p1(0.f, 2.f, 0.f),
m_l(4.f),
m_dir(0.f, 1.f, 0.f)
{}

void RenderableCylinder::setPoints(const Vector3F &p0, const Vector3F &p1)
{
	m_p0 = p0; m_p1 = p1;
	m_dir = p1 - p0; 
	m_l = m_dir.length();
	m_dir /= m_l;
}

void RenderableCylinder::setRadius(float x)
{ m_r = x; }

bool RenderableCylinder::intersectRay(const Ray& aray, IntersectResult& result)
{
	float tt = result.rayDistance();
	Vector3F tn;
	if(!rayCylinderIntersect(tt, tn, aray, m_p0, m_p1, m_r, m_l, m_dir) )
		return false;

	return result.updateRayDistance(tt, tn);
}

}
