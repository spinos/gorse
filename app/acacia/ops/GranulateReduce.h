/*
 *  GranulateReduce.h
 *  acacia
 *
 */
 
#ifndef ACA_GRANULATE_REDUCE_H
#define ACA_GRANULATE_REDUCE_H

#include "DrawableOps.h"

namespace alo {
class AdaptableMesh;
class HistoryMesh;
class ViewFrustumCull;
class VisibleDetail;
class LevelOfDetailSelect;
class Hexahedron;
class PerspectiveCamera;
class GranulateReduce : public DrawableOps {

public:
    GranulateReduce();
    virtual ~GranulateReduce();
    
protected:
    int reduce(ViewFrustumCull *culler, const AdaptableMesh *srcMesh);
    void viewDependentReform(const PerspectiveCamera *persp,
                const ViewFrustumCull *culler, VisibleDetail *details);
    void simpleReform(const float &lod, bool shoAsUV);
    
    struct MeshReformTrio {
		AdaptableMesh *_outMesh;
		HistoryMesh *_stageMesh;
        HistoryMesh *_srcMesh;
	};
	
	void addMeshReformTrio();
	
	int numMeshTrios() const;
	MeshReformTrio &meshTrio(int i);
	
	const int &outMeshNv(int i) const;
	static void SimplifyAndReform(MeshReformTrio &p, DrawableResource *rec);
    static void Reform(MeshReformTrio &p, int nt, DrawableResource *rec);
    static void LodReform(LevelOfDetailSelect &lod, const Hexahedron &hexa, const PerspectiveCamera &camera,
                MeshReformTrio &p, DrawableResource *rec);
    static void LodReform1(const float &lod, bool shoAsUV,
                MeshReformTrio &p, DrawableResource *rec);

private:
    std::vector<MeshReformTrio> m_meshes;
	
};

}

#endif

