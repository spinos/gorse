/*
 *  RenderInterface.cpp
 *  aloe
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "RenderInterface.h"
#include "BufferBlock.h"
#include "DeepBuffer.h"
#include "DisplayImage.h"
#include "DisplayCamera.h"
#include "NoiseRenderer.h"
#include "BoxRenderer.h"
#include "RenderContext.h"
#include "RenderBuffer.h"
#include <math/BaseCamera.h>
#include <ctime>

namespace alo {

RenderInterface::RenderInterface()
{
	m_camera = new DisplayCamera;
	m_buffer = new DeepBuffer;
	m_image = new DisplayImage;
	m_context = new RenderContext;
	m_context->createSampler();
	m_context->createProjector();
#if 0
	m_renderer = new NoiseRenderer;
#else
	m_renderer = new BoxRenderer;
#endif 
    std::time_t secs = std::time(0);
    for(int i=0;i<8;++i) {
        m_renderBuffer[i] = new RenderBuffer;
        m_renderBuffer[i]->createRng(secs + i * 10);
    }
}

RenderInterface::~RenderInterface()
{
	for(int i=0;i<8;++i)
    	delete m_renderBuffer[i];
}

void RenderInterface::setScene(RenderableScene* x)
{ m_context->setScene(x); }

void RenderInterface::setRenderer(Renderer* x)
{ m_renderer = x; }

void RenderInterface::setCamera(BaseCamera* x)
{ m_camera->setCamera(x); }

void RenderInterface::setChangedCamera()
{ m_camera->setChanged(); }

bool RenderInterface::cameraChanged() const
{ return m_camera->isChanged(); }

bool RenderInterface::imageSizeChanged() const
{
	return (xres() != m_resizedImageDim[0]
	|| yres() != m_resizedImageDim[1] );
}

bool RenderInterface::sceneChanged() const
{ return m_context->sceneChanged(); }

void RenderInterface::createImage(int w, int h)
{ 
	m_buffer->create(w, h);
	
	int rxres = m_buffer->width();
	int ryres = m_buffer->height();
	m_image->create(rxres, ryres);
	m_resizedImageDim[0] = rxres;
	m_resizedImageDim[1] = ryres;
}

void RenderInterface::setResizedImage(int w, int h)
{
	m_resizedImageDim[0] = w;
	m_resizedImageDim[1] = h;
}

int RenderInterface::resizedImageWidth() const
{ return m_resizedImageDim[0]; }

int RenderInterface::resizedImageHeight() const
{ return m_resizedImageDim[1]; }

void RenderInterface::updateDisplayView()
{
	m_camera->updateViewFrame();
	m_buffer->setBegin(m_camera);
}

DisplayImage* RenderInterface::image()
{ return m_image; }

const DisplayImage* RenderInterface::c_image() const
{ return m_image; }

uchar* RenderInterface::imageScanline(int i)
{ return m_image->scanline(i); }

const int& RenderInterface::xres() const
{ return m_image->xres(); }
	
const int& RenderInterface::yres() const
{ return m_image->yres(); }

void RenderInterface::sortBlocks()
{ m_buffer->sortByResidual(); }

void RenderInterface::selectBlocks(BufferBlock **blocks, int n)
{ m_buffer->highResidualBlocks(blocks, n); }

BufferBlock* RenderInterface::selectBlock()
{ return m_buffer->highResidualBlock(); }

int RenderInterface::bufferNumBlocks() const
{ return m_buffer->numBlocks(); }

Renderer* RenderInterface::getRenderer()
{ return m_renderer; }

RenderContext* RenderInterface::getContext()
{ return m_context; }

bool RenderInterface::isResidualLowEnough() const
{ return m_buffer->maxResidual() < 1e-9f; }

void RenderInterface::updateScene()
{ m_context->updateScene(); }

RenderBuffer *RenderInterface::renderBuffer(int i)
{ return m_renderBuffer[i]; }

}
