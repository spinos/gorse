/*
 *  PVSTest.h
 *  acacia
 *
 *  fissure mesh 
 *  extract pvs
 *
 */

#ifndef ACA_PVS_TEST_H
#define ACA_PVS_TEST_H

#include "DrawableOps.h"

namespace alo {
class CameraEvent;
class AdaptableMesh;
class HistoryMesh;
class ViewFrustumCull;
class VisibleDetail;
class LevelOfDetailSelect;
class Hexahedron;
class PerspectiveCamera;
class PVSTest : public DrawableOps {

	struct MeshReformTrio {
		AdaptableMesh *_outMesh;
		HistoryMesh *_stageMesh;
        HistoryMesh *_srcMesh;
	};

	std::vector<MeshReformTrio> m_meshes;
    ViewFrustumCull *m_culler;
    VisibleDetail *m_details;
    bool m_freeze;
    
public:
    PVSTest();
    virtual ~PVSTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;
    virtual void recvCameraChanged(const CameraEvent &x) override;
    
protected:
    
private:
    void computeMesh();
    void addMeshReformPair();
    void computeLod(const PerspectiveCamera *persp);
    static void SimplifyAndReform(MeshReformTrio &p, DrawableResource *rec);
    static void Reform(MeshReformTrio &p, float lod, DrawableResource *rec);
    static void LodReform(LevelOfDetailSelect &lod, const Hexahedron &hexa, const PerspectiveCamera &camera,
                MeshReformTrio &p, DrawableResource *rec);

};

}

#endif