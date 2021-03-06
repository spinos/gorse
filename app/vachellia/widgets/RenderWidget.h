#ifndef VCHL_RENDER_WIDGET_H
#define VCHL_RENDER_WIDGET_H

#include <QWidget>
//#include <QMatrix4x4>
#include <QPixmap>
#include <math/CameraEvent.h>
Q_DECLARE_METATYPE(alo::CameraEvent)
#include <math/Hexahedron.h>
Q_DECLARE_METATYPE(alo::Hexahedron)
#include <math/Float4.h>
Q_DECLARE_METATYPE(alo::Float4)

namespace alo {
class PerspectiveCamera;
class RenderThread;
class QImgRenderInterface;
class RenderableScene;
class Renderer;
}

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    RenderWidget(alo::RenderableScene *scene, QWidget *parent = 0);
    ~RenderWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    //void setDisplaySolid();
    //void setDisplayWire();

public slots:
    void recvStopRender();
    void recvUpdateScene();
    void recvRerender();
    void recvBound(const alo::Hexahedron &x);
    void recvFocusCameraOn(const alo::Float4 &centerRadius);

signals:
    void cameraChanged(const alo::CameraEvent &x);
    void requestBound();
    void preRenderRestart();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void updatePixmap();
    void recvPreRenderRestart();
    
private:
    void processCamera(QMouseEvent *event);

	alo::RenderableScene *m_scene;
    alo::Renderer* m_renderer;
    alo::PerspectiveCamera* m_perspCamera;
    alo::QImgRenderInterface* m_interface;
    alo::RenderThread* m_thread;
    
    QPoint m_lastMousePos;
    int m_dx, m_dy;
};

#endif
