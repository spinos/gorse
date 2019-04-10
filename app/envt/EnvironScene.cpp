/*
 *  EnvironScene.cpp
 *  aloe environment test
 *
 *  Created by zhang on 18-2-22.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "EnvironScene.h"
#include <img/ExrImage.h>
#include <interface/EnvironmentLight.h>
#include <math/raySphere.h>

namespace alo {

EnvironScene::EnvironScene()
{
	m_envmap = new EnvLightTyp;
}

bool EnvironScene::readImage(const char* filename)
{ 
	ExrImage img;
	if(!img.read(filename)) return false;
	return m_envmap->read<ExrImage>(img); 
}

const EnvLightTyp* EnvironScene::environmentLight() const
{ return m_envmap; }

bool EnvironScene::intersectRay(const Ray& aray, IntersectResult& result) const
{
	if(!raySphereIntersect(result.rayDistance(), aray, Vector3F::Zero, 10.f) ) {
		return false;
	}
	
	Vector3F& nml = result.hitNormal();
	nml = aray.travel(result.rayDistance() );
	nml.normalize();
	return true;
}

void EnvironScene::getBackgroundColor(float* rgba, const Vector3F& dir, SampleState* state) const
{ 
    state->setLevelOfDetail(0.f);
    m_envmap->lookup(rgba, dir, state); 
    Clamp01(rgba[0]);
    Clamp01(rgba[1]);
    Clamp01(rgba[2]);
}

}
