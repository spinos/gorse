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
    m_mesh = new AdaptableMesh;
    m_sourceMesh = new HistoryMesh;
    m_stageMesh = new HistoryMesh;
    m_stageMesh->createTriangleMesh(1000, 1000);
    m_stageMesh->addHistoryStage();
    m_reformer = new HistoryReformSrc;
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
    
    EdgeCollapse ech;

    boost::chrono::system_clock::time_point t0 = boost::chrono::system_clock::now();

    ech.simplify(m_sourceMesh);

    boost::chrono::system_clock::time_point t1 = boost::chrono::system_clock::now();

    boost::chrono::duration<double> sec = t1 - t0;
    std::cout << "\n finished in " << sec.count() << " seconds ";
    
    m_lod = .5f;

    DrawableResource *rec = createResource();
    setResource(rec);
}

EdgeCollapseTest::~EdgeCollapseTest()
{ 
    delete m_mesh; 
    delete m_sourceMesh;
    delete m_stageMesh;
    delete m_reformer;
}
    
void EdgeCollapseTest::update()
{
    QAttrib * al = findAttrib("lod");
    FloatAttrib *fl = static_cast<FloatAttrib *>(al);
    fl->getValue(m_lod);
    
    computeMesh();

    DrawableScene *scene = drawableScene();
    DrawableObject *d = drawable();
    const DrawableResource *rec = resource();
    if(rec->toRelocate()) {
        scene->enqueueRemoveDrawable(d->drawId(), opsId());
        DrawableObject *d1 = setMeshDrawable(m_mesh, rec);
        setDrawable(d1);
        scene->lock();
        scene->enqueueCreateDrawable(d1, opsId());
        scene->unlock();

    } else {
        updateDrawableResource(d, rec, m_mesh->numIndices());
        scene->enqueueEditDrawable(d->drawId(), opsId());
    }
}

void EdgeCollapseTest::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    const DrawableResource *rec = resource();
    DrawableObject *d = setMeshDrawable(m_mesh, rec);
    setDrawable(d);
    scene->lock();
    scene->enqueueCreateDrawable(d, opsId());
    scene->unlock();
}

void EdgeCollapseTest::computeMesh()
{
    m_reformer->reformSrc(m_mesh, m_stageMesh, m_lod, m_sourceMesh);

    DrawableResource *rec = resource();
    const int oldL = rec->size();
    m_mesh->createPositionNormalArray(rec->posnmlBuffer());
    if(oldL < rec->size()) {
        rec->setToRelocate(true);
        m_mesh->createBarycentricCoordinates(rec->barycBuffer());
    } else
        rec->setToRelocate(false);
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

