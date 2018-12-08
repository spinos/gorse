/*
 *  GranulateReduce.cpp
 *  acacia
 *
 */
 
#include "GranulateReduce.h"
#include <qt_ogl/DrawableResource.h>
#include <mesh/HistoryMesh.h>
#include <mesh/Fissure.h>
#include <mesh/EdgeCollapse.h>
#include <mesh/HistoryReformSrc.h>
#include <bvh/BVHNodeIterator.h>
#include <cull/ViewFrustumCull.h>
#include <cull/VisibleDetail.h>
#include <math/PerspectiveCamera.h>
#include <boost/chrono/include.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <h5_mesh/HHistoryMesh.h>
#include <boost/format.hpp>

namespace alo {

AFileDlgProfile GranulateReduce::SWriteProfile(AFileDlgProfile::FWrite,
        "Choose File To Save",
        ":images/save_big.png",
        "Save mesh",
        "Save .hes",
        ".hes",
        "./",
        "untitled");
    
GranulateReduce::GranulateReduce()
{}

GranulateReduce::~GranulateReduce()
{
    std::vector<MeshReformTrio>::iterator it = m_meshes.begin();
    for(;it!=m_meshes.end();++it) {
        delete it->_outMesh;
        delete it->_stageMesh;
        delete it->_srcMesh;
    }
}

int GranulateReduce::numMeshTrios() const
{ return m_meshes.size(); }

GranulateReduce::MeshReformTrio &GranulateReduce::meshTrio(int i)
{ return m_meshes[i]; }

void GranulateReduce::addMeshReformTrio()
{
    m_meshes.push_back(MeshReformTrio());
    MeshReformTrio &p = m_meshes.back();
    p._outMesh = new AdaptableMesh;
    p._stageMesh = new HistoryMesh;
    p._stageMesh->createTriangleMesh(1000, 1000);
    p._stageMesh->addHistoryStage();
    p._srcMesh = new HistoryMesh;
}

const int &GranulateReduce::outMeshNv(int i) const
{ return m_meshes[i]._outMesh->numVertices(); }

int GranulateReduce::reduce(ViewFrustumCull *culler, const AdaptableMesh *srcMesh)
{
    Fissure fis;
    const int npart = fis.granulate(srcMesh);
    std::cout << "\n granulate to " << npart << " parts ";
            
    if(culler) culler->create(fis.bvh());

    for(int i=0;i<npart;++i) {
        if(numMeshTrios() < i + 1)
            addMeshReformTrio();

        if(!hasResource(i)) {
            DrawableResource *rec = createResource();
            setResource(rec, i);
        }
    }

    boost::chrono::system_clock::time_point t0 = boost::chrono::system_clock::now();

    boost::thread tref[8];
    int ntref = 0;

    int meshCount = 0;
    BVHNodeIterator it = fis.firstPart();
    while(it._node) {
        
        MeshReformTrio &p = meshTrio(meshCount);
        fis.reformPart(p._srcMesh, it, srcMesh);

        DrawableResource *rec = resource(meshCount);

        tref[ntref] = boost::thread(boost::bind(&GranulateReduce::SimplifyAndReform, _1, _2), p, rec);
        ntref++;

        if(ntref==8) {
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

    m_nv1 =0;
    m_nt1 =0;
    for(int i=0;i<npart;++i) {
        m_nv1 += m_meshes[i]._outMesh->numVertices();
        m_nt1 += m_meshes[i]._outMesh->numTriangles();
    }
    m_nv0 = srcMesh->numVertices();
    m_nt0 = srcMesh->numTriangles();
    const float nvPercent = (float)(m_nv0 - m_nv1) / (float)m_nv0 * 100.f;
    const float ntPercent = (float)(m_nt0 - m_nt1) / (float)m_nt0 * 100.f;
    std::cout << "\n n vertex ["<<m_nv1<<":"<<m_nv0<<"] reduced by " << nvPercent << " percent "
        << "\n n face ["<<m_nt1<<":"<<m_nt0<<"] reduced by " << ntPercent << " percent ";

    boost::chrono::system_clock::time_point t1 = boost::chrono::system_clock::now();

    boost::chrono::duration<double> sec = t1 - t0;
    std::cout << "\n finished in " << sec.count() << " seconds ";
    return npart;
}

void GranulateReduce::viewDependentReform(const PerspectiveCamera *persp,
                              const ViewFrustumCull *culler, VisibleDetail *details)
{
    boost::thread tref[8];
    int ntref = 0;
    
    const int beforeFrame = frameNumber() - 2;
    const int n = numResources();
    for(int i=0;i<n;++i) {
        const VisibilityState &vis = details->c_visibilities()[i];
        DrawableResource *rec = resource(i);

        if(vis.isVisible() && rec->changedOnFrame() < beforeFrame) {
            LevelOfDetailSelect &lod = details->levelOfDetails()[i];

            MeshReformTrio &p = meshTrio(i);
            tref[ntref] = boost::thread(boost::bind(&GranulateReduce::LodReform, _1, _2, _3, _4, _5), lod, culler->leafHexahedron(i), *persp, p, rec);
            ntref++;
        } 

        if(ntref==8) {
            for(int i=0;i<ntref;++i)
                tref[i].join();
            ntref = 0;
        } 
    }

    if(ntref>0) {
        for(int i=0;i<ntref;++i)
            tref[i].join();
    }
}

void GranulateReduce::simpleReform(const float &lod, bool shoAsUV)
{
    boost::thread tref[8];
    int ntref = 0;
    
    const int beforeFrame = frameNumber() - 1;
    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        
        if(rec->changedOnFrame() < beforeFrame) {
            MeshReformTrio &p = meshTrio(i);
            tref[ntref] = boost::thread(boost::bind(&GranulateReduce::LodReform1, _1, _2, _3, _4), lod, shoAsUV, p, rec);
            ntref++;
        }

        if(ntref==8) {
            for(int i=0;i<ntref;++i)
                tref[i].join();
            ntref = 0;
        } 
    }

    if(ntref>0) {
        for(int i=0;i<ntref;++i)
            tref[i].join();
    }
}

void GranulateReduce::SimplifyAndReform(MeshReformTrio &p, DrawableResource *rec)
{
    EdgeCollapse ech;
    ech.simplify(p._srcMesh);
    Reform(p, 0, rec);
}

void GranulateReduce::Reform(MeshReformTrio &p, int nt, DrawableResource *rec)
{
    HistoryReformSrc reformer;
    reformer.reformSrc1(p._outMesh, p._stageMesh, nt, p._srcMesh);
    UpdateMeshResouce(rec, p._outMesh);
}

void GranulateReduce::LodReform(LevelOfDetailSelect &lod, const Hexahedron &hexa, const PerspectiveCamera &camera,
                MeshReformTrio &p, DrawableResource *rec)
{
    lod.select(hexa, camera);
    if(lod.isStateChanged())
        Reform(p, lod.value(), rec);
}

void GranulateReduce::LodReform1(const float &lod, bool shoAsUV,
                MeshReformTrio &p, DrawableResource *rec)
{
    HistoryReformSrc reformer;
    reformer.reformSrc(p._outMesh, p._stageMesh, lod, p._srcMesh);
    UpdateMeshResouce(rec, p._outMesh, shoAsUV);
}

bool GranulateReduce::hasMenu() const
{ return numMeshTrios() > 0; }

void GranulateReduce::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const 
{
    ks.push_back(std::make_pair("Save", AFileDlgProfile::FWrite));
}

void GranulateReduce::recvAction(int x) 
{
    if(x == AFileDlgProfile::FWrite) saveToFile(SWriteProfile.getFilePath());
}

AFileDlgProfile *GranulateReduce::writeFileProfileR () const
{
    SWriteProfile._notice = boost::str(boost::format("level-of-detail mesh cache \n name %1%\n n vertices [%2%:%3%] \n n faces [%4%:%5%] ") 
        % meshCacheName() % m_nv1 % m_nv0 % m_nt1 % m_nt0 );
    return &SWriteProfile; 
}

bool GranulateReduce::saveToFile(const std::string &fileName)
{
    ver1::H5IO hio;
    bool stat = hio.begin(fileName, HDocument::oCreate);
    if(!stat) return false;

    ver1::HBase w("/world");
    ver1::HBase b("/world/meshes");

    const std::string mcName = meshCacheName();
    const int n = numMeshTrios();
    for(int i=0;i<n;++i) {
        std::cout << ".";
        const std::string mciName = boost::str(boost::format("/world/meshes/%1%_part%2%") % mcName % i);
        HHistoryMesh hmh(mciName);
        
        const MeshReformTrio &p = meshTrio(i);
        hmh.save(p._srcMesh);
        hmh.close();
    }

    b.close();
    w.close();

    hio.end();
    std::cout << " finished saving file " << fileName;
    return true; 
}

std::string GranulateReduce::meshCacheName() const
{ return "test"; }

}
