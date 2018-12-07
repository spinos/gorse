/*
 *  PVSTest.h
 *  acacia
 *
 *  fissure mesh 
 *  extract pvs
 *
 */

#ifndef ACA_PVS_TEST_H
#define ACA_PVS_TEST_H

#include "GranulateReduce.h"

namespace alo {
class CameraEvent;
class PVSTest : public GranulateReduce {

    ViewFrustumCull *m_culler;
    VisibleDetail *m_details;
    bool m_freeze;
    
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