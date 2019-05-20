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
#include <grd/InstanceRecord.h>
#include <grd/GridInCell.h>
#include <h5/V1H5IO.h>
#include <h5_grd/HInstanceRecord.h>
#include <sds/FZOrder.h>
#include <QProgressDialog>
#include <QApplication>
#include <interface/GlobalFence.h>
#include <boost/thread/lock_guard.hpp>
#include <qt_base/AFileDlg.h>

namespace alo {
    
RepeatOps::RepeatOps() :
m_isActive(false),
m_instanceFilePath("unknown")
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
    
    QAttrib * acp = findAttrib("ainstfile");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    
    if(m_instanceFilePath != scachePath && scachePath.size() > 3)
        loadInstanceFile(scachePath);

    if(m_inOps.isDirty()) {
//todo build instancer
    }
}

bool RepeatOps::intersectRay(IntersectResult& result) const
{
    if(m_worldLookupRule->isEmpty()) 
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
    m_inOps.append(r);
}

void RepeatOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    m_inOps.remove(r);
}

bool RepeatOps::hasInstance() const
{ return false; }

void RepeatOps::updateInstancer(bool isAppending)
{
    QProgressDialog progress("Processing...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    m_worldLookupRule->detach();
    RenderableOps::resetAabb();
    
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
        
        memcpy(RenderableObject::aabb(), &m_worldGrid->aabb(), 24);

        std::cout << "\n end building instancer ";
    }

    progress.setValue(1);
    
}

bool RepeatOps::loadInstanceFile(const std::string &fileName)
{
    grd::InstanceRecord rec;
    ver1::H5IO hio;
    hio.begin(fileName);
    
    std::string instName;
    
    ver1::HBase ga("/");
    bool stat = ga.lsFirstTypedChild<HInstanceRecord>(instName);
    ga.close();
    
    if(stat) {
        HInstanceRecord w(instName);
        stat = w.load(rec);
        w.close();
    }

    hio.end();
    
    if(!stat || rec.isEmpty()) return false;
    
    interface::GlobalFence &fence = interface::GlobalFence::instance();
	boost::lock_guard<interface::GlobalFence> guard(fence);
    
    const int &n = rec.numInstances();
    m_instancer->createInstances(n);
    
    for(int i=0;i<n;++i) {
        grd::TestInstance &ti = m_instancer->instance(i);
        ti.setObjectId(rec.inds()[i]);
        ti.setSpace(rec.tms()[i]);
    }

    m_instancer->setNumInstancedObjects(rec.numObjects());
    m_instancer->setMinimumCellSize(rec.getMinimumCellSize());
    
    m_instanceFilePath = fileName;
    return true;
}

bool RepeatOps::hasEnable() const
{ return true; }

void RepeatOps::setActivated(bool x)
{    
    m_isActive =x; 
    if(m_isActive) updateInstancer(false);
}

AFileDlgProfile RepeatOps::SReadProfile(AFileDlgProfile::FRead,
        "Choose File To Open",
        ":images/open_big.png",
        "Open Instance Cache File",
        "Open .hes",
        ".hes",
        "./",
        "");

AFileDlgProfile *RepeatOps::readFileProfileR () const
{ return &SReadProfile; }

QString RepeatOps::getShortDescription() const
{
    return QString("instance file %1").arg(QString::fromStdString(m_instanceFilePath)); 
}

} /// end of alo
