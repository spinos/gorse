/*
 *  prng.h
 *  aloe interface
 *
 *  Created by jian zhang on 3/26/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_INTERFACE_PRNG_H
#define ALO_INTERFACE_PRNG_H

#include <rng/Uniform.h>
#include <rng/Lehmer.h>

namespace alo {

typedef Uniform<Lehmer> PrngTyp;

}
#endif
