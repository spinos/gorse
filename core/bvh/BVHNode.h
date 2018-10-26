/*
 *  BVHNode.h
 *
 *  box
 *  index0
 *  index1
 *  inner node index0 to left child node
 *  leaf node index0 to primitive begin index1 to primitive end
 *  it is a leaf node if index0 > 2^24 - 1
 *
 */

#ifndef ALO_BVH_NODE_H
#define ALO_BVH_NODE_H

#include <math/BoundingBox.h>

namespace alo {

class BVHNode {

	BoundingBox m_aabb;

public:

	BVHNode();

	void resetAABB();
	void expandAABB(const BoundingBox &box);
	void setAABB(const BoundingBox &box);
	void setInner(int leftChild);
	void setLeaf(int begin, int end);
	bool isLeaf() const;
	int leafBegin() const;
	const int &leafEnd() const;
	const int &leftChild() const;

private:

	enum LeafNodeNum {
		LeafNodeMask =  16777215,
		LeafNodeBegin = 16777216
	};

};

}

#endif
