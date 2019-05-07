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

int BVH::splitNode(int i)
{ 
	BVHNode *parentNode = &m_nodes[i];
	const int lft = numNodes();
	parentNode->setInner(lft);
	m_nodes << BVHNode(i); 
	m_nodes << BVHNode(i); 
	node(lft).setSibling(lft+1);
	node(lft + 1).setSibling(lft);
    return lft;
}

void BVH::addPrimitive(const BVHPrimitive &x)
{ 
	m_primitives << x; 
	m_nodes[0].expandAABB(x.bbox());
}

void BVH::setRootLeaf()
{ m_nodes[0].setLeaf(0, numPrimitives()); }

const BoundingBox &BVH::aabb() const
{ return m_nodes[0].aabb(); }

const int &BVH::numNodes() const
{ return m_nodes.count(); }

const int &BVH::numPrimitives() const
{ return m_primitives.count(); }

int BVH::numLeafNodes() const
{
	int c = 0;
	for(int i=0;i<numNodes();++i) {
		if(m_nodes[i].isLeaf())
			c++;
	}
	return c;
}

BVHNode *BVH::rootNode()
{ return &m_nodes[0]; }

BVHNode *BVH::lastNode()
{ return &m_nodes.last(); }

BVHNode *BVH::nodes()
{ return m_nodes.data(); }

BVHPrimitive *BVH::primitives()
{ return m_primitives.data(); }

const BVHNode *BVH::c_nodes() const
{ return m_nodes.c_data(); }

const BVHPrimitive *BVH::c_primitives() const
{ return m_primitives.c_data(); }

BVHNodeIterator BVH::firstLeaf() const
{
	BVHNodeIterator it;
	for(int i=0;i<numNodes();++i) {
		if(m_nodes[i].isLeaf()) {
			it._node = &m_nodes[i];
			it._ind = i;
			return it;
		}
	}
	it._node = 0;
	return it;
}

BVHNodeIterator BVH::nextLeaf(BVHNodeIterator x) const
{
	BVHNodeIterator it;
	it._ind = x._ind + 1;
	if(it._ind >= numNodes()) {
		it._node = 0;
		return it;
	}
	for(int i=it._ind;i<numNodes();++i) {
		if(m_nodes[i].isLeaf()) {
			it._node = &m_nodes[i];
			it._ind = i;
			return it;
		}
	}
	
	it._node = 0;
	return it;
}

const BoundingBox &BVH::primitiveBox(int i) const
{ return m_primitives[i].bbox(); }

BVHNode &BVH::node(int i)
{ return m_nodes[i]; }

BVHPrimitive &BVH::primitive(int i)
{ return m_primitives[i]; }

const BVHNode &BVH::c_node(int i) const
{ return m_nodes[i]; }

const BVHPrimitive &BVH::c_primitive(int i) const
{ return m_primitives[i]; }

void BVH::buildIndirection()
{
    m_indirections.resetBuffer(numPrimitives());
    
    const int &n = numNodes();
    for(int i=0;i<n;++i) {
        const BVHNode &ni = c_node(i);
        if(ni.isLeaf()) {

            int b = ni.leafBegin();
            int e = ni.leafEnd();
            
            for(int j=b;j<e;++j) {
                m_indirections[j] = c_primitive(j).index();
            }
            
        }
    }
}

std::ostream& operator<<(std::ostream &output, const BVH & p) 
{
    output << "\n bvh aabb " << p.aabb() 
    	<< "\n n node "<<p.numNodes()
    	<< " n leaf "<<p.numLeafNodes()
    	<< " n primitive "<< p.numPrimitives();
    return output;
}

}
