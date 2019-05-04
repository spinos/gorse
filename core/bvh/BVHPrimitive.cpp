#include "BVHPrimitive.h"

namespace alo {

BVHPrimitive::BVHPrimitive()
{}

void BVHPrimitive::setBBox(const BoundingBox &box)
{ memcpy(m_aabb.m_data, box.m_data, 24); }

void BVHPrimitive::setIndex(int x)
{ m_aabb.m_padding0 = x; }

void BVHPrimitive::setKey(int x)
{ m_aabb.m_padding1 = x; }

const BoundingBox &BVHPrimitive::bbox() const
{ return m_aabb; }

const int &BVHPrimitive::index() const
{ return m_aabb.m_padding0; }

const int &BVHPrimitive::key() const
{ return m_aabb.m_padding1; }

}
