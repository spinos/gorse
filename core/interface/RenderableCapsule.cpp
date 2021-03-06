/*
 *  RenderableCapsule.cpp
 *  vachellia
 *
 */

#include "RenderableCapsule.h"
#include <math/rayCapsule.h>
#include <math/Ray.h>
#include "IntersectResult.h"

namespace alo {

RenderableCapsule::RenderableCapsule() :
m_p0(Vector3F::Zero),
m_r(1.f),
m_p1(10.f, 0.f, 0.f),
m_l(10.f),
m_dir(1.f, 0.f, 0.f)
{}

bool RenderableCapsule::intersectRay(IntersectResult& result) const
{
	float rayData[8];
    result.copyRayData(rayData);

	float tt = result.rayDistance();
	Vector3F tn;
	if(!rayCapsuleIntersect(tt, tn,
		rayData, 
		m_p0, m_p1, m_r, m_l, m_dir) ) {
		return false;
	}

	return result.updateRayDistance(tt, tn);
}

}
