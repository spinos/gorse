/*
 *  MorphologyOps.h
 *  acacia
 *
 *  2019/8/28
 */

#ifndef ACA_MORPHOLOGY_OPS_H
#define ACA_MORPHOLOGY_OPS_H

#include "DrawableOps.h"

namespace alo {
    
namespace morph {
class PlantProfile;
class StemProfile;
class Plant;
}

class MorphologyOps : public DrawableOps {
    
    AdaptableMesh *m_mesh;
    morph::PlantProfile *m_plf;
    morph::StemProfile *m_stf;
    int m_randseed;
    
public:

    enum { Type = 716945 };

    MorphologyOps();
    virtual ~MorphologyOps();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();
    void synthesize(morph::Plant &aplant);

};

}

#endif

