#include "PVSTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/AdaptableMesh.h>
#include <mesh/FrontMesher.h>
#include <mesh/Fissure.h>
#include <math/miscfuncs.h>
#include <bvh/BVHNodeIterator.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <qt_ogl/CameraEvent.h>
#include <math/AFrustum.h>

namespace alo {
   
PVSTest::PVSTest()
{
    m_mesh = new AdaptableMesh; 

    DrawableResource *rec = createResource();
    setResource(rec);
}

PVSTest::~PVSTest()
{ delete m_mesh; }
    
void PVSTest::update()
{
    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    
    //if(cachePathChanged(scachePath) )
    //    loadCache(scachePath);
    
    computeMesh();

    DrawableResource *rec = resource();
    processResource(rec);
}

void PVSTest::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    DrawableResource *rec = resource();
    initiateResource(rec);
}

void PVSTest::computeMesh()
{
    m_mesh->initAdaptableMesh();

    const int nu = 255;
    const double du = 4.0 / (double)nu;
    for(int i=0;i<nu;++i) {
        double xu = -2.0 + du * i;
        m_mesh->addVertex(Vector3F(xu, tanh(xu), sinh(xu)) * 50.f);
    }

    FrontLine originLine;
    FrontLine::EnableMerging = false;

    for(int i=0;i<nu;++i)
        originLine.addVertex(m_mesh->vertexPositionR(i), i);

    Vector3F up(-.85f, 2.1f, 0.f);
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
    FrontLine l[255];

    FrontMesher msher;
    msher.attachMesh(m_mesh);

    for(int i=0;i<255;++i) {
        msher.setFrontId(i+1);

        l[i].rotateLocalBy(lq);
        l[i].rotateLocalBy(tq);
        l[i].setShrinking(cshrinking);
        up.x += RandomFn11() * .199f;
        up.y += RandomFn11() * .399f;
        up.z += RandomFn11() * .499f;
        l[i].setDirection(up);
        l[i].setMinEdgeLength(du);

        msher.advanceFront(l[i], *la);

        la = &l[i];
    }

    m_mesh->calculateVertexNormals();

    std::cout << " mesh nv " << m_mesh->numVertices()
            << " nt " << m_mesh->numTriangles();

    Fissure fis;
    fis.granulate(m_mesh);

    AdaptableMesh transient;

    BVHNodeIterator it = fis.firstPart();
    while(it._node) {

        fis.reformPart(&transient, it, m_mesh);
        it = fis.nextPart(it);
    }

    DrawableResource *rec = resource();
    updateMeshResouce(rec, m_mesh);

}

void PVSTest::recvCameraChanged(const CameraEvent &x)
{
   // std::cout << "\n camera frustum " << *(x.frustum());
}

}
