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
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

namespace alo {
    
namespace sdf {
class SsdField;
}

class ThinPlateTest : public DrawableOps {
    
    AdaptableMesh *m_mesh;
    static boost::mutex sMutex;
    
public:

    enum { Type = 704005 };

    ThinPlateTest();
    virtual ~ThinPlateTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:

    static bool WriteField(const AdaptableMesh *msh, 
				const int &grpId,
				std::vector<sdf::SsdField *> *fields,
				float *grdBox);
                
    static bool WriteGrid(const std::vector<sdf::SsdField *> &fields,
				const float *grdBox);
                
    static bool WriteTerrainSamples(const AdaptableMesh *msh, 
                const float *grdBox);
    
private:
    void computeMesh();

};

}

#endif

