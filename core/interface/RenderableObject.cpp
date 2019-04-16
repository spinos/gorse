#include "RenderableObject.h"
#include <math/Ray.h>
#include <math/raySphere.h>
#include "IntersectResult.h"

namespace alo {

RenderableObject::RenderableObject() : m_objectId(0),
m_state(stNormal)
{}

RenderableObject::~RenderableObject()
{}

void RenderableObject::setObjectId(int x)
{ m_objectId = x; }

const int& RenderableObject::objectId() const
{ return m_objectId; }

void RenderableObject::setToDestroy()
{ m_state = stWaitDestroy; }

void RenderableObject::setVisibility(bool x)
{ m_state = m_state ^ stHiddenMask; }

bool RenderableObject::isToDestroy() const
{ return m_state == stWaitDestroy; }

bool RenderableObject::isHidden() const
{ return m_state > stHiddenMask; }

bool RenderableObject::isOverlay() const
{ return m_state == stOverlay; }

bool RenderableObject::intersectRay(const Ray& aray, IntersectResult& result)
{ 
    float tt = result.rayDistance();
	if(!raySphereIntersect(tt, aray, Vector3F::Zero, 5.f) ) {
		return false;
	}
	
	Vector3F tn = aray.travel(tt );
	tn.normalize();
	return result.updateRayDistance(tt, tn);
}

}
