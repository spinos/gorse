/*
 *  Lambert.cpp
 *  aloe
 *
 *  Created by jian zhang on 3/23/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "Lambert.h"

namespace alo {

Lambert::Lambert(const SpaceTransform* tm) : AbstractBrdf(tm)
{}

float Lambert::computeFr(const Vector3F& wi) const
{ return ONE_OVER_PIF; }

bool Lambert::checkDirection(const Vector3F& wi) const
{ return surfaceNormal().dot(wi) > .4e-3f; }

void Lambert::getFr(float& fr, float& pdf, float& cosTheta, const Vector3F& wi) const
{ 
	fr = ONE_OVER_PIF;
	cosTheta = surfaceNormal().dot(wi);
	pdf = cosTheta * ONE_OVER_PIF;
}

}