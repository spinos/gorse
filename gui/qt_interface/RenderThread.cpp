/*
 *  RenderThread.cpp
 *  aloe
 *  
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */
 
#include "RenderThread.h"
#include <interface/RenderInterface.h>
#include <interface/BufferBlock.h>
#include <interface/Renderer.h>
#include <interface/RenderBuffer.h>
#include <interface/DisplayImage.h>
#include <QtConcurrent/QtConcurrent>
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

void RenderThread::interruptAndRestart(bool toResizeImage, bool toChangeScene)
{
//qDebug()<<"interruptAndRestart";	
	interruptRender();
	
	if(toResizeImage)
		m_interface->createImage(m_interface->resizedImageWidth(),
							m_interface->resizedImageHeight() );
	
	m_interface->updateDisplayView();

	if(toChangeScene) 
		m_interface->updateScene();
	
	this->m_abort = false;
	start(LowPriority);
}

void RenderThread::render()
{
	bool imageResized = m_interface->imageSizeChanged();
	bool viewChanged = m_interface->cameraChanged();
	bool sceneChanged = m_interface->sceneChanged();
    
    //if(sceneChanged) qDebug() << " RenderThread::render scene changed ";

	if(imageResized || viewChanged || sceneChanged) {
		interruptAndRestart(imageResized, sceneChanged);
		return;
	}

	QMutexLocker locker(&mutex);
	
	this->m_abort = false;
	
	if (!isRunning()) {
        start(LowPriority);
    } else {
        condition.wakeOne();
    }
}

void RenderThread::renderWork(BufferBlock* packet, RenderBuffer *buf, Renderer* tracer, RenderContext* ctx)
{
    tracer->renderFragment(buf, *ctx, *packet);
}

void RenderThread::imageWork(BufferBlock* packet, DisplayImage* dspImg)
{
    packet->projectImage(dspImg);   
}

void RenderThread::run()
{
    forever {
		
        mutex.lock();
        
        mutex.unlock();
				
		if (m_abort) {
			//qDebug()<<" abort";
			return;
		}
		
		if(m_interface->isResidualLowEnough() ) {
			return;
		}

        m_interface->sortBlocks();
        
        BufferBlock* packets[4];
        m_interface->selectBlocks(packets, 4);
        
		Renderer* tracer = m_interface->getRenderer();
		RenderContext* ctx = m_interface->getContext();
        
        RenderBuffer *buf1 = m_interface->renderBuffer(0);
        RenderBuffer *buf2 = m_interface->renderBuffer(1);
        RenderBuffer *buf3 = m_interface->renderBuffer(2);
        RenderBuffer *buf4 = m_interface->renderBuffer(3);
        
        QFuture<void> f1 = QtConcurrent::run(this, &RenderThread::renderWork, packets[0], buf1, tracer, ctx);
        QFuture<void> f2 = QtConcurrent::run(this, &RenderThread::renderWork, packets[1], buf2, tracer, ctx);
        QFuture<void> f3 = QtConcurrent::run(this, &RenderThread::renderWork, packets[2], buf3, tracer, ctx);
        QFuture<void> f4 = QtConcurrent::run(this, &RenderThread::renderWork, packets[3], buf4, tracer, ctx);
        
        f1.waitForFinished();
        f2.waitForFinished();
        f3.waitForFinished();
        f4.waitForFinished();
/// not thread safe
        buf1->reproject(*ctx, *packets[0]);
        buf2->reproject(*ctx, *packets[1]);
        buf3->reproject(*ctx, *packets[2]);
        buf4->reproject(*ctx, *packets[3]);
        
        DisplayImage* dspImg = m_interface->image();
        
        f1 = QtConcurrent::run(this, &RenderThread::imageWork, packets[0], dspImg);
        f2 = QtConcurrent::run(this, &RenderThread::imageWork, packets[1], dspImg);
        f3 = QtConcurrent::run(this, &RenderThread::imageWork, packets[2], dspImg);
        f4 = QtConcurrent::run(this, &RenderThread::imageWork, packets[3], dspImg);
        
        f1.waitForFinished();
        f2.waitForFinished();
        f3.waitForFinished();
        f4.waitForFinished();
        
		emit renderedImage();

        mutex.lock();
       // if (!restart)
		//condition.wait(&mutex);
			
        //restart = false;
        mutex.unlock();
    }
}

}
