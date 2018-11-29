#include "RootOps.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/AdaptableMesh.h>
#include <mesh/FrontMesher.h>
#include <math/EllipseFunc.h>
#include <math/wavefuncs.h>
#include <rng/Uniform.h>
#include <rng/Lehmer.h>

namespace alo {
   
RootOps::RootOps()
{ 
    for(int i=0;i<20;++i) m_fronts[i] = new FrontLine;
    m_mesh = new AdaptableMesh; 

    DrawableResource *rec = createResource();
    setResource(rec);
}

RootOps::~RootOps()
{ 
    for(int i=0;i<20;++i) delete m_fronts[i];
    delete m_mesh; 
}
    
void RootOps::update()
{
    QAttrib * aa = findAttrib("a");
    FloatAttrib *fa = static_cast<FloatAttrib *>(aa);
    QAttrib * ab = findAttrib("b");
    FloatAttrib *fb = static_cast<FloatAttrib *>(ab);
    QAttrib * ashr = findAttrib("shr");
    FloatAttrib *fshr = static_cast<FloatAttrib *>(ashr);
    QAttrib * alen = findAttrib("len");
    FloatAttrib *flen = static_cast<FloatAttrib *>(alen);
    QAttrib * atws = findAttrib("tws");
    FloatAttrib *ftws = static_cast<FloatAttrib *>(atws);
    QAttrib * abend = findAttrib("bend");
    Float2Attrib *fbend = static_cast<Float2Attrib *>(abend);
    QAttrib * abump = findAttrib("bump");
    Float2Attrib *fbump = static_cast<Float2Attrib *>(abump);
    QAttrib * alob = findAttrib("lob");
    FloatAttrib *flob = static_cast<FloatAttrib *>(alob);
    fa->getValue(m_profile._a);
    fb->getValue(m_profile._b);
    flen->getValue(m_profile._len);
    fshr->getValue(m_profile._shr);
    ftws->getValue(m_profile._tws);
    fbend->getValue(m_profile._bend);
    fbump->getValue(m_profile._bump);
    flob->getValue(m_profile._lob);
    
    computeMesh();

    DrawableScene *scene = drawableScene();
    DrawableObject *d = drawable();
    const DrawableResource *rec = resource();
    if(rec->toRelocate()) {
        scene->enqueueRemoveDrawable(d);
        DrawableObject *d1 = setMeshDrawable(m_mesh, rec);
        setDrawable(d1);
        scene->enqueueCreateDrawable(d1, opsId());

    } else {
        updateDrawableResource(d, rec, m_mesh->numIndices());
        scene->enqueueEditDrawable(d);
    }
}

void RootOps::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    const DrawableResource *rec = resource();
    DrawableObject *d = setMeshDrawable(m_mesh, rec);
    setDrawable(d);
    scene->enqueueCreateDrawable(d, opsId());
}

void RootOps::computeMesh()
{
    m_mesh->initAdaptableMesh();
    
    Uniform<Lehmer> ur(7999999);

    EllipseFunc ellip;
    ellip.setA(m_profile._a);
    ellip.setB(m_profile._b);
    const float lobeSize = (m_profile._a + m_profile._b) * .125f;
    
    int nu = Absolute<float>(m_profile._tws) * 3.f
            + Absolute<float>(m_profile._bend[0]) * 1.5f
             + m_profile._lob * 8.f;
    if(nu < 32) nu = 32;
    if(nu > 125) nu = 125;
    const float da = 6.28318531f / (float)nu;

    for(int i=0;i<nu;++i) {
        float phi = da * i;
        float wr = ScaledSin(phi * m_profile._lob + ur.randf1() * 1.4f, m_profile._bump[0], m_profile._bump[1])
                    + ScaledSin(phi * m_profile._lob * .5f + ur.randf1() * .8f, m_profile._bump[0] * .5f, m_profile._bump[1] * .5f)
                    + ScaledSin(phi * m_profile._lob * .1f + ur.randf1() * .5f, m_profile._bump[0] * .2f, m_profile._bump[1] * .2f);

        float radius = ellip.polarForm(phi) +  lobeSize * wr;
        m_mesh->addVertex(Vector3F(radius * cos(phi), 0.f, -radius * sin(phi) ));
    }

    FrontLine &originLine = *m_fronts[0];
    originLine.clearLine();
    
    for(int i=0;i<nu;++i) {
        originLine.addVertex(m_mesh->vertexPositionR(i), i);
    }
    originLine.closeLine();

    originLine.smooth(.19f);

    const float minEdgeL = .2f * lobeSize;
    float segL = minEdgeL * 5.3f;
    int numSeg = m_profile._len / segL;
    if(numSeg > 19)
        numSeg = 19;
    if(numSeg < 2)
        numSeg = 2;
    segL = m_profile._len / (float)numSeg;
    if(segL < minEdgeL * 2.f)
        segL = minEdgeL * 2.f;

    const Vector3F up(0.f, segL, 0.f);
    const float segShr = m_profile._shr / (float)numSeg;
    const Quaternion tq(m_profile._tws / (float)numSeg, Vector3F::YAxis);
    const Quaternion lq(m_profile._bend[0] / (float)numSeg, 
                Vector3F(sin(m_profile._bend[1]), 0.f, cos(m_profile._bend[1]) ) );
    
    originLine.setWorldSpace(Matrix33F::IdentityMatrix);
    originLine.rotateLocalBy(lq);
    originLine.rotateLocalBy(tq);
    originLine.setShrinking(segShr);
    originLine.setDirection(up);
    originLine.setMinEdgeLength(minEdgeL);
    
    FrontLine *la = &originLine;

    FrontMesher msher;
    msher.attachMesh(m_mesh);

    for(int i=1;i<1+numSeg;++i) {
        msher.setFrontId(i);

        FrontLine &li = *m_fronts[i];
        li.clearLine();
        li.rotateLocalBy(lq);
        li.rotateLocalBy(tq);
        li.setShrinking(segShr);
        li.setDirection(up);
        li.setMinEdgeLength(minEdgeL);

        msher.advanceFront(li, *la);
        
        li.smooth(.02f);
        la = &li;
    }

    m_mesh->calculateVertexNormals();

    DrawableResource *rec = resource();
    updateMeshResouce(rec, m_mesh);
    
}

}

