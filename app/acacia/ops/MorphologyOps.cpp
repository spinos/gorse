/*
 *  MorphologyOps.cpp
 *  acacia
 *
 */

#include "MorphologyOps.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/AdaptableMesh.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include <morph/Monopodial.h>
#include <morph/PlantMesher.h>

namespace alo {

MorphologyOps::MorphologyOps() : m_randseed(7654321),
m_plf(new morph::PlantProfile),
m_stf(new morph::StemProfile),
m_triangulatePercent(1.f)
{
    m_mesh = new AdaptableMesh;

    DrawableResource *rec = createResource();
    setResource(rec);
}

MorphologyOps::~MorphologyOps()
{
    delete m_plf;
    delete m_stf;
}
    
void MorphologyOps::update()
{
    int age = 8;
    getIntAttribValue(age, "aage");
    m_plf->setAge(age);
    
    float branchprob;
    getFloatAttribValue(branchprob, "abranchprob");
    m_plf->setBranchProbability(branchprob);
    
    float flow;
    getFloatAttribValue(flow, "abranchflow");
    m_plf->setResourceRatio(flow);
    
    int branchAge = 4;
    getIntAttribValue(branchAge, "abranchmin");
    m_plf->setMinBranchSeason(branchAge);
    
    int stop1, stop2;
    getIntAttribValue(stop1, "astop1st");
    getIntAttribValue(stop2, "astop2nd");
    m_plf->setStopAge(Int2(stop1,stop2));
    
    float stopp;
    getFloatAttribValue(stopp, "astopprob");
    m_plf->setStopProbability(stopp);
    
    float axilAngle = .8f;
    getFloatAttribValue(axilAngle, "aaxilangle");
    m_stf->setAxilAngle(axilAngle);
    
    getIntAttribValue(m_randseed, "arandseed");
    getFloatAttribValue(m_triangulatePercent, "atripercent");
    
    computeMesh();
}

void MorphologyOps::addDrawableTo(DrawableScene *scene)
{
    setDrawableScene(scene);
    computeMesh();  
}

void MorphologyOps::computeMesh()
{
    morph::Plant aplant;
    synthesize(aplant);
    m_mesh->purgeMesh();
    
    morph::PlantMesher plmshr;
    plmshr.attach(m_mesh);
    plmshr.triangulate(aplant, m_triangulatePercent, *m_plf, *m_stf);
    plmshr.detach();
    
    DrawableResource *rec = resource();

    lockScene();
    const int beforeFrame = frameNumber() - 2;
    if(rec->changedOnFrame() > beforeFrame) {
/// prevent editing unsynchronized resource
        unlockScene();
        return;
    }

    UpdateMeshResouce(rec, m_mesh);

    processResourceNoLock(rec);
    unlockScene();

    setBound(m_mesh);
}

void MorphologyOps::synthesize(morph::Plant &aplant)
{
    Uniform<Lehmer> lmlcg(m_randseed);

    morph::Monopodial<Uniform<Lehmer> > plantRule(&lmlcg);

    m_plf->setGrowDirections(Vector3F(0.f, 1.f, 0.f), Vector3F(0.f, 1.3f, 0.f));
    plantRule.grow(&aplant, *m_plf, *m_stf);
}

}
