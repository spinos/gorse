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
#include <QDebug>

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
		
		BufferBlock* packet = m_interface->selectBlock();
		Renderer* tracer = m_interface->getRenderer();
		RenderContext* ctx = m_interface->getContext();
		
		tracer->renderFragment(*ctx, *packet);
					
		packet->projectImage(m_interface->image() );

		emit renderedImage();

        mutex.lock();
       // if (!restart)
		//condition.wait(&mutex);
			
        //restart = false;
        mutex.unlock();
    }
}

}
