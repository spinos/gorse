/*
 *  BVH.h
 *
 */

#ifndef ALO_BVH_H
#define ALO_BVH_H

#include <math/SimpleBuffer.h>
#include "BVHNode.h"
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
	const BoundingBox &aabb() const;
	BVHNode *rootNode();
	BVHNode *lastNode();
	BVHNode *nodes();
	BVHPrimitive *primitives();

	friend std::ostream& operator<<(std::ostream &output, const BVH & p);

};

}

#endif
