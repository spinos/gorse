/*
 *  IblRenderer.h
 *  aloe
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef IBL_RENDERER_H
#define IBL_RENDERER_H

#include <interface/Renderer.h>

namespace alo {

class SampleState;
class Shader;

class IblRenderer : public Renderer {

	SampleState* m_colorState;
	Shader* m_shader;
	float m_exposure;

public:
	IblRenderer();
	virtual ~IblRenderer();
	
	void setImageExposure(const float& x);
	
	virtual void renderFragment(RenderContext& context, BufferBlock& blk);

private:

};

}

#endif
