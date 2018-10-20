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

namespace alo {
    
class View3DWidget : public QOpenGLWidget, protected QOpenGLFunctions, public CameraResponse
{
    Q_OBJECT

public:
    
    View3DWidget(QWidget* parent = 0);
    virtual ~View3DWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
	
protected:

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
	
	virtual void processCamera(QMouseEvent *event);
	virtual void clientInit();
    virtual void clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);
    virtual void clientResize(int width, int height);
    
private:

};

}
#endif        //  #ifndef ALO_VIEW3DWIDGET_H

