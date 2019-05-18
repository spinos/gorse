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
    m_thre = 0.001f;
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
		//this->m_thre = 1e10f;
		condition.wakeAll();
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
	//this->m_thre = 0.001f;
	start(LowPriority);
}

void RenderThread::rerender()
{
	m_interface->updateDisplayView();
	m_interface->updateScene();
	
	emit preRenderRestart();

	this->m_abort = false;
	//this->m_thre = 0.001f;
	start(LowPriority);

}

void RenderThread::render()
{
	bool imageResized = m_interface->imageSizeChanged();
	bool viewChanged = m_interface->cameraChanged();
	
	if(imageResized || viewChanged)
		interruptAndRestart(imageResized);

}

void RenderThread::stopRender()
{ 
	interruptRender();
}

int RenderThread::renderWork(BufferBlock* packet, RenderBuffer *buf)
{
	Renderer* tracer = m_interface->getRenderer();
	RenderContext* ctx = m_interface->getContext();
	DisplayImage* dspImg = m_interface->image();

	if(m_abort) {
			return 0;
		}

    tracer->renderFragment(buf, *ctx, *packet);

    if(m_abort) {
			return 0;
		}

    buf->reproject(*ctx, *packet);

    if(m_abort) {
			return 0;
		}

    packet->projectImage(dspImg);  
    return 1; 
}

void RenderThread::run()
{
    for(;;) {

    	if(m_abort) {
			//qDebug() << " abort run ";
			break;
		}

    	m_interface->sortBlocks();
    	const float rd = m_interface->residual();
    	
    	if(rd < m_thre ) {
    		//qDebug() << " residual " << rd << " < " << m_thre;
    		break;
    	}

    	interface::GlobalFence &fence = interface::GlobalFence::instance();
		boost::lock_guard<interface::GlobalFence> guard(fence);
        
#define NUM_FUTURE 6
        BufferBlock* packets[NUM_FUTURE];
        m_interface->selectBlocks(packets, NUM_FUTURE);
        
		Renderer* tracer = m_interface->getRenderer();
		RenderContext* ctx = m_interface->getContext();
		DisplayImage* dspImg = m_interface->image();

		RenderBuffer *buf[NUM_FUTURE];

		for(int i=0;i<NUM_FUTURE;++i) {
			buf[i] = m_interface->renderBuffer(i); 
		}

		QFuture<int> ff[NUM_FUTURE];

		for(int i=0;i<NUM_FUTURE;++i) {
			ff[i] = QtConcurrent::run(this, &RenderThread::renderWork, packets[i], buf[i]);
		}

		for(int i=0;i<NUM_FUTURE;++i) {
			ff[i].waitForFinished();
		}

/*       
		buf[0] = m_interface->renderBuffer(0); 
		renderWork(packets[0], buf[0]);
*/
		emit renderedImage();

    }

}

}
