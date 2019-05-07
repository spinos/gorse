/*
 *  BVHBuildTraverseRule.h
 *  gorse
 *
 *  stackless build of bvh
 *  three directions
 *  down from parent to near child
 *  horizontal from near child to far sibling
 *  up from child to parent
 *  exit when hit a leaf or back to root
 *
 *  2019/5/7
 */

#ifndef ALO_BVH_BUILDE_TRAVERSE_RULE_H
#define ALO_BVH_BUILDE_TRAVERSE_RULE_H

#include <iostream>

namespace alo {

class BVH;
class BVHNode;

namespace bvh {

struct BuildTraverseResult {
	int _state;
	int _current;
	int _child;
	int _primBegin;
    int _level;
    bool _toSplit;
    
    void printCurrent() const {
        std::cout << " node"<<_current<<" l"<<_level;
    }
    
    void printSplit() const {
        std::cout << "\n split "<<_current << " down ";
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

class BuildTraverseRule {

	BVH *m_bvh;
	
	enum TraverseState {
		tNone = 0,
		tFromParent,
		tFromSibling,
		tFromChild
	};

public:

	BuildTraverseRule();

	void attach(BVH *x);
    void detach();
    bool isEmpty() const;

    void begin(BuildTraverseResult &result) const;
	void traverse(BuildTraverseResult &result) const;
	bool end(const BuildTraverseResult &result) const;

protected:


private:

	int findParent(int x) const;
	int findSibling(int x) const;
	
};

} /// end of namepsace bvh

} /// end of namespace alo

#endif
