#ifndef ACA_LOD_MESH_IN_H
#define ACA_LOD_MESH_IN_H

#include "DrawableOps.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>
#include <h5_mesh/LodMeshCache.h>

namespace alo {
class AdaptableMesh;
class HistoryMesh;
class LodMeshIn : public DrawableOps, public LodMeshCache {
    
    SimpleBuffer<Vector3F> posnml;
    SimpleBuffer<Vector3F> baryc;
    AdaptableMesh *m_mesh;
    float m_lod;
    bool m_toRelocate;
    bool m_shoUV;
    
public:
    LodMeshIn();
    virtual ~LodMeshIn();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();
    void setMeshDrawable(DrawableScene *scene);
    
};

}

#endif
