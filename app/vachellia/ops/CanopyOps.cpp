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

bool CanopyOps::hasGeodesicSamples() const
{ return false; }

bool CanopyOps::hasSurfaceSamples() const
{ return false; }

bool CanopyOps::hasEnable() const
{ return true; }

void CanopyOps::setActivated(bool x)
{
    RenderableOps::resetAabb();
	if(x && isInstancerReady())
		synthesizeBranchInstances();

    setInstancerActivated(renderableScene(), x);
    if(x && isInstancerComplete())
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
	const int portId = IdentifyInput(portName);

    if(!another->hasRenderable()) return false;
    RenderableOps *r = static_cast<RenderableOps *>(another);
    if(hasInputRenderable(r)) return false;
	
	if(portId == inBranch) return (r->hasInstance() && !r->hasGeodesicSamples());
	if(portId == inTrunk) return (r->hasInstance() && r->hasGeodesicSamples());
	
    return false;
}

void CanopyOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
	const int portId = IdentifyInput(portName);
	RenderableOps *r = static_cast<RenderableOps *>(another);
	if(portId == inBranch || portId == inTrunk) {
		appendInputRenderable(r);
	}
}

void CanopyOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
	const int portId = IdentifyInput(portName);
    RenderableOps *r = static_cast<RenderableOps *>(another);
    if(portId == inBranch || portId == inTrunk) {
		removeInputRenderable(r);
	}
}

QString CanopyOps::getShortDescription() const
{
    const int nobjs = numInputRenderables();
    QString r = QString("canopy of %1 objs").arg(nobjs);
    if(nobjs > 0) r = r + getInputRenderablesDescription();
    if(isInstancerReady()) 
        r = r + QString("\n Ready");
    else
        r = r + QString("\n Not Ready");
    return r;
}
	
int CanopyOps::IdentifyInput(const std::string &name)
{
	if(name == "inbranchsdf") return inBranch;
	if(name == "intrunksdf") return inTrunk;
	if(name == "interrainsdf") return inTerrain;
	return inUnknown;
}

} /// end of alo
