/*
 *  CameraEvent.h
 *  aloe
 *
 *  for view-dependent response
 *  contains camera, frustum, and progress mode
 *  call
 *  Q_DECLARE_METATYPE(alo::CameraEvent)
 *  and
 *  qRegisterMetaType<alo::CameraEvent>();
 *  before it can be usded as a signal value
 *
 */

#ifndef ALO_CAMERA_EVENT_H
#define ALO_CAMERA_EVENT_H

namespace alo {

class BaseCamera;
class AFrustum;
	
class CameraEvent {

	BaseCamera *m_camera;
	AFrustum *m_frustum;

public:

	enum ProgressMode {
		MBegin = 0,
		MInProgress,
		MEnd
	};
	
	CameraEvent();
	CameraEvent(BaseCamera *camera, AFrustum *frustum);
	
	void setProgressMode(ProgressMode mode);

	const AFrustum *frustum() const;
	const BaseCamera *camera() const;
	ProgressMode progressMode() const;

private:

	ProgressMode m_mode;

};

}

#endif
