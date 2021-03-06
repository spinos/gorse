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

	int m_parent;
	int m_sibling;
	BoundingBox m_aabb;

public:

	BVHNode();
	BVHNode(int parent);

	void setSibling(int x);

	void resetAABB();
	void expandAABB(const BoundingBox &box);
	void setAABB(const BoundingBox &box);
	void setInner(int leftChild);
	void setLeaf(int begin, int end);
	bool isLeaf() const;
	int leafBegin() const;
	const int &leafEnd() const;
	const int &leftChild() const;
	const BoundingBox &aabb() const;
	const int &parent() const;
	const int &sibling() const;

private:

	enum LeafNodeNum {
		LeafNodeMask =  16777215,
		LeafNodeBegin = 16777216
	};

};

}

#endif
