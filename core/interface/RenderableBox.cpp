/*
 *  RenderableBox.cpp
 *  vachellia
 *
 */

#include "RenderableBox.h"
#include <math/rayBox.h>
#include <math/Ray.h>
#include "IntersectResult.h"

namespace alo {

RenderableBox::RenderableBox()
{
	m_centerHalfSpan[0] = m_centerHalfSpan[1] = m_centerHalfSpan[2] = 0.f;
	m_centerHalfSpan[3] = 5.f;
}

bool RenderableBox::intersectRay(const Ray& aray, IntersectResult& result)
{
	aray.get(m_rayData);

	if(!rayBoxIntersect(m_rayData, m_centerHalfSpan) ) {
		return false;
	}

/// t0
	result.rayDistance() = m_rayData[6];
	
	const Vector3F pnt = aray.travel(result.rayDistance() );
/// side on box
	getNormalOnBox((float *)&result.hitNormal(), (const float *)&pnt, m_centerHalfSpan);
	
	return true;
}

}
