/*
 *  InstancerBase.cpp
 *  vachellia
 *
 *  2019/7/18
 */

#include "InstancerBase.h"
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
#include <grd/InstanceRecord.h>
#include <grd/GridInCell.h>
#include <h5/V1H5IO.h>
#include <h5_grd/HInstanceRecord.h>
#include <sds/FZOrder.h>
#include <QProgressDialog>
#include <QApplication>

namespace alo {
    
InstancerBase::InstancerBase() :
m_isActive(false)
{
    m_instancer = new InstancerTyp;
    m_worldGrid = new WorldTyp;
    m_worldRule = new WorldRuleTyp;
    m_worldBuilder = new WorldBuilderTyp;
    m_worldLookupRule = new WorldLookupRuleTyp;
}

InstancerBase::~InstancerBase()
{
    delete m_worldLookupRule;
    delete m_instancer;
    delete m_worldBuilder;
    delete m_worldRule;
    delete m_worldGrid;
}

void InstancerBase::appendInputRenderable(RenderableOps *r)
{
    m_inOps.append(r);
}

void InstancerBase::removeInputRenderable(RenderableOps *r)
{
    m_inOps.remove(r);
}

bool InstancerBase::hasInputRenderable(RenderableOps *r) const
{
    return m_inOps.contains(r);
}

bool InstancerBase::isInputRenderableDirty() const
{
    return m_inOps.isDirty();
}

bool InstancerBase::isLookupEmpty() const
{
    return m_worldLookupRule->isEmpty();
}

void InstancerBase::updateInstancer(RenderableScene *scene, bool isAppending)
{
    if(scene->isInProgress())
        return updateInstancerInProgress(isAppending);

    QProgressDialog progress("Processing...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    updateInstancerInProgress(isAppending);

    progress.setValue(1);
    
}

void InstancerBase::updateInstancerInProgress(bool isAppending)
{
    m_worldLookupRule->detach();
    
    if(isAppending) {
        m_instancer->addObject(m_inOps.back());
    } else {
        m_instancer->clearObjects();
        const int n = m_inOps.numElements();
        for(int i=0;i<n;++i) {
            m_instancer->addObject(m_inOps.element(i));
        }
    }
    
    m_instancer->verbose();
    
    if(m_instancer->validateNumObjects() ) {
        
        sds::FZOrderCurve sfc;
        sfc.setCoord(32.f, 32.f, 32.f, 16.f);
        
        typedef grd::LocalGridBuildRule<sds::FZOrderCurve> CellBuildRuleTyp;
        CellBuildRuleTyp cellRule(&sfc);
        cellRule.setP(6);
        
        typedef grd::LocalGridBuilder<grd::LocalGrid<float> > CellBuilderTyp;
        CellBuilderTyp cellBuilder;
        
        float fcellSize = m_instancer->getMediumObjectSize() * 6.f;
        m_instancer->limitCellSize(fcellSize);
        int cellSize = fcellSize;
        cellSize = Round64(cellSize);
        std::cout << "\n cell size " << cellSize;
        const int cencz[4] = {0,0,0,cellSize};
        m_worldRule->setCenterCellSize(cencz);

        m_worldGrid->clear();
        m_worldBuilder->attach(m_worldGrid);

        m_worldBuilder->addInstances<InstancerTyp, WorldRuleTyp, CellBuilderTyp, CellBuildRuleTyp >(*m_instancer, *m_worldRule, cellBuilder, cellRule);
       
        m_worldBuilder->detach();

        m_worldLookupRule->attach<InstancerTyp>(m_worldGrid, m_instancer);
        
        std::cout << "\n end building instancer ";
    }
}

void InstancerBase::setInstancerActivated(RenderableScene *scene, bool x)
{
    if(x) updateInstancer(scene, false);
    if(x) m_isActive = isInstancerComplete(); 
    else m_isActive = x; 
}

bool InstancerBase::isInstancerReady() const
{
    if(m_instancer->numInstances() < 1) return false;
    if(m_instancer->numInstancedObjects() < 1) return false;
    return m_inOps.numElements() >= m_instancer->numInstancedObjects(); 
}

bool InstancerBase::isInstancerComplete() const
{ return !m_worldLookupRule->isEmpty(); }

bool InstancerBase::isInstancerActivated() const
{ return m_isActive; }

const BoundingBox &InstancerBase::instancerAabb() const
{ return m_worldGrid->aabb(); }

void InstancerBase::setLookupMaxNumSteps(int x)
{ m_worldLookupRule->setMaxNumStep(x); }

int InstancerBase::numInputRenderables() const
{ return m_inOps.numElements(); }

const int &InstancerBase::numInstancedObjects() const
{ return m_instancer->numInstancedObjects(); }

const int &InstancerBase::numInstances() const
{ return m_instancer->numInstances(); }

const RenderableOps *InstancerBase::inputRenderable(int i) const
{ return m_inOps.element(i); }

void InstancerBase::loadInstanceRecord(grd::InstanceRecord &rec)
{
    const int &n = rec.numInstances();
    m_instancer->createInstances(n);
    
    for(int i=0;i<n;++i) {
        grd::TestInstance &ti = m_instancer->instance(i);
        ti.setObjectId(rec.inds()[i]);
        ti.setSpace(rec.tms()[i]);
    }

    m_instancer->setNumInstancedObjects(rec.numObjects());
    m_instancer->setMinimumCellSize(rec.getMinimumCellSize());
}

QString InstancerBase::getInputRenderablesDescription() const
{
    const int nobjs = numInputRenderables();
    QString r;
    for(int i=0;i<nobjs;++i) {
        const RenderableOps *e = inputRenderable(i);
        QString dspn = QString::fromStdString(e->displayName());
        r = r + QString("\n obj[%1]: %2").arg(QString::number(i), dspn);
    }
    return r;
}

} /// end of alo
