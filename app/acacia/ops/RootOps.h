#ifndef ACA_ROOT_OPS_H
#define ACA_ROOT_OPS_H

#include "DrawableOps.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

namespace alo {
class AdaptableMesh;
class RootOps : public DrawableOps {
    
    SimpleBuffer<Vector3F> posnml;
    SimpleBuffer<Vector3F> baryc;
    AdaptableMesh *m_mesh;
    bool m_toRelocate;
    
public:
    RootOps();
    virtual ~RootOps();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();
    void setMeshDrawable(DrawableScene *scene);
    
    struct TubeProfile {
        float _a;
        float _b;
        
        TubeProfile() {
            _a = 10.f;
            _b = 10.f;
        }
    };
    
    TubeProfile m_profile;
};

}

#endif

