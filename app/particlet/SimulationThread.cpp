/*
 *  SimulationThread.cpp
 *  particlet
 *
 *  2019/10/5
 */

#include "SimulationThread.h"
#include <QMutex>
#include <QWaitCondition>
#include <QTime>
#include <pbd/ParticleSystem.h>
#include <math/Vector3F.h>

struct SimulationThread::Impl {
    
    QMutex _mutex;
    QWaitCondition _condition;
    bool _toAbort;
    alo::ParticleSystem *_particles;
    
    Impl() : _toAbort(false),
    _particles(0)
    {}
    
};

SimulationThread::SimulationThread(QObject *parent)
    : QThread(parent),
m_pimpl(new Impl)
{
}

SimulationThread::~SimulationThread()
{
    stopSimulation();
}

void SimulationThread::stopSimulation()
{
    if(isRunning() ) {
		m_pimpl->_toAbort = true;
		m_pimpl->_condition.wakeAll();
		wait();
	}
}

void SimulationThread::setParticles(alo::ParticleSystem *x)
{ m_pimpl->_particles = x; }

void SimulationThread::run()
{
    for(;;) {
        
        if(m_pimpl->_toAbort) break;
        
        QMutexLocker fence(&m_pimpl->_mutex);
        
        const QTime t0 = QTime::currentTime();

        alo::ParticleSystem *particles = m_pimpl->_particles;
        const float dt = .016f;
        particles->addGravity(alo::Vector3F(0.f, -9.8f * dt, 0.f));
        particles->projectPosition(dt);
        
        particles->updateVelocityAndPosition(dt);
        
        const QTime t1 = QTime::currentTime();
        const int elapsedMs = t1.msecsTo(t0);
        
        fence.unlock();
        
        if(elapsedMs < 16) {
            msleep(16 - elapsedMs);
        }
        emit doneStep();
    }
}

QMutex &SimulationThread::mutex()
{ return m_pimpl->_mutex; }
