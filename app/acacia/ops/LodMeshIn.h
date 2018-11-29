#ifndef ACA_LOD_MESH_IN_H
#define ACA_LOD_MESH_IN_H

#include "DrawableOps.h"
#include <h5_mesh/LodMeshCache.h>

namespace alo {
class AdaptableMesh;
class HistoryMesh;
class LodMeshIn : public DrawableOps, public LodMeshCache {
    
    AdaptableMesh *m_mesh;
    float m_lod;
    bool m_shoUV;
    
public:
    LodMeshIn();
    virtual ~LodMeshIn();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();
};

}

#endif
