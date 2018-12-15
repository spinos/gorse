/*
 *  LodMeshInOps.h
 *  acacia
 *
 */

#ifndef ACA_LOD_MESH_IN_OPS_H
#define ACA_LOD_MESH_IN_OPS_H

#include "DrawableOps.h"
#include <h5_mesh/LodMeshGroup.h>
#include <boost/thread.hpp>
#include <h5/V1H5IO.h>
#include <cull/ViewDependentFunction.h>

namespace alo {
class LodMeshCache;
class PerspectiveCamera;
class LodMeshInOps : public DrawableOps, public ViewDependentFunction {
    
    LodMeshGroup m_cache;
    float m_lod;
    bool m_shoUV;
    boost::mutex m_mtx;
    ver1::H5IO m_hio;
    static AFileDlgProfile SReadProfile;
    
public:
    LodMeshInOps();
    virtual ~LodMeshInOps();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;
    virtual void recvCameraChanged(const CameraEvent &x) override;
    virtual AFileDlgProfile *readFileProfileR () const override;
    
protected:
    
private:
    void computeMesh();
    void reformMesh(LodMeshCache *c, DrawableResource *rec);
    void reformMesh1(LodMeshCache *c, int nv, bool forcedUpdate, DrawableResource *rec);
    bool loadCache(const std::string &fileName);
    void viewDependentReform(const PerspectiveCamera *persp);

};

}

#endif
