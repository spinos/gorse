/*
 *  PerspectiveCamera.h
 *  aloe
 *
 *  Created by jian zhang on 4/28/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ALO_PERSPECTIVE_CAMERA_H
#define ALO_PERSPECTIVE_CAMERA_H

#include "BaseCamera.h"

namespace alo {

class PerspectiveCamera : public BaseCamera {
/// field of view / 2 angle in degrees
	float m_fov;
/// field width relative to depth
	float m_2tanfov;
public:
	PerspectiveCamera();
	virtual ~PerspectiveCamera();
	
	virtual bool isOrthographic() const;
	virtual float fieldOfView() const;
	virtual float frameWidth() const;
	virtual float frameWidthRel() const;
	virtual void zoom(int y);
	virtual void incidentRay(int x, int y, Vector3F & origin, Vector3F & worldVec) const;
	virtual void setFieldOfView(float x);
	virtual void screenToWorldVectorAt(int x, int y, float depth, Vector3F & worldVec) const;
/// tan(fieldOfView / 2)
	float tanhfov() const;

private:
	
};

}
#endif
