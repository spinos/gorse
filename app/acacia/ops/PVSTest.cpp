#include "PVSTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/AdaptableMesh.h>
#include <mesh/FrontMesher.h>
#include <math/miscfuncs.h>
#include <cull/ViewFrustumCull.h>
#include <cull/VisibleDetail.h>
#include <qt_ogl/CameraEvent.h>
#include <math/AFrustum.h>
#include <math/PerspectiveCamera.h>

namespace alo {
   
PVSTest::PVSTest() :
m_freeze(false)
{
    m_culler = new ViewFrustumCull;
    m_details = new VisibleDetail;
}

PVSTest::~PVSTest()
{
    delete m_culler;
    delete m_details;
}
    
void PVSTest::update()
{    
    QAttrib * afreeze = findAttrib("freeze_view");
    BoolAttrib *ffreeze = static_cast<BoolAttrib *>(afreeze);
    ffreeze->getValue(m_freeze);
}

void PVSTest::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        initiateResource(rec);
    }
}

void PVSTest::computeMesh()
{
    AdaptableMesh srcMesh;
    srcMesh.initAdaptableMesh();

    const int nu = 256;
    const double du = 4.0 / (double)nu;
    for(int i=0;i<nu;++i) {
        double xu = -2.0 + du * i;
        srcMesh.addVertex(Vector3F(xu, tanh(xu), sinh(xu)) * 99.f);
    }

    FrontLine originLine;
    FrontLine::EnableMerging = false;

    for(int i=0;i<nu;++i)
        originLine.addVertex(srcMesh.vertexPositionR(i), i);

    Vector3F up(-1.95f, 4.1f, 0.f);
    const Quaternion lq(-.00972f, Vector3F::ZAxis);
    const Quaternion tq(.00293f, Vector3F::YAxis);
    constexpr float cshrinking = .0f;
    originLine.setWorldSpace(Matrix33F::IdentityMatrix);
    originLine.rotateLocalBy(lq);
    originLine.rotateLocalBy(tq);
    originLine.setShrinking(cshrinking);
    originLine.setDirection(up);
    originLine.setMinEdgeLength(du);

    FrontLine *la = &originLine;
    FrontLine l[286];

    FrontMesher msher;
    msher.attachMesh(&srcMesh);

    for(int i=0;i<286;++i) {
        msher.setFrontId(i+1);

        l[i].rotateLocalBy(lq);
        l[i].rotateLocalBy(tq);
        l[i].setShrinking(cshrinking);
        up.x += RandomFn11() * .299f;
        up.y += RandomFn11() * .399f;
        up.z += RandomFn11() * .499f;
        l[i].setDirection(up);
        l[i].setMinEdgeLength(du);

        msher.advanceFront(l[i], *la);

        la = &l[i];
    }

    srcMesh.calculateVertexNormals();

    int npart = reduce(m_culler, &srcMesh);

    m_details->create(npart);
    m_details->setVisible(true);
    m_details->setDetail(0);
    m_details->setDeltaDistance(m_culler->getMeanSize() / 16.f);
    for(int i=0;i<npart;++i) {
        m_details->setMinDetail(outMeshNv(i), i);
    }
}

void PVSTest::recvCameraChanged(const CameraEvent &x)
{
    if(m_freeze) return;
    if(x.progressBegin() || x.progressEnd()) return;
    if(!m_details->updateView(*x.camera())) return;

    m_culler->compare(m_details->visibilities(), *(x.frustum()));

    const PerspectiveCamera *persp = static_cast<const PerspectiveCamera *>(x.camera());
    viewDependentReform(persp, m_culler, m_details);

    drawableScene()->lock();
    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        const VisibilityState &vis = m_details->c_visibilities()[i];
        processResource(rec, vis);
    }
    drawableScene()->unlock();
}

}
