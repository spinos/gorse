/*
 *  Hammersley.h
 *  aloe
 *
 *  pre-computed using formula
 *  http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
 *
 *  Created by jian zhang on 3/24/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_HAMMERSLEY_H
#define ALO_HAMMERSLEY_H

namespace alo {
	
class Hammersley {

	static const float Table[8192][2];
	
public:
	
/// cast the dice
	template<typename T>
	static const float* Rand2(T* rng);
	
};

template<typename T>
const float* Hammersley::Rand2(T* rng)
{
	int j = rng->rand(8192);
	return Table[j];
}

}

#endif
