/*
 *  ViewFrustumEvent.h
 *  aloe
 *
 */

#ifndef ALO_VIEW_FRUSTUM_EVENT_H
#define ALO_VIEW_FRUSTUM_EVENT_H

namespace alo {

class BVHNode;
class Hexahedron;
class AFrustum;

class ViewFrustumEvent {

	int m_leftChildInd;
	int m_leafInd;

public:

	enum CompareResult {
		rInside = 0,
		rIntersected,
		rOutside,
		rFarAway
	};

	ViewFrustumEvent(const BVHNode &node, const Hexahedron &hexa, const AFrustum &fru);
	ViewFrustumEvent(const BVHNode &node, CompareResult r);

	CompareResult result() const;
	const int &leftChildInd() const;
	const int &leafInd() const;

	static ViewFrustumEvent *Create(const BVHNode &node, const Hexahedron &hexa, 
										const AFrustum &fru, CompareResult r);

private:
	CompareResult m_result;
};

}

#endif
