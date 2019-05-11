/*
 *  ConeOps.cpp
 *  vachellia
 *
 *  2019/5/11
 */

#include "ConeOps.h"
#include <qt_graph/GlyphConnection.h>
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>
#include <math/rayCone.h>
#include <math/pointBox.h>

namespace alo {
   
ConeOps::ConeOps() : m_height(10.f),
m_radius(5.f),
m_lcrc(11.18034f)
{
    setAabb(-m_radius, 0.f, -m_radius, m_radius, m_height, m_radius);
}

ConeOps::~ConeOps()
{}

std::string ConeOps::opsName() const
{ return "cone"; }

void ConeOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->createRenderable(this, opsId());
}
  
void ConeOps::update()
{
    TransformOps::update();
    
    getFloatAttribValue(m_height, "h");
    getFloatAttribValue(m_radius, "r");
    m_lcrc = sqrt(m_radius * m_radius + m_height * m_height);

    setAabb(-m_radius, 0.f, -m_radius, m_radius, m_height, m_radius);

    m_outOps.sendImpulse();
}

bool ConeOps::intersectRay(IntersectResult& result) const
{
    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    if(!rayLocalConeIntersect(rayData, m_height, m_radius, m_lcrc, c_aabb() ) )
        return false;

    float q[3];
    rayTravel(q, rayData);

    Vector3F tn = mapNormal(q);

    rayToWorld(rayData);
    normalToWorld((float *)&tn);
    
    float &tt = rayData[6];
    return result.updateRayDistance(tt, tn);
}

float ConeOps::mapDistance(const float *q) const
{
    Vector3F va(q[0], q[1], q[2]);
    pointToLocal((float *)&va);

    float d = movePointOntoLocalCone(va, m_height, m_radius, m_lcrc);

    pointToWorld((float *)&va);

    d = GetSign(d) * distancePointToPoint(q, (const float *)&va);

    return d;
}

Vector3F ConeOps::mapNormal(const float *q) const
{
    return normalOnLocalCone(q, m_radius, m_height, m_lcrc);
}

bool ConeOps::hasInstance() const
{ return false; }

void ConeOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.append(line);
}

void ConeOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.remove(line);
}

}
