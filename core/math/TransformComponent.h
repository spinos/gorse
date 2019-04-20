/*
 *  TransformComponent.h
 *  gorse
 *
 *  2019/4/20
 */

#ifndef ALO_TRANSFORM_COMPONENT_H
#define ALO_TRANSFORM_COMPONENT_H

#include "Matrix44F.h"

namespace alo {

class TransformComponent {

	Vector3F m_position;
	Vector3F m_rotation;
	Vector3F m_scale;

	Matrix44F m_tm;
	Matrix44F m_invTm;

public:
	TransformComponent();
	virtual ~TransformComponent();

protected:

	Vector3F &position();
	Vector3F &rotation();
	Vector3F &scale();

	void calculateSpace();

private:

};

}

#endif
