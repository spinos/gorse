#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <qt_ogl/View3DWidget.h>

QT_FORWARD_DECLARE_CLASS(QThread)
QT_FORWARD_DECLARE_CLASS(QMatrix4x4)

class SimulationThread;
class ParticleRenderer;

namespace alo {
class ParticleSystem;
class ShapeMatchingSolver;
}

class GLWidget : public alo::View3DWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);

public slots:
    void cleanup();
    void recvDoneStep();

signals:

protected:
    virtual void clientInit();
    virtual void clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);

private:
    SimulationThread *m_thread;
    ParticleRenderer *m_renderer;
    alo::ParticleSystem *m_particle;
    alo::ShapeMatchingSolver *m_solver;

};

#endif
