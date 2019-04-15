/*
 *  RenderInterface.h
 *  aloe
 *  
 *  access to camera, image, buffer, renderer, context
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RENDER_INTERFACE_H
#define RENDER_INTERFACE_H

namespace alo {

class BaseCamera;
class DisplayCamera;
class DisplayImage;
class DeepBuffer;
class BufferBlock;
class Renderer;
class RenderContext;
class RenderableScene;

class RenderInterface {

	DisplayCamera* m_camera;
	DeepBuffer* m_buffer;
	DisplayImage* m_image;
	RenderContext* m_context;
	Renderer* m_renderer;
	int m_resizedImageDim[2];
	
public:

	RenderInterface();
	
    void setScene(RenderableScene* x);
	void setRenderer(Renderer* x);
	void setCamera(BaseCamera* x);
	void setChangedCamera();
	bool cameraChanged() const;
	bool sceneChanged() const;
	
	bool imageSizeChanged() const;
	void createImage(int w, int h);
	void setResizedImage(int w, int h);
	int resizedImageWidth() const;
	int resizedImageHeight() const;
/// set frame in each block	and restart
	void updateDisplayView();
	void updateScene();
/// by high residual
	BufferBlock* selectBlock();
	DisplayImage* image();
    const DisplayImage* c_image() const;

typedef unsigned char uchar;
	
	uchar* imageScanline(int i);
	
	const int& xres() const;
	const int& yres() const;
	int bufferNumBlocks() const;
	
	Renderer* getRenderer();
	
	RenderContext* getContext();
	
/// quality threshold
	bool isResidualLowEnough() const;
	
};

}

#endif
