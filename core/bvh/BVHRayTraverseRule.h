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
 *  bitmap record if down to left child on each level
 *  30 levels max
 *
 *  2019/5/7
 */

#ifndef ALO_BVH_RAY_TRAVERSE_RULE_H
#define ALO_BVH_RAY_TRAVERSE_RULE_H

#include <iostream>

namespace alo {

class BVH;
class BVHNode;

namespace bvh {

struct RayTraverseResult {
	short _state;
	short _level;
	int _bitmap;
	int _current;
	int _child;
	int _primBegin;
	int _primEnd;
	float _rayD[8];
	float _t0;
	float _t1;

	void goDown(short state, int child) 
	{
		_state = state;
		_current = child;
		_level++;
	}

	void goHorizontal(short state, int sibling)
	{
		_state = state;
		_current = sibling;
	}

	void goUp(short state, int parent) 
	{
		_state = state;
		_child = _current;
		_current = parent;
		_level--;
	}

	bool isDirectionLeft() const
	{
		const int mask = 1<<_level;
		return (_bitmap & mask) > 0;
	}

/// record left-or-right at current level
	void recordDirection(bool isLeft) 
	{
		const int mask = 1<<_level;
		if(isLeft) {
/// set to one
			_bitmap = _bitmap | mask;
		} else {
			if(_bitmap & mask) {
/// set to zero
				_bitmap = _bitmap ^ mask;
			}
		}
	}

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
