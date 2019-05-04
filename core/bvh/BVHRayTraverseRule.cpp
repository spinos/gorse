/*
 *  BVHRayTraverseRule.cpp
 *  gorse
 *
 *
 *  2019/5/2
 */

#include "BVHRayTraverseRule.h"
#include "BVH.h"
#include <math/rayBox.h>
#include <math/pointBox.h>

namespace alo {

namespace bvh {

RayTraverseRule::RayTraverseRule() : m_bvh(nullptr)
{}

void RayTraverseRule::attach(const BVH *x)
{
	m_bvh = x;
	m_nodes = x->c_nodes();
}

void RayTraverseRule::detach()
{}

bool RayTraverseRule::isEmpty() const
{ return m_bvh == nullptr; }

void RayTraverseRule::begin(RayTraverseResult &result) const
{
	result._state = tFromParent;
	result._current = 0;
	result._primBegin = -1;
}

void RayTraverseRule::traverse(RayTraverseResult &result, const float *rayData) const
{
	for(;;) {
/// until reaching a leaf node, or finished	

		int &state = result._state;

		if(result._current < 0) {
/// up from or sibling of root, finished
			state = tNone;
			return;
		}

		if(result._primBegin > -1) {
/// end process leaf
#ifdef RAY_BVH_DGB
			result.printLeafOut();
#endif

			result._primBegin = -1;

			if(state == tFromParent) {
/// visited near child, going to far child
#ifdef RAY_BVH_DGB
				result.printHorizontal();
#endif

				state = tFromSibling;
				result._current = findSibling(result._current);
			} else {
/// visited far child, going up
#ifdef RAY_BVH_DGB
				result.printUp();
#endif

				state = tFromChild;
				result._child = result._current;
				result._current = findParent(result._current);
			} 

			continue;
		}

		const BVHNode &n = m_nodes[result._current];

		if(state == tFromChild) {

			if(result._child != findNearChild(rayData, n) ) {
/// visited far child, going up
#ifdef RAY_BVH_DGB
				result.printUp();
#endif

				state = tFromChild;
				result._child = result._current;
				result._current = findParent(result._current);
			} else {
/// visited near child, going to far child
#ifdef RAY_BVH_DGB
				result.printHorizontal();
#endif

				state = tFromSibling;
				result._current = findSibling(result._child);
			}

			continue;
		}

		float tmp[8];
		memcpy(tmp, rayData, 32);
	
		if(rayAabbIntersect(tmp, (const float *)&n.aabb())) {

			if(n.isLeaf()) {
				result._t0 = tmp[6];
				result._t1 = tmp[7];
/// begin process leaf
#ifdef RAY_BVH_DGB
				result.printLeafIn();
#endif

				result._primBegin = n.leafBegin();
				result._primEnd = n.leafEnd();
				return;
			}

/// inner node, going down to near child
#ifdef RAY_BVH_DGB
			result.printDown();
#endif

			state = tFromParent;
			
			result._current = findNearChild(rayData, n);
			continue;
		}

		//std::cout << "\n missed " << result._current << n.aabb();


/// missed
		if(state == tFromParent) {
/// missed near child, going to far child
#ifdef RAY_BVH_DGB
			result.printHorizontal();
#endif

			state = tFromSibling;
			result._current = findSibling(result._current);
		} else {
/// missed far child, going up
#ifdef RAY_BVH_DGB
			result.printUp();
#endif

			state = tFromChild;
			result._child = result._current;
			result._current = findParent(result._current);
		}

	}
	
}

bool RayTraverseRule::end(const RayTraverseResult &result) const
{ return result._state == tNone; }

int RayTraverseRule::findParent(int x) const
{ 
	if(x < 1) return -1;
	return m_nodes[x].parent();
}

int RayTraverseRule::findSibling(int x) const
{
	if(x < 1) return -1;
	return m_nodes[x].sibling();
}

int RayTraverseRule::findNearChild(const float *rayData, const BVHNode &parentNode) const
{
	const int near = parentNode.leftChild();
	const BVHNode &left = m_nodes[near];

	const float *leftBox = (const float *)&left.aabb();
	if(!isPointOutsideBox(rayData, leftBox ) ) {
/// ray origin inside left
		return near;
	}

	float leftRay[8];
	memcpy(leftRay, rayData, 32);

	if(!rayAabbIntersect(leftRay, leftBox) ) {
/// missed left, right is near
		return near + 1;
	}

	const BVHNode &right = m_nodes[near + 1];
	
	float rightRay[8];
	memcpy(rightRay, rayData, 32);

	const float *rightBox = (const float *)&right.aabb();
	if(!rayAabbIntersect(rightRay, rightBox) ) {
/// missed right, left is near
		return near;
	}
	
	if(leftRay[6] < rightRay[6])
		return near;

	return near+1;
}

} /// end of namespace bvh

} /// end of namespace alo