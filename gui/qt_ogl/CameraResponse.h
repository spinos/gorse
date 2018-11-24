#ifndef ALO_CAMERA_RESPONSE_H
#define ALO_CAMERA_RESPONSE_H

#include <QMatrix4x4>

namespace alo {
    
class AFrustum;
class PerspectiveCamera;
class Matrix44F;
class CameraEvent;

class CameraResponse {

public:

    CameraResponse();
    virtual ~CameraResponse();
    
    void resetCamera(const Matrix44F &mat);
    void resetCamera();
    const QMatrix4x4 &projectionMatrix() const;
    const QMatrix4x4 &cameraMatrix() const;
    
protected:
    void setPortSize(int width, int height);
    void resetCursorPos(const QPoint &pos);
    void updateCursorPos(const QPoint &pos);
    void tumble();
    void track();
    void zoom();
    const QMatrix4x4 &calcProjectionMatrix();
    const QMatrix4x4 &calcCameraMatrix();
    void calcCameraFrustum();
    CameraEvent getCameraEvent() const;
	
private:

    AFrustum *m_frustum;
    PerspectiveCamera *m_persp;
    QPoint m_lastPos;
	QPoint m_deltaPos;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    
};

}

#endif

