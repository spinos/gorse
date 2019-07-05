/*
 *  Fusion.h
 *  aloe
 *
 *  combine many meshes into one
 *  merge overlapping vertices
 * 
 *  2019/7/6
 */
 
#ifndef ALO_MESH_FUSION_H
#define ALO_MESH_FUSION_H

#include <vector>
#include <math/SimpleBuffer.h>

namespace alo {

class AdaptableMesh;
class BoundingBox;

class Fusion {
    
    struct MergeIndex {
        int _key;
        int _ind;
        int _merged;
        int _padding;
    };
    
    SimpleBuffer<MergeIndex> m_mergeInds;

public:

    Fusion();
	virtual ~Fusion();
    
    bool combine(AdaptableMesh *outMesh, const std::vector<AdaptableMesh *> &inMeshes);
    
protected:

private:
/// find overlapping vertices by space filling curve
    void mapMergeIndices(const std::vector<AdaptableMesh *> &inMeshes,
                        const BoundingBox &bbox);
/// assuming all in meshes has the same uv set
    void combineMeshUv(AdaptableMesh *outMesh, const std::vector<AdaptableMesh *> &inMeshes);
    
};

}

#endif
