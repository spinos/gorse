/*
 *  CanopyOps.cpp
 *  vachellia
 *
 *  2019/7/18
 */

#include "CanopyOps.h"
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>
#include <grd/LocalGrid.h>
#include <grd/LocalGridLookupRule.h>
#include <grd/WorldGrid.h>
#include <grd/WorldGridLookupRule.h>
#include <grd/ObjectInstancer.h>
#include <grd/GridInCell.h>

namespace alo {
    
CanopyOps::CanopyOps()
{
}

CanopyOps::~CanopyOps()
{
}

std::string CanopyOps::opsName() const
{ return "canopy"; }

void CanopyOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->createRenderable(this, opsTypeId());
}

void CanopyOps::update()
{
    TransformOps::update();

    int mns;
    getIntAttribValue(mns, "amaxnstep");
    setLookupMaxNumSteps(mns);
    
}

bool CanopyOps::hasInstance() const
{ return false; }

bool CanopyOps::hasEnable() const
{ return true; }

void CanopyOps::setActivated(bool x)
{
    RenderableOps::resetAabb();
    setInstancerActivated(renderableScene(), x);
    if(x)
       memcpy(RenderableObject::aabb(), &instancerAabb(), 24); 
}

bool CanopyOps::getActivatedState() const
{ return isInstancerActivated(); }

bool CanopyOps::intersectRay(IntersectResult& result) const
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

bool CanopyOps::canConnectTo(GlyphOps *another, const std::string &portName) const
{ 
    if(!another->hasRenderable()) return false;
    RenderableOps *r = static_cast<RenderableOps *>(another);
    if(!r->hasInstance()) return false;
    if(hasInputRenderable(r)) return false;
    return true;
}

void CanopyOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    appendInputRenderable(r);
}

void CanopyOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    removeInputRenderable(r);
}

QString CanopyOps::getShortDescription() const
{
    const int nobjs = numInputRenderables();
    QString r = QString("canopy of %1 objs").arg(nobjs);
    for(int i=0;i<nobjs;++i) {
        const RenderableOps *e = inputRenderable(i);
        QString dspn = QString::fromStdString(e->displayName());
        r = r + QString("\n obj[%1]: %2").arg(QString::number(i), dspn);
    }
    r = r + QString("\n Not Ready");
    return r;
}

} /// end of alo
