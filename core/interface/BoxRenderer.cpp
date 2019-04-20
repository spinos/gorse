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
#include "RenderBuffer.h"
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

void BoxRenderer::renderFragment(RenderBuffer *buffer, RenderContext& context, BufferBlock& blk) const
{
	const PixelSampler* pxsamp = context.sampler();
	pxsamp->generateViewRays(blk, buffer->prng() );
    
    RenderableScene* scn = context.scene();
	IntersectResult &result = *buffer->primaryIntersction();
    SampleState &colorState = *buffer->evironmentLatlongSampleState();

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
		
		buffer->setFragmentColor(col, i);
	}
}

}
