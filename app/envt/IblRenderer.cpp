/*
 *  IblRenderer.cpp
 *  aloe
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "IblRenderer.h"
#include <interface/BufferBlock.h>
#include <interface/RenderContext.h>
#include <interface/PixelSampler.h>
#include <interface/Scene.h>
#include <math/Ray.h>
#include <math/Matrix44F.h>
#include <math/BoundingBox.h>
#include <interface/ImageFragment.h>
#include <math/miscfuncs.h>
#include <img/SampleState.h>
#include <interface/Shader.h>

namespace alo {

IblRenderer::IblRenderer() : m_exposure(0)
{
	m_colorState = new SampleState;
	m_colorState->setAccess(SampleState::Wrap, SampleState::Clamp);
	m_colorState->setFilter(SampleState::Box);
	m_colorState->setChannel(SampleState::RGB);
	m_colorState->setExposure(-2.47393f);
	m_shader = new Shader;
}

IblRenderer::~IblRenderer()
{}

void IblRenderer::setImageExposure(const float& x)
{ 
	m_exposure = x; 
	m_colorState->setExposure(x-2.47393f);
}

void IblRenderer::renderFragment(RenderContext& context, BufferBlock& blk)
{
	const PixelSampler* pxsamp = context.sampler();
	pxsamp->generateViewRays(blk, prng() );
    
    const Scene* scn = context.scene();
	IntersectResult result;
	result.radianceState()->setExposure(m_exposure);
	
	float col[4];
    Ray rayi;
	const int& ns = blk.numSamples();
	for(int i=0;i<ns;++i) {

        rayi.set(blk.viewRay(i) );
			
		if(scn->intersectRay(rayi, result) ) {
		    m_shader->evaluate(col, rayi, result, prng(), scn);
	
        } else {
			scn->getBackgroundColor(col, rayi.m_dir, m_colorState);
		}
		
		setFragmentColor(col, i);
	}
	reproject(context, blk);
}

}
