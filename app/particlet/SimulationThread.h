/*
 *  SimulationThread.h
 *  particlet
 *
 *  2019/10/5
 */

#ifndef SIMULATION_THREAD_H
#define SIMULATION_THREAD_H

#include <QThread>
#include <boost/shared_ptr.hpp>

QT_FORWARD_DECLARE_CLASS(QMutex)

namespace alo {
class ParticleSystem;
class ShapeMatchingSolver;
}

class SimulationThread : public QThread
{
    Q_OBJECT
    
public:

    SimulationThread(QObject *parent = 0);
    ~SimulationThread();
    
    void stopSimulation();
    
    void setShapeMatchingSolver(alo::ShapeMatchingSolver *x);
    
    QMutex &mutex();
    
signals:
    void doneStep();
    
protected:
    void run();
    
private:
    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
    
};

#endif
