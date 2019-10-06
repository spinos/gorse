#include "glwidget.h"
#include <QMouseEvent>
#include <QCoreApplication>
#include <QMatrix4x4>
#include <QMutexLocker>
#include "SimulationThread.h"
#include "ParticleRenderer.h"
#include <pbd/ParticleSystem.h>
#include <pbd/CurveEmitter.h>
#include <pbd/ShapeMatchingSolver.h>

GLWidget::GLWidget(QWidget *parent) : alo::View3DWidget(parent),
m_thread(new SimulationThread),
m_renderer(new ParticleRenderer),
m_particle(new alo::ParticleSystem),
m_solver(new alo::ShapeMatchingSolver)
{}

void GLWidget::cleanup()
{
    makeCurrent();
    m_renderer->cleanup();
    doneCurrent();
    m_thread->stopSimulation();
}

void GLWidget::clientInit()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    m_renderer->initializeGL();
    
    m_solver->clearRegions();
    
    alo::CurveEmitter curve;
    for(int j=0;j<20;++j) {
        curve.begin();
        for(int i=0;i<40;++i) {
            curve << alo::Vector3F(-1.5f + 2.f * i, 25.f + .57f * j, -1.35f * j - .5f * i);
        }
        curve.end();
        curve.update();
        const int offset = m_particle->numParticles();
        m_particle->create(curve);
        m_solver->addRegions(curve, offset);
    }
    
    m_solver->setParticles(m_particle);
    m_solver->createConstraints();

    m_renderer->setParticles(m_particle);
    
    connect(m_thread, &SimulationThread::doneStep, this, &GLWidget::recvDoneStep);
    
    m_thread->setShapeMatchingSolver(m_solver);
    m_thread->start();
}

void GLWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    QMutexLocker fence(&m_thread->mutex());
    m_renderer->paintGL(proj, cam);
}

void GLWidget::recvDoneStep()
{ 
    update(); 
}
