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
class BVHPrimitive;

class ViewFrustumCull {

	SimpleBuffer<Hexahedron> m_nodeHexa;
	SimpleBuffer<Hexahedron> m_primHexa;
	SimpleBuffer<BVHNode> m_nodes;
/// node-to-primitive map
	SimpleBuffer<int> m_primInd;
/// internal bvh multiple primitives for each leaf
	BVH *m_bvh;

public:
	ViewFrustumCull();
	virtual ~ViewFrustumCull();

/// from reducer one primitive for each leaf in order
	void create(const BVH *bvh);
	float getMeanSize() const;
	void compare(VisibilityState *visibilities, const AFrustum &fru) const;

/// i-th primitve
	const Hexahedron &primitiveHexahedron(int i) const;
	const int &numPrimitives() const;

	void clearBvh();
	void addBvhPrimitive(const BVHPrimitive &x);
	void buildBvh();

	const BoundingBox &bvhAabb() const;

};
	
}

#endif
