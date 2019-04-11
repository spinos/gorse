#include "RenderableObject.h"
#include <math/Ray.h>
#include "IntersectResult.h"

namespace alo {

RenderableObject::RenderableObject() : m_drawId(0)
{}

RenderableObject::~RenderableObject()
{}

void RenderableObject::setDrawId(int x)
{ m_drawId = x; }

const int& RenderableObject::drawId() const
{ return m_drawId; }

bool RenderableObject::intersectRay(const Ray& aray, IntersectResult& result)
{ 
	return false;
}

}
