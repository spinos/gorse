#include "BVHPrimitive.h"

namespace alo {

BVHPrimitive::BVHPrimitive()
{}

void BVHPrimitive::setAABB(const BoundingBox &box)
{ memcpy(m_aabb.m_data, box.m_data, 24); }

void BVHPrimitive::setIndex(int x)
{ m_aabb.m_padding0 = x; }

const BoundingBox &BVHPrimitive::aabb() const
{ return m_aabb; }

const int &BVHPrimitive::index() const
{ return m_aabb.m_padding0; }

}
