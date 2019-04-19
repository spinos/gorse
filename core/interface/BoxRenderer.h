/*
 *  BoxRenderer.h
 *  aloe
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BOX_RENDERER_H
#define BOX_RENDERER_H

#include "Renderer.h"

namespace alo {

class BoxRenderer : public Renderer {

public:
	BoxRenderer();
	virtual ~BoxRenderer();
	
	virtual void renderFragment(RenderBuffer *buffer, RenderContext& context, BufferBlock& blk) const override;

private:

    static void NormalToBGR(float* bgr, const float* nml);
    
};

}

#endif
