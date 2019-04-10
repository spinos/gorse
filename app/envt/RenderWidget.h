/* RenderWidget.h
 * aloe
 *
 * environment test
 */
 
#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QPixmap>
#include <QWidget>

namespace alo {

class PerspectiveCamera;
class RenderThread;
class QImgRenderInterface;
class Scene;
class IblRenderer;

}

class RenderWidget : public QWidget
{
    Q_OBJECT

public:

    RenderWidget(QWidget *parent = 0);
	bool readImage(const char* filename);
	void setImageExposure(const float& x);
	
	const alo::Scene* scene() const;

protected:

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
	
private slots:
    void updatePixmap();
	
private:
	void processCamera(QMouseEvent *event);

    alo::Scene* m_scene;
	alo::IblRenderer* m_renderer;
	alo::PerspectiveCamera* m_perspCamera;
	alo::QImgRenderInterface* m_interface;
    alo::RenderThread* thread;
	
    QPoint m_lastMousePos;
    int m_dx, m_dy;
	
};

#endif
