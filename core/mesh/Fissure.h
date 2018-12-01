/*
 *  Fissure.h
 *  aloe
 *
 *  build bvh from faces
 *  each leaf forms a mesh
 *  split one mesh to many
 *  limit mesh face count and tighter aabb
 * 
 */

#ifndef ALO_MESH_FISSURE_H
#define ALO_MESH_FISSURE_H

namespace alo {

class AdaptableMesh;
struct BVHNodeIterator;
class BVH;
	
class Fissure {

	BVH *m_bvh;
	
public:
	Fissure();
	virtual ~Fissure();

	void granulate(const AdaptableMesh *inMesh);

	BVHNodeIterator firstPart() const;
	BVHNodeIterator nextPart(BVHNodeIterator x) const;
	void reformPart(AdaptableMesh *outMesh, BVHNodeIterator x, const AdaptableMesh *inMesh) const;

};

}

#endif
