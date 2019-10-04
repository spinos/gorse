#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <qt_ogl/View3DWidget.h>

QT_FORWARD_DECLARE_CLASS(QMatrix4x4)

class ParticleRenderer;

class GLWidget : public alo::View3DWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

public slots:
    void cleanup();

signals:

protected:
    virtual void clientInit();
    virtual void clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);

private:

    ParticleRenderer *m_particleRenderer;

};

#endif
