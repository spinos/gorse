/*
 *  PerspectiveCamera.cpp
 *  aloe
 *
 *  Created by jian zhang on 4/28/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include "PerspectiveCamera.h"
#include "miscfuncs.h"

namespace alo {

PerspectiveCamera::PerspectiveCamera()
{
	m_fov = 17.5f;
	m_2tanfov = tan(DegreeToAngle<float>(m_fov)) * 2.f; // half fov angle * 2
}

PerspectiveCamera::~PerspectiveCamera()
{
}
	
bool PerspectiveCamera::isOrthographic() const
{
	return false;
}

float PerspectiveCamera::fieldOfView() const
{
	return m_fov;
}

/// width of near clipping plane
float PerspectiveCamera::frameWidth() const
{
	return m_2tanfov * focusDistance();
}

float PerspectiveCamera::frameWidthRel() const
{
	return frameWidth() / portWidth();
}

void PerspectiveCamera::zoom(int y)
{
	moveForward(-y/2);
}

void PerspectiveCamera::incidentRay(int x, int y, Vector3F & origin, Vector3F & worldVec) const
{		
	float cx, cy;
	getScreenCoord(cx, cy, x, y);
	worldVec.x = cx * frameWidth();
	worldVec.y = cy * frameHeight();
	worldVec.z = -focusDistance();
	
	origin = transformToWorld(worldVec);
	worldVec.normalize();
	worldVec = transformNormalToWorld(worldVec);
}

void PerspectiveCamera::setFieldOfView(float x)
{
	m_fov = x;
	m_2tanfov = tan(DegreeToAngle<float>(m_fov)) * 2.f; // half fov angle * 2
}

float PerspectiveCamera::tanhfov() const
{ return m_2tanfov * .5f; }

void PerspectiveCamera::screenToWorldVectorAt(int x, int y, float depth, Vector3F & worldVec) const
{
	Vector3F vecNear;
	screenToWorldVector(x, y, vecNear);
	const Vector3F vecFar = vecNear * farClipPlane() / nearClipPlane();
	float alpha = (depth - nearClipPlane()) / (farClipPlane() - nearClipPlane());
	if(alpha < 0.f) alpha = 0.f;
	else if(alpha > 1.f) alpha = 1.f;
	worldVec = vecFar * alpha + vecNear * (1.f - alpha);
}

}