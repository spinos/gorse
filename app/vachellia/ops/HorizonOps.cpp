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
    scene->createRenderable(this, opsTypeId());
}
  
void HorizonOps::update()
{
    getFloatAttribValue(m_planetRadius, "r");
    m_center.y = -radiusInMeters();
}

bool HorizonOps::intersectRay(IntersectResult& result) const
{
	float rayData[8];
    result.copyRayData(rayData);

	if(!raySphereIntersect(rayData, m_center, radiusInMeters() ) )
		return false;
	
	float &tt = rayData[6];
	Vector3F tn(rayData[0] + rayData[3] * tt,
				rayData[1] + rayData[4] * tt + radiusInMeters(),
				rayData[2] + rayData[5] * tt);
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

bool HorizonOps::hasInstance() const
{ return false; }

float HorizonOps::mapLocalDistance(const float *q) const
{ return mapDistance(q); }

void HorizonOps::genSamples(sds::SpaceFillingVector<grd::PointSample> &samples) const
{}

QString HorizonOps::getShortDescription() const
{
    return QString("planet radius %1 km").arg(m_planetRadius); 
}

}
