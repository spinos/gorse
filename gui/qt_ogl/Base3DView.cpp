/*
 *  Base3DView.cpp
 *  aloe
 *
 */
 
#include <QtGui>
#include <QtOpenGL>

#include "Base3DView.h"
#include <math/Ray.h>
#include <math/PerspectiveCamera.h>
#include <ogl/GLHUD.h>
#include "boost/date_time/posix_time/posix_time.hpp"
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

namespace alo {

const boost::posix_time::ptime time_t_epoch(boost::gregorian::date(2010,1,1));

Base3DView::Base3DView(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    m_backgroundColor = QColor::fromCmykF(0.29, 0.29, 0.20, 0.0);
	m_orthoCamera = new BaseCamera;
	m_perspCamera = new PerspectiveCamera;
	fCamera = m_orthoCamera;
	m_incidentRay = new Ray;
	m_timer = new QTimer(this);
	//connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer->start(34);
	setFocusPolicy(Qt::ClickFocus);
	m_isFocused = 0;
		
	m_hud = new GLHUD;
	m_hud->setCamera(fCamera);
	
	m_lastTime = m_startTime = (boost::posix_time::ptime(boost::posix_time::microsec_clock::local_time()) - time_t_epoch).total_milliseconds();
}

Base3DView::~Base3DView()
{
	delete fCamera;
}

QTimer * Base3DView::internalTimer() { return m_timer; }

QSize Base3DView::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize Base3DView::sizeHint() const
{
    return QSize(640, 480);
}

BaseCamera * Base3DView::getCamera() const
{
	return fCamera;
}

BaseCamera * Base3DView::perspCamera()
{
	return m_perspCamera;
}

BaseCamera * Base3DView::orthoCamera()
{
	return m_orthoCamera;
}

const Ray* Base3DView::getIncidentRay() const
{	
	return m_incidentRay;
}

void Base3DView::initializeGL()
{
#ifdef WIN32   
    if(gExtensionInit()) qDebug()<<"GL Extensions checked.";
#endif
	qglClearColor(m_backgroundColor.dark());

    glShadeModel(GL_SMOOTH);
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    // glEnable(GL_MULTISAMPLE);
	glDepthFunc(GL_LEQUAL);
	m_hud->reset();
	
	clientInit();
}

void Base3DView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glLoadIdentity();

    float m[16];
	getCamera()->getMatrix(m);
	glMultMatrixf(m);
	
	//getDrawer()->m_markerProfile.apply();
	//getDrawer()->coordsys(20.f);

	//getDrawer()->setAlignDir(getCamera()->eyeDirection() );
	
	clientDraw();
	return;
	if(m_isFocused) {
		Vector3F corners[4];
		getCamera()->frameCorners(corners[0], corners[1], corners[2], corners[3]);
		glColor3f(0.f, 0.f, 1.f);
		glBegin(GL_LINE_LOOP);
		glVertex3f(corners[0].x, corners[0].y, corners[0].z);
		glVertex3f(corners[1].x, corners[1].y, corners[1].z);
		glVertex3f(corners[2].x, corners[2].y, corners[2].z);
		glVertex3f(corners[3].x, corners[3].y, corners[3].z);
		glEnd();
	}
	glFlush();
}

void Base3DView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
	m_perspCamera->setPortWidth(width);
	m_perspCamera->setPortHeight(height);
	m_orthoCamera->setPortWidth(width);
	m_orthoCamera->setPortHeight(height);
	if(getCamera()->isOrthographic())
		updateOrthoProjection();
	else
		updatePerspProjection();
}

void Base3DView::mousePressEvent(QMouseEvent *event)
{	
    m_lastPos = event->pos();
    if(event->modifiers() == Qt::AltModifier) 
        return;
    
    processSelection(event);
}

void Base3DView::mouseReleaseEvent(QMouseEvent *event)
{
    processDeselection(event);
}

void Base3DView::mouseMoveEvent(QMouseEvent *event)
{
    if(event->modifiers() == Qt::AltModifier)
        processCamera(event);
    else 
        processMouseInput(event);

    m_lastPos = event->pos();
}

void Base3DView::processCamera(QMouseEvent *event)
{
    m_dx = event->x() - m_lastPos.x();
    m_dy = event->y() - m_lastPos.y();
    if (event->buttons() & Qt::LeftButton) {
        getCamera()->tumble(m_dx, m_dy);
    } 
	else if (event->buttons() & Qt::MidButton) {
		getCamera()->track(m_dx, m_dy);
    }
	else if (event->buttons() & Qt::RightButton) {
		getCamera()->zoom(-m_dx / 2 + -m_dy / 2);
		if(getCamera()->isOrthographic())
			updateOrthoProjection();
		else
			updatePerspProjection();
    }
	update();
}

void Base3DView::processSelection(QMouseEvent *event)
{
	computeIncidentRay(event->x(), event->y());
	clientSelect(event);
}

void Base3DView::processDeselection(QMouseEvent *event)
{
	if(event->modifiers() != Qt::AltModifier)
		clientDeselect(event);
}

void Base3DView::processMouseInput(QMouseEvent *event)
{
    computeIncidentRay(event->x(), event->y());
	
    m_dx = event->x() - m_lastPos.x();
    m_dy = event->y() - m_lastPos.y();

    clientMouseInput(event);
}

void Base3DView::clientInit() {}

void Base3DView::clientDraw()
{
    
}

void Base3DView::clientSelect(QMouseEvent *)
{
    
}

void Base3DView::clientDeselect(QMouseEvent *)
{
    
}

void Base3DView::clientMouseInput(QMouseEvent *)
{
    
}

void Base3DView::updateOrthoProjection()
{
	makeCurrent();
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	float aspect = getCamera()->aspectRatio();
	float fov = getCamera()->fieldOfView();
	float right = fov/ 2.f;
	float top = right / aspect;

    glOrtho(-right, right, -top, top, getCamera()->nearClipPlane(), getCamera()->farClipPlane());

    glMatrixMode(GL_MODELVIEW);
	doneCurrent();
}

void Base3DView::updatePerspProjection()
{
	makeCurrent();
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	GLdouble left,right,bottom,top;
		
	right = getCamera()->frameWidth() * 0.5f;
	left = -right;
	top = getCamera()->frameHeight() * 0.5f;
	bottom = -top;

    glFrustum(left, right, bottom, top, getCamera()->nearClipPlane(), getCamera()->farClipPlane());
	
    glMatrixMode(GL_MODELVIEW);
	doneCurrent();
}

void Base3DView::resetPerspViewTransform()
{
static const float mm[16] = {1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					0.f, 0.f, 120.f, 1.f};
	Matrix44F mat(mm);
	perspCamera()->setViewTransform(mat, 120.f);
}

void Base3DView::resetOrthoViewTransform()
{
static const float mm[16] = {1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					0.f, 0.f, 120.f, 1.f};
	Matrix44F mat(mm);
	orthoCamera()->setViewTransform(mat, 120.f);
	orthoCamera()->setHorizontalAperture(120.f);
}

void Base3DView::resetView()
{
	if(getCamera()->isOrthographic()) {
		resetOrthoViewTransform();
		updateOrthoProjection();
	} else {
		resetPerspViewTransform();
		updatePerspProjection();
	}
}

void Base3DView::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_O) {
		if(getCamera()->isOrthographic()) {
			fCamera = m_perspCamera;
			fCamera->copyTransformFrom(m_orthoCamera);
			updatePerspProjection();
			m_hud->setCamera(fCamera);
		}
		else {
			fCamera = m_orthoCamera;
			fCamera->copyTransformFrom(m_perspCamera);
			updateOrthoProjection();
			m_hud->setCamera(fCamera);
		}
	}
	
	switch (e->key()) {
		case Qt::Key_Space:
			break;
		case Qt::Key_H:
			resetView();
			break;
		case Qt::Key_BracketRight:
			break;
		case Qt::Key_BracketLeft:
			break;
		case Qt::Key_Up:
			getCamera()->moveForward(23);
			break;
		case Qt::Key_Down:
			getCamera()->moveForward(-23);
			break;
		case Qt::Key_G:
			break;
		default:
			break;
	}
	update();
	QWidget::keyPressEvent(e);
}

void Base3DView::keyReleaseEvent(QKeyEvent *event)
{
	QWidget::keyReleaseEvent(event);
}

void Base3DView::focusInEvent(QFocusEvent * event)
{
	m_isFocused = 1;
	m_timer->start(34);
	QGLWidget::focusInEvent(event);
}

void Base3DView::focusOutEvent(QFocusEvent * event)
{
	m_isFocused = 0;
	m_timer->stop();
	QGLWidget::focusOutEvent(event);
}

void Base3DView::computeIncidentRay(int x, int y)
{
	getCamera()->incidentRay(x, y, m_incidentRay->m_origin, m_incidentRay->m_dir);
    m_incidentRay->m_dir.normalize();
	m_incidentRay->m_tmin = 0.f;
    m_incidentRay->m_tmax = getCamera()->farClipPlane();
}

QPoint Base3DView::lastMousePos() const
{
	return m_lastPos;
}

void Base3DView::usePerspCamera(bool transferSpace)
{
	if(getCamera()->isOrthographic()) {
		fCamera = m_perspCamera;
		if(transferSpace) {
			fCamera->copyTransformFrom(m_orthoCamera);
		}
		updatePerspProjection();
	}
		
}

void Base3DView::useOrthoCamera(bool transferSpace)
{
	if(!getCamera()->isOrthographic()) {
		fCamera = m_orthoCamera;
		if(transferSpace) {
			fCamera->copyTransformFrom(m_perspCamera);
		}
		updateOrthoProjection();
	}
}

const Vector3F Base3DView::strokeVector(const float & depth) const
{
	Vector3F res;
	getCamera()->screenToWorldVectorAt(m_dx, m_dy, depth, res);
	return res;
}

void Base3DView::hudText(const std::string & t, const int & row) const
{
	m_hud->drawString(t, row);
}

float Base3DView::deltaTime() 
{
	long tnow = (boost::posix_time::ptime(boost::posix_time::microsec_clock::local_time()) - time_t_epoch).total_milliseconds();
	float dt = ((float) ( tnow - m_lastTime ) );
	m_lastTime = tnow;
	return dt; 
}

float Base3DView::elapsedTime() const 
{
	long tnow = (boost::posix_time::ptime(boost::posix_time::microsec_clock::local_time()) - time_t_epoch).total_milliseconds();
	return ((float) ( tnow - m_startTime ) ); 
}

float Base3DView::frameRate()
{ return 1000.f/deltaTime(); }

BaseCamera * Base3DView::camera()
{ return fCamera; }

}
//:~
