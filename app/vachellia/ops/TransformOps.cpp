/*
 *  TransformOps.cpp
 *  vachellia
 *
 */

#include "TransformOps.h"
#include <math/rayBox.h>
#include <math/Ray.h>
#include <interface/IntersectResult.h>

namespace alo {

TransformOps::TransformOps()
{}

TransformOps::~TransformOps()
{}

void TransformOps::update()
{
	Vector3F &t = TransformComponent::position();
	getFloatAttribValue(t.x, "tx");
	getFloatAttribValue(t.y, "ty");
	getFloatAttribValue(t.z, "tz");
	Vector3F &r = TransformComponent::rotation();
	getFloatAttribValue(r.x, "rx");
	getFloatAttribValue(r.y, "ry");
	getFloatAttribValue(r.z, "rz");
	Vector3F &s = TransformComponent::scale();
	getFloatAttribValue(s.x, "sx");
	getFloatAttribValue(s.y, "sy");
	getFloatAttribValue(s.z, "sz");
	TransformComponent::calculateSpace();
}

bool TransformOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    float rayData[8];
	result.copyRayData(rayData);

	rayToLocal(rayData);

	if(!rayAabbIntersect(rayData, aabb())) return false;
/// near intersection
	const float &tt = rayData[6];

	const Vector3F pnt(rayData[0] + rayData[3] * tt, 
						rayData[1] + rayData[4] * tt, 
						rayData[2] + rayData[5] * tt);
    Vector3F tn;
	getNormalOnAabb((float *)&tn, (const float *)&pnt, aabb(), tt * 1e-5f);

	rayToWorld(rayData);
	normalToWorld((float *)&tn);

	return result.updateRayDistance(tt, tn);
}

bool TransformOps::hasMenu() const
{ return true; }

void TransformOps::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const 
{
    ks.push_back(std::make_pair("Focus", 0));
}

void TransformOps::recvAction(int x) 
{
    if(x == 0) {}
}

}
