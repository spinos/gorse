#ifndef ACA_LOD_MESH_IN_H
#define ACA_LOD_MESH_IN_H

#include "DrawableOps.h"

namespace alo {
class LodMeshCache;
class LodMeshIn : public DrawableOps {
    
    std::vector<LodMeshCache *> m_cacheList;
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
    bool cachePathChanged(const std::string &fileName) const;

};

}

#endif
