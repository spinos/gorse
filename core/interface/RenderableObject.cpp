#include "RenderableObject.h"
#include <math/Ray.h>
#include <math/raySphere.h>
#include <math/rayBox.h>
#include "IntersectResult.h"

namespace alo {

RenderableObject::RenderableObject() : m_objectId(0),
m_state(stNormal)
{
	m_aabb[0] = m_aabb[1] = m_aabb[2] = -10.f;
	m_aabb[3] = m_aabb[4] = m_aabb[5] = 10.f;
}

RenderableObject::~RenderableObject()
{}

void RenderableObject::setObjectId(int x)
{ m_objectId = x; }

const int& RenderableObject::objectId() const
{ return m_objectId; }

void RenderableObject::setToDestroy()
{ m_state = stWaitDestroy; }

void RenderableObject::setVisible(bool x)
{ 
    if(x == isHidden())
        m_state = m_state ^ stHiddenMask; 
}

void RenderableObject::setOverlay(bool x)
{ 
    if(x != isOverlay())
        m_state = m_state ^ stOverlay; 
}

bool RenderableObject::isToDestroy() const
{ return m_state == stWaitDestroy; }

bool RenderableObject::isHidden() const
{ return m_state > stHiddenMask; }

bool RenderableObject::isOverlay() const
{ return (m_state & stOverlayMask) > stOverlay; }

bool RenderableObject::intersectRay(const Ray& aray, IntersectResult& result)
{ 
	float rayData[8];
	result.copyRayData(rayData);

	if(!rayAabbIntersect(rayData, m_aabb)) return false;
	const float &tt = rayData[6];

	const Vector3F pnt = aray.travel(tt);
    Vector3F tn;
/// side on box
	getNormalOnAabb((float *)&tn, (const float *)&pnt, m_aabb, tt * 1e-5f);

	return result.updateRayDistance(tt, tn);
}

float *RenderableObject::aabb()
{ return m_aabb; }

void RenderableObject::extractAabb(float *y) const
{ memcpy(y, m_aabb, 24); }

}
