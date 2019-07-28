/*
 *  ThinPlateTest.h
 *  acacia
 *
 *
 *  2019/7/29
 */

#ifndef ACA_THIN_PLATE_TEST_H
#define ACA_THIN_PLATE_TEST_H

#include "DrawableOps.h"

namespace alo {

class ThinPlateTest : public DrawableOps {
    
    AdaptableMesh *m_mesh;
    
public:

    enum { Type = 704005 };

    ThinPlateTest();
    virtual ~ThinPlateTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();

};

}

#endif

