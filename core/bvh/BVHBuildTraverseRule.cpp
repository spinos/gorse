/*
 *  BVHBuildTraverseRule.cpp
 *  gorse
 *
 *
 *  2019/5/7
 */

#include "BVHBuildTraverseRule.h"
#include "BVH.h"
#include "BVHNode.h"
#include <math/rayBox.h>
#include <math/pointBox.h>

//#define BUILD_BVH_DGB

namespace alo {

namespace bvh {

BuildTraverseRule::BuildTraverseRule() : m_bvh(nullptr)
{}

void BuildTraverseRule::attach(BVH *x)
{ m_bvh = x; }

void BuildTraverseRule::detach()
{}

bool BuildTraverseRule::isEmpty() const
{ return m_bvh == nullptr; }

void BuildTraverseRule::begin(BuildTraverseResult &result) const
{
	result._state = tFromParent;
	result._current = 0;
	result._primBegin = -1;
    result._level = 0;
}

void BuildTraverseRule::traverse(BuildTraverseResult &result) const
{
    
	for(;;) {
/// until reaching a leaf node, or finished	

		int &state = result._state;

		if(result._current < 0) {
/// up from or sibling of root, finished
			state = tNone;
			return;
		}
        
        const BVHNode &n = m_bvh->c_node(result._current);

		if(result._primBegin > -1) {
              
/// end process leaf
#ifdef BUILD_BVH_DGB
			result.printLeafOut();
#endif

			result._primBegin = -1;
            
            if(result._toSplit) {
#ifdef BUILD_BVH_DGB
                result.printSplit();
#endif

                state = tFromParent;
                result._current = n.leftChild();
                result._level++;
                
            } else {
                
/// no split, going up
#ifdef BUILD_BVH_DGB
				result.printUp();
#endif                
                state = tFromChild;
				result._child = result._current;
				result._current = findParent(result._current);
                result._level--;
            }

			continue;
		}

		if(state == tFromChild) {

			if(result._child != n.leftChild() ) {
/// visited far child, going up
#ifdef BUILD_BVH_DGB
				result.printUp();
#endif

				state = tFromChild;
				result._child = result._current;
				result._current = findParent(result._current);
                result._level--;
			} else {
/// visited near child, going to far child
#ifdef BUILD_BVH_DGB
				result.printHorizontal();
#endif

				state = tFromSibling;
				result._current = findSibling(result._child);
                result._level++;
			}

			continue;
		}

		if(n.isLeaf()) {
/// begin process leaf
#ifdef BUILD_BVH_DGB
			result.printLeafIn();
#endif

			result._primBegin = n.leafBegin();
			return;
		}

/// inner node, going down to near child
#ifdef BUILD_BVH_DGB
		result.printDown();
#endif

		state = tFromParent;
		result._current = n.leftChild();
        result._level++;
		
	}
	
}

bool BuildTraverseRule::end(const BuildTraverseResult &result) const
{ return result._state == tNone; }

int BuildTraverseRule::findParent(int x) const
{ 
	if(x < 1) return -1;
	return m_bvh->c_node(x).parent();
}

int BuildTraverseRule::findSibling(int x) const
{
	if(x < 1) return -1;
	return m_bvh->c_node(x).sibling();
}

} /// end of namespace bvh

} /// end of namespace alo
