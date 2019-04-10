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
	m_radianceState = new SampleState;
	m_radianceState->setAccess(SampleState::Wrap, SampleState::Clamp);
	m_radianceState->setFilter(SampleState::Box);
	m_radianceState->setChannel(SampleState::RGB);
}

float& IntersectResult::rayDistance()
{ return m_dist; }

Vector3F& IntersectResult::hitNormal()
{ return m_normal; }

SampleState* IntersectResult::radianceState()
{ return m_radianceState; }

}