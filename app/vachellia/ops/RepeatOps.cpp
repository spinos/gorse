/*
 *  RepeatOps.cpp
 *  vachellia
 *
 *  2019/7/18
 */

#include "RepeatOps.h"
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>
#include <grd/LocalGrid.h>
#include <grd/LocalGridLookupRule.h>
#include <grd/WorldGrid.h>
#include <grd/WorldGridLookupRule.h>
#include <grd/ObjectInstancer.h>
#include <grd/InstanceRecord.h>
#include <grd/GridInCell.h>
#include <h5/V1H5IO.h>
#include <h5_grd/HInstanceRecord.h>
#include <QProgressDialog>
#include <QApplication>
#include <qt_base/AFileDlg.h>

namespace alo {
    
RepeatOps::RepeatOps() :
m_instanceFilePath("unknown")
{
}

RepeatOps::~RepeatOps()
{
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
    setLookupMaxNumSteps(mns);
    
    QAttrib * acp = findAttrib("ainstfile");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    
    if(m_instanceFilePath != scachePath && scachePath.size() > 3)
        loadInstanceFile(scachePath);

    if(isInputRenderableDirty()) {
// rebuild instancer?
    }
}

bool RepeatOps::intersectRay(IntersectResult& result) const
{
    if(isLookupEmpty()) 
        return TransformOps::intersectRay(result);

    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    WorldLookupRuleTyp::LookupResultTyp param;

    if(!traceRay<WorldLookupRuleTyp::LookupResultTyp>(param, rayData)) return false;

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
    if(hasInputRenderable(r)) return false;
    return true;
}

void RepeatOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    appendInputRenderable(r);
}

void RepeatOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    removeInputRenderable(r);
}

bool RepeatOps::hasInstance() const
{ return false; }

bool RepeatOps::hasGeodesicSamples() const
{ return false; }

bool RepeatOps::hasSurfaceSamples() const
{ return false; }

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
    
    loadInstanceRecord(rec);
    
    m_instanceFilePath = fileName;
    return true;
}

bool RepeatOps::hasEnable() const
{ return true; }

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
    const int nobjs = numInputRenderables();
    QString r = QString("repeat %1 objs").arg(nobjs);
    if(nobjs > 0) r = r + getInputRenderablesDescription();
    r = r + QString("\n instance file: %1").arg(QString::fromStdString(m_instanceFilePath));
    r = r + QString("\n %1 instances").arg(QString::number(numInstances()));
    if(isInstancerReady()) 
        r = r + QString("\n Ready");
    else
        r = r + QString("\n Not Ready");
    return r;
}

bool RepeatOps::getActivatedState() const
{ return isInstancerActivated(); }

void RepeatOps::setActivated(bool x)
{
    RenderableOps::resetAabb();
    setInstancerActivated(renderableScene(), x);
    if(x && isInstancerComplete())
       memcpy(RenderableObject::aabb(), &instancerAabb(), 24); 
}

} /// end of alo
