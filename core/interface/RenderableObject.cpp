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

bool RenderableObject::intersectRay(const Ray& aray, IntersectResult& result)
{ 
	if(!raySphereIntersect(result.rayDistance(), aray, Vector3F::Zero, 4.f) ) {
		return false;
	}
	
	Vector3F& nml = result.hitNormal();
	nml = aray.travel(result.rayDistance() );
	nml.normalize();
	return true;
}

}
