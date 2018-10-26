#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <qt_ogl/View3DWidget.h>
#include <qt_ogl/DrawableScene.h>

class GLWidget : public alo::View3DWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    
    alo::DrawableScene *scene();

public slots:
    void cleanup();

signals:

protected:
    virtual void clientInit();
    virtual void clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);

private:

	alo::DrawableScene *m_scene;

};

#endif
