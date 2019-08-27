/*
 *  InstancerBase.cpp
 *  vachellia
 *
 *  2019/8/24
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
#include <h5_grd/HInstanceRecord.h>
#include <QProgressDialog>
#include <QApplication>
#include <h5/V1H5IO.h>
#include "VoxelOps.h"
#include <boost/format.hpp>

namespace alo {
    
InstancerBase::InstancerBase() :
m_isActive(false)
{
    m_instancer = new InstancerTyp;
    m_worldGrid = new WorldTyp;
    m_worldRule = new WorldRuleTyp;
    m_worldBuilder = new WorldBuilderTyp;
    m_worldLookupRule = new WorldLookupRuleTyp;
    m_worldCenterCellSize[0] = 0;
    m_worldCenterCellSize[1] = 0;
    m_worldCenterCellSize[2] = 0;
    m_worldCenterCellSize[3] = 64;
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
        
        float fcellSize = m_instancer->getMediumObjectSize(m_objectCounter) * 4.f;
        m_instancer->limitCellSize(fcellSize);
        int cellSize = fcellSize;
        cellSize = Round64(cellSize);
        m_worldCenterCellSize[3] = cellSize;
        std::cout << "\n world center (" << m_worldCenterCellSize[0]
                    << ", " << m_worldCenterCellSize[1]
                    << ", " << m_worldCenterCellSize[2]
                    << ") cell size " << m_worldCenterCellSize[3];
        m_worldRule->setCenterCellSize(m_worldCenterCellSize);

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

int InstancerBase::numInstancedObjects() const
{ return m_instancer->numInstancedObjects(); }

const int &InstancerBase::numInstances() const
{ return m_instancer->numInstances(); }

const RenderableOps *InstancerBase::inputRenderable(int i) const
{ return m_inOps.element(i); }

void InstancerBase::loadInstanceRecord(grd::InstanceRecord &rec)
{
    const int &n = rec.numInstances();
    createInstances(n);
    
    std::map<int, int> counter;
    for(int i=0;i<n;++i) {
        grd::TestInstance &ti = instance(i);
        ti.setObjectId(rec.inds()[i]);
        ti.setSpace(rec.tms()[i]);
        
        counter[rec.inds()[i]] = 0;
    }
	
	setInstancedObjectCountAndSize(counter, rec.getMinimumCellSize());
}

void InstancerBase::createInstances(int count)
{ m_instancer->createInstances(count); }

grd::TestInstance &InstancerBase::instance(int i)
{ return m_instancer->instance(i); }

void InstancerBase::setInstancedObjectCountAndSize(std::map<int, int> &counter, float size)
{
	m_instancer->setMinimumCellSize(size);
    
    m_objectCounter.clear();
    std::map<int, int>::const_iterator it = counter.begin();
    for(;it!=counter.end();++it) {
        m_objectCounter[it->first] = 0;
    }
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

void InstancerBase::clearClusters()
{ m_instancer->clearClusters(); }

void InstancerBase::clusterBegin(const int &i)
{ m_instancer->clusterBegin(i); }

void InstancerBase::clusterEnd(const int &i)
{ m_instancer->clusterEnd(i); }

void InstancerBase::setWorldCenter(const float *b)
{
    int cx = (b[0] + b[3]) * .5f;
    int cy = (b[1] + b[4]) * .5f;
    int cz = (b[2] + b[5]) * .5f;
    int sx = cx > 0 ? 1 : -1;
    int sy = cy > 0 ? 1 : -1;
    int sz = cz > 0 ? 1 : -1;
    if(sx < 0) cx = -cx;
    if(sy < 0) cy = -cy;
    if(sz < 0) cz = -cz;
    cx = Round32(cx);
    cy = Round32(cy);
    cz = Round32(cz);
    m_worldCenterCellSize[0] = cx * sx;
    m_worldCenterCellSize[1] = cy * sy;
    m_worldCenterCellSize[2] = cz * sz;
}

void InstancerBase::saveInstanceToFile(const std::string &filename)
{
	QProgressDialog progress("Processing...", QString(), 0, 2, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();
	
	ver1::H5IO hio;
	bool stat = hio.begin(filename, HDocument::oCreate );
	if(!stat) {
		std::cout << "\n ERROR cannot create file " << filename;
		return;
	}
	
	ver1::HBase ga("/asset");
	
	saveObjects("/asset");
	progress.setValue(1);
	
	saveInstances("/asset/inst");
	
	ga.writeModifiedTime();
	ga.close();
	
	hio.end();
	
	progress.setValue(2);
}

void InstancerBase::saveObjects(const std::string &parentName)
{
	const int nobjs = numInputRenderables();
    for(int i=0;i<nobjs;++i) {
		const std::string idName = boost::str(boost::format("%04i") % i);
		const std::string fullName = boost::str(boost::format("/asset/obj%1%") % idName );

		ver1::HBase gi(fullName);
		
		gi.addIntAttr(".objid");
		gi.writeIntAttr(".objid", &i);
		
		const RenderableOps *e = inputRenderable(i);
		gi.addVLStringAttr(".objname");
		gi.writeVLStringAttr(".objname", e->displayName());
		
		std::string altName = "unknown";
		const VoxelOps *v = static_cast<const VoxelOps *>(e);
		if(v) altName = v->assetName();
		
		gi.addVLStringAttr(".altname");
		gi.writeVLStringAttr(".altname", altName);
		gi.close();
    }
}

void InstancerBase::saveInstances(const std::string &pathName)
{
	HInstanceRecord writer(pathName);
	writer.save<InstancerTyp>(*m_instancer);
	writer.close();
}

} /// end of alo
