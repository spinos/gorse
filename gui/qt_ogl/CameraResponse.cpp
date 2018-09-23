#include "CameraResponse.h"
#include <math/PerspectiveCamera.h>

namespace alo {

CameraResponse::CameraResponse() 
{ m_persp = new PerspectiveCamera;}

CameraResponse::~CameraResponse() 
{ delete m_persp; }

void CameraResponse::resetCamera(const Matrix44F &mat)
{ m_persp->setViewTransform(mat, 100.f); }

void CameraResponse::resetCamera()
{
    static const float mm[16] = {1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					0.f, 0.f, 100.f, 1.f};
	Matrix44F mat(mm);
    resetCamera(mat);
}

void CameraResponse::resetCursorPos(const QPoint &pos)
{ m_lastPos = pos; }

void CameraResponse::updateCursorPos(const QPoint &pos)
{ 
    m_deltaPos = pos - m_lastPos; 
    m_lastPos = pos;
}

void CameraResponse::tumble()
{ m_persp->tumble(m_deltaPos.x(), m_deltaPos.y() ); }

void CameraResponse::track()
{ m_persp->track(m_deltaPos.x(), m_deltaPos.y() ); }

void CameraResponse::zoom()
{ m_persp->zoom(-m_deltaPos.x() / 2 - m_deltaPos.y() / 2 ); }

void CameraResponse::setPortSize(int width, int height)
{
    m_persp->setPortWidth(width);
	m_persp->setPortHeight(height);
}

const QMatrix4x4 &CameraResponse::calcProjectionMatrix()
{
    m_proj.setToIdentity();

	float left,right,bottom,top;
		
	right = m_persp->frameWidth() * 0.5f;
	left = -right;
	top = m_persp->frameHeight() * 0.5f;
	bottom = -top;

    m_proj.frustum(left, right, bottom, top, m_persp->nearClipPlane(), m_persp->farClipPlane());
    
    return m_proj;
}

const QMatrix4x4 &CameraResponse::calcCameraMatrix()
{ 
    const Matrix44F &invmat = m_persp->fInverseSpace;
    m_camera = QMatrix4x4(invmat.v); 
    return m_camera;
}

const QMatrix4x4 &CameraResponse::projectionMatrix() const
{ return m_proj; }

const QMatrix4x4 &CameraResponse::cameraMatrix() const
{ return m_camera.transposed(); }
    
}