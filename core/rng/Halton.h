/*
 *  Halton.h
 *  aloe
 *  
 *  pseudo random number in (0,1) in R^2
 *
 *  Created by jian zhang on 2/6/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_HALTON_H
#define ALO_HALTON_H

namespace alo {

class Halton {

static const float Halton32Tabel[1024][2];

public:

	template<typename T>
	static const float* Rand2(T* rng);
	
};

template<typename T>
const float* Halton::Rand2(T* rng)
{
	int j = rng->rand(1024);
	return Halton32Tabel[j];
}

}

#endif