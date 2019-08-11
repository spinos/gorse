#include "PVSTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/AdaptableMesh.h>
#include <mesh/FrontMesher.h>
#include <math/miscfuncs.h>
#include <math/CameraEvent.h>
#include <math/PerspectiveCamera.h>

namespace alo {
   
PVSTest::PVSTest()
{}

PVSTest::~PVSTest()
{}
    
void PVSTest::update()
{    
    bool fz;
    getBoolAttribValue(fz, "freeze_view");
    setFreezeView(fz);
}

void PVSTest::addDrawableTo(DrawableScene *scene)
{ 
    setDrawableScene(scene);
    computeMesh();
}

void PVSTest::computeMesh()
{
    AdaptableMesh srcMesh;

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
    msher.attach(&srcMesh);

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

    int npart = reduce(culler(), &srcMesh);

    initializeDetails();

    lockScene();
    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        processResourceNoLock(rec);
    }
    unlockScene();
}

void PVSTest::recvCameraChanged(const CameraEvent &x)
{
    if(!drawableVisible()) return;
    if(freezeView()) return;
    if(x.progressBegin() || x.progressEnd()) return;
    const PerspectiveCamera *persp = static_cast<const PerspectiveCamera *>(x.camera());
    if(!updateView(*persp, *x.frustum())) return;
    
    viewDependentReform(culler(), details());

    lockScene();
    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        const VisibilityState &vis = visibility(i);
        processResource(rec, vis);
    }
    unlockScene();
}

}
