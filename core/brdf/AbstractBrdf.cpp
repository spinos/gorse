/*
 *  AbstractBrdf.cpp
 *  
 *
 *  Created by jian zhang on 3/31/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "AbstractBrdf.h"
#include <math/SpaceTransform.h>

namespace alo {

AbstractBrdf::AbstractBrdf(const SpaceTransform* tm)
{ m_space = tm; }

void AbstractBrdf::setWo(const Vector3F& eyed)
{
	toLocalDirectionReversed(m_wo, eyed);
	computeSpecularSpace();
}

const float& AbstractBrdf::wodotn() const
{ return m_wo.z; }

Vector3F AbstractBrdf::surfaceNormal() const
{ return m_space->frontDirection(); }

Vector3F AbstractBrdf::viewDirection() const
{ 
	Vector3F wo;
	m_space->toParentDirection(wo, m_wo); 
	return wo;
}

Vector3F AbstractBrdf::reflectionDirection() const
{
	Vector3F wr(0,0,1);
	m_specularSpace.transformInPlace(wr);
	m_space->toParentDirectionInPlace(wr); 
	return wr;
}

void AbstractBrdf::toWorldInPlace(Vector3F& wi) const
{ m_space->toParentDirectionInPlace(wi); }

void AbstractBrdf::specularToLocalInPlace(Vector3F& wi) const
{ m_specularSpace.transformInPlace(wi); }

void AbstractBrdf::toLocalDirectionReversed(Vector3F& wo, const Vector3F& eyed) const
{ m_space->toLocalDirectionReversed(wo, eyed); }

Vector3F AbstractBrdf::halfVector(const Vector3F& wi, float& vdoth) const
{ 
	Vector3F hr = wi + m_wo;
	hr.normalize();
	vdoth = m_wo.dot(hr);
	return hr;
}

void AbstractBrdf::computeSpecularSpace()
{
	Vector3F specularDir(-m_wo.x, -m_wo.y, m_wo.z);
	Vector3F side = specularDir.perpendicular();
	Vector3F up = specularDir.cross(side);
	m_specularSpace.fill(side, up, specularDir);
}

}

