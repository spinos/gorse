#include "CameraResponse.h"
#include <math/PerspectiveCamera.h>
#include <math/AFrustum.h>
#include <math/CameraEvent.h>

namespace alo {

CameraResponse::CameraResponse() 
{
    m_frustum = new AFrustum; 
    m_persp = new PerspectiveCamera;
}

CameraResponse::~CameraResponse() 
{ 
    delete m_frustum;
    delete m_persp; 
}

void CameraResponse::resetCamera(const Matrix44F &mat)
{ 
    m_persp->setViewTransform(mat, 100.f); 
    calcCameraMatrix();
    calcCameraFrustum();
}

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
	
	const float d = .5f * m_persp->nearClipPlane() / m_persp->focusDistance();
		
	right = m_persp->frameWidth() * d;
	left = -right;
	top = m_persp->frameHeight() * d;
	bottom = -top;

    m_proj.frustum(left, right, bottom, top, m_persp->nearClipPlane(), m_persp->farClipPlane());
    
    return m_proj;
}

const QMatrix4x4 &CameraResponse::calcCameraMatrix()
{ 
    const Matrix44F &invmat = m_persp->inverseSpace();
    m_camera = QMatrix4x4(invmat.v);
    m_camera = m_camera.transposed();
    return m_camera;
}

void CameraResponse::calcCameraFrustum()
{
    m_frustum->set(m_persp->tanhfov(),
           m_persp->heightWidthRatio(),
           -m_persp->nearClipPlane(),
           -m_persp->farClipPlane(),
           m_persp->space() );
}

const QMatrix4x4 &CameraResponse::projectionMatrix() const
{ return m_proj; }

const QMatrix4x4 &CameraResponse::cameraMatrix() const
{ return m_camera; }

CameraEvent CameraResponse::getCameraEvent() const
{ return CameraEvent(m_persp, m_frustum); }

void CameraResponse::frameAll(const Vector3F &center, const float &width)
{
    const float preFarClip = m_persp->farClipPlane();

    m_persp->lookAt(Float4(center.x, center.y, center.z, width * .5f ));
    
    if(preFarClip < m_persp->farClipPlane()) {
        std::cout << " INFO extend camera far clipping plane to " << m_persp->farClipPlane();
        calcProjectionMatrix();
    }
    calcCameraMatrix();
    calcCameraFrustum();
}
   
}