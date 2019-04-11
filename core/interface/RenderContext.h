/*
 *  RenderContext.h
 *  aloe
 *
 *  access to sampler, projector, scene
 *
 *  Created by jian zhang on 2/3/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_RENDER_CONTEXT_H
#define ALO_RENDER_CONTEXT_H

namespace alo {

class PixelSampler;
class PixelProjector; 
class RenderableScene;

class RenderContext {

	PixelSampler* m_sampler;
	PixelProjector* m_projector;
    RenderableScene* m_scene;
	
public:

	RenderContext();
	
	void createSampler();
	void createProjector();
    void setScene(RenderableScene* x);
	
	const PixelSampler* sampler() const;
	PixelProjector* projector();
    RenderableScene* scene();
	
protected:

private:
	
};

}

#endif