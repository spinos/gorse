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
{}

HorizonOps::~HorizonOps()
{}

std::string HorizonOps::opsName() const 
{ return "horizon"; }

void HorizonOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->enqueueCreateRenderable(this, opsId());
}
  
void HorizonOps::update()
{
    getFloatAttribValue(m_planetRadius, "r");
    m_center.y = -radiusInMeters();
}

bool HorizonOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    float tt = result.rayDistance();
	if(!raySphereIntersect(tt, aray, m_center, radiusInMeters() ) )
		return false;
	
	Vector3F tn = aray.travel(tt);
	tn.y += radiusInMeters();
	tn.normalize();
	return result.updateRayDistance(tt, tn);
}

void HorizonOps::expandAabb(float *box) const
{}

float HorizonOps::mapDistance(const float *q) const
{ return Vector3F(q[0], q[1] + radiusInMeters(), q[2]).length() - radiusInMeters(); }

Vector3F HorizonOps::mapNormal(const float *q) const
{
	Vector3F tn(q[0], q[1] + radiusInMeters(), q[2]);
	tn.normalize();
	return tn;
}

float HorizonOps::radiusInMeters() const
{ return m_planetRadius * 1000.f; }

}
