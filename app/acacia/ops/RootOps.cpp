#include "RootOps.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <geom/AdaptableMesh.h>
#include <mesh/FrontMesher.h>
#include <math/EllipseFunc.h>
#include <math/wavefuncs.h>
#include <rng/Uniform.h>
#include <rng/Lehmer.h>

namespace alo {
   
RootOps::RootOps()
{ m_mesh = new AdaptableMesh; }

RootOps::~RootOps()
{ delete m_mesh; }
    
void RootOps::update()
{
    QAttrib * aa = findAttrib("a");
    FloatAttrib *fa = static_cast<FloatAttrib *>(aa);
    QAttrib * ab = findAttrib("b");
    FloatAttrib *fb = static_cast<FloatAttrib *>(ab);
    fa->getValue(m_profile._a);
    fb->getValue(m_profile._b);
    
    computeMesh();

    DrawableScene *scene = drawableScene();
    DrawableObject *d = drawable();
    if(m_toRelocate) {       
        d->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
        d->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
        d->setDrawArrayLength(m_mesh->numIndices());
        scene->enqueueEditDrawable(d);
    } else {
        scene->enqueueRemoveDrawable(d);
        setMeshDrawable(scene);
    }
}

void RootOps::setMeshDrawable(DrawableScene *scene)
{
    DrawableObject *cly = scene->createDrawable();
    cly->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
    cly->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
    cly->setDrawArrayLength(m_mesh->numIndices());
    setDrawable(cly);
}

void RootOps::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    setMeshDrawable(scene);
}

void RootOps::computeMesh()
{
    m_mesh->initAdaptableMesh();
    
    Uniform<Lehmer> ur(7999999);

    EllipseFunc ellip;
    ellip.setA(m_profile._a);
    ellip.setB(m_profile._b);
    
    const int nu = 60;
    const float da = 6.28318531f / (float)nu;

    for(int i=0;i<nu;++i) {
        float phi = da * i;
        float wr = ScaledSin(phi * 9.8f + ur.randf1() * .8f, .3f, .6f)
                    + ScaledSin(phi * 4.1 + ur.randf1(), .0f, .3f)
                    + ScaledSin(phi * 1.7 + ur.randf1(), .0f, .2f);

        float radius = ellip.polarForm(phi) +  2.f * wr;
        m_mesh->addVertex(Vector3F(radius * cos(phi), 0.f, -radius * sin(phi) ));
    }

    FrontLine originLine;
    
    const Vector3F up(0.f, 1.9f, 0.f);
    const Quaternion lq(.139f, Vector3F::XAxis);
    const Quaternion tq(.059f, Vector3F::YAxis);
    constexpr float cshrinking = .05f;
    
    for(int i=0;i<nu;++i) {
        originLine.addVertex(m_mesh->vertexPositionR(i), i);
    }
    originLine.closeLine();

    originLine.smooth(.14f);

    originLine.setWorldSpace(Matrix33F::IdentityMatrix);
    originLine.rotateLocalBy(lq);
    originLine.rotateLocalBy(tq);
    originLine.setShrinking(cshrinking);
    originLine.setDirection(up);
    originLine.setMinEdgeLength(.4f);
    
    FrontLine *la = &originLine;
    FrontLine l[12];

    FrontMesher msher;
    msher.attachMesh(m_mesh);

    for(int i=0;i<12;++i) {
        msher.setFrontId(i+1);

        l[i].rotateLocalBy(lq);
        l[i].rotateLocalBy(tq);
        l[i].setShrinking(cshrinking);
        l[i].setDirection(up);
        l[i].setMinEdgeLength(.4f);

        msher.advanceFront(l[i], *la);
        
        l[i].smooth(.02f);
        la = &l[i];
    }

    m_mesh->calculateVertexNormals();
    const int oldL = posnml.capacity();
    m_mesh->createPositionNormalArray(posnml);
    m_toRelocate = posnml.capacity() > oldL;
    if(m_toRelocate) m_mesh->createBarycentricCoordinates(baryc);
}

}

