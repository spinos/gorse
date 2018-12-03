#include "LodMeshIn.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <mesh/HistoryMesh.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <h5_mesh/HHistoryMesh.h>

namespace alo {
   
LodMeshIn::LodMeshIn() :
m_lod(.5f),
m_shoUV(false)
{
    m_mesh = new AdaptableMesh;
    DrawableResource *rec = createResource();
    setResource(rec);
}

LodMeshIn::~LodMeshIn()
{ 
    delete m_mesh; 
}
    
void LodMeshIn::update()
{
    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    
    if(cachePathChanged(scachePath) )
        loadCache(scachePath);
    
    QAttrib * al = findAttrib("lod");
    FloatAttrib *fl = static_cast<FloatAttrib *>(al);
    fl->getValue(m_lod);
    
    QAttrib * ashouv = findAttrib("sho_uv");
    BoolAttrib *fshouv = static_cast<BoolAttrib *>(ashouv);
    fshouv->getValue(m_shoUV);
    
    computeMesh();

    DrawableScene *scene = drawableScene();
    
    DrawableResource *rec = resource();
    processResource(rec);
}

void LodMeshIn::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    DrawableResource *rec = resource();
    initiateResource(rec);
}

void LodMeshIn::computeMesh()
{
    if(LodMeshCache::isValid()) {
        int istage, nv;
        selectStage(istage, nv, m_lod);
        reformStage(m_mesh, nv, istage);

    }
    else
        m_mesh->createMinimal();
    
    DrawableResource *rec = resource();
    updateMeshResouce(rec, m_mesh, m_shoUV);
}

}
