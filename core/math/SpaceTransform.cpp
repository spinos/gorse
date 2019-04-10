/*
 *  SpaceTransform.cpp
 *  aloe
 *
 *  Created by jian zhang on 3/31/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "SpaceTransform.h"

namespace alo {

SpaceTransform::SpaceTransform()
{ m_p.setZero(); }

void SpaceTransform::setPoint(const Vector3F& P)
{ m_p = P; }

void SpaceTransform::setRotation(const Vector3F& N)
{
	Vector3F ref(0.f,-1.f,0.f);
	if(N.y < -0.9f || N.y > 0.9f) ref.set(-1.f,0.f,0.f);
	Vector3F side = N.cross(ref);
	side.normalize();
	Vector3F up = N.cross(side);
	m_rotation.fill(side, up, N);
	m_invRotation = m_rotation;
	m_invRotation.inverse();
}

void SpaceTransform::toLocalDirection(Vector3F& wo, const Vector3F& wi) const
{ m_invRotation.transform1(wo, wi); }

void SpaceTransform::toLocalDirectionReversed(Vector3F& wo, const Vector3F& wi) const
{ m_invRotation.transformReversed(wo, wi); }

void SpaceTransform::toParentDirection(Vector3F& wo, const Vector3F& wi) const
{ m_rotation.transform1(wo, wi); }

void SpaceTransform::toParentDirectionInPlace(Vector3F& wo) const
{ m_rotation.transformInPlace(wo); }

void SpaceTransform::toLocalPoint(Vector3F& wi) const
{ wi = m_p - wi; }

void SpaceTransform::toParentPoint(Vector3F& wi) const
{ wi += m_p; }

void SpaceTransform::getMatrix(float* mat) const
{
	mat[0] = m_rotation.M(0,0);
	mat[1] = m_rotation.M(0,1);
	mat[2] = m_rotation.M(0,2);
	mat[4] = m_rotation.M(1,0);
	mat[5] = m_rotation.M(1,1);
	mat[6] = m_rotation.M(1,2);
	mat[8] = m_rotation.M(2,0);
	mat[9] = m_rotation.M(2,1);
	mat[10] = m_rotation.M(2,2);
	mat[12] = m_p.x;
	mat[13] = m_p.y;
	mat[14] = m_p.z;
}

Vector3F SpaceTransform::frontDirection() const
{ return Vector3F(m_rotation.M(2,0), m_rotation.M(2,1), m_rotation.M(2,2) ); }

}