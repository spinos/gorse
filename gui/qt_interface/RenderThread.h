/*
 *  RenderThread.h
 *  aloe
 *  
 *
 *  Created by jian zhang on 8/8/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_RENDERTHREAD_H
#define ALO_RENDERTHREAD_H

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

namespace alo {

class RenderInterface;
class BufferBlock;
class Renderer;
class RenderContext;
class RenderBuffer;
class DisplayImage;

class RenderThread : public QThread
{
    Q_OBJECT

public:
    RenderThread(RenderInterface* interf, QObject *parent = 0);
    ~RenderThread();

    void rerender();
    void render();
    void stopRender();
	
signals:
    void renderedImage();
    void preRenderRestart();
	
protected:
    void run();

private:
	void interruptRender();
	void interruptAndRestart(bool toResizeImage);
	
private:
    QMutex mutex;
    QWaitCondition condition;
/// loop again
	//bool m_restart;
/// end before loop
    bool m_abort;
    float m_thre;
	
	RenderInterface* m_interface;
    
    int renderWork(BufferBlock* packet, RenderBuffer *buf);
    
};

}

#endif
