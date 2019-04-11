/*
 *  RenderableCone.h
 *  vachellia
 *
 */

#ifndef VCHL_RENDERABLE_CONE_H
#define VCHL_RENDERABLE_CONE_H

#include "RenderableObject.h"
#include <math/Vector3F.h>

namespace alo {

class RenderableCone : public RenderableObject {

	Vector3F m_p0;
	float m_r;
	Vector3F m_p1;
	float m_l;
	Vector3F m_dir;
	
public:
	RenderableCone();

	virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

};

}

#endif
