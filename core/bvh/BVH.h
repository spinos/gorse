/*
 *  BVH.h
 *  aloe
 *
 *  nodes and primitives
 *	2019/5/5
 */

#ifndef ALO_BVH_H
#define ALO_BVH_H

#include <math/SimpleBuffer.h>
#include "BVHNodeIterator.h"
#include "BVHPrimitive.h"

namespace alo {

class BVH {

	SimpleBuffer<BVHNode > m_nodes;
	SimpleBuffer<BVHPrimitive > m_primitives;
    SimpleBuffer<int > m_indirections;

public:
	BVH();
	~BVH();

	void clear();
/// left child of node i
	int splitNode(int i);
	void addPrimitive(const BVHPrimitive &x);
	void setRootLeaf();
/// after build
    void buildIndirection();

	const int &numNodes() const;
	const int &numPrimitives() const;
	int numLeafNodes() const;
	const BoundingBox &aabb() const;
	BVHNode *rootNode();
	BVHNode *lastNode();
	BVHNode *nodes();
	BVHPrimitive *primitives();
    
    BVHNode &node(int i);
    const BVHNode &c_node(int i) const;
    BVHPrimitive &primitive(int i);
    const BVHPrimitive &c_primitive(int i) const;
	
	const BVHNode *c_nodes() const;
	const BVHPrimitive *c_primitives() const;
	const BoundingBox &primitiveBox(int i) const;
	const int &primitiveIndex(int i) const;
	const int &primitiveIndirection(int i) const;

	friend std::ostream& operator<<(std::ostream &output, const BVH & p);

	BVHNodeIterator firstLeaf() const;
	BVHNodeIterator nextLeaf(BVHNodeIterator x) const;

};

}

#endif
