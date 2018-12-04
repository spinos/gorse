/*
 *  ViewFrustumEvent.cpp
 *  aloe
 *
 */

#include "ViewFrustumEvent.h"
#include <bvh/BVHNode.h>
#include <math/Hexahedron.h>
#include <math/AFrustum.h>
#include <gjk/GjkIntersection.h>

namespace alo {

ViewFrustumEvent::ViewFrustumEvent(const BVHNode &node, const Hexahedron &hexa, const AFrustum &fru)
{
	Vector3F separateAxis;
	bool inst = gjk::Intersect1<AFrustum, Hexahedron >::Evaluate(fru, hexa, &separateAxis);
	if(inst) {
		m_result = rIntersected;
		if(fru.enclose <Hexahedron> (hexa))
			m_result = rInside;
	} else {
		m_result = rOutside;
		if(separateAxis.length() > hexa.size() * 2.f )
			m_result = rFarAway;
	}

	if(node.isLeaf()) {
		m_leafInd = node.leafBegin();
		m_leftChildInd = -1;
	}
	else
		m_leftChildInd = node.leftChild();
}

ViewFrustumEvent::ViewFrustumEvent(const BVHNode &node, CompareResult r) :
m_result(r)
{
	if(node.isLeaf()) {
		m_leafInd = node.leafBegin();
		m_leftChildInd = -1;
	}
	else
		m_leftChildInd = node.leftChild();
}

ViewFrustumEvent::CompareResult ViewFrustumEvent::result() const
{ return m_result; }

const int &ViewFrustumEvent::leftChildInd() const
{ return m_leftChildInd; }

const int &ViewFrustumEvent::leafInd() const
{ return m_leafInd; }

ViewFrustumEvent *ViewFrustumEvent::Create(const BVHNode &node, const Hexahedron &hexa, 
										const AFrustum &fru, CompareResult r)
{
	if(r == ViewFrustumEvent::rIntersected)
		return new ViewFrustumEvent(node, hexa, fru);
	
	return new ViewFrustumEvent(node, r);
}

}