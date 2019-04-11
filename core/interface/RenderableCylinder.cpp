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
m_r(1.5f),
m_p1(0.f, 2.f, 0.f),
m_dir(0.f, 1.f, 0.f)
{}

bool RenderableCylinder::intersectRay(const Ray& aray, IntersectResult& result)
{
	float tt = result.rayDistance();
	if(!rayCylinderIntersect(tt, result.hitNormal(),
		aray, 
		m_p0, m_p1, m_r, m_dir) ) {
		return false;
	}

	return result.updateRayDistance(tt);
}

}
