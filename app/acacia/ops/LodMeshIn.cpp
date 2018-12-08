#include "LodMeshIn.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <mesh/HistoryMesh.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <h5_mesh/HHistoryMesh.h>
#include <h5_mesh/LodMeshCache.h>

namespace alo {
   
LodMeshIn::LodMeshIn() :
m_lod(.5f),
m_shoUV(false)
{}

LodMeshIn::~LodMeshIn()
{ 
    std::vector<LodMeshCache *>::iterator it = m_cacheList.begin();
    for(;it!=m_cacheList.end();++it)
        delete *it;
    m_cacheList.clear(); 
}
    
void LodMeshIn::update()
{
    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    
    if(cachePathChanged(scachePath) )
        LodMeshCache::Load(m_cacheList, scachePath);
    
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
    const int n = m_cacheList.size();
    if(n<1) return;
    
    for(int i=0;i<n;++i) {
        if(!hasResource(i)) {
            DrawableResource *rec = createResource();
            setResource(rec, i);
        }
    }
    
    ver1::H5IO hio;
    bool stat = hio.begin(m_cacheList[0]->cacheFilePath());
    if(!stat) return;
    
    AdaptableMesh transient;
    for(int i=0;i<n;++i) {
        LodMeshCache &ci = *m_cacheList[i];
        if(!ci.isValid()) continue;
            
        int istage, nv;
        ci.selectStage(istage, nv, m_lod);
        ci.reformStage(&transient, nv, istage);

        DrawableResource *rec = resource(i);
        UpdateMeshResouce(rec, &transient, m_shoUV);
    }
    
    hio.end();
    
    drawableScene()->lock();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        processResourceNoLock(rec);
    }
    drawableScene()->unlock();
    
}

bool LodMeshIn::cachePathChanged(const std::string &x) const
{
    if(m_cacheList.size() < 1) return true;
    if(x == m_cacheList[0]->cacheFilePath()) return false;
    return true;
}

}
