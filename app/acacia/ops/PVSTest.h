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
#include <cull/ViewDependentFunction.h>

namespace alo {
class CameraEvent;
class PVSTest : public GranulateReduce, public ViewDependentFunction {
    
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