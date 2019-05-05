/*
 *  BVHRayTraverseRule.h
 *  gorse
 *
 *  stackless traversal of bvh
 *  three directions
 *  down from parent to near child
 *  horizontal from near child to far sibling
 *  up from child to parent
 *  exit when hit a leaf or back to root
 *
 *  2019/5/2
 */

#ifndef ALO_BVH_RAY_TRAVERSE_RULE_H
#define ALO_BVH_RAY_TRAVERSE_RULE_H

#include <iostream>

namespace alo {

class BVH;
class BVHNode;

namespace bvh {

struct RayTraverseResult {
	int _state;
	int _current;
	int _child;
	int _primBegin;
	int _primEnd;
	float _rayD[8];
	float _t0;
	float _t1;

	void printDown() const {
		std::cout << "\n "<<_current<< " down ";
	}

	void printHorizontal() const {			
		std::cout << "\n -->";
	}

	void printUp() const {
		std::cout << "\n up "<<_current;
	}

	void printLeafIn() const {
		std::cout << "\n [";
	}

	void printLeafOut() const {
		std::cout << " ]";
	}

};

class RayTraverseRule {

	const BVH *m_bvh;
	const BVHNode *m_nodes;

	enum TraverseState {
		tNone = 0,
		tFromParent,
		tFromSibling,
		tFromChild
	};

public:

	RayTraverseRule();

	void attach(const BVH *x);
    void detach();
    bool isEmpty() const;

    void begin(RayTraverseResult &result) const;
	void traverse(RayTraverseResult &result, const float *rayData) const;
	bool end(const RayTraverseResult &result) const;

protected:


private:

	int findParent(int x) const;
	int findSibling(int x) const;
	int findNearChild(const float *rayData, const BVHNode &parentNode) const;

};

} /// end of namepsace bvh

} /// end of namespace alo

#endif
