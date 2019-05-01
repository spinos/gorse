#include "BVHNode.h"
#ifdef LINUX
#include <string.h>
#endif

namespace alo {

BVHNode::BVHNode() : m_parent(-1)
{}

BVHNode::BVHNode(int parent) : m_parent(parent)
{}

void BVHNode::setSibling(int x)
{ m_sibling = x; }

void BVHNode::resetAABB()
{ m_aabb.reset(); }

void BVHNode::expandAABB(const BoundingBox &box)
{ m_aabb.expandBy(box); }

void BVHNode::setAABB(const BoundingBox &box)
{ memcpy(m_aabb.m_data, box.m_data, 24); }

void BVHNode::setInner(int leftChild)
{ m_aabb.m_padding0 = leftChild; }

void BVHNode::setLeaf(int begin, int end)
{ 
	m_aabb.m_padding0 = begin | LeafNodeNum::LeafNodeBegin;
	m_aabb.m_padding1 = end;
}

bool BVHNode::isLeaf() const
{ return m_aabb.m_padding0 > LeafNodeNum::LeafNodeMask; }

int BVHNode::leafBegin() const
{ return m_aabb.m_padding0 & LeafNodeNum::LeafNodeMask; }

const int &BVHNode::leafEnd() const
{ return m_aabb.m_padding1; }

const int &BVHNode::leftChild() const
{ return m_aabb.m_padding0; } 

const BoundingBox &BVHNode::aabb() const
{ return m_aabb; }

const int &BVHNode::parent() const
{ return m_parent; }

const int &BVHNode::sibling() const
{ return m_sibling; }

}
