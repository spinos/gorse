#include "CameraEvent.h"
#include <math/BaseCamera.h>
#include <math/AFrustum.h>

namespace alo {

CameraEvent::CameraEvent() {}

CameraEvent::CameraEvent(BaseCamera *camera, AFrustum *frustum) :
m_camera(camera),
m_frustum(frustum)
{}

void CameraEvent::setProgressMode(ProgressMode mode)
{ m_mode = mode; }

const AFrustum *CameraEvent::frustum() const
{ return m_frustum; }

const BaseCamera *CameraEvent::camera() const
{ return m_camera; }

CameraEvent::ProgressMode CameraEvent::progressMode() const
{ return m_mode; }

}
