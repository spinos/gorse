/*
 *  RepeatOps.cpp
 *  vachellia
 *
 */

#include "RepeatOps.h"
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>
#include <math/rayBox.h>
#include <grd/LocalGrid.h>
#include <grd/LocalGridBuilder.h>
#include <grd/LocalGridBuildRule.h>
#include <grd/LocalGridLookupRule.h>
#include <grd/WorldGridBuilder.h>
#include <grd/WorldGridBuildRule.h>
#include <grd/WorldGridLookupRule.h>
#include <grd/TestCell.h>
#include <grd/GridInCell.h>
#include <sds/FZOrder.h>
#include <QProgressDialog>
#include <QApplication>
#include <interface/GlobalFence.h>
#include <boost/thread/lock_guard.hpp>
#include <ctime>

namespace alo {
    
RepeatOps::RepeatOps()
{
    m_instancer = new InstancerTyp;
    m_worldGrid = new WorldTyp;
    m_worldRule = new WorldRuleTyp;
    m_worldBuilder = new WorldBuilderTyp;
    m_worldLookupRule = new WorldLookupRuleTyp;
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
#if 1
    QProgressDialog progress("Processing...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    grd::BoxObject *bo = new grd::BoxObject;
    bo->_bbox.set(-4.f, 0.f, -3.f, 4.f, 25.f, 3.f);

    m_instancer->addObject(bo);

    static const int udim = 100;
    static const int vdim = 100;
    static const float spacing = 23.9;
    static const float xzSpan = 4.5f;
    static const float ySpan = 0.3f;
    static const float coverOrigin = 0;
    static const float scaleSpan = .5f;

    std::time_t secs = std::time(0);
    m_instancer->createPhalanx(udim, vdim, spacing, xzSpan, ySpan, coverOrigin, scaleSpan, secs);

    sds::FZOrderCurve sfc;
    sfc.setCoord(32.f, 32.f, 32.f, 16.f);
    
    typedef grd::LocalGridBuildRule<sds::FZOrderCurve> CellBuildRuleTyp;
    CellBuildRuleTyp cellRule(&sfc);
    cellRule.setP(5);
    
    typedef grd::LocalGridBuilder<grd::LocalGrid<float> > CellBuilderTyp;
    CellBuilderTyp cellBuilder;
    
    const int cencz[4] = {0,0,0,256};
    m_worldRule->setCenterCellSize(cencz);

    m_worldBuilder->attach(m_worldGrid);

    m_worldBuilder->addInstances<InstancerTyp, WorldRuleTyp, CellBuilderTyp, CellBuildRuleTyp >(*m_instancer, *m_worldRule, cellBuilder, cellRule);
   
    m_worldBuilder->detach();

    m_worldLookupRule->attach(m_worldGrid);
    m_worldLookupRule->setPrimitiveRule<InstancerTyp>(m_instancer);

    memcpy(RenderableObject::aabb(), &m_worldGrid->aabb(), 24);

    progress.setValue(1);
#endif 
    setRenderableScene(scene);
    scene->createRenderable(this, opsId());
}
  
void RepeatOps::update()
{
    TransformOps::update();

    if(m_inOps.isDirty()) {
        std::cout << "\n in ops dirty ";
        m_inOps.setClean();
    }

    const int ne = m_inOps.numElements();
    if(ne < 1) {
//todo clear instancer
        RenderableOps::resetAabb();
        return;
    }
//todo build instancer
}

bool RepeatOps::intersectRay(IntersectResult& result) const
{
    if(m_worldLookupRule->isEmpty() ) 
        return TransformOps::intersectRay(result);

    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    WorldLookupRuleTyp::LookupResultTyp param;

    if(!m_worldLookupRule->lookup(param, rayData)) return false;

    rayData[6] = param._t0;

    rayToWorld(rayData);
    normalToWorld((float *)&param._nml);

    return result.updateRayDistance(rayData[6], param._nml);

}

float RepeatOps::mapDistance(const float *q) const
{
    return TransformOps::mapDistance(q);
//todo
}

Vector3F RepeatOps::mapNormal(const float *q) const
{
    return TransformOps::mapNormal(q);
//todo
}

bool RepeatOps::canConnectTo(GlyphOps *another, const std::string &portName) const
{ 
    if(!another->hasRenderable()) return false;
    RenderableOps *r = static_cast<RenderableOps *>(another);
    if(!r->hasInstance()) return false;
    if(m_inOps.contains(r)) return false;
    return true;
}

void RepeatOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    std::cout << "\n RepeatOps " << this << " connectTo renderable " << r;
    m_inOps.append(r);
    update();
}

void RepeatOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    m_inOps.remove(r);
    update();
    std::cout << "\n RepeatOps " << this << " disconnectFrom renderable " << r;
}

bool RepeatOps::hasInstance() const
{ return false; }

}
