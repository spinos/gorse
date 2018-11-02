#include "EdgeCollapseTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <geom/AdaptableMesh.h>
#include <mesh/FrontMesher.h>
#include <math/wavefuncs.h>
#include <rng/Uniform.h>
#include <rng/Lehmer.h>
#include <mesh/EdgeCollapse.h>
#include <mesh/HistoryMesh.h>
#include <mesh/HistoryReform.h>

namespace alo {
   
EdgeCollapseTest::EdgeCollapseTest()
{
    m_mesh = new AdaptableMesh;
    m_sourceMesh = new HistoryMesh;
    m_reformer = new HistoryReform;
    const int nu = 34;
    for(int i=0;i<nu;++i) {
        float phi = .21f * i;
        m_sourceMesh->addVertex(Vector3F(-67.f + 3.53f * i + RandomFn11() * 1.41f, -40.f + 11.f * sin(phi), -30.f -13.f * sin(phi) ));
    }
    
    FrontLine originLine;
    FrontLine::EnableMerging = false;
    
    for(int i=0;i<nu;++i)
        originLine.addVertex(m_sourceMesh->vertexPositionR(i), i);
    
    Vector3F up(0.f, 4.7f, 0.1f);
    const Quaternion lq(-.039f, Vector3F::ZAxis);
    const Quaternion tq(.015f, Vector3F::YAxis);
    constexpr float cshrinking = .0f;
    originLine.setWorldSpace(Matrix33F::IdentityMatrix);
    originLine.rotateLocalBy(lq);
    originLine.rotateLocalBy(tq);
    originLine.setShrinking(cshrinking);
    originLine.setDirection(up);
    originLine.setMinEdgeLength(.1f);
    
    FrontLine *la = &originLine;
    FrontLine l[24];

    FrontMesher msher;
    msher.attachMesh(m_sourceMesh);

    for(int i=0;i<24;++i) {
        msher.setFrontId(i+1);

        l[i].rotateLocalBy(lq);
        l[i].rotateLocalBy(tq);
        l[i].setShrinking(cshrinking + RandomFn11() * .02f);
        up.y += RandomFn11() * 1.97f;
        up.z += RandomFn11() * 1.99f;
        l[i].setDirection(up);
        l[i].setMinEdgeLength(.1f);

        msher.advanceFront(l[i], *la);

        la = &l[i];
    }

    m_sourceMesh->calculateVertexNormals();
    
    EdgeCollapse ech;
    ech.simplify(m_sourceMesh);
    
    m_lod = .5f;
}

EdgeCollapseTest::~EdgeCollapseTest()
{ 
    delete m_mesh; 
    delete m_sourceMesh;
}
    
void EdgeCollapseTest::update()
{
    QAttrib * al = findAttrib("lod");
    FloatAttrib *fl = static_cast<FloatAttrib *>(al);
    fl->getValue(m_lod);
    
    computeMesh();

    DrawableScene *scene = drawableScene();
    DrawableObject *d = drawable();
    if(m_toRelocate) {
        scene->enqueueRemoveDrawable(d);
        setMeshDrawable(scene);
    } else {
        d->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
        d->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
        d->setDrawArrayLength(m_mesh->numIndices());
        scene->enqueueEditDrawable(d);
    }
}

void EdgeCollapseTest::setMeshDrawable(DrawableScene *scene)
{
    DrawableObject *cly = scene->createDrawable();
    cly->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
    cly->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
    cly->setDrawArrayLength(m_mesh->numIndices());
    setDrawable(cly);
}

void EdgeCollapseTest::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    setMeshDrawable(scene);
}

void EdgeCollapseTest::computeMesh()
{
    m_reformer->reform(m_mesh, m_lod, m_sourceMesh);
    
    const int oldL = posnml.capacity();
    m_mesh->createPositionNormalArray(posnml);
    m_toRelocate = oldL < posnml.capacity();
    if(m_toRelocate) m_mesh->createBarycentricCoordinates(baryc);
}

}

