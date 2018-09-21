/*
 *  var.h
 *  aloe
 *
 *  Created by jian zhang on 3/21/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_MATH_VAR_H
#define ALO_MATH_VAR_H

namespace alo {

template<typename T>
inline T var(T * y, T mu, int n)
{
    T dev = 0;
    for(int i=0;i<n;++i) {
        T d = y[i] - mu;
		d *= d;
		dev += d;
    }
	
	return dev / (T)n;
}

}

#endif