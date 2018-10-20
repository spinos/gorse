#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <qt_ogl/View3DWidget.h>
#include <QMatrix4x4>

namespace alo {
class DrawableScene;
}

class GLWidget : public alo::View3DWidget
{
    Q_OBJECT

public:
    GLWidget(alo::DrawableScene *scene, QWidget *parent = 0);
    ~GLWidget();

public slots:
    void cleanup();
    void recvAttribChanged();

signals:

protected:
    virtual void clientInit();
    virtual void clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);

private:

	alo::DrawableScene *m_scene;
};

#endif
