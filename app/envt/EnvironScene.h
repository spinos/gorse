/*
 *  EnvironScene.h
 *  aloe environment test
 *
 *  Created by zhang on 18-2-22.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ENVIRON_SCENE_H
#define ENVIRON_SCENE_H

#include <interface/Scene.h>

namespace alo {

class EnvironScene : public Scene {
    
	EnvLightTyp* m_envmap;
	
public:

    EnvironScene();
	bool readImage(const char* filename);
	
    virtual bool intersectRay(const Ray& aray, IntersectResult& result) const;
    
	virtual void getBackgroundColor(float* rgba, const Vector3F& dir, SampleState* state) const;
	
	virtual const EnvLightTyp* environmentLight() const;
	
protected:

private:

};

}

#endif
