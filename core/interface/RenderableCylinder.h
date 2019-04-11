/*
 *  RenderableCylinder.h
 *  vachellia
 *
 */

#ifndef VCHL_RENDERABLE_CYLINDER_H
#define VCHL_RENDERABLE_CYLINDER_H

#include "RenderableObject.h"
#include <math/Vector3F.h>

namespace alo {

class RenderableCylinder : public RenderableObject {

	Vector3F m_p0;
	float m_r;
	Vector3F m_p1;
	Vector3F m_dir;
	
public:
	RenderableCylinder();

	virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

};

}

#endif
