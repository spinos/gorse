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
	void addNode(const BVHNode &x);
	void addPrimitive(const BVHPrimitive &x);

	const int &numPrimitives() const;

	BVHPrimitive *primitives();

};

}

#endif
