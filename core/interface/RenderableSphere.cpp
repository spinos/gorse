/*
 *  RenderableSphere.cpp
 *  vachellia
 *
 */

#include "RenderableSphere.h"
#include <math/raySphere.h>
#include "IntersectResult.h"

namespace alo {

RenderableSphere::RenderableSphere() : m_center(0.f, 0.f, 0.f),
m_radius(5.f)
{}

void RenderableSphere::setRadius(float x)
{ m_radius = x; }

void RenderableSphere::setPoint(const Vector3F &p)
{ m_center = p; }

bool RenderableSphere::intersectRay(const Ray& aray, IntersectResult& result) const
{
    float tt = result.rayDistance();
	if(!raySphereIntersect(tt, aray, m_center, m_radius) ) {
		return false;
	}
	
	Vector3F tn = aray.travel(tt ) - m_center;
	tn.normalize();
	return result.updateRayDistance(tt, tn);
}

}
