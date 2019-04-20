/*
 *  HorizonOps.cpp
 *  vachellia
 *
 */

#include "HorizonOps.h"
#include <interface/RenderableScene.h>
#include <math/raySphere.h>
#include <interface/IntersectResult.h>

namespace alo {
   
HorizonOps::HorizonOps() :
m_center(0.f, -6378000.f, 0.f),
m_planetRadius(6378.f)
{
}

HorizonOps::~HorizonOps()
{}

void HorizonOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->enqueueCreateRenderable(this, opsId());
}
  
void HorizonOps::update()
{
    getFloatAttribValue(m_planetRadius, "r");
    m_center.y = -m_planetRadius * 1000.f;
}

bool HorizonOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    float tt = result.rayDistance();
	if(!raySphereIntersect(tt, aray, m_center, m_planetRadius * 1000.f) )
		return false;
	
	Vector3F tn = aray.travel(tt);
	tn.y += m_planetRadius * 1000.f;
	tn.normalize();
	return result.updateRayDistance(tt, tn);
}

}
