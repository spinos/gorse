/*
 *  TransformOps.cpp
 *  vachellia
 *
 *  2019/5/12
 */

#include "TransformOps.h"
#include <math/rayBox.h>
#include <math/Ray.h>
#include <interface/IntersectResult.h>
#include <qt_graph/GlyphScene.h>
#include <math/BaseCamera.h>
#include <math/pointBox.h>
#include <math/pointPoint.h>

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

bool TransformOps::intersectRay(IntersectResult& result) const
{
    float rayData[8];
	result.copyRayData(rayData);

	rayToLocal(rayData);

	if(!rayAabbIntersect(rayData, c_aabb())) return false;
/// near intersection
	const float &tt = rayData[6];

	float q[3];
	rayTravel(q, rayData);

    Vector3F tn = mapNormal(q);
    
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

void TransformOps::expandAabb(float *box) const
{
    float b[6];
    extractAabb(b);
    float a[3];
    for(int i=0;i<8;++i) {
        getAabbCorner(a, b, i);
        pointToWorld(a);
        expandAabbByPoint(box, a);
    }
}

float TransformOps::mapDistance(const float *q) const
{
	float a[3];
	memcpy(a, q, 12);
	pointToLocal(a);

	const float *b = c_aabb();
	
	float d = -1.f;
	if(isPointOutsideBox(a, b))
		d = 1.f;

	movePointOntoBox(a, b);
	pointToWorld(a);

	d = GetSign(d) * distancePointToPoint(q, a);

	return d;
}

Vector3F TransformOps::mapNormal(const float *q) const
{
	Vector3F tn;
	normalOnBox((float *)&tn, q, c_aabb());
	return tn;
}

float TransformOps::mapLocalDistance(const float *q) const
{
	float a[3];
	memcpy(a, q, 12);

	const float *b = c_aabb();
	
	float d = -1.f;
	if(isPointOutsideBox(a, b))
		d = 1.f;

	d = GetSign(d) * movePointOntoBox(a, b);

	return d;
}

void TransformOps::genSamples(sds::SpaceFillingVector<grd::CellSample> &samples) const
{
    const float *b = c_aabb();
    const float dx = b[3] - b[0];
    const float dy = b[4] - b[1];
    const float dz = b[5] - b[2];
    const float delta = (dx + dy + dz) * .33f * .0625f;
    const float h = delta * .5f;
    int nx = dx / delta; if(dx > delta * nx) nx++;
    int ny = dy / delta; if(dy > delta * ny) ny++;
    int nz = dz / delta; if(dz > delta * nz) nz++;

    grd::CellSample ap;
    ap._span = delta;
    for(int k=0;k<nz;++k) {
    	ap._pos.z = b[2] + h + delta * k;

    	for(int j=0;j<ny;++j) {
    		ap._pos.y = b[1] + h + delta * j;

    		for(int i=0;i<nx;++i) {
        		ap._pos.x = b[0] + h + delta * i;

        		samples << ap;
        	}
        }
    }
}

}
