/*
 *  RenderableCapsule.h
 *  vachellia
 *
 */

#ifndef VCHL_RENDERABLE_CAPSULE_H
#define VCHL_RENDERABLE_CAPSULE_H

#include "RenderableObject.h"
#include <math/Vector3F.h>

namespace alo {

class RenderableCapsule : public RenderableObject {

	Vector3F m_p0;
	float m_r;
	Vector3F m_p1;
	float m_l;
	Vector3F m_dir;
	
public:
	RenderableCapsule();

	virtual bool intersectRay(IntersectResult& result) const override;

};

}

#endif
