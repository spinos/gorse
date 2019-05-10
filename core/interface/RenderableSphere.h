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

	void setRadius(float x);
	void setPoint(const Vector3F &p);

	virtual bool intersectRay(const Ray& aray, IntersectResult& result) const override;

};

}

#endif
