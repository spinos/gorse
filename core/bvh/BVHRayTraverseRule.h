/*
 *  BVHRayTraverseRule.h
 *  gorse
 *
 *  stackless traversal of bvh
 *  three directions
 *  down from parent to near child
 *  horizontal from near child to far sibling
 *  up from child to parent
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
/// (origin, direction, t_min, t_max)
	float _rayData[8];
	int _state;
	int _current;
	int _child;
	int _primBegin;
	int _primEnd;

	void printDown() const {
		std::cout << "\n "<<_current
			<< "\n down";
	}

	void printHorizontal() const {			
		std::cout << "\n -->";
	}

	void printUp() const {
		std::cout << "\n up"
				  << "\n "<<_current;
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

    void begin(RayTraverseResult &result, const float *rayData) const;
	void traverse(RayTraverseResult &result) const;

protected:


private:

	int findParent(int x) const;
	int findSibling(int x) const;
	int findNearChild(const float *rayData, const BVHNode &parentNode) const;

};

} /// end of namepsace bvh

} /// end of namespace alo

#endif
