#include "RenderWidget.h"
#include <math/PerspectiveCamera.h>
#include <QMouseEvent>
#include <QPainter>
#include <qt_interface/RenderThread.h>
#include <qt_interface/QImgRenderInterface.h>
#include <interface/RenderableScene.h>
#include <interface/BoxRenderer.h>
#include <qDebug>

using namespace alo;

RenderWidget::RenderWidget(RenderableScene *scene, QWidget *parent)
    : QWidget(parent)
{
	m_scene = scene;

    m_perspCamera = new PerspectiveCamera;
    Matrix44F mat;
    mat.rotateX(-0.5f);
    mat.translate(0.f, 120.f * sin(.5f), 120.f * cos(.5f));
    m_perspCamera->setViewTransform(mat, 120.f);

    m_interface = new QImgRenderInterface;
    m_interface->setScene(m_scene);
    m_renderer = new BoxRenderer;
    m_interface->setRenderer(m_renderer);
    m_interface->setCamera(m_perspCamera);
    m_interface->createImage(1024, 800);
    m_thread = new RenderThread(m_interface);
    
    connect(m_thread, SIGNAL(renderedImage()),
            this, SLOT(updatePixmap()));
}

RenderWidget::~RenderWidget()
{
    delete m_thread;
}

QSize RenderWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize RenderWidget::sizeHint() const
{
    return QSize(500, 500);
}

void RenderWidget::recvRerender()
{ 
//qDebug() << "RenderWidget::recvRerender";	
    m_thread->render();
}

void RenderWidget::paintEvent(QPaintEvent * /* event */)
{
//qDebug()<<" paint event";
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    
    painter.drawImage(QPoint(), m_interface->getQImage() );

}

void RenderWidget::updatePixmap()
{
    update();
}

void RenderWidget::resizeEvent(QResizeEvent * /* event */)
{
    const int pw = size().width();
    const int ph = size().height();
    m_perspCamera->setPortWidth(pw);
    m_perspCamera->setPortHeight(ph);
    m_interface->setResizedImage(pw, ph);
    m_thread->render();
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastMousePos = event->pos();
    if(event->modifiers() == Qt::AltModifier) 
        return;
    
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->modifiers() == Qt::AltModifier)
        processCamera(event);
        
    m_lastMousePos = event->pos();
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
}

void RenderWidget::processCamera(QMouseEvent *event)
{
    m_dx = event->x() - m_lastMousePos.x();
    m_dy = event->y() - m_lastMousePos.y();
    if (event->buttons() & Qt::LeftButton) {
        m_perspCamera->tumble(m_dx, m_dy);
    } 
    else if (event->buttons() & Qt::MidButton) {
        m_perspCamera->track(m_dx, m_dy);
    }
    else if (event->buttons() & Qt::RightButton) {
        m_perspCamera->zoom(-m_dx / 2 + -m_dy / 2);
    }
    
    m_interface->setChangedCamera();
    m_thread->render();
}

void RenderWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        break;
    default:
        ;
    }   
    QWidget::keyPressEvent(event);
}

void RenderWidget::recvBound(const Hexahedron &x)
{
    const float d = x.size();
/// invalid bound
    if(d < .9f) return;
    const Vector3F c = x.center();
    //frameAll(c, d);
    update();
    //CameraEvent e = getCameraEvent();
    //e.setProgressMode(CameraEvent::MInProgress);
    //emit cameraChanged(e);
}
