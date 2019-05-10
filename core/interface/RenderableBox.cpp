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

bool RenderableBox::intersectRay(const Ray& aray, IntersectResult& result) const
{
    float rayData[8];
	aray.get(rayData);
    rayData[7] = result.rayDistance();
    
	if(!rayBoxIntersect(rayData, m_centerHalfSpan) ) return false;
    
    const float &tt = rayData[6];

	const Vector3F pnt = aray.travel(tt);
    Vector3F tn;
/// side on box
	getNormalOnBox((float *)&tn, (const float *)&pnt, m_centerHalfSpan, tt * 1e-5f);
	
	return result.updateRayDistance(tt, tn);
}

}
