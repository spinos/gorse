#include "RenderableObject.h"
#include <math/Ray.h>
#include <math/raySphere.h>
#include <math/rayBox.h>
#include "IntersectResult.h"

namespace alo {

RenderableObject::RenderableObject() : m_objectId(0),
m_state(stNormal)
{
	resetAabb();
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

float *RenderableObject::aabb()
{ return m_aabb; }

const float *RenderableObject::c_aabb() const
{ return m_aabb; }

void RenderableObject::extractAabb(float *y) const
{ memcpy(y, m_aabb, 24); }

void RenderableObject::resetAabb()
{
	m_aabb[0] = m_aabb[1] = m_aabb[2] = -5.f;
	m_aabb[3] = m_aabb[4] = m_aabb[5] = 5.f;
}

void RenderableObject::setAabbNull()
{
    m_aabb[0] = m_aabb[1] = m_aabb[2] = 1e10f;
	m_aabb[3] = m_aabb[4] = m_aabb[5] = -1e10f;
}

void RenderableObject::setAabb(float x0, float y0, float z0,
                float x1, float y1, float z1)
{
    m_aabb[0] = x0;
    m_aabb[1] = y0;
    m_aabb[2] = z0;
    m_aabb[3] = x1;
    m_aabb[4] = y1;
    m_aabb[5] = z1;
}

void RenderableObject::getAabb(float *y) const
{ memcpy(y, m_aabb, 24); }

}
