/*
 *  LodMeshOps.h
 *  acacia
 *
 */


#ifndef ACA_LOD_MESH_OPS_H
#define ACA_LOD_MESH_OPS_H

#include "MeshListener.h"

namespace alo {

class LodMeshOps : public MeshListener {
    
    float m_lod;
    bool m_shoUV;
    
public:
    LodMeshOps();
    virtual ~LodMeshOps();

    virtual void update() override;

protected:

private:
    void computeMesh();
    
};

}

#endif

