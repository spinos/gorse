/*
 *  Scene.cpp
 *  aloe
 *
 *  Created by zhang on 18-2-22.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "Scene.h"

namespace alo {

Scene::Scene()
{}

bool Scene::intersectRay(const Ray& aray, IntersectResult& result) const
{ return false; }

void Scene::getBackgroundColor(float* col, const Vector3F& dir, SampleState* state) const
{ memset(col, 0, 16); }

const EnvLightTyp* Scene::environmentLight() const
{ return 0; }

}
