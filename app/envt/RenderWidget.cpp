/* RenderWidget.cpp
 * aloe
 *
 * environment test
 */
 
#include <QtGui>

#include <math.h>
#include <math/PerspectiveCamera.h>
#include "RenderWidget.h"
#include <qt_interface/RenderThread.h>
#include <qt_interface/QImgRenderInterface.h>
#include "EnvironScene.h"
#include "IblRenderer.h"

using namespace alo;

RenderWidget::RenderWidget(QWidget *parent)
    : QWidget(parent)
{
    m_scene = new EnvironScene;
	m_perspCamera = new PerspectiveCamera;
	static const float mm[16] = {1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					0.f, 0.f, 100.f, 1.f};
	Matrix44F mat(mm);
	m_perspCamera->setViewTransform(mat, 100.f);

	m_interface = new QImgRenderInterface;
    m_interface->setScene(m_scene);
	m_renderer = new IblRenderer;
	m_interface->setRenderer(m_renderer);
	m_interface->setCamera(m_perspCamera);
	m_interface->createImage(1024, 800);
	thread = new RenderThread(m_interface);
	
    connect(thread, SIGNAL(renderedImage()),
            this, SLOT(updatePixmap()));

}

bool RenderWidget::readImage(const char* filename)
{ 
	EnvironScene* envs = static_cast<EnvironScene* > (m_scene);
	return envs->readImage(filename); 
}

void RenderWidget::setImageExposure(const float& x)
{ m_renderer->setImageExposure(x); }

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
	thread->render();
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
	thread->render();
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

const alo::Scene* RenderWidget::scene() const
{ return m_scene; }
