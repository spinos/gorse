/*
 *  ViewFrustumCull.h
 *  aloe
 *
 *  compare bounding volume (hexahedron) to view frustum
 *
 */

#ifndef ALO_VIEW_FRUSTUM_CULL_H
#define ALO_VIEW_FRUSTUM_CULL_H

#include <math/SimpleBuffer.h>
#include <math/Hexahedron.h>
#include <bvh/BVHNode.h>

namespace alo {

class BVH;
class AFrustum;
class VisibilityState;

class ViewFrustumCull {

	SimpleBuffer<Hexahedron> m_hexa;
	SimpleBuffer<BVHNode> m_nodes;
	SimpleBuffer<int> m_leafInds;

public:
	ViewFrustumCull();
	virtual ~ViewFrustumCull();

	void create(const BVH *bvh);
	float getMeanSize() const;
	void compare(VisibilityState *visibilities, const AFrustum &fru) const;

	const Hexahedron *c_hexahedrons() const;
/// i-th leaf
	const Hexahedron &leafHexahedron(int i) const;

};
	
}

#endif
