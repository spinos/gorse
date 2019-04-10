/*
 *  Scene.h
 *  aloe
 *
 *  Created by zhang on 18-2-22.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SCENE_H
#define ALO_SCENE_H

#include <math/Ray.h>
#include "IntersectResult.h"
#include "light.h"

namespace alo {

class SampleState;

class Scene {

public:

    Scene();
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result) const;
    
	virtual void getBackgroundColor(float* col, const Vector3F& dir, SampleState* state) const;
	
	virtual const EnvLightTyp* environmentLight() const;
	
protected:

private:

};

}

#endif
