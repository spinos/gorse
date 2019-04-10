/*
 *  Renderer.cpp
 *  aloe
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "Renderer.h"
#include "BufferBlock.h"
#include "RenderContext.h"
#include "ImageFragment.h"
#include "PixelProjector.h"
#include <rng/Uniform.h>
#include <rng/Lehmer.h>
#include <ctime>

namespace alo {

Renderer::Renderer()
{ 
	m_fragment = new ImageFragment;
	std::time_t secs = std::time(0);
	m_prng = new PrngTyp(secs);
}

Renderer::~Renderer()
{ 
	delete m_fragment;
	delete m_prng;
}

PrngTyp* Renderer::prng()
{ return m_prng; }

void Renderer::renderFragment(RenderContext& context, BufferBlock& blk)
{}

void Renderer::reproject(RenderContext& context, BufferBlock& blk)
{
    blk.progressAge();
	PixelProjector* prjr = context.projector();
	prjr->reproject(blk, *m_fragment);
	
}

void Renderer::setFragmentColor(const float* x, const int& i)
{ m_fragment->setColor(x, i); }

ImageFragment* Renderer::fragment()
{ return m_fragment; }

float& Renderer::pixelDepthBuffer(const int& i)
{ return m_depth[i]; }

}
