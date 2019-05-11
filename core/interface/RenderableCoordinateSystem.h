/*
 *  RenderableCoordinateSystem.h
 *  vachellia
 *
 */

#ifndef VCHL_RENDERABLE_COORDINATE_SYSTEM_H
#define VCHL_RENDERABLE_COORDINATE_SYSTEM_H

#include "RenderableObject.h"
#include <math/TransformComponent.h>

namespace alo {

class RenderableSphere;
class RenderableCylinder;
class RenderableCone;

class RenderableCoordinateSystem : public RenderableObject, public TransformComponent {

	RenderableSphere *m_sphere;
	RenderableCylinder *m_axis[3];
	RenderableCone *m_arrow[3];
	
public:
	RenderableCoordinateSystem();
	virtual ~RenderableCoordinateSystem();

	virtual bool intersectRay(IntersectResult& result) const override;

};

}

#endif
