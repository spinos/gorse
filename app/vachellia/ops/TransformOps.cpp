/*
 *  TransformOps.cpp
 *  vachellia
 *
 */

#include "TransformOps.h"
#include <math/rayBox.h>
#include <math/Ray.h>
#include <interface/IntersectResult.h>
#include <qt_graph/GlyphScene.h>
#include <math/BaseCamera.h>

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
    ks.push_back(std::make_pair("Focus", BaseCamera::ActFocusIn3D));
}

void TransformOps::recvAction(int x) 
{
    if(x == BaseCamera::ActFocusIn3D) {
    	float b[6];
    	extractAabb(b);
    	pointToWorld(b);
    	pointToWorld(&b[3]);
    	Float4 centerR;
    	centerR.x = (b[0] + b[3]) * .5f;
    	centerR.y = (b[1] + b[4]) * .5f;
    	centerR.z = (b[2] + b[5]) * .5f;
    	centerR.w = Vector3F(b[0] - centerR.x, b[1] - centerR.y, b[2] - centerR.z).length();

    	glyphScene()->onFocusIn3D(centerR);
    }
}

}
