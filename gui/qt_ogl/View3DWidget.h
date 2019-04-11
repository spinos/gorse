/*
 *  View3DWidget.h
 *  aloe
 *
 */

#ifndef ALO_VIEW3DWIDGET_H
#define ALO_VIEW3DWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "CameraResponse.h"
#include <math/CameraEvent.h>
Q_DECLARE_METATYPE(alo::CameraEvent)
#include <math/Hexahedron.h>
Q_DECLARE_METATYPE(alo::Hexahedron)

namespace alo {
    
class View3DWidget : public QOpenGLWidget, protected QOpenGLFunctions, public CameraResponse
{
    Q_OBJECT

public:
    
    View3DWidget(QWidget* parent = 0);
    virtual ~View3DWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    
signals:
    void cameraChanged(const CameraEvent &x);
    void requestBound();

public slots:
    void recvBound(const Hexahedron &x);
	
protected:

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent *event);
	
	virtual void processCamera(QMouseEvent *event);
	virtual void clientInit();
    virtual void clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);
    virtual void clientResize(int width, int height);
    
private:

};

}
#endif        //  #ifndef ALO_VIEW3DWIDGET_H

