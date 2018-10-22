#ifndef ACA_ROOT_OPS_H
#define ACA_ROOT_OPS_H

#include "DrawableOps.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

namespace alo {
class FrontLine;
class AdaptableMesh;
class RootOps : public DrawableOps {
    
    SimpleBuffer<Vector3F> posnml;
    SimpleBuffer<Vector3F> baryc;
    FrontLine *m_fronts[20];
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
        float _len;
        float _lob;
        float _shr;
        float _tws;
        float _bend[2];
        float _bump[2];
        
        TubeProfile() {
            _a = 10.f;
            _b = 10.f;
            _len = 40.f;
            _lob = 8.f;
            _shr = .5f;
            _tws = 0.f;
            _bend[0] = 0.f;
            _bend[1] = 0.f;
            _bump[0] = .5f;
            _bump[1] = .5f;
        }
    };
    
    TubeProfile m_profile;
};

}

#endif

