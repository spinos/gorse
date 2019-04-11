/*
 *  RenderableSphere.cpp
 *  vachellia
 *
 */

#include "RenderableSphere.h"
#include <math/raySphere.h>
#include "IntersectResult.h"

namespace alo {

RenderableSphere::RenderableSphere() : m_center(Vector3F::Zero),
m_radius(10.f)
{}

bool RenderableSphere::intersectRay(const Ray& aray, IntersectResult& result)
{
	if(!raySphereIntersect(result.rayDistance(), aray, m_center, m_radius) ) {
		return false;
	}
	
	Vector3F& nml = result.hitNormal();
	nml = aray.travel(result.rayDistance() ) - m_center;
	nml.normalize();
	return true;
}

}
