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
#include <sds/FZOrder.h>

#include "BoxOps.h"

namespace alo {
    
RepeatOps::RepeatOps()
{
    BoxOps *abox = new BoxOps;
    m_inOps.append(abox);
    
    m_grid = new grd::IndexGrid;
    m_grid->create(16);
    float originH[4] = {0.f, 0.f, 0.f, 4.f};
    m_grid->setOriginCellSize(originH);
    
    sds::FZOrderCurve sfc;
    sfc.setCoord(32.f, 32.f, 32.f, 16.f);
    
    typedef grd::IndexGridBuildRule<sds::FZOrderCurve> CellBuildRuleTyp;
    CellBuildRuleTyp cellRule(&sfc);
    
    InstanceBound inst;
    inst._instanceId = 0;
    inst._aabb = abox->c_aabb();
    
    Matrix44F space = Matrix44F::IdentityMatrix;
    Matrix44F invSpace = Matrix44F::IdentityMatrix;
    inst._tm = &space;
    inst._invTm = &invSpace;
    
    typedef grd::IndexGridBuilder<4> CellBuilderTyp;
    CellBuilderTyp cellBuilder;
    cellBuilder.attach(m_grid);
    cellBuilder.measure<InstanceBound, CellBuildRuleTyp >(inst, 0, cellRule);
    cellBuilder.detach();

    m_worldGrid = new WorldTyp;
    
    m_worldRule = new WorldRuleTyp;
    const int cencz[4] = {0,0,0,512};
    m_worldRule->setCenterCellSize(cencz);

    m_worldBuilder = new WorldBuilderTyp;

    m_worldBuilder->attach(m_worldGrid);

    m_objs.resetBuffer(5000);
    
    int acount = 0;

    for(int i=0;i<5000;++i) {
        grd::TestBox &ccube = m_objs[i];
        
/// randomly placed boxes
        int rx = -1600 + rand() % 3200;
        int ry = -1600 + rand() % 3200;
        int rz = -1600 + rand() % 3200;
        ccube._bbox.setMin(-4.f + 4.f * rx, -3.f + 2.f * ry, -3.f + 3.f * rz);
        ccube._bbox.setMax( 4.f + 4.f * rx,  5.f + 2.f * ry,  3.f + 3.f * rz);

        m_worldBuilder->addObject<grd::TestBox, WorldRuleTyp >(ccube, i, *m_worldRule);

        acount++;
        if((acount & 1023) == 0) {
            acount = 0;
            m_worldBuilder->buildCells<grd::TestBox, CellBuilderTyp, CellBuildRuleTyp>(m_objs.c_data(), cellBuilder, cellRule);
        }
    }

    if(acount > 0) {
        m_worldBuilder->buildCells<grd::TestBox, CellBuilderTyp, CellBuildRuleTyp>(m_objs.c_data(), cellBuilder, cellRule);
    }
    
    m_worldBuilder->detach();

    m_worldLookupRule = new WorldLookupRuleTyp;
    m_worldLookupRule->attach(m_worldGrid);

    float rayD[8];
    rayD[0] = 0.f; rayD[1] = 0.f; rayD[2] = 1000.f;
    Vector3F dir(0.7f, -0.1f, -1.f); dir.normalize();
    rayD[3] = dir.x; rayD[4] = dir.y; rayD[5] = dir.z; 
    rayD[6] = 1.f; rayD[7] = 1e8f;

    grd::WorldGridLookupResult param;
    m_worldLookupRule->lookup(param, rayD);

    std::cout << " hit t0 " << param._t0<< " nml " << param._nml;
    memcpy(RenderableObject::aabb(), &m_worldGrid->aabb(), 24);
}

RepeatOps::~RepeatOps()
{
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
