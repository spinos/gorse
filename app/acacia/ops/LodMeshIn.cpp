#include "LodMeshIn.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <mesh/HistoryMesh.h>
#include <mesh/HistoryReform.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <h5_mesh/HHistoryMesh.h>

namespace alo {
   
LodMeshIn::LodMeshIn() :
m_lod(.5f)
{
    m_mesh = new AdaptableMesh;
    m_reformer = new HistoryReform;
}

LodMeshIn::~LodMeshIn()
{ 
    delete m_mesh; 
    delete m_reformer;
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
    
    computeMesh();

    DrawableScene *scene = drawableScene();
    DrawableObject *d = drawable();
    if(m_toRelocate) {
        scene->enqueueRemoveDrawable(d);
        setMeshDrawable(scene);
    } else {
        d->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
        d->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
        d->setDrawArrayLength(m_mesh->numIndices());
        scene->enqueueEditDrawable(d);
    }
}

void LodMeshIn::setMeshDrawable(DrawableScene *scene)
{
    DrawableObject *cly = scene->createDrawable();
    cly->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
    cly->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
    cly->setDrawArrayLength(m_mesh->numIndices());
    setDrawable(cly);
}

void LodMeshIn::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    setMeshDrawable(scene);
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
    
    const int oldL = posnml.capacity();
    m_mesh->createPositionNormalArray(posnml);
    m_toRelocate = oldL < posnml.capacity();
    if(m_toRelocate) m_mesh->createBarycentricCoordinates(baryc);
}

}
