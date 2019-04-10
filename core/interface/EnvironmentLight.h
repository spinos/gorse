/*
 *  EnvironmentLight.h
 *  aloe
 *
 *  Created by jian zhang on 3/28/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_ENVIRONMENT_LIGHT_H
#define ALO_ENVIRONMENT_LIGHT_H

#include "EnvironmentMap.h"
#include <math/Distribution2D.h>
#include <math/convolute.h>

namespace alo {

template <typename T>
class EnvironmentLight : public EnvironmentMap<T> {

	Distribution2D m_distrib;
	VectorN<float> m_sinThetaTable;
	
public:

	EnvironmentLight();
	~EnvironmentLight();

	template<typename Timg>
	bool read(const Timg& image);
	
	void getRadiance(Vector3F& contrib, float& pdf, SampleState* state, const Vector3F& wi) const;

	void getPdf(float& pdf, const Vector3F& wi) const;
	
	void lookupRadiance(Vector3F& contrib, const float& levelOfDetail, SampleState* state, const Vector3F& wi) const;

/// wi incoming direction in world space
/// lr light radiance
/// pdf pobability density function
/// pdf(phi,theta) <- pdf(u,v) / sin(theta)
/// Ts pseudo random number sequence in 2D
/// Tr pseudo random number generator
/// Tb check fr of b 
	template<typename Ts, typename Tr, typename Tb>
	bool sample(Vector3F& wi, Vector3F& lr, float& pdf, SampleState* state, const Tb* b, Tr* rng) const;
	
private:

};

template <typename T>
EnvironmentLight<T>::EnvironmentLight()
{}

template <typename T>
EnvironmentLight<T>::~EnvironmentLight()
{}

template <typename T>
template<typename Timg>
bool EnvironmentLight<T>::read(const Timg& image)
{
	if(!EnvironmentMap<T>:: template read<Timg>(image) ) return false;
/// latlong
	const int& nu = image.imageWidth();
	const int& nv = image.imageHeight();
	m_distrib.create(nu, nv);
	SampleState state;
	state.setAccess(SampleState::Clamp, SampleState::Clamp);
	state.setFilter(SampleState::Nofilter);
	state.setChannel(SampleState::Luminance);
	state.setExposure(-2.47393f);
	
	m_sinThetaTable.create(nv);
	const float dv = 1.f / (float)nv;
/// table of sin(theta) [0, PI]
	for(int i = 0;i<nv;++i) {
		m_sinThetaTable[i] = sin(PIF * (float)(i + .5f) * dv);
	}
	
	VectorN<float> src;
	src.create(nv);
	static const float kern7tap[7] = {0.004711, 0.069321, 0.245410, 0.361117, 0.245410, 0.069321, 0.004711};
	
	float col[3];
	for(int i = 0;i<nu;++i) {
		state.u()[0] = i;
		
		for(int j = 0;j<nv;++j) {
			state.v()[0] = j;
			
			image.sample(col, state);

/// separate by 2 
			if(col[0] <= 2.f)
/// cannot be zero
			    src[j] = col[0] + 1e-2f;
			else
			    src[j] = log2f(col[0]) + 1.f;

			src[j] *= m_sinThetaTable[j];

		}
		
		convolute<float>(m_distrib.vpdf(i), src.v(), nv, kern7tap, 7);
	}
	
	m_distrib.compute();
	
	for(int i = 0;i<nv;++i) {
		m_sinThetaTable[i] = 1.f / m_sinThetaTable[i];
	}
	return true;
}

template <typename T>
void EnvironmentLight<T>::getRadiance(Vector3F& contrib, float& pdf, SampleState* state, const Vector3F& wi) const
{
	EnvironmentMap<T>::lookup((float*) &contrib, wi, state);
	m_distrib.lookup(pdf, state->u()[0], state->v()[0]);
	pdf *= m_sinThetaTable[state->v()[0]];
}

template <typename T>
void EnvironmentLight<T>::getPdf(float& pdf, const Vector3F& wi) const
{
    float p[2];
    EnvironmentMap<T>::worldToLatlong(p, wi);
    int u = p[0] * m_distrib.uDim(); 
    int v = p[1] * m_distrib.vDim();
    m_distrib.lookup(pdf, u, v);
	pdf *= m_sinThetaTable[v];
}

template <typename T>
void EnvironmentLight<T>::lookupRadiance(Vector3F& contrib, const float& levelOfDetail, SampleState* state, const Vector3F& wi) const
{
    state->setLevelOfDetail(levelOfDetail);
    EnvironmentMap<T>::lookup((float*) &contrib, wi, state);
}

template <typename T>
template<typename Ts, typename Tr, typename Tb>
bool EnvironmentLight<T>::sample(Vector3F& wi, Vector3F& lr, float& pdf, SampleState* state, const Tb* b, Tr* rng) const
{
	const float* r = Ts::Rand2(rng);
	m_distrib.sample(state->p(), pdf, r);
	EnvironmentMap<T>::latlongToWorld(wi, state->p() );
	if(!b->checkDirection(wi)) return false;
	
	EnvironmentMap<T>::lookup((float* )&lr, wi, state);
	pdf *= m_sinThetaTable.sample(state->p()[1]);
	return true;
}

}

#endif
