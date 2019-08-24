/*
 *  CanopyOps.cpp
 *  vachellia
 *
 *  2019/8/24
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
#include <qt_base/AFileDlg.h>
#include <boost/format.hpp>

namespace alo {
	
AFileDlgProfile CanopyOps::SWriteProfile(AFileDlgProfile::FWrite,
        "Choose File To Save",
        ":images/canopy_save.png",
        "Export instancer setup ",
        "Save .hes",
        ".hes",
        "./",
        "untitled");
    
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
    
    int ntrunk;
    getIntAttribValue(ntrunk, "antree");
    setSynthesizeNumTrunk(ntrunk);
    
    int nbranch;
    getIntAttribValue(nbranch, "anbranch");
    setSynthesizeNumBranch(nbranch);
    
    int iseed;
    getIntAttribValue(iseed, "arandseed");
    setRandomSeed(iseed);
    
    float fspacing;
    getFloatAttribValue(fspacing, "aspacing");
    setRelativeSpacing(fspacing);
	
	float fbranchSpacing;
    getFloatAttribValue(fbranchSpacing, "aspacingb");
    setRelativeBranchSpacing(fbranchSpacing);
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
	if(portId == inTerrain) return (!r->hasInstance() && r->hasSurfaceSamples());
	
    return false;
}

void CanopyOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
	const int portId = IdentifyInput(portName);
	RenderableOps *r = static_cast<RenderableOps *>(another);
	if(portId == inBranch 
        || portId == inTrunk
        || portId == inTerrain) {
		appendInputRenderable(r);
	}
}

void CanopyOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
	const int portId = IdentifyInput(portName);
    RenderableOps *r = static_cast<RenderableOps *>(another);
    if(portId == inBranch 
        || portId == inTrunk
        || portId == inTerrain) {
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

void CanopyOps::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const 
{
	TransformOps::getMenuItems(ks);
    if(isInstancerComplete()) 
		ks.push_back(std::make_pair("Export", AFileDlgProfile::FWrite));
}

void CanopyOps::recvAction(int x) 
{
	TransformOps::recvAction(x);
    if(x == AFileDlgProfile::FWrite) {
    	saveInstanceToFile(SWriteProfile.getFilePath());
    }
}

AFileDlgProfile *CanopyOps::writeFileProfileR () const
{
	SWriteProfile._notice = boost::str(boost::format(" export %1% instances of %2% objects ") 
		% numInstances() % numInstancedObjects() );
    return &SWriteProfile; 
}

} /// end of alo
