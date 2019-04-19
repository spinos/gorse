/*
 *  Renderer.h
 *  aloe
 *
 *  access to pseudo random number generator
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_RENDERER_H
#define ALO_RENDERER_H

namespace alo {

class BufferBlock;
class RenderContext;
class RenderBuffer;

class Renderer {
	
public:
	Renderer();
	virtual ~Renderer();

	virtual void renderFragment(RenderBuffer *buffer, RenderContext& context, BufferBlock& blk) const = 0;

protected:
	 
};

}

#endif
