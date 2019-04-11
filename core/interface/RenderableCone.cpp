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
m_dir(1.f, 0.f, 0.f)
{}

bool RenderableCone::intersectRay(const Ray& aray, IntersectResult& result)
{
	float tt = result.rayDistance();
	if(!rayConeIntersect(tt, result.hitNormal(),
		aray, 
		m_p0, m_p1, m_r, m_l, m_dir) ) {
		return false;
	}

	return result.updateRayDistance(tt);
}

}
