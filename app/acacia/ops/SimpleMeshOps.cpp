/*
 *  SimpleMeshOps.cpp
 *  acacia
 *
 */

#include "SimpleMeshOps.h"
#include <QListWidget>
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
   
namespace alo {

SimpleMeshOps::SimpleMeshOps() :
m_shoUV(false)
{}

SimpleMeshOps::~SimpleMeshOps()
{}
    
void SimpleMeshOps::update()
{
    bool dataChanged;
    if(checkMeshChange(dataChanged) ) {
        AdaptableMesh transient;
        loadMesh(&transient, dataChanged);
/// todo one mesh
    }
        
    getBoolAttribValue(m_shoUV, "sho_uv");

    computeMesh();
}

void SimpleMeshOps::computeMesh()
{
    const int n = numResources();
    if(n<1) return;
    
    //simpleReform(m_lod, m_shoUV);
    
    drawableScene()->lock();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        processResourceNoLock(rec);
    }
    drawableScene()->unlock();
}

}
