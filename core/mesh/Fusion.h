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
#include <math/Vector3F.h>
#include <math/SimpleBuffer.h>

namespace alo {

class AdaptableMesh;
class BoundingBox;

class Fusion {
    
    struct MergeIndex {
        int _key;
        int _ind;
        int _merged;
        int _padding1;
        Vector3F _pos;
        int _padding2;
    };
    
    SimpleBuffer<MergeIndex> m_mergeInds;
    SimpleBuffer<int> m_mergeMap;

public:

    Fusion();
	virtual ~Fusion();
    
    bool combine(AdaptableMesh *outMesh, const std::vector<const AdaptableMesh *> &inMeshes);
    
protected:

private:
/// find overlapping vertices by space filling curve
    int mapMergeIndices(const std::vector<const AdaptableMesh *> &inMeshes,
                        const BoundingBox &bbox);
/// sorted and merged positions                      
    void remapPositions(AdaptableMesh *outMesh, const int &nv) const;
/// connect to mapped vertices
    void remapFaces(AdaptableMesh *outMesh, const int &faceOffset,
                    const AdaptableMesh *inMesh, const int &vertexOffset) const;
/// assuming all in meshes has the same uv set
    void combineMeshUv(AdaptableMesh *outMesh, const std::vector<const AdaptableMesh *> &inMeshes) const;
    
};

}

#endif
