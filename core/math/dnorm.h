/*
 *  dnorm.h
 *  aloe
 *
 *  normal distribution has density
 *  f(x) = 1/sqrt(2PI)sigma exp (-((x-mu)^2/(2sigma^2))
 *  with mean equal to mu
 *  standard deviation equal to sd
 *  https://stat.ethz.ch/R-manual/R-devel/library/stats/html/Normal.html
 *  http://mathworld.wolfram.com/NormalDistribution.html
 *
 *  Created by jian zhang on 3/21/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_MATH_DNORM_H
#define ALO_MATH_DNORM_H

#include <cmath>

namespace alo {

#define INVSQRT2PI 0.398942280401 

template<typename T>
inline T dnorm(T x, T mu, T sd)
{
	T xmmu = x - mu;
	return exp( xmmu*xmmu/(sd*sd*-2.) ) / sd * INVSQRT2PI;
}

}

#endif
