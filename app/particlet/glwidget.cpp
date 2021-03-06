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
#include <pbd/SampleEmitter.h>
#include <pbd/UniformGridCollisionSolver.h>
#include <smp/Triangle.h>
#include <smp/SurfaceSample.h>
#include <sds/SpaceFillingVector.h>
#include <geom/GeodesicSphere.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>

GLWidget::GLWidget(QWidget *parent) : alo::View3DWidget(parent),
m_thread(new SimulationThread),
m_renderer(new ParticleRenderer),
m_particle(new alo::ParticleSystem),
m_sphere(new alo::ParticleSystem),
m_solver(new alo::ShapeMatchingSolver),
m_collider(new alo::UniformGridCollisionSolver)
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
    typedef alo::SurfaceSample SampleTyp;
    typedef alo::SampleInterp<SampleTyp> SampleInterpTyp;
    typedef alo::smp::Triangle<SampleTyp > SamplerTyp;
    SamplerTyp sampler;
	sampler.setSampleSize(.2f);
    
    typedef alo::sds::SpaceFillingVector<SampleTyp> PntArrTyp;
	PntArrTyp pnts;
    
    SampleTyp asmp;
    SampleInterpTyp interp;
    
    alo::GeodesicSphere transient;
    transient.scaleBy(23.f);
    transient.translateBy(1.f, -10.f, -11.f);
    
    alo::Uniform<alo::Lehmer> lmlcg(654123);
    
    const int nt = transient.numTriangles();
    for(int i=0;i<nt;++i) {
        transient.getTriangle<SampleTyp, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterpTyp, alo::Uniform<alo::Lehmer> >(pnts, asmp, interp, &lmlcg);
    }
    
    alo::BoundingBox shapeBox;
    transient.getAabb(shapeBox);
    
    alo::SampleEmitter surf;
    surf.setSamples(&pnts);
    surf.setSampleBox(shapeBox, 1.9f);
    surf.update();
    
    m_sphere->create(surf);
    m_sphere->setIsDynamic(false);
    m_collider->addParticles(m_sphere);
    
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    m_renderer->initializeGL();
    
	m_solver->setStiffness(40000);
    m_solver->clearRegions();
    
    alo::CurveEmitter curve;
    for(int j=0;j<32;++j) {
        curve.begin();
        for(int i=0;i<50;++i) {
            curve << alo::Vector3F(-1.5f + 2.f * i, 25.f + 1.f * j, -1.f * j - 1.f * i);
        }
        curve.end();
        curve.update();
        const int offset = m_particle->numParticles();
        m_particle->create(curve);
        m_solver->addRegions(curve, offset);
    }
    m_particle->updateAabb();
    m_collider->addParticles(m_particle);
    m_collider->setCellSize(4.f);
    
    m_solver->setParticles(m_particle);
    m_solver->createConstraints();
    
    connect(m_thread, &SimulationThread::doneStep, this, &GLWidget::recvDoneStep);
    
    m_thread->setShapeMatchingSolver(m_solver);
    m_thread->setCollisionSolver(m_collider);
    m_thread->start();
}

void GLWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    QMutexLocker fence(&m_thread->mutex());
    
    m_renderer->setParticles(m_particle);
    m_renderer->paintGL(proj, cam);
    
    m_renderer->setParticles(m_sphere);
    m_renderer->paintGL(proj, cam);
}

void GLWidget::recvDoneStep()
{ 
    update(); 
}
