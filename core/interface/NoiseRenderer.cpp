/*
 *  NoiseRenderer.cpp
 *  aloe
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "NoiseRenderer.h"
#include "BufferBlock.h"
#include "ImageFragment.h"
#include "RenderBuffer.h"
#include <math/miscfuncs.h>

namespace alo {

NoiseRenderer::NoiseRenderer()
{}

NoiseRenderer::~NoiseRenderer()
{}

void NoiseRenderer::renderFragment(RenderBuffer *buffer, RenderContext& context, BufferBlock& blk) const
{
	float col[4];
	col[3] = 1.f;
	float rgray = RandomF01();
	col[0] = col[1] = col[2] = rgray;
/// fill renderd fragment	
	const int& ns = buffer->fragmentNumPixels();
	for(int i=0;i<ns;++i) {
		buffer->setFragmentColor(col, i);
	}
	buffer->reproject(context, blk);
}

}
