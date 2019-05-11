/*
 *  SphereOps.cpp
 *  vachellia
 *
 *  2019/5/11
 */

#include "SphereOps.h"
#include <qt_graph/GlyphConnection.h>
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>
#include <math/raySphere.h>
#include <math/pointBox.h>

namespace alo {
   
SphereOps::SphereOps() : m_radius(5.f)
{}

SphereOps::~SphereOps()
{}

std::string SphereOps::opsName() const
{ return "sphere"; }

void SphereOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->createRenderable(this, opsId());
}
  
void SphereOps::update()
{
    TransformOps::update();
    
    getFloatAttribValue(m_radius, "r");

    setAabb(-m_radius, -m_radius, -m_radius, m_radius, m_radius, m_radius);

    m_outOps.sendImpulse();
}

bool SphereOps::intersectRay(IntersectResult& result) const
{
    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    float &tt = rayData[6];
    if(!rayLocalSphereIntersect(rayData, m_radius ) )
        return false;

    float q[3];
    rayTravel(q, rayData);

    Vector3F tn = mapNormal(q);

    rayToWorld(rayData);
    normalToWorld((float *)&tn);
    return result.updateRayDistance(tt, tn);
}

float SphereOps::mapDistance(const float *q) const
{
    Vector3F va(q[0], q[1], q[2]);
    pointToLocal((float *)&va);

    float lva = va.length();

    float d = lva - m_radius;
    va *= m_radius / lva;

    pointToWorld((float *)&va);

    d = GetSign(d) * distancePointToPoint(q, (const float *)&va);

    return d;
}

Vector3F SphereOps::mapNormal(const float *q) const
{
    Vector3F nml(q[0], q[1], q[2]);
    return nml;
}

bool SphereOps::hasInstance() const
{ return false; }

void SphereOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.append(line);
}

void SphereOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.remove(line);
}

}