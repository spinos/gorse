/*
 *  CookTorrance.h
 *  aloe
 *
 *  fr = DFG/ (4 (i . n) (o . n))
 *  pdf = D (h . n)
 *
 *  Created by jian zhang on 3/23/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_COOK_TORRANCE_H
#define ALO_COOK_TORRANCE_H

#include "AbstractBrdf.h"

namespace alo {

class CookTorrance : public AbstractBrdf {

/// roughness
	float m_alpha, m_alpha2;
	float m_k;
/// reflectance for V . H = 1
/// R0 <- ( (1 - n2) / (1 + n2) )^2
/// https://en.wikipedia.org/wiki/Schlick%27s_approximation 
	float m_R0;
/// index of refraction as n2
	float m_ior;
	
public:
	
	CookTorrance(const SpaceTransform* tm);

/// generate a pair of random numbers [0,1]
/// wi sample direction in tangent space
/// fr(o,i)
/// pdf(o,i) probability density function weight
	template<typename Ts, typename Tr>
	void sample(Vector3F& wi, float& fr, float& pdf, Tr* rng) const;
/// wi incomming direction (light) in tangent space
	float computeFr(const Vector3F& wi) const;
	float computePdf(const Vector3F& wi) const;
		
private:

	void computeR0();

/// 1 if x > 0
/// 0 if x <= 0
	float chiX(const float& x) const;

/// chiX(n . h) alpah^2 / PI / ((n . h)^2 alpha^2 - (n . h)^2 + 1 )^2
	float microfacetDistribution(const float& ndoth) const;
/// Schlick's approximation for fresnel factor in specular reflection
/// R(theta) <- R0 + (1 - R0) (1 - cos(theta) )^5
/// cosTheta is V . H
	float schlickFresnel(const float cosTheta) const;
/// G(o,h,n,alpha) G(i,h,n,alpha)
	float geometryTerm(const float& odoth, const float& idoth,
					const float& odotn, const float idotn) const;
/// chiX((v . h) / (v . n)) * 2 / (1 + sqrt(1 + alpha^2 (1 - (v . h)^2) / (v . h)^2) )
	float geometryFunc(const float& vdoth, const float vdotn) const;
	
};

template<typename Ts, typename Tr>
void CookTorrance::sample(Vector3F& wi, float& fr, float& pdf, Tr* rng) const
{
	wi.z = 0;
	while (wi.z < 2.9e-2f) {
		const float* r = Ts::Rand2(rng);
		if(r[0] < .3f) continue;
/// Microfacet Models for Refraction through Rough Surfaces 
/// sampling D(h)(h.n)
		float theta = atan(m_alpha * sqrt(r[0]) / sqrt(1.f - r[0]));
		float phi = TWOPIF * r[1];
		float sinTheta = sin(theta);
		wi.set(cos(phi) * sinTheta,
			sin(phi) * sinTheta,
			cos(theta) );
/// from specular space to tangent space
		specularToLocalInPlace(wi);
	}
	
	fr = computeFr(wi);
	pdf = computePdf(wi);
	
	toWorldInPlace(wi);
}	

}

#endif
