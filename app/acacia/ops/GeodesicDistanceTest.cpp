/*
 *  GeodesicDistanceTest.cpp
 *  acacia
 *
 */

#include "GeodesicDistanceTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/AdaptableMesh.h>
#include <mesh/GeodesicDistance.h>
#include <mesh/FrontMesher.h>
#include <math/miscfuncs.h>

namespace alo {
   
GeodesicDistanceTest::GeodesicDistanceTest() 
{
    m_displacement[0] = m_displacement[1] = m_displacement[2] = 0.f;
    m_mesh = new AdaptableMesh;
    const int nu = 6;
    for(int i=0;i<nu;++i)
        m_mesh->addVertex(Vector3F(RandomFn11() * .13f + .53f * i, RandomFn11() * .19f, .3f * i));
    
    FrontLine originLine;
    FrontLine::EnableMerging = false;
    
    for(int i=0;i<nu;++i)
        originLine.addVertex(m_mesh->vertexPositionR(i), i);
    
    Vector3F up(0.f, .41f, 0.f);
    const Quaternion lq(-.0572f, Vector3F::ZAxis);
    const Quaternion tq(.083f, Vector3F::YAxis);
    constexpr float cshrinking = .0f;
    originLine.setWorldSpace(Matrix33F::IdentityMatrix);
    originLine.rotateLocalBy(lq);
    originLine.rotateLocalBy(tq);
    originLine.setShrinking(cshrinking);
    originLine.setDirection(up);
    originLine.setMinEdgeLength(.2f);
    
    FrontLine *la = &originLine;
    FrontLine l[100];
    
    FrontMesher msher;
    msher.attachMesh(m_mesh);
    for(int i=0;i<100;++i) {
        msher.setFrontId(i+1);

        l[i].rotateLocalBy(lq);
        l[i].rotateLocalBy(tq);
        l[i].setShrinking(cshrinking + RandomFn11() * .07f);
        up.y += RandomFn11() * .29f;
        up.z += RandomFn11() * .29f;
        l[i].setDirection(up);
        l[i].setMinEdgeLength(.2f);

        msher.advanceFront(l[i], *la);

        la = &l[i];
    }

    m_mesh->calculateVertexNormals();
    
    m_geod = new GeodesicDistance;
    m_geod->buildTopology(m_mesh);
    
    int v0 = m_geod->closestVertexTo(Vector3F::Zero, m_mesh);
    m_geod->calcDistanceFromVertex(v0, m_mesh);
    m_geod->normalizeDistance(m_mesh);
    
    DrawableResource *rec = createResource();
    setResource(rec);
}

GeodesicDistanceTest::~GeodesicDistanceTest()
{}
    
void GeodesicDistanceTest::update()
{
    getFloat3AttribValue(m_displacement, "disp");
    computeMesh();
}

void GeodesicDistanceTest::addDrawableTo(DrawableScene *scene)
{
    setDrawableScene(scene);
    computeMesh();  
}

void GeodesicDistanceTest::computeMesh()
{
    m_geod->deform(m_mesh, Vector3F(m_displacement));
    m_geod->calcGradient(m_mesh);
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

}
