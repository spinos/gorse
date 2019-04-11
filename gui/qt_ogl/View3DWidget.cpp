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
/// must QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
/// before QApplication
    //qDebug()<<" glb ogl ctx"<<QOpenGLContext::globalShareContext();
    initializeOpenGLFunctions();
    qDebug()<<(char *)glGetString(GL_VERSION);
    qDebug()<<(char *)glGetString(GL_SHADING_LANGUAGE_VERSION );
    
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
    calcCameraMatrix();
    calcCameraFrustum();
    clientResize(width, height);
    CameraEvent e = getCameraEvent();
    e.setProgressMode(CameraEvent::MInProgress);
    emit cameraChanged(e);
}

void View3DWidget::mousePressEvent(QMouseEvent *event)
{
    resetCursorPos(event->pos());
    if(event->modifiers() == Qt::AltModifier) {
        CameraEvent e = getCameraEvent();
        e.setProgressMode(CameraEvent::MBegin);
        emit cameraChanged(e);
    }
}

void View3DWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->modifiers() == Qt::AltModifier) {
        CameraEvent e = getCameraEvent();
        e.setProgressMode(CameraEvent::MEnd);
        emit cameraChanged(e);
    }
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
        zoom();
    }
    calcCameraMatrix();
    calcCameraFrustum();
    CameraEvent e = getCameraEvent();
    e.setProgressMode(CameraEvent::MInProgress);
    emit cameraChanged(e);
}

void View3DWidget::clientInit() {}

void View3DWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam) {}

void View3DWidget::clientResize(int width, int height) {}

void View3DWidget::keyPressEvent(QKeyEvent *event)
{
    bool viewChanged = false;
    switch (event->key()) {
        case Qt::Key_H:
        case Qt::Key_Home:
            resetCamera();
            viewChanged = true;
            break;
        case Qt::Key_F:
            emit requestBound();
            break;
        default:
            break;
    }
    if(viewChanged) {
        update();
        CameraEvent e = getCameraEvent();
        e.setProgressMode(CameraEvent::MInProgress);
        emit cameraChanged(e);
    }
}

void View3DWidget::recvBound(const Hexahedron &x)
{
    const float d = x.size();
/// invalid bound
    if(d < .9f) return;
    const Vector3F c = x.center();
    frameAll(c, d);
    update();
    CameraEvent e = getCameraEvent();
    e.setProgressMode(CameraEvent::MInProgress);
    emit cameraChanged(e);
}

}
