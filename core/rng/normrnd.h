/*
 *  normrnd.h
 *  aloe
 *
 *  generates a random number from the normal distribution 
 *  with mean parameter mu and standard deviation parameter sigma.
 *
 *  Created by jian zhang on 3/21/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_NORM_RND_H
#define ALO_NORM_RND_H

#include <cmath>
namespace alo {

/// Box-Muller transform
template<typename T, typename Tg>
inline T normrnd(T mu, T sigma, Tg* rng)
{
	const T epsilon = 1e-6;
	const T two_pi = 6.283185307179586;

	static T z0, z1;
	static bool generate;
	generate = !generate;

	if (!generate)
	   return z1 * sigma + mu;

	double u1, u2;
	do
	 {
	   u1 = rng->randf(0, 1, 1<<24);
	   u2 = rng->randf(0, 1, 1<<24);
	 }
	while ( u1 <= epsilon );

	z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
	z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
	return z0 * sigma + mu;
}

}

#endif