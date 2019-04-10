/*
 *  SpaceTransform.h
 *  aloe
 *
 *  space by p and rotation
 *
 *  Created by jian zhang on 3/31/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SPACE_TRANSFORM_H
#define ALO_SPACE_TRANSFORM_H

#include <math/Matrix33F.h>

namespace alo {

class SpaceTransform {

	Matrix33F m_rotation;
	Matrix33F m_invRotation;
	Vector3F m_p;
	
public:

	SpaceTransform();
	
	void setPoint(const Vector3F& P);
/// N as z-axis
	void setRotation(const Vector3F& N);
	
	void toLocalDirection(Vector3F& wo, const Vector3F& wi) const;
	void toLocalDirectionReversed(Vector3F& wo, const Vector3F& wi) const;
	void toParentDirection(Vector3F& wo, const Vector3F& wi) const;
	void toParentDirectionInPlace(Vector3F& wo) const;
	void toLocalPoint(Vector3F& wi) const;
	void toParentPoint(Vector3F& wi) const;
/// mat[16]
	void getMatrix(float* mat) const;
	Vector3F frontDirection() const;
	
private:

};

}

#endif