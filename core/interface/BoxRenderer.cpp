/*
 *  BoxRenderer.cpp
 *  aloe
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "BoxRenderer.h"
#include "BufferBlock.h"
#include "RenderContext.h"
#include "PixelSampler.h"
#include "RenderableScene.h"
#include <math/Ray.h>
#include <math/Matrix44F.h>
#include <math/BoundingBox.h>
#include "ImageFragment.h"
#include <math/miscfuncs.h>
#include <img/SampleState.h>
#include "IntersectResult.h"

namespace alo {

BoxRenderer::BoxRenderer()
{}

BoxRenderer::~BoxRenderer()
{}

void BoxRenderer::NormalToBGR(float* bgr, const float* nml)
{
    bgr[0] = nml[0] * .5f + .5f;
    bgr[1] = nml[1] * .5f + .5f;
    bgr[2] = nml[2] * .5f + .5f;
}

void BoxRenderer::renderFragment(RenderContext& context, BufferBlock& blk)
{
	const PixelSampler* pxsamp = context.sampler();
	pxsamp->generateViewRays(blk, prng() );
    
    RenderableScene* scn = context.scene();
	IntersectResult result;
/// from context?
    SampleState colorState;
	colorState.setAccess(SampleState::Wrap, SampleState::Clamp);
	colorState.setFilter(SampleState::Box);
	colorState.setChannel(SampleState::RGB);
	
	float col[4];
    Ray rayi;
	const int& ns = blk.numSamples();
	for(int i=0;i<ns;++i) {

        rayi.set(blk.viewRay(i) );
		if(scn->intersectRay(rayi, result) ) {
            NormalToBGR(col, (const float* )&result.hitNormal() );
            col[3] = 1.f;
        } else {
			scn->getBackgroundColor(col, rayi.m_dir, &colorState);
		}
		
		setFragmentColor(col, i);
	}
	reproject(context, blk);
}

}
