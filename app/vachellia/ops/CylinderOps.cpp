/*
 *  CylinderOps.cpp
 *  vachellia
 *
 *  2019/5/11
 */

#include "CylinderOps.h"
#include <qt_graph/GlyphConnection.h>
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>
#include <math/rayCylinder.h>

namespace alo {
   
CylinderOps::CylinderOps() : m_height(10.f),
m_radius(5.f)
{
    setAabb(-m_radius, 0.f, -m_radius, m_radius, m_height, m_radius);
}

CylinderOps::~CylinderOps()
{}

std::string CylinderOps::opsName() const
{ return "cylinder"; }

void CylinderOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->createRenderable(this, opsId());
}
  
void CylinderOps::update()
{
    TransformOps::update();
    
    getFloatAttribValue(m_height, "h");
    getFloatAttribValue(m_radius, "r");

    setAabb(-m_radius, 0.f, -m_radius, m_radius, m_height, m_radius);

    m_outOps.sendImpulse();
}

bool CylinderOps::intersectRay(IntersectResult& result) const
{
    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    if(!rayLocalCylinderIntersect(rayData, m_height, m_radius, c_aabb() ) )
        return false;

    float q[3];
    rayTravel(q, rayData);

    Vector3F tn = mapNormal(q);

    rayToWorld(rayData);
    normalToWorld((float *)&tn);
    
    float &tt = rayData[6];
    return result.updateRayDistance(tt, tn);
}

Vector3F CylinderOps::mapNormal(const float *q) const
{
    return normalOnLocalCylinder(q, m_radius, m_height);
}

bool CylinderOps::hasInstance() const
{ return true; }

void CylinderOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.append(line);
}

void CylinderOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.remove(line);
}

}
