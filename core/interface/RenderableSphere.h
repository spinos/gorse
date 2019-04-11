/*
 *  RenderableSphere.h
 *  vachellia
 *
 */

#ifndef VCHL_RENDERABLE_SPHERE_H
#define VCHL_RENDERABLE_SPHERE_H

#include "RenderableObject.h"
#include <math/Vector3F.h>

namespace alo {

class RenderableSphere : public RenderableObject {

	Vector3F m_center;
	float m_radius;
	
public:
	RenderableSphere();

	virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

};

}

#endif
