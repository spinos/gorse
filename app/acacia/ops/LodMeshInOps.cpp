/*
 *  LodMeshInOps.cpp
 *  acacia
 *
 */

#include "LodMeshInOps.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <qt_base/AFileDlg.h>
#include <h5_mesh/LodMeshCache.h>
#include <math/CameraEvent.h>
#include <math/PerspectiveCamera.h>
#include <cull/ViewFrustumCull.h>
#include <cull/VisibleDetail.h>
#include <cull/LevelOfDetailSelect.h>
#include <bvh/BVHPrimitive.h>
#include <boost/bind.hpp>

namespace alo {
    
AFileDlgProfile LodMeshInOps::SReadProfile(AFileDlgProfile::FRead,
        "Choose File To Open",
        ":images/open_big.png",
        "Open LOD Mesh Cache File",
        "Open .hes",
        ".hes",
        "./",
        "");
   
LodMeshInOps::LodMeshInOps() :
m_lod(.5f),
m_shoUV(false)
{
    m_aabb.set(Vector3F::Zero, 1.f);
    setBound(m_aabb);
}

LodMeshInOps::~LodMeshInOps()
{
    if(m_cache.cacheFilePath() != "unknown")
        m_hio.end();
}
    
void LodMeshInOps::update()
{
    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    
    if(m_cache.cacheFilePath() != scachePath )
        loadCache(scachePath);
    
    getFloatAttribValue(m_lod, "lod");
    getBoolAttribValue(m_shoUV, "sho_uv");
    
    bool fz;
    getBoolAttribValue(fz, "freeze_view");
    setFreezeView(fz);

    if(fz) computeMesh();

    if(m_shoUV)
        setBound(BoundingBox(-.5f, -.5f, -.5f, 1.5f, 1.5f, 1.f));
    else
        setBound(m_aabb);
}

void LodMeshInOps::addDrawableTo(DrawableScene *scene)
{ 
    setDrawableScene(scene);
}

void LodMeshInOps::computeMesh()
{
    const int n = m_cache.numMeshes();
    if(n<1) return;
    
    boost::thread tref[16];
    int ntref = 0;
    for(int i=0;i<n;++i) {
        LodMeshCache &ci = *m_cache.mesh(i);
        if(!ci.isValid()) continue;
   
        DrawableResource *rec = resource(i);
        
        tref[ntref] = boost::thread(boost::bind( &LodMeshInOps::reformMesh, this, _1, _2 ), 
                            &ci, rec);
        ntref++;
        
        if(ntref==16) {
            for(int j=0;j<ntref;++j)
                tref[j].join();
            ntref = 0;
        }
    }

    if(ntref>0) {
        for(int i=0;i<ntref;++i)
            tref[i].join();
    }
    
    lockScene();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        processResourceNoLock(rec);
    }
    unlockScene();
    
}

void LodMeshInOps::reformMesh(LodMeshCache *c, DrawableResource *rec)
{
    int nv;
    int istage = c->selectStage(nv, m_lod);
    if(!c->switchToStage(istage)) {
        m_mtx.lock();
        c->loadStage(istage);
        m_mtx.unlock();
        c->sortCurrentStage();
    }
    c->reformInCore(nv);

    UpdateMeshResouce(rec, c->c_outMesh(), m_shoUV);
}

bool LodMeshInOps::loadCache(const std::string &fileName)
{
    if(m_cache.cacheFilePath() != "unknown")
        m_hio.end();

    bool stat = m_hio.begin(fileName);
    if(!stat) return false;

    if(!m_cache.loadMeshes(fileName) ) return false;

    const int n = m_cache.numMeshes();

    setDrawableSize(n);

    clearBvh();
    BoundingBox box;
    BVHPrimitive ap;
    for(int i=0;i<n;++i) {
        m_cache.getMeshAabb(box, i);

        ap.setBBox(box);
        ap.setIndex(i);

        addBvhPrimitive(ap);
    }
    buildBvh();

    initializeDetails();

    m_aabb = bvhAabb();
    setBound(m_aabb);

    return true;
}

void LodMeshInOps::recvCameraChanged(const CameraEvent &x)
{
    if(!drawableVisible()) return;
    if(m_cache.numMeshes() < 1) return;
    if(freezeView()) return;
    if(x.progressBegin() || x.progressEnd()) return;
    
    const PerspectiveCamera *persp = static_cast<const PerspectiveCamera *>(x.camera());
    if(!updateView(*persp, *x.frustum())) return;
    
    viewDependentReform();

    lockScene();
    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        const VisibilityState &vis = visibility(i);
        processResource(rec, vis);
    }
    unlockScene();
}

void LodMeshInOps::viewDependentReform()
{
    const int n = m_cache.numMeshes();
   
    boost::thread tref[16];
    int ntref = 0;
    const int beforeFrame = frameNumber() - 3;
    for(int i=0;i<n;++i) {
        LodMeshCache &ci = *m_cache.mesh(i);
        if(!ci.isValid()) continue;

        LevelOfDetailSelect &lod = details()->levelOfDetails()[i];
        const VisibilityState &vis = details()->c_visibilities()[i];
        if(!vis.isVisible()) {
            lod.select(0);
            continue;
        }

        lod.select(culler()->primitiveHexahedron(i), details()->param() );

        DrawableResource *rec = resource(i);
        if(rec->changedOnFrame() > beforeFrame) {
            rec->setDeferred(true);
            lod.revert();
            continue;
        }
      
        tref[ntref] = boost::thread(boost::bind( &LodMeshInOps::reformMesh1, this, _1, _2, _3, _4 ), 
                            &ci, lod.value(), lod.isIncreased(), rec);
        ntref++;
        
        if(ntref==16) {
            for(int j=0;j<ntref;++j)
                tref[j].join();
            ntref = 0;
        }
    }

    if(ntref>0) {
        for(int i=0;i<ntref;++i)
            tref[i].join();
    }
}

void LodMeshInOps::reformMesh1(LodMeshCache *c, int lodNv, bool forcedUpdate, DrawableResource *rec)
{
    if(rec->drawArrayLength() == c->maxNumTriangles() * 3 && forcedUpdate) return;
    if(rec->drawArrayLength() == c->minNumTriangles() * 3 && !forcedUpdate) return;
    
    int nv = lodNv;
    int istage = c->selectStageByNv(nv);
    if(c->isMeshCached(nv) ) return;
    if(!c->switchToStage(istage)) {
        m_mtx.lock();
        c->loadStage(istage);
        m_mtx.unlock();
        c->sortCurrentStage();
    }
    c->reformInCore(nv);
    UpdateMeshResouce(rec, c->c_outMesh() );
}

AFileDlgProfile *LodMeshInOps::readFileProfileR () const
{ return &SReadProfile; }

}
