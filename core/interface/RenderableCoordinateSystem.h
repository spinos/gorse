/*
 *  RenderableCoordinateSystem.h
 *  vachellia
 *
 */

#ifndef VCHL_RENDERABLE_COORDINATE_SYSTEM_H
#define VCHL_RENDERABLE_COORDINATE_SYSTEM_H

#include "RenderableObject.h"
#include <math/Matrix44F.h>

namespace alo {

class RenderableSphere;
class RenderableCylinder;
class RenderableCone;

class RenderableCoordinateSystem : public RenderableObject {

	Matrix44F m_tm;
	Matrix44F m_invTm;
	RenderableSphere *m_sphere;
	RenderableCylinder *m_axis[3];
	RenderableCone *m_arrow[3];
	
public:
	RenderableCoordinateSystem();
	virtual ~RenderableCoordinateSystem();

	virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

};

}

#endif
