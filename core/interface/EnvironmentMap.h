/*
 *  EnvironmentMap.h
 *  aloe
 *
 *  Pdf for sampled direction
 *  updf * vpdf / (2 PI^2 sinTheta)
 *  infinite area light source with importance sampling
 *
 *  Created by jian zhang on 3/25/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_ENVIRONMENT_MAP_H
#define ALO_ENVIRONMENT_MAP_H

#include <math/Matrix33F.h>
#include <math/miscfuncs.h>
#include <img/GaussianPyramid.h>

namespace alo {

template <typename T>
class EnvironmentMap {

	GaussianPyramid<T, 5> m_mipmap;
	Matrix33F m_space;
	Matrix33F m_invSpace;
	
public:
	
	EnvironmentMap();
	~EnvironmentMap();
	
	template<typename Timg>
	bool read(const Timg& image);
	
/// wi world direction
	void lookup(float* li, const Vector3F& wi, SampleState* state) const;

/// r [0,1] in 2D
/// wi in world
	void latlongToWorld(Vector3F& wi, const float* r) const;
	void worldToLatlong(float* r, const Vector3F& wi) const;
	
	const Array2<Vector3F >& levelImage(int i) const;

protected:
    
private:

};

template <typename T>
EnvironmentMap<T>::EnvironmentMap()
{
/// rotate theta = 0 to world up
	Quaternion q(HALFPIF, Vector3F::XAxis);
	q.normalize();
	m_space = Matrix33F(q);
	m_invSpace = m_space;
	m_invSpace.inverse();
}

template <typename T>
EnvironmentMap<T>::~EnvironmentMap()
{}

template <typename T>
template<typename Timg>
bool EnvironmentMap<T>::read(const Timg& image)
{ 
	SampleState state;
	state.setAccess(SampleState::Wrap, SampleState::Clamp);
	state.setFilter(SampleState::Nofilter);
	state.setChannel(SampleState::RGB);
	m_mipmap. template create<Timg>(image, state);
	return true;
}

template <typename T>
void EnvironmentMap<T>::lookup(float* li, const Vector3F& wi, SampleState* state) const
{
	worldToLatlong(state->p(), wi);
	m_mipmap.sample(li, *state);
}

template <typename T>
void EnvironmentMap<T>::latlongToWorld(Vector3F& wi, const float* r) const
{
	float phi = r[0] * TWO_PIF;
	float theta = r[1] * PIF;
	float sinTheta = sin(theta);
/// top is theta = 0
/// bottom is theta = PI 
	wi.set(cos(phi) * sinTheta,
			sin(phi) * sinTheta,
			-cos(theta) );
	m_space.transformInPlace(wi);
}

template <typename T>
void EnvironmentMap<T>::worldToLatlong(float* r, const Vector3F& wi) const
{
	Vector3F localV;
	m_invSpace.transform1(localV, wi);
	ToSphericalCoord(r, (const float* )&localV);
	r[0] *= ONE_OVER_2PIF;
	if(r[0] > .999999f) r[0] = .999999f;
	r[1] *= ONE_OVER_PIF;
	if(r[1] > .999999f) r[1] = .999999f;
}

template <typename T>
const Array2<Vector3F >& EnvironmentMap<T>::levelImage(int i) const
{ return m_mipmap.levelSignal(i); }

}

#endif
