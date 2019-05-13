/*
 *  RenderThread.cpp
 *  vachellia
 *  
 *
 *  2019/5/11
 */
 
#include "RenderThread.h"
#include <interface/RenderInterface.h>
#include <interface/BufferBlock.h>
#include <interface/Renderer.h>
#include <interface/RenderBuffer.h>
#include <interface/DisplayImage.h>
#include <interface/GlobalFence.h>
#include <QtConcurrent/QtConcurrent>
#include <boost/thread/lock_guard.hpp>
#include <QDebug>
#include <QFuture>

namespace alo {

RenderThread::RenderThread(RenderInterface* interf, QObject *parent)
    : QThread(parent)
{
	m_interface = interf;
    m_abort = false;
}

RenderThread::~RenderThread()
{
    interruptRender();
}

void RenderThread::interruptRender()
{
	if(isRunning() ) {
		mutex.lock();
		this->m_abort = true;
		condition.wakeOne();
		mutex.unlock();

		wait();
	}
}

void RenderThread::interruptAndRestart(bool toResizeImage)
{
	interruptRender();
	
	if(toResizeImage)
		m_interface->createImage(m_interface->resizedImageWidth(),
							m_interface->resizedImageHeight() );
	
	m_interface->updateDisplayView();

	emit preRenderRestart();
	
	this->m_abort = false;
	start(LowPriority);
}

void RenderThread::rerender()
{
	m_interface->updateDisplayView();
	m_interface->updateScene();
	
	if (isRunning()) return;
	
	emit preRenderRestart();

	this->m_abort = false;
	start(LowPriority);

}

void RenderThread::render()
{
	bool imageResized = m_interface->imageSizeChanged();
	bool viewChanged = m_interface->cameraChanged();
	
	if(imageResized || viewChanged)
		interruptAndRestart(imageResized);

}

void RenderThread::renderWork(BufferBlock* packet, RenderBuffer *buf)
{
	Renderer* tracer = m_interface->getRenderer();
	RenderContext* ctx = m_interface->getContext();
	DisplayImage* dspImg = m_interface->image();

    tracer->renderFragment(buf, *ctx, *packet);
    buf->reproject(*ctx, *packet);
    packet->projectImage(dspImg);   
}

void RenderThread::run()
{
    forever {
				
		if(m_abort) break;
		
		if(m_interface->isResidualLowEnough() ) break;

		interface::GlobalFence &fence = interface::GlobalFence::instance();
		boost::lock_guard<interface::GlobalFence> guard(fence);

        m_interface->sortBlocks();
        
        BufferBlock* packets[8];
        m_interface->selectBlocks(packets, 8);

        if(m_abort) break;
        
		Renderer* tracer = m_interface->getRenderer();
		RenderContext* ctx = m_interface->getContext();
		DisplayImage* dspImg = m_interface->image();

#define NUM_FUTURE 6
		RenderBuffer *buf[NUM_FUTURE];
		QFuture<void> ff[NUM_FUTURE];

		for(int i=0;i<NUM_FUTURE;++i) {
			buf[i] = m_interface->renderBuffer(i); 
			ff[i] = QtConcurrent::run(this, &RenderThread::renderWork, packets[i], buf[i]);
		}

		for(int i=0;i<NUM_FUTURE;++i) {
			ff[i].waitForFinished();
		}
        
		emit renderedImage();

    }
}

}
