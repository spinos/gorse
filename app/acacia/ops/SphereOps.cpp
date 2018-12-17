#include "SphereOps.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/GeodesicSphere.h>

namespace alo {
   
SphereOps::SphereOps() :
m_level(6),
m_radius(10.f),
m_shoUV(false)
{
    DrawableResource *rec = createResource();
    setResource(rec);
}

SphereOps::~SphereOps()
{}
    
void SphereOps::update()
{
    getIntAttribValue(m_level, "l");
    getFloatAttribValue(m_radius, "r");
    getBoolAttribValue(m_shoUV, "sho_uv");
    computeMesh();

    lockScene();
    DrawableResource *rec = resource();
    processResourceNoLock(rec);
    unlockScene();
}

void SphereOps::addDrawableTo(DrawableScene *scene)
{
    computeMesh();
    setDrawableScene(scene);

    lockScene();
    DrawableResource *rec = resource();
    processResourceNoLock(rec);
    unlockScene();
}

void SphereOps::computeMesh()
{
    GeodesicSphere transient(m_level);
    transient.scaleBy(m_radius);
    DrawableResource *rec = resource();
    UpdateMeshResouce(rec, &transient, m_shoUV);
}

}

