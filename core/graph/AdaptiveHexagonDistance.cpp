/*
 *  AdaptiveHexagonDistance.cpp
 *  aloe
 *
 *  Created by jian zhang on 3/3/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "AdaptiveHexagonDistance.h"

namespace alo {

namespace sdf {

AdaptiveHexagonDistance::AdaptiveHexagonDistance() :
m_enableGuard(false)
{}

AdaptiveHexagonDistance::~AdaptiveHexagonDistance()
{}

void AdaptiveHexagonDistance::beginGuardCells()
{
	m_enableGuard = true;
	m_cellsToDivide.clear(); 
}

void AdaptiveHexagonDistance::setNodeDistance2(const int& i, const Vector3F& ref)
{
	float d = nodes()[i].pos.distanceTo(ref);		
	setNodeDistance(i, d);
}

int AdaptiveHexagonDistance::findNode(int k) const
{ return m_nodeHash.lookupKey(k); }

float AdaptiveHexagonDistance::getNodeDistance(int i) const
{ return nodes()[i].val; }

Vector3F AdaptiveHexagonDistance::getNodePosition(int i) const
{ return nodes()[i].pos; }

}

}