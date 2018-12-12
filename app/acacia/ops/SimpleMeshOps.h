/*
 *  SimpleMeshOps.h
 *  acacia
 *
 */


#ifndef ACA_SIMPLE_MESH_OPS_H
#define ACA_SIMPLE_MESH_OPS_H

#include "MeshListener.h"

namespace alo {

class SimpleMeshOps : public MeshListener {
    
    bool m_shoUV;
    
public:
    SimpleMeshOps();
    virtual ~SimpleMeshOps();

    virtual void update() override;

protected:

private:
    void computeMesh();
    
};

}

#endif

