#ifndef ACA_PVS_TEST_H
#define ACA_PVS_TEST_H

#include "DrawableOps.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

namespace alo {
class CameraEvent;
class PVSTest : public DrawableOps {
    
    SimpleBuffer<Vector3F> posnml;
    SimpleBuffer<Vector3F> baryc;
    int m_drawLength;
    bool m_toRelocate;
    
public:
    PVSTest();
    virtual ~PVSTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;
    virtual void recvCameraChanged(const CameraEvent &x) override;
    
protected:
    
private:
    void computeMesh();
    void setMeshDrawable(DrawableScene *scene);
    
};

}

#endif