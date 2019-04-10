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

class RenderThread : public QThread
{
    Q_OBJECT

public:
    RenderThread(RenderInterface* interf, QObject *parent = 0);
    ~RenderThread();

    void render();
	
signals:
    void renderedImage();
	
protected:
    void run();

private:
	void interruptAndResize();
	void interruptAndReview();
	void interruptRender();
	
private:
    QMutex mutex;
    QWaitCondition condition;
/// loop again
	bool m_restart;
/// end before loop
    bool m_abort;
	
	RenderInterface* m_interface;
	
};

}

#endif
