/*
 *  BVH.h
 *  aloe
 *
 *  nodes and primitives
 *
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

public:
	BVH();
	~BVH();

	void clear();
	void splitNode(int i);
	void addPrimitive(const BVHPrimitive &x);
	void setRootLeaf();

	const int &numNodes() const;
	const int &numPrimitives() const;
	int numLeafNodes() const;
	const BoundingBox &aabb() const;
	BVHNode *rootNode();
	BVHNode *lastNode();
	BVHNode *nodes();
	BVHPrimitive *primitives();
	
	const BVHPrimitive *c_primitives() const;

	friend std::ostream& operator<<(std::ostream &output, const BVH & p);

	BVHNodeIterator firstLeaf() const;
	BVHNodeIterator nextLeaf(BVHNodeIterator x) const;

};

}

#endif
