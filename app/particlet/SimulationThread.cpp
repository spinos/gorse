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
#include <pbd/ShapeMatchingSolver.h>
#include <pbd/UniformGridCollisionSolver.h>
#include <math/BoundingBox.h>

struct SimulationThread::Impl {
    
    QMutex _mutex;
    QWaitCondition _condition;
    bool _toAbort;
    alo::ShapeMatchingSolver *_sms;
    alo::UniformGridCollisionSolver *_coll;
    
    Impl() : _toAbort(false)
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

void SimulationThread::setShapeMatchingSolver(alo::ShapeMatchingSolver *x)
{ m_pimpl->_sms = x; }

void SimulationThread::setCollisionSolver(alo::UniformGridCollisionSolver *x)
{ m_pimpl->_coll = x; }

void SimulationThread::run()
{
    for(;;) {
        
        if(m_pimpl->_toAbort) break;
        
        QMutexLocker fence(&m_pimpl->_mutex);
        
        const QTime t0 = QTime::currentTime();

        alo::ShapeMatchingSolver *smSolver = m_pimpl->_sms;
        alo::UniformGridCollisionSolver *cls = m_pimpl->_coll;
        cls->setGridBox(smSolver->aabb());
        
        const float dt = 1.f / 60.f;
		
        smSolver->applyGravity(alo::Vector3F(0.f, -98.f * dt, 0.f));
        smSolver->applyDamping(.002f);
		
		cls->resolveCollision();
		
		smSolver->projectPosition(dt);
		
        smSolver->applyPositionConstraint();
		
        smSolver->integrate(dt);
		
		cls->resolveCollision(true);
        
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
