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

void BVH::splitNode(int i)
{ 
	BVHNode *parentNode = &m_nodes[i];
	int lft = numNodes();
	parentNode->setInner(lft);
	m_nodes << BVHNode(); 
	m_nodes << BVHNode(); 
}

void BVH::addPrimitive(const BVHPrimitive &x)
{ 
	m_primitives << x; 
	m_nodes[0].expandAABB(x.aabb());
}

void BVH::setRootLeaf()
{ m_nodes[0].setLeaf(0, numPrimitives()); }

const BoundingBox &BVH::aabb() const
{ return m_nodes[0].aabb(); }

const int &BVH::numNodes() const
{ return m_nodes.count(); }

const int &BVH::numPrimitives() const
{ return m_primitives.count(); }

BVHNode *BVH::rootNode()
{ return &m_nodes[0]; }

BVHNode *BVH::lastNode()
{ return &m_nodes.last(); }

BVHNode *BVH::nodes()
{ return m_nodes.data(); }

BVHPrimitive *BVH::primitives()
{ return m_primitives.data(); }

std::ostream& operator<<(std::ostream &output, const BVH & p) 
{
    output << " aabb " << p.aabb() 
    	<< " n primitive "<< p.numPrimitives()
    	<< " n node "<<p.numNodes();
    return output;
}

}
