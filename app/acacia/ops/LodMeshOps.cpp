/*
 *  LodMeshOps.cpp
 *  acacia
 *
 */

#include "LodMeshOps.h"
#include <QListWidget>
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
   
namespace alo {

LodMeshOps::LodMeshOps() :
m_lod(.5f),
m_shoUV(false)
{}

LodMeshOps::~LodMeshOps()
{}
    
void LodMeshOps::update()
{
    bool dataChanged;
    if(checkMeshChange(dataChanged) ) {
        AdaptableMesh transient;
        loadMesh(&transient, dataChanged);
        reduce(0, 0, &transient);
    }
        
    getFloatAttribValue(m_lod, "lod");
    getBoolAttribValue(m_shoUV, "sho_uv");

    computeMesh();
}

void LodMeshOps::computeMesh()
{
    const int n = numResources();
    if(n<1) return;
    
    simpleReform(m_lod, m_shoUV);
    
    drawableScene()->lock();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        processResourceNoLock(rec);
    }
    drawableScene()->unlock();
}

}
