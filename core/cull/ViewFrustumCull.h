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

class ViewFrustumCull {

	SimpleBuffer<Hexahedron> m_hexa;
	SimpleBuffer<BVHNode> m_nodes;

public:
	ViewFrustumCull();
	virtual ~ViewFrustumCull();

	void create(const BVH *bvh);

	void compare(bool *visibilities, const AFrustum &fru) const;

};
	
}

#endif
