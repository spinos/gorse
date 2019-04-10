/*
 *  AbstractBrdf.h
 *  
 *
 *  Created by jian zhang on 3/31/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_ABSTRACT_BRDF_H
#define ALO_ABSTRACT_BRDF_H

#include <math/Matrix33F.h>
#include <math/miscfuncs.h>

namespace alo {

class SpaceTransform;

class AbstractBrdf {

	const SpaceTransform* m_space;
/// outgoing (view) direction in tangent space
	Vector3F m_wo;
/// specular space reflection direction as z-axis
	Matrix33F m_specularSpace;
	
public:

	AbstractBrdf(const SpaceTransform* tm);

/// eyed from eye to surface in world 
	void setWo(const Vector3F& eyed);
/// N in world space
	Vector3F surfaceNormal() const;
/// wo in world space
	Vector3F viewDirection() const;
/// specular in world 
	Vector3F reflectionDirection() const;
	
protected:

/// cos(theta(wo, n))
	const float& wodotn() const;
/// half-vector (i+o)/||i+o||
/// v.h <- cos(theta(wo, hr))
	Vector3F halfVector(const Vector3F& wi, float& vdoth) const;
	
	void toWorldInPlace(Vector3F& wi) const;
	void toLocalDirectionReversed(Vector3F& wo, const Vector3F& eyed) const;
	void specularToLocalInPlace(Vector3F& wi) const;

private:

	void computeSpecularSpace();
	
};

}

#endif