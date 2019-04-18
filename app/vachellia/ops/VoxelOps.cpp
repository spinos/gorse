/*
 *  HorizonOps.cpp
 *  vachellia
 *
 */

#include "VoxelOps.h"
#include <interface/RenderableScene.h>
#include <math/raySphere.h>
#include <interface/IntersectResult.h>
#include <qt_base/AFileDlg.h>

namespace alo {
    
AFileDlgProfile VoxelOps::SReadProfile(AFileDlgProfile::FRead,
        "Choose File To Open",
        ":images/open_big.png",
        "Open Sparse Voxel Field File",
        "Open .hes",
        ".hes",
        "./",
        "");
   
VoxelOps::VoxelOps()
{
}

VoxelOps::~VoxelOps()
{}

void VoxelOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->enqueueCreateRenderable(this, opsId());
}
  
void VoxelOps::update()
{
    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    
}

bool VoxelOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    float tt = result.rayDistance();
	if(!raySphereIntersect(tt, aray, Vector3F::Zero, 10.f) )
		return false;
	
	Vector3F tn = aray.travel(tt);
	tn.normalize();
	return result.updateRayDistance(tt, tn);
}

AFileDlgProfile *VoxelOps::readFileProfileR () const
{ return &SReadProfile; }

}
