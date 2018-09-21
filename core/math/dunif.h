/*
 *  dunif.h
 *  aloe
 *
 *  uniform distribution has density
 *  f(x) = 1/(max - min) 
 *  for min <= x <= max
 *  https://stat.ethz.ch/R-manual/R-devel/library/stats/html/Uniform.html
 *
 *  Created by jian zhang on 3/21/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_MATH_DUNIF_H
#define ALO_MATH_DUNIF_H

#include <cmath>

namespace alo {

template<typename T>
inline T dunif(T x, T lmin, T lmax)
{
	if(x<lmin)
		return 0;
	if(x>lmax)
		return 0;
		
	return 1.0 / ((T)lmax - (T)lmin);
}

}

#endif
