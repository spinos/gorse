/*
 *  PixelSampler.h
 *  aloe
 *  
 *  sample rays in block
 *
 *  Created by jian zhang on 8/9/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PIXEL_SAMPLER_H
#define PIXEL_SAMPLER_H

#include "prng.h"

namespace alo {

class BufferBlock;

class PixelSampler {

public:
	PixelSampler();
	virtual ~PixelSampler();
	
	virtual void generateViewRays(BufferBlock& blk, PrngTyp* prng) const;
	
};

}

#endif
