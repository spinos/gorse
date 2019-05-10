/*
 *  TranslateController.h
 *  vachellia
 *
 */

#ifndef VCHL_TRANSLATE_CONTROLLER_H
#define VCHL_TRANSLATE_CONTROLLER_H

#include "RenderableObject.h"
#include <math/TransformComponent.h>

namespace alo {

class RenderableSphere;
class RenderableCylinder;
class RenderableCone;

class TranslateController : public RenderableObject, public TransformComponent {

	RenderableSphere *m_sphere;
	RenderableCylinder *m_axis[3];
	RenderableCone *m_arrow[3];
	
public:
	TranslateController();
	virtual ~TranslateController();

	virtual bool intersectRay(const Ray& aray, IntersectResult& result) const override;

};

}

#endif
