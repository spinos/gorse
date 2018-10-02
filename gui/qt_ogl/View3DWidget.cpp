/*
 *  View3DWidget.cpp
 *  aloe
 *
 */

#include "View3DWidget.h"
#include <QMouseEvent>

namespace alo {

View3DWidget::View3DWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
	setFocusPolicy(Qt::ClickFocus);
}

View3DWidget::~View3DWidget() 
{}

QSize View3DWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize View3DWidget::sizeHint() const
{
    return QSize(500, 500);
}

void View3DWidget::initializeGL()
{
    resetCamera();
    initializeOpenGLFunctions();
    qDebug()<<(char *)glGetString(GL_VERSION);
    qDebug()<<(char *)glGetString(GL_SHADING_LANGUAGE_VERSION );
    QOpenGLContext * ctx = context();
    //qDebug()<<" "<<ctx->extensions();
    //qDebug()<<(char *)getExtension("OES_standard_derivatives");
    
    glClearColor(0.37, 0.37, 0.37, 1);
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

	clientInit();
}

void View3DWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	clientDraw(projectionMatrix(), cameraMatrix());
	
}

void View3DWidget::resizeGL(int width, int height)
{
    setPortSize(width, height);
    calcProjectionMatrix();
    //qDebug()<<"res cam"<<
    calcCameraMatrix();
    clientResize(width, height);
}

void View3DWidget::mousePressEvent(QMouseEvent *event)
{
    resetCursorPos(event->pos());
}


void View3DWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->modifiers() == Qt::AltModifier)
        processCamera(event);
    update();
}

void View3DWidget::processCamera(QMouseEvent *event)
{
    updateCursorPos(event->pos());
    
    if (event->buttons() & Qt::LeftButton) {
        tumble();
    } 
	else if (event->buttons() & Qt::MidButton) {
        track();
    }
	else if (event->buttons() & Qt::RightButton) {
		//if(getCamera()->isOrthographic())
		//	updateOrthoProjection();
		//else
		//	updatePerspProjection();
        zoom();
    }
    //qDebug()<<"cam"<<
    calcCameraMatrix();
}

void View3DWidget::clientInit() {}

void View3DWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam) {}

void View3DWidget::clientResize(int width, int height) {}

}
//:~
