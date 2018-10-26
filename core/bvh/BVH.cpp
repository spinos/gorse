#include "BVH.h"

namespace alo {

BVH::BVH()
{}

BVH::~BVH()
{}

void BVH::clear()
{
	m_nodes.resetBuffer(1);
	m_nodes[0].resetAABB();
	m_primitives.resetBuffer(0);
}

void BVH::addNode(const BVHNode &x)
{ 
	m_nodes << x; 
}

void BVH::addPrimitive(const BVHPrimitive &x)
{ 
	m_primitives << x; 
	m_nodes[0].expandAABB(x.aabb());
}

const int &BVH::numPrimitives() const
{ return m_primitives.count(); }

BVHPrimitive *BVH::primitives()
{ return m_primitives.data(); }

}
