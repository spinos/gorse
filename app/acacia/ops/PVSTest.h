/*
 *  PVSTest.h
 *  acacia
 *
 *  fissure test mesh 
 *  extract pvs
 *
 */

#ifndef ACA_PVS_TEST_H
#define ACA_PVS_TEST_H

#include "DrawableOps.h"

namespace alo {
class CameraEvent;
class AdaptableMesh;
class PVSTest : public DrawableOps {
    
    AdaptableMesh *m_mesh;

public:
    PVSTest();
    virtual ~PVSTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;
    virtual void recvCameraChanged(const CameraEvent &x) override;
    
protected:
    
private:
    void computeMesh();
   
};

}

#endif