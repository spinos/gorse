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

bool RenderableSphere::intersectRay(IntersectResult& result) const
{
	float rayData[8];
    result.copyRayData(rayData);

	if(!raySphereIntersect(rayData, m_center, m_radius) ) {
		return false;
	}

	float &tt = rayData[6];
	
	Vector3F tn(rayData[0] + rayData[3] * tt - m_center.x, 
				rayData[1] + rayData[4] * tt - m_center.y,
				rayData[2] + rayData[5] * tt - m_center.z);
	tn.normalize();
	return result.updateRayDistance(tt, tn);
}

}
