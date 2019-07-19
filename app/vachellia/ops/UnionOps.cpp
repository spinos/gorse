/*
 *  UnionOps.cpp
 *  vachellia
 *
 *  2019/5/11
 */

#include "UnionOps.h"
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>
#include <math/rayBox.h>

namespace alo {
   
   
UnionOps::UnionOps()
{
}

UnionOps::~UnionOps()
{
}

std::string UnionOps::opsName() const
{ return "union"; }

void UnionOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->createRenderable(this, opsTypeId());
}
  
void UnionOps::update()
{
    TransformOps::update();

}

bool UnionOps::intersectRay(IntersectResult& result) const
{
    const unsigned ne = m_inOps.numElements();
    if(ne < 1) return TransformOps::intersectRay(result);

    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    if(!rayAabbIntersect(rayData, c_aabb())) return false;

    float &tt = rayData[6];
    const float &tLimit = rayData[7];
    float q[3];

    for(int i=0;i<256;++i) {
        
        rayTravel(q, rayData);

        float d = mapDistance(q);

        if(d < 1e-3f) break;

        if(d < tt * 1e-5f) break;

        tt += d * .8f;
     
        if(tt > tLimit) return false;
    }

    Vector3F tn = mapNormal(q);

    rayToWorld(rayData);
    normalToWorld((float *)&tn);

    return result.updateRayDistance(tt, tn);
}

float UnionOps::mapDistance(const float *q) const
{
    float md = 1e10f;
    const unsigned ne = m_inOps.numElements();
    
    for(unsigned i=0;i<ne;++i) {
        const RenderableOps *e = m_inOps.element(i);
        float d = e->mapDistance(q);
        if(md > d) md = d;
    }

    return md;
}

Vector3F UnionOps::mapNormal(const float *q) const
{
    Vector3F tn;

    float q0[3];
    float q1[3];

    memcpy(q0, q, 12);
    memcpy(q1, q, 12);

    q0[0] -= 1e-3f;
    q1[0] += 1e-3f;

    tn.x = mapDistance(q1) - mapDistance(q0);

    q0[0] = q[0];
    q1[0] = q[0];
    q0[1] -= 1e-3f;
    q1[1] += 1e-3f;

    tn.y = mapDistance(q1) - mapDistance(q0);

    q0[1] = q[1];
    q1[1] = q[1];
    q0[2] -= 1e-3f;
    q1[2] += 1e-3f;

    tn.z = mapDistance(q1) - mapDistance(q0);

    tn.normalize();
    return tn;
}

bool UnionOps::canConnectTo(GlyphOps *another, const std::string &portName) const
{ 
    if(!another->hasRenderable()) return false;
    RenderableOps *r = static_cast<RenderableOps *>(another);
    if(m_inOps.contains(r)) return false;
    return true;
}

void UnionOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    m_inOps.append(r);
    combineInputs();
}

void UnionOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    m_inOps.remove(r);
    combineInputs();
}

void UnionOps::receiveImpulse(GlyphOps *another, const std::string &portName)
{
    combineInputs();
}

void UnionOps::combineInputs()
{
    const unsigned ne = m_inOps.numElements();
    if(ne < 1) {
        RenderableOps::resetAabb();
        return;
    }

    setAabbNull();
    for(unsigned i=0;i<ne;++i) {
        const RenderableOps *e = m_inOps.element(i);
        e->expandAabb(aabb());
    }

}

bool UnionOps::hasInstance() const
{ return false; }

bool UnionOps::hasGeodesicSamples() const
{ return false; }

QString UnionOps::getShortDescription() const
{
    QString r = QString("union of %1 objs").arg(m_inOps.numElements());
    for(int i=0;i<m_inOps.numElements();++i) {
        const RenderableOps *e = m_inOps.element(i);
        QString dspn = QString::fromStdString(e->displayName());
        r = r + QString("\n obj[%1]: %2").arg(QString::number(i), dspn);
    }
    return r;
}

} /// end of alo
