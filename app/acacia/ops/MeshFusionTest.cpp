/*
 *  MeshFusionTest.cpp
 *  acacia
 *
 */

#include "MeshFusionTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/GeodesicSphere.h>
#include <mesh/Fissure.h>
#include <bvh/BVHNodeIterator.h>
#include <mesh/Fusion.h>

namespace alo {
   
MeshFusionTest::MeshFusionTest() :
m_level(43),
m_radius(10.f),
m_shoUV(false)
{
    DrawableResource *rec = createResource();
    setResource(rec);
}

MeshFusionTest::~MeshFusionTest()
{}
    
void MeshFusionTest::update()
{
    getIntAttribValue(m_level, "l");
    getFloatAttribValue(m_radius, "r");
    getBoolAttribValue(m_shoUV, "sho_uv");
    computeMesh();
}

void MeshFusionTest::addDrawableTo(DrawableScene *scene)
{
    setDrawableScene(scene);
    computeMesh();  
}

void MeshFusionTest::computeMesh()
{
    GeodesicSphere transient(m_level);
    transient.scaleBy(m_radius);
    
    Fissure fis;
    const int npart = fis.granulate(&transient);
    std::cout << " granulate to " << npart << " parts ";
    
    std::vector<AdaptableMesh *> meshParts;
    BVHNodeIterator it = fis.firstPart();
    while(it._node) {

        AdaptableMesh *am = new AdaptableMesh;
        fis.reformPart(am, it, &transient);
        meshParts.push_back(am);

        it = fis.nextPart(it);
    }
    
    AdaptableMesh combinedMesh;
    Fusion fus;
    fus.combine(&combinedMesh, meshParts);
    
    DrawableResource *rec = resource();

    lockScene();
    const int beforeFrame = frameNumber() - 2;
    if(rec->changedOnFrame() > beforeFrame) {
/// prevent editing unsynchronized resource
        unlockScene();
        return;
    }

    UpdateMeshResouce(rec, &transient, m_shoUV);

    processResourceNoLock(rec);
    unlockScene();

    if(m_shoUV)
        setBound(BoundingBox(-.5f, -.5f, -.5f, 1.5f, 1.5f, 1.f));
    else
        setBound(&transient);
}

}

