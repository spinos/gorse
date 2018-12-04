#include "PVSTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/AdaptableMesh.h>
#include <mesh/FrontMesher.h>
#include <mesh/Fissure.h>
#include <mesh/HistoryMesh.h>
#include <mesh/HistoryReformSrc.h>
#include <mesh/EdgeCollapse.h>
#include <math/miscfuncs.h>
#include <bvh/BVHNodeIterator.h>
#include <cull/ViewFrustumCull.h>
#include <cull/VisibleDetail.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <qt_ogl/CameraEvent.h>
#include <math/AFrustum.h>
#include <boost/chrono/include.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

namespace alo {
   
PVSTest::PVSTest() :
m_freeze(false)
{
    DrawableResource *rec = createResource();
    setResource(rec);
    m_culler = new ViewFrustumCull;
    m_details = new VisibleDetail;
}

PVSTest::~PVSTest()
{
    std::vector<MeshReformPair>::iterator it = m_meshes.begin();
    for(;it!=m_meshes.end();++it) {
        delete it->_outMesh;
        delete it->_stageMesh;
    }
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
    FrontLine l[266];

    FrontMesher msher;
    msher.attachMesh(&srcMesh);

    for(int i=0;i<266;++i) {
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

    Fissure fis;
    const int npart = fis.granulate(&srcMesh);
    std::cout << "\n fissure to " << npart << " parts ";
    m_culler->create(fis.bvh());

    for(int i=0;i<npart;++i) {
        if(m_meshes.size() < i + 1)
            addMeshReformPair();

        if(!hasResource(i)) {
            DrawableResource *rec = createResource();
            setResource(rec, i);
        }
    }

    boost::chrono::system_clock::time_point t0 = boost::chrono::system_clock::now();

    HistoryMesh transient[4];

    boost::thread tref[4];
    int ntref = 0;

    int meshCount = 0;
    BVHNodeIterator it = fis.firstPart();
    while(it._node) {
        HistoryMesh *tmesh = &transient[ntref];
        fis.reformPart(tmesh, it, &srcMesh);

        MeshReformPair &p = m_meshes[meshCount];
        DrawableResource *rec = resource(meshCount);

        //SimplifyAndReform(p, &transient, rec);
        tref[ntref] = boost::thread(boost::bind(&PVSTest::SimplifyAndReform, _1, _2, _3), p, tmesh, rec);
        ntref++;

        if(ntref==4) {
            for(int i=0;i<ntref;++i)
                tref[i].join();
            ntref = 0;
        }

        meshCount++;
        it = fis.nextPart(it);
    }

    if(ntref>0) {
        for(int i=0;i<ntref;++i)
            tref[i].join();
    }

    int nv1 =0;
    int nt1 =0;
    for(int i=0;i<npart;++i) {
        nv1 += m_meshes[i]._outMesh->numVertices();
        nt1 += m_meshes[i]._outMesh->numTriangles();
    }
    const int nv0 = srcMesh.numVertices();
    const int nt0 = srcMesh.numTriangles();
    const float nvPercent = (float)(nv0 - nv1) / (float)nv0 * 100.f;
    const float ntPercent = (float)(nt0 - nt1) / (float)nt0 * 100.f;
    std::cout << "\n n vertex "<<nv1<<":"<<nv0<<" reduced by " << nvPercent << " percent "
        << "\n n face "<<nt1<<":"<<nt0<<" reduced by " << ntPercent << " percent ";

    boost::chrono::system_clock::time_point t1 = boost::chrono::system_clock::now();

    boost::chrono::duration<double> sec = t1 - t0;
    std::cout << "\n finished in " << sec.count() << " seconds ";

    m_details->create(npart);
    m_details->set(true, 0.f);
}

void PVSTest::addMeshReformPair()
{
    m_meshes.push_back(MeshReformPair());
    MeshReformPair &p = m_meshes.back();
    p._outMesh = new HistoryMesh;
    p._stageMesh = new HistoryMesh;
    p._stageMesh->createTriangleMesh(1000, 1000);
    p._stageMesh->addHistoryStage();
}

void PVSTest::SimplifyAndReform(MeshReformPair &p, HistoryMesh *srcMesh, DrawableResource *rec)
{
    EdgeCollapse ech;
    ech.simplify(srcMesh);

    HistoryReformSrc reformer;
    reformer.reformSrc(p._outMesh, p._stageMesh, 0.f, srcMesh);

    UpdateMeshResouce(rec, p._outMesh);
}

void PVSTest::recvCameraChanged(const CameraEvent &x)
{
    if(m_freeze) return;
    m_culler->compare(m_details->visibilities(), *(x.frustum()));
    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        rec->setVisible(m_details->c_visibilities()[i]);
        processResource(rec);
    }
}

}
