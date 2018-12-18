#include "EdgeCollapseTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <mesh/FrontMesher.h>
#include <math/wavefuncs.h>
#include <mesh/EdgeCollapse.h>
#include <mesh/HistoryMesh.h>
#include <mesh/HistoryReformSrc.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <h5_mesh/HHistoryMesh.h>
#include <boost/format.hpp>
#include <boost/chrono/include.hpp>
#include <ctime>
#include <geom/GeodesicSphere.h>

namespace alo {

AFileDlgProfile EdgeCollapseTest::SWriteProfile(AFileDlgProfile::FWrite,
        "Choose File To Save",
        ":images/save_big.png",
        "Save mesh",
        "Save .hes",
        ".hes",
        "./",
        "untitled");
   
EdgeCollapseTest::EdgeCollapseTest()
{
    m_sourceMesh = new HistoryMesh;
    m_stageMesh = new HistoryMesh;
    m_stageMesh->createTriangleMesh(1000, 1000);
    m_stageMesh->addHistoryStage();

#if 0
    const int nu = 125;
    const float du = 39.f / (float)nu;
    for(int i=0;i<nu;++i) {
        float phi = du * i;
        m_sourceMesh->addVertex(Vector3F(-167.f + 7.93f * i + RandomFn11(), -40.f + 19.f * cos(phi), -30.f - 19.f * sin(phi) ));
    }
    
    FrontLine originLine;
    FrontLine::EnableMerging = false;
    
    for(int i=0;i<nu;++i)
        originLine.addVertex(m_sourceMesh->vertexPositionR(i), i);
    
    Vector3F up(0.f, 13.3f, 0.2f);
    const Quaternion lq(-.0472f, Vector3F::ZAxis);
    const Quaternion tq(.093f, Vector3F::YAxis);
    constexpr float cshrinking = .0f;
    originLine.setWorldSpace(Matrix33F::IdentityMatrix);
    originLine.rotateLocalBy(lq);
    originLine.rotateLocalBy(tq);
    originLine.setShrinking(cshrinking);
    originLine.setDirection(up);
    originLine.setMinEdgeLength(.2f);
    
    FrontLine *la = &originLine;
    FrontLine l[143];

    FrontMesher msher;
    msher.attachMesh(m_sourceMesh);

    for(int i=0;i<143;++i) {
        msher.setFrontId(i+1);

        l[i].rotateLocalBy(lq);
        l[i].rotateLocalBy(tq);
        l[i].setShrinking(cshrinking);
        up.y += RandomFn11() * .99f;
        up.z += RandomFn11() * .99f;
        l[i].setDirection(up);
        l[i].setMinEdgeLength(.2f);

        msher.advanceFront(l[i], *la);

        la = &l[i];
    }

    m_sourceMesh->calculateVertexNormals();
#else
    GeodesicSphere sphere(70);
    sphere.scaleBy(18.f);
    sphere.copyMeshTo(m_sourceMesh, sphere.numVertices(), sphere.numTriangles());
#endif
    
    EdgeCollapse ech;

    boost::chrono::system_clock::time_point t0 = boost::chrono::system_clock::now();

    const int nv0 = m_sourceMesh->numVertices();
    const int nt0 = m_sourceMesh->numTriangles();
    ech.simplify(m_sourceMesh);

    for(int i=0;i<m_sourceMesh->numStages();++i)
        std::cout<<"\n stage"<<i<<m_sourceMesh->stage(i);

    const float nvPercent = (float)(nv0 - m_sourceMesh->numVertices()) / (float)nv0 * 100.f;
    const float ntPercent = (float)(nt0 - m_sourceMesh->numTriangles()) / (float)nt0 * 100.f;
    std::cout << "\n n vertex "<<m_sourceMesh->numVertices()<<":"<<nv0<<" reduced by " << nvPercent << " percent "
        << "\n n face "<<m_sourceMesh->numTriangles()<<":"<<nt0<<" reduced by " << ntPercent << " percent ";

    boost::chrono::system_clock::time_point t1 = boost::chrono::system_clock::now();

    boost::chrono::duration<double> sec = t1 - t0;
    std::cout << "\n finished in " << sec.count() << " seconds ";
    
    m_lod = .5f;

    DrawableResource *rec = createResource();
    setResource(rec);
}

EdgeCollapseTest::~EdgeCollapseTest()
{  
    delete m_sourceMesh;
    delete m_stageMesh;
}
    
void EdgeCollapseTest::update()
{
    getFloatAttribValue(m_lod, "lod");
    
    computeMesh();
}

void EdgeCollapseTest::addDrawableTo(DrawableScene *scene)
{ 
    setDrawableScene(scene);
    computeMesh();
}

void EdgeCollapseTest::computeMesh()
{
    AdaptableMesh transient;
    HistoryReformSrc reformer;
    reformer.reformSrc(&transient, m_stageMesh, m_lod, m_sourceMesh);

    DrawableResource *rec = resource();

    lockScene();
    const int beforeFrame = frameNumber() - 2;
    if(rec->changedOnFrame() > beforeFrame) {
/// prevent editing unsynchronized resource
        unlockScene();
        return;
    }
    
    UpdateMeshResouce(rec, &transient);
    
    processResourceNoLock(rec);
    unlockScene();
}

bool EdgeCollapseTest::hasMenu() const
{ return true; }

void EdgeCollapseTest::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const 
{
    ks.push_back(std::make_pair("Save", AFileDlgProfile::FWrite));
}

void EdgeCollapseTest::recvAction(int x) 
{
    if(x == AFileDlgProfile::FWrite) saveToFile(SWriteProfile.getFilePath());
}

AFileDlgProfile *EdgeCollapseTest::writeFileProfileR () const
{
    SWriteProfile._notice = boost::str(boost::format("level-of-detail mesh cache \nn vertices [%1%:%2%] \nn faces [%3%:%4%] ") 
        % m_sourceMesh->minNumVertices() % m_sourceMesh->maxNumVertices() 
        % m_sourceMesh->minNumTriangles() % m_sourceMesh->maxNumTriangles() );
    return &SWriteProfile; 
}

bool EdgeCollapseTest::saveToFile(const std::string &fileName)
{
    ver1::H5IO hio;
    bool stat = hio.begin(fileName, HDocument::oCreate);
    if(!stat) return false;

    ver1::HBase w("/world");
    ver1::HBase b("/world/meshes");

    HHistoryMesh hmh("/world/meshes/mesh_1");
    hmh.save(m_sourceMesh);
    hmh.close();

    b.close();
    w.close();

    hio.end();
    return true; 
}

}

