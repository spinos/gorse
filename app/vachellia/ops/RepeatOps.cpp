/*
 *  RepeatOps.cpp
 *  vachellia
 *
 */

#include "RepeatOps.h"
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>
#include <math/rayBox.h>
#include <grd/IndexGridBuilder.h>
#include <grd/IndexGridBuildRule.h>
#include <grd/InstanceBound.h>
#include <sds/FZOrder.h>
#include <svf/SvfBuildRule.h>
#include <grd/WorldGridBuilder.h>
#include <grd/WorldGridBuildRule.h>
#include <grd/WorldGridLookupRule.h>
#include <grd/TestCell.h>
#include <grd/GridInCell.h>
#include <sds/FZOrder.h>

#include "BoxOps.h"

namespace alo {
    
RepeatOps::RepeatOps()
{
    BoxOps *abox = new BoxOps;
    m_inOps.append(abox);

    grd::BoxObject *bo = new grd::BoxObject;
    bo->_bbox.setMin(-5.f, -5.f, -5.f);
    bo->_bbox.setMax( 5.f, 5.f,  5.f);

    m_instancer = new InstancerTyp;
    m_instancer->addObject(bo);

    m_instancer->createInstances(9000);
    for(int i=0;i<9000;++i) {
        grd::TestInstance &sample = m_instancer->instance(i);

        int rx = -1000 + rand() % 2000;
        int ry = -1000 + rand() % 2000;
        int rz = -1000 + rand() % 2000;

        sample.setObjectId(0);
        sample.resetSpace();
        sample.setPosition(2.f * rx, 1.f * ry, 1.5f * rz);
        sample.calcSpace();
    }

    sds::FZOrderCurve sfc;
    sfc.setCoord(32.f, 32.f, 32.f, 16.f);
    
    typedef grd::IndexGridBuildRule<sds::FZOrderCurve> CellBuildRuleTyp;
    CellBuildRuleTyp cellRule(&sfc);
    
    typedef grd::IndexGridBuilder<4> CellBuilderTyp;
    CellBuilderTyp cellBuilder;

    m_worldGrid = new WorldTyp;
    
    m_worldRule = new WorldRuleTyp;
    const int cencz[4] = {0,0,0,256};
    m_worldRule->setCenterCellSize(cencz);

    m_worldBuilder = new WorldBuilderTyp;

    m_worldBuilder->attach(m_worldGrid);

    m_worldBuilder->addInstances<InstancerTyp, WorldRuleTyp, CellBuilderTyp, CellBuildRuleTyp >(*m_instancer, *m_worldRule, cellBuilder, cellRule);
   
    m_worldBuilder->detach();

    m_worldLookupRule = new WorldLookupRuleTyp;
    m_worldLookupRule->attach(m_worldGrid, m_instancer);

    memcpy(RenderableObject::aabb(), &m_worldGrid->aabb(), 24);
}

RepeatOps::~RepeatOps()
{
    delete m_worldLookupRule;
    delete m_instancer;
    delete m_worldBuilder;
    delete m_worldRule;
    delete m_worldGrid;
}

std::string RepeatOps::opsName() const
{ return "union"; }

void RepeatOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->enqueueCreateRenderable(this, opsId());
}
  
void RepeatOps::update()
{
    TransformOps::update();

    const int ne = m_inOps.numElements();
    if(ne < 1) {
        RenderableOps::resetAabb();
        return;
    }

    setAabbNull();
    for(int i=0;i<ne;++i) {
        const RenderableOps *e = m_inOps.element(i);
        e->expandAabb(aabb());
    }
}

bool RepeatOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    //const int ne = m_inOps.numElements();
    //if(ne < 1
    if(m_worldLookupRule->isEmpty() ) return TransformOps::intersectRay(aray, result);

    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    grd::WorldGridLookupResult param;

    if(!m_worldLookupRule->lookup(param, rayData)) return false;

    rayData[6] = param._t0;

    rayToWorld(rayData);
    normalToWorld((float *)&param._nml);

    return result.updateRayDistance(rayData[6], param._nml);

/*
    if(!rayAabbIntersect(rayData, c_aabb())) return false;

    float &tt = rayData[6];
    const float &tLimit = rayData[7];
    float q[3];

    for(int i=0;i<256;++i) {
        
        rayTravel(q, rayData);

        float d = mapDistance(q);

        if(d < 1e-3f) break;

        if(d < tt * 1e-5f) break;

        tt += d * .9f;
     
        if(tt > tLimit) return false;
    }

    Vector3F tn = mapNormal(q);

    rayToWorld(rayData);
    normalToWorld((float *)&tn);

    return result.updateRayDistance(tt, tn);*/
}

float RepeatOps::mapDistance(const float *q) const
{
    float md = 1e10f;
    const int ne = m_inOps.numElements();
    
    for(int i=0;i<ne;++i) {
        const RenderableOps *e = m_inOps.element(i);
        float d = e->mapDistance(q);
        if(md > d) md = d;
    }

    return md;
}

Vector3F RepeatOps::mapNormal(const float *q) const
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

bool RepeatOps::canConnectTo(GlyphOps *another, const std::string &portName) const
{ 
    if(!another->hasRenderable()) return false;
    RenderableOps *r = static_cast<RenderableOps *>(another);
    if(m_inOps.contains(r)) return false;
    return true;
}

void RepeatOps::connectTo(GlyphOps *another, const std::string &portName)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    std::cout << "\n RepeatOps " << this << " connectTo renderable " << r;
    m_inOps.append(r);
    update();
}

void RepeatOps::disconnectFrom(GlyphOps *another, const std::string &portName)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    m_inOps.remove(r);
    update();
    std::cout << "\n RepeatOps " << this << " disconnectFrom renderable " << r;
}

}
