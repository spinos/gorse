/*
 *  mean.h
 *  aloe
 *
 *  Created by jian zhang on 3/21/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_MATH_MEAN_H
#define ALO_MATH_MEAN_H

namespace alo {

template<typename T>
inline T mean(T * y, int n)
{
    T mu = 0;
    for(int i=0;i<n;++i) {
        mu += y[i];
    }
	
	mu /= (T)n;
	return mu;
}

}

#endif