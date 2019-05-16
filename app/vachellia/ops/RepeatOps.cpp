/*
 *  RepeatOps.cpp
 *  vachellia
 *
 *  2019/5/12
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
#include <grd/ObjectInstancer.h>
#include <grd/GridInCell.h>
#include <sds/FZOrder.h>
#include <QProgressDialog>
#include <QApplication>
#include <interface/GlobalFence.h>
#include <boost/thread/lock_guard.hpp>
#include <ctime>

namespace alo {
    
RepeatOps::RepeatOps() :
m_isActive(false)
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
{ return "repeat"; }

void RepeatOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->createRenderable(this, opsTypeId());
}
  
void RepeatOps::update()
{
    TransformOps::update();

    int mns;
    getIntAttribValue(mns, "amaxnstep");
    m_worldLookupRule->setMaxNumStep(mns);

    if(m_inOps.isDirty()) {
//todo build instancer
    }
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
    //std::cout << "\n RepeatOps " << this << " connectTo renderable " << r;
    m_inOps.append(r);
    if(m_isActive) updateInstancer(true);
}

void RepeatOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    m_inOps.remove(r);
    if(m_isActive) updateInstancer(false);
    //std::cout << "\n RepeatOps " << this << " disconnectFrom renderable " << r;
}

bool RepeatOps::hasInstance() const
{ return false; }

void RepeatOps::updateInstancer(bool isAppending)
{
    QProgressDialog progress("Processing...", QString(), 0, 2, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    m_worldLookupRule->detach();
    RenderableOps::resetAabb();
    
    if(isAppending) {
       m_instancer->addObject(m_inOps.back());
    } else {
/// order changed
       m_instancer->clear();
       const int n = m_inOps.numElements();
       for(int i=0;i<n;++i) {
           m_instancer->addObject(m_inOps.element(i));
       }
    }
    
    progress.setValue(1);
    
    m_instancer->verbose();
    
    if(m_instancer->numObjects() > 0) {
        
        const float spacing = m_instancer->getMediumObjectSize();
        std::cout << "\n spacing " << spacing;
        const float xzSpan = spacing * .33f;
        
        static const int udim = 100;
        static const int vdim = 100;
            
        static const float ySpan = 0.3f;
        static const float coverOrigin = 0;
        static const float scaleSpan = .5f;

        std::time_t secs = std::time(0);
        m_instancer->createPhalanx(udim, vdim, spacing, xzSpan, ySpan, coverOrigin, scaleSpan, secs);

        sds::FZOrderCurve sfc;
        sfc.setCoord(32.f, 32.f, 32.f, 16.f);
        
        typedef grd::LocalGridBuildRule<sds::FZOrderCurve> CellBuildRuleTyp;
        CellBuildRuleTyp cellRule(&sfc);
        cellRule.setP(6);
        
        typedef grd::LocalGridBuilder<grd::LocalGrid<float> > CellBuilderTyp;
        CellBuilderTyp cellBuilder;
        
        int cellSize = spacing * 5.f;
        cellSize = Round64(cellSize);
        std::cout << "\n cell size " << cellSize;
        const int cencz[4] = {0,0,0,cellSize};
        m_worldRule->setCenterCellSize(cencz);

        m_worldGrid->clear();
        m_worldBuilder->attach(m_worldGrid);

        m_worldBuilder->addInstances<InstancerTyp, WorldRuleTyp, CellBuilderTyp, CellBuildRuleTyp >(*m_instancer, *m_worldRule, cellBuilder, cellRule);
       
        m_worldBuilder->detach();

        memcpy(RenderableObject::aabb(), &m_worldGrid->aabb(), 24);

        m_worldLookupRule->attach(m_worldGrid);
        m_worldLookupRule->setPrimitiveRule<InstancerTyp>(m_instancer);
        
    }

    progress.setValue(2);

}

bool RepeatOps::hasEnable() const
{ return true; }

void RepeatOps::setActivated(bool x)
{ 
    m_isActive =x; 
    if(m_isActive) updateInstancer(false);
}

} /// end of alo
