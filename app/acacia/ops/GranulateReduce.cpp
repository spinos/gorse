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

namespace alo {
    
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

    boost::thread tref[12];
    int ntref = 0;

    int meshCount = 0;
    BVHNodeIterator it = fis.firstPart();
    while(it._node) {
        
        MeshReformTrio &p = meshTrio(meshCount);
        fis.reformPart(p._srcMesh, it, srcMesh);

        DrawableResource *rec = resource(meshCount);

        tref[ntref] = boost::thread(boost::bind(&GranulateReduce::SimplifyAndReform, _1, _2), p, rec);
        ntref++;

        if(ntref==12) {
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
    const int nv0 = srcMesh->numVertices();
    const int nt0 = srcMesh->numTriangles();
    const float nvPercent = (float)(nv0 - nv1) / (float)nv0 * 100.f;
    const float ntPercent = (float)(nt0 - nt1) / (float)nt0 * 100.f;
    std::cout << "\n n vertex "<<nv1<<":"<<nv0<<" reduced by " << nvPercent << " percent "
        << "\n n face "<<nt1<<":"<<nt0<<" reduced by " << ntPercent << " percent ";

    boost::chrono::system_clock::time_point t1 = boost::chrono::system_clock::now();

    boost::chrono::duration<double> sec = t1 - t0;
    std::cout << "\n finished in " << sec.count() << " seconds ";
    return npart;
}

void GranulateReduce::viewDependentReform(const PerspectiveCamera *persp,
                              const ViewFrustumCull *culler, VisibleDetail *details)
{
    boost::thread tref[16];
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

        if(ntref==16) {
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
    boost::thread tref[16];
    int ntref = 0;
    
    const int beforeFrame = frameNumber() - 2;
    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        
        if(rec->changedOnFrame() < beforeFrame) {
            MeshReformTrio &p = meshTrio(i);
            tref[ntref] = boost::thread(boost::bind(&GranulateReduce::LodReform1, _1, _2, _3, _4), lod, shoAsUV, p, rec);
            ntref++;
        }

        if(ntref==16) {
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
}

}

