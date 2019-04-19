/*
 *  IntersectResult.cpp
 *  
 *
 *  Created by jian zhang on 3/29/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "IntersectResult.h"
#include <img/SampleState.h>

namespace alo {

IntersectResult::IntersectResult()
{
	m_rayData[7] = 1e7f;
	m_radianceState = new SampleState;
	m_radianceState->setAccess(SampleState::Wrap, SampleState::Clamp);
	m_radianceState->setFilter(SampleState::Box);
	m_radianceState->setChannel(SampleState::RGB);
}

IntersectResult::~IntersectResult()
{ delete m_radianceState; }

float *IntersectResult::rayData()
{ return m_rayData; }

void IntersectResult::copyRayData(float *y) const
{ memcpy(y, m_rayData, 32); }

bool IntersectResult::updateRayDistance(float x, const Vector3F &nml)
{
	if(m_rayData[7] < x) return false;
	m_rayData[7] = x;
	m_normal = nml;
	return true;
}

float& IntersectResult::rayDistance()
{ return m_rayData[7]; }

Vector3F& IntersectResult::hitNormal()
{ return m_normal; }

SampleState* IntersectResult::radianceState()
{ return m_radianceState; }

}