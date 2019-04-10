/*
 *  Lambert.h
 *  aloe
 *
 *  lambertian reflection
 *  fr(wi) = 1 / PI
 *  p(wi) = wi.N / PI
 *  wi reflection direction
 *  N surface normal, z-axis in tangent space
 *
 *  Created by jian zhang on 3/23/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_LAMBERT_H
#define ALO_LAMBERT_H

#include "AbstractBrdf.h"

namespace alo {

class Lambert : public AbstractBrdf {

public:

	Lambert(const SpaceTransform* tm);
	
/// generate a pair of random numbers [0,1]
/// wi incoming direction in world space
/// fr(o,i) <- 1 / PI
/// probability density function weight by cosine law
/// pdf(o,i) <- cos(theta) / PI
/// Ts pseudo random number sequence
/// Tr uniform pseudo random number generator
	template<typename Ts, typename Tr>
	void sample(Vector3F& wi, float& fr, float& pdf, float& cosTheta, Tr* rng) const;
/// wi in world space
	void getFr(float& fr, float& pdf, float& cosTheta, const Vector3F& wi) const;
/// wi in tangent space
	float computeFr(const Vector3F& wi) const;
/// wi in world space
	bool checkDirection(const Vector3F& wi) const;
	
private:

};

template<typename Ts, typename Tr>
void Lambert::sample(Vector3F& wi, float& fr, float& pdf, float& cosTheta, 
					Tr* rng) const
{
	wi.z = 0;
	for(;;) {
		const float* r = Ts::Rand2(rng);
		wi.z = std::sqrt(r[1]);
		if(wi.z < .4e-3f) continue;
		
		const float term1 = 2.f * PIF * r[0];
		const float term2 = std::sqrt(1.f - r[1]);
		
		wi.x = std::cos(term1) * term2;
		wi.y = std::sin(term1) * term2;
		
		break;
		
	}
	fr = ONE_OVER_PIF;
	pdf = wi.z * ONE_OVER_PIF;
	cosTheta = wi.z;
	toWorldInPlace(wi);
}	

}

#endif
