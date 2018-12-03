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
    
namespace sdb {
template<typename T1, typename T2, int D1, int D2, int D3>  
class L3Tree;
}

struct Int3;
class AdaptableMesh;
struct BVHNodeIterator;
class BVH;
	
class Fissure {

	BVH *m_bvh;
	
public:
	Fissure();
	virtual ~Fissure();

	int granulate(const AdaptableMesh *inMesh);

	BVHNodeIterator firstPart() const;
	BVHNodeIterator nextPart(BVHNodeIterator x) const;
	void reformPart(AdaptableMesh *outMesh, BVHNodeIterator x, const AdaptableMesh *inMesh) const;

private:
/// from primitive[begin] to (excluding) primitive[end]
    void mapVertices(sdb::L3Tree<int, int, 2048, 512, 1024> &vertexMap, 
                        int pbegin, int pend, const Int3 *faces) const;
    void setMeshVertices(AdaptableMesh *outMesh,
                        sdb::L3Tree<int, int, 2048, 512, 1024> &vertexMap, 
                        int pbegin, int pend, const AdaptableMesh *inMesh) const;
    void setMeshFaceIndices(Int3 *outFaces, 
                        sdb::L3Tree<int, int, 2048, 512, 1024> &vertexMap, 
                        int pbegin, int pend, const Int3 *faces) const;
};

}

#endif
