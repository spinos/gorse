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
class HistoryMesh;
class ViewFrustumCull;
class VisibleDetail;
class PVSTest : public DrawableOps {

	struct MeshReformPair {
		HistoryMesh *_outMesh;
		HistoryMesh *_stageMesh;
	};

	std::vector<MeshReformPair> m_meshes;
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
    static void SimplifyAndReform(MeshReformPair &p, HistoryMesh *srcMesh, DrawableResource *rec);

};

}

#endif