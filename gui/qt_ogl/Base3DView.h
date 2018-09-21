/*
 *  Base3DView.h
 *  aloe
 *
 */

#ifndef APH_BASE3DVIEW_H
#define APH_BASE3DVIEW_H

#include <QGLWidget>

class QTimer;

namespace alo {

class BaseCamera;
class PerspectiveCamera;
class GLHUD;
class Vector3F;
class Ray;

class Base3DView : public QGLWidget
{
    Q_OBJECT

public:
    
    Base3DView(QWidget* parent = 0);
    virtual ~Base3DView();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    QTimer* internalTimer();
	BaseCamera* getCamera() const;
	BaseCamera* perspCamera();
	BaseCamera* orthoCamera();
	const Ray* getIncidentRay() const;
	
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    
    void resetView();
	
	void updateOrthoProjection();
	void updatePerspProjection();
	
	QPoint lastMousePos() const;
	
	void usePerspCamera(bool transferSpace = true);
	void useOrthoCamera(bool transferSpace = true);
	
	float deltaTime();
	float elapsedTime() const;
	
public slots:
	
protected:

	virtual void processSelection(QMouseEvent *event);
    virtual void processDeselection(QMouseEvent *event);
    virtual void processMouseInput(QMouseEvent *event);
	virtual void processCamera(QMouseEvent *event);
	virtual void clientInit();
    virtual void clientDraw();
    virtual void clientSelect(QMouseEvent *event);
    virtual void clientDeselect(QMouseEvent *event);
    virtual void clientMouseInput(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void focusInEvent(QFocusEvent * event);
	virtual void focusOutEvent(QFocusEvent * event);
	
	const Vector3F strokeVector(const float & depth) const;
	
	void hudText(const std::string & t, const int & row) const;
	float frameRate();
	
	BaseCamera * camera();
	
	virtual void resetPerspViewTransform();
	virtual void resetOrthoViewTransform();
	
private:

	void computeIncidentRay(int x, int y);
	
private:

	QPoint m_lastPos;
    QColor m_backgroundColor;
	BaseCamera* fCamera;
	BaseCamera* m_orthoCamera;
	PerspectiveCamera* m_perspCamera;
	Ray* m_incidentRay;
	QTimer *m_timer;
	int m_dx, m_dy;
	char m_isFocused;
	GLHUD* m_hud;
	long m_startTime, m_lastTime;
    
};

}
#endif        //  #ifndef BASE3DVIEW_H

