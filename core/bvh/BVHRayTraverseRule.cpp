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
	result._level = 0;
	result._bitmap = 0;
	result._current = 0;
	result._primBegin = -1;
}

void RayTraverseRule::traverse(RayTraverseResult &result, const float *rayData) const
{
	memcpy(result._rayD, rayData, 24);

	for(;;) {
/// until reaching a leaf node, or finished	

		short &state = result._state;
		int &current = result._current;

		if(current < 0) {
/// up from or sibling of root, finished
			state = tNone;
			return;
		}

		if(result._primBegin > -1) {
/// end process leaf
			result._primBegin = -1;

			if(state == tFromParent) {
/// visited near child, going to far child
				result.goHorizontal(tFromSibling, findSibling(current));
			} else {
/// visited far child, going up
				result.goUp(tFromChild, findParent(current));
			} 

			continue;
		}

		const BVHNode &n = m_nodes[current];

		if(state == tFromChild) {

			int visitedNear = n.leftChild();
			if(!result.isDirectionLeft()) visitedNear++;

			if(result._child != visitedNear ) {
/// visited far child, going up
				result.goUp(tFromChild, findParent(current));

			} else {
/// visited near child, going to far child
				result.goDown(tFromSibling, findSibling(result._child));
			}

			continue;
		}

		memcpy(&result._rayD[6], &rayData[6], 8);
	
		if(rayAabbIntersect(result._rayD, (const float *)&n.aabb())) {

			if(n.isLeaf()) {
				result._t0 = result._rayD[6];
				result._t1 = result._rayD[7];
/// begin process leaf
				result._primBegin = n.leafBegin();
				result._primEnd = n.leafEnd();
				return;
			}

			const int hitChild = findNearChild(rayData, n);

			if(hitChild < 0) {

				result.goUp(tFromChild, findParent(current));

			} else {
/// inner node, going down to near child
				result.recordDirection(hitChild == n.leftChild() );
				result.goDown(tFromParent, hitChild);

			}

			continue;
		}

/// missed
		if(state == tFromParent) {
/// missed near child, going to far child
			result.goHorizontal(tFromSibling, findSibling(current));

		} else {
/// missed far child, going up
			result.goUp(tFromChild, findParent(current));
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

	bool hitLeft = rayAabbIntersect(leftRay, leftBox);

	const BVHNode &right = m_nodes[near + 1];
	const float *rightBox = (const float *)&right.aabb();
	
	float rightRay[8];
	memcpy(rightRay, rayData, 32);

	bool hitRight = rayAabbIntersect(rightRay, rightBox);

	if(!hitLeft && !hitRight)
		return -1;

	if(hitLeft && !hitRight)
		return near;

	if(!hitLeft && hitRight)
		return near + 1;
	
	if(leftRay[6] < rightRay[6])
		return near;

	return near+1;
}

} /// end of namespace bvh

} /// end of namespace alo
