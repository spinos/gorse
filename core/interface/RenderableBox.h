/*
 *  RenderableBox.h
 *  vachellia
 *
 */

#ifndef VCHL_RENDERABLE_BOX_H
#define VCHL_RENDERABLE_BOX_H

#include "RenderableObject.h"
#include <math/Vector3F.h>

namespace alo {

class RenderableBox : public RenderableObject {

	float m_centerHalfSpan[4];
	
public:
	RenderableBox();

	virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

};

}

#endif
