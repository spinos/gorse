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
#include <qt_ogl/CameraEvent.h>
#include <math/PerspectiveCamera.h>
#include <cull/ViewFrustumCull.h>
#include <cull/VisibleDetail.h>
#include <cull/LevelOfDetailSelect.h>
#include <bvh/BVHPrimitive.h>
#include <boost/bind.hpp>

namespace alo {
   
LodMeshInOps::LodMeshInOps() :
m_lod(.5f),
m_shoUV(false)
{}

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
    
    QAttrib * al = findAttrib("lod");
    FloatAttrib *fl = static_cast<FloatAttrib *>(al);
    fl->getValue(m_lod);
    
    QAttrib * ashouv = findAttrib("sho_uv");
    BoolAttrib *fshouv = static_cast<BoolAttrib *>(ashouv);
    fshouv->getValue(m_shoUV);
    
    bool fz;
    getBoolAttribValue(fz, "freeze_view");
    setFreezeView(fz);

    if(fz) computeMesh();
}

void LodMeshInOps::addDrawableTo(DrawableScene *scene)
{ 
    setDrawableScene(scene);
}

void LodMeshInOps::computeMesh()
{
    const int n = m_cache.numMeshes();
    if(n<1) return;

    setDrawableSize(n);
    
    boost::thread tref[16];
    int ntref = 0;
    AdaptableMesh transient[16];
    for(int i=0;i<n;++i) {
        LodMeshCache &ci = *m_cache.mesh(i);
        if(!ci.isValid()) continue;
   
        DrawableResource *rec = resource(i);
        
        tref[ntref] = boost::thread(boost::bind( &LodMeshInOps::reformMesh, this, _1, _2, _3 ), 
                            &ci, &transient[ntref], rec);
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

void LodMeshInOps::reformMesh(LodMeshCache *c, AdaptableMesh *mesh, DrawableResource *rec)
{
    int istage, nv;
    c->selectStage(istage, nv, m_lod);
    if(c->stageChanged(istage)) {
        m_mtx.lock();
        c->loadStage(istage);
        m_mtx.unlock();
    }
    c->reformInCore(mesh, nv, istage);

    UpdateMeshResouce(rec, mesh, m_shoUV);
}

bool LodMeshInOps::loadCache(const std::string &fileName)
{
    if(m_cache.cacheFilePath() != "unknown")
        m_hio.end();

    bool stat = m_hio.begin(fileName);
    if(!stat) return false;

    if(!m_cache.loadMeshes(fileName) ) return false;

    const int n = m_cache.numMeshes();

    clearBvh();
    BoundingBox box;
    BVHPrimitive ap;
    for(int i=0;i<n;++i) {
        m_cache.getMeshAabb(box, i);

        ap.setAABB(box);
        ap.setIndex(i);

        addBvhPrimitive(ap);
    }
    buildBvh();

    initializeDetails();

    return true;
}

void LodMeshInOps::recvCameraChanged(const CameraEvent &x)
{
    if(m_cache.numMeshes() < 1) return;
    if(freezeView()) return;
    if(x.progressBegin() || x.progressEnd()) return;
    if(!updateView(*x.camera(), *x.frustum())) return;
    
    const PerspectiveCamera *persp = static_cast<const PerspectiveCamera *>(x.camera());
    lockScene();
    viewDependentReform(persp);

    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        const VisibilityState &vis = visibility(i);
        processResource(rec, vis);
    }
    unlockScene();
}

void LodMeshInOps::viewDependentReform(const PerspectiveCamera *persp)
{
    const int n = m_cache.numMeshes();
   
    boost::thread tref[16];
    int ntref = 0;
    AdaptableMesh transient[16];
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

        lod.select(culler()->primitiveHexahedron(i), *persp);

        DrawableResource *rec = resource(i);
        if(rec->changedOnFrame() > beforeFrame) {
            rec->setDeferred(true);
            lod.revert();
            continue;
        }
      
        tref[ntref] = boost::thread(boost::bind( &LodMeshInOps::reformMesh1, this, _1, _2, _3, _4, _5 ), 
                            &ci, &transient[ntref], lod.value(), lod.isIncreased(), rec);
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

void LodMeshInOps::reformMesh1(LodMeshCache *c, AdaptableMesh *mesh, int lodNv, bool forcedUpdate, DrawableResource *rec)
{
    if(rec->drawArrayLength() == c->maxNumTriangles() * 3 && forcedUpdate) return;
    if(rec->drawArrayLength() == c->minNumTriangles() * 3 && !forcedUpdate) return;
    
    int istage, nv;
    c->selectStageByNv(istage, nv, lodNv);
    if(!c->nvChanged(nv) && !forcedUpdate) return;
    if(c->stageChanged(istage)) {
        m_mtx.lock();
        c->loadStage(istage);
        m_mtx.unlock();
    }
    c->reformInCore(mesh, nv, istage);
    UpdateMeshResouce(rec, mesh);
}

}
