#include "LodMeshIn.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5_mesh/LodMeshCache.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace alo {
   
LodMeshIn::LodMeshIn() :
m_lod(.5f),
m_shoUV(false)
{}

LodMeshIn::~LodMeshIn()
{}
    
void LodMeshIn::update()
{
    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    
    if(m_cache.cacheFilePath() != scachePath )
        m_cache.load(scachePath);
    
    QAttrib * al = findAttrib("lod");
    FloatAttrib *fl = static_cast<FloatAttrib *>(al);
    fl->getValue(m_lod);
    
    QAttrib * ashouv = findAttrib("sho_uv");
    BoolAttrib *fshouv = static_cast<BoolAttrib *>(ashouv);
    fshouv->getValue(m_shoUV);
    
    computeMesh();
}

void LodMeshIn::addDrawableTo(DrawableScene *scene)
{ 
    setDrawableScene(scene);
}

void LodMeshIn::computeMesh()
{
    const int n = m_cache.numMeshes();
    if(n<1) return;

    setDrawableSize(n);
    
    ver1::H5IO hio;
    bool stat = hio.begin(m_cache.cacheFilePath());
    if(!stat) return;
    
    boost::thread tref[16];
    int ntref = 0;
    AdaptableMesh transient[16];
    for(int i=0;i<n;++i) {
        LodMeshCache &ci = *m_cache.mesh(i);
        if(!ci.isValid()) continue;
   
        DrawableResource *rec = resource(i);
        
        tref[ntref] = boost::thread(boost::bind( &LodMeshIn::reformMesh, this, _1, _2, _3 ), 
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
    
    hio.end();
    
    drawableScene()->lock();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        processResourceNoLock(rec);
    }
    drawableScene()->unlock();
    
}

void LodMeshIn::reformMesh(LodMeshCache *c, AdaptableMesh *mesh, DrawableResource *rec)
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

}
