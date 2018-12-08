#ifndef ACA_LOD_MESH_IN_H
#define ACA_LOD_MESH_IN_H

#include "DrawableOps.h"
#include <h5_mesh/LodMeshGroup.h>
#include <boost/thread.hpp>

namespace alo {
class LodMeshCache;
class LodMeshIn : public DrawableOps {
    
    LodMeshGroup m_cache;
    float m_lod;
    bool m_shoUV;
    boost::mutex m_mtx;
    
public:
    LodMeshIn();
    virtual ~LodMeshIn();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();
    void reformMesh(LodMeshCache *c, AdaptableMesh *mesh, DrawableResource *rec);

};

}

#endif
