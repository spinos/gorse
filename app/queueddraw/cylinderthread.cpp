#include "cylinderthread.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <geom/YCylinder.h>
#include <geom/ABox.h>
#include <bvh/BVH.h>
#include <ctime>

using namespace alo;

CylinderThread::CylinderThread(DrawableScene *scene, BVH *mbvh, QObject *parent)
    : QThread(parent)
{
    restart = false;
    abort = false;
    m_scene = scene;
    m_bvh = mbvh;
    
    std::time_t secs = std::time(nullptr);
    m_rng = new Uniform<Lehmer>(secs);
}

CylinderThread::~CylinderThread()
{
    m_scene->lock();
    abort = true;
    condition.wakeOne();
    m_scene->unlock();

    wait();
}

void CylinderThread::recvCameraChange()
{
    //QMutexLocker locker(&mutex);

    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}

void CylinderThread::run()
{
    int i = 0;
    for(;;) {
        addCylinder(i++);
        msleep(10);
        
        //if (restart)
        //    break;
        if (abort)
            return;
    }
}


void CylinderThread::addCylinder(int it)
{
    bool added = false;
    CylHostD *ac;
    if(it<100) {
        ac = new CylHostD;
        m_hostData.push_back(ac);
        added = true;
    } else {
        ac = m_hostData[it % 100];
    }

    //YCylinder c;
    //c.createCylinder(16, 8, 1.f, 3.f);
    ABox c;
    BoundingBox box;
    Vector3F v0(m_rng->randf1() * 5.f - 5.f,
                m_rng->randf1() * 5.f - 5.f,
                m_rng->randf1() * 5.f - 5.f);
    box.setMin(v0.x, v0.y, v0.z);
    box.setMax(v0.x + m_rng->randf1() * 5.f + 5.f, 
                v0.y + m_rng->randf1() * 4.f + 3.f , 
                v0.z + m_rng->randf1() * 5.f + 4.f );
    c.updateBox(box);
    c.createPositionNormalArray(ac->posnml);
    c.createBarycentricCoordinates(ac->baryc);
    
    if(added) {
        DrawableObject *cly = new DrawableObject;
        cly->setDrawId(it);
        cly->setPosnml((const float *)ac->posnml.c_data(), ac->posnml.capacityByteSize());
        cly->setBarycentric((const float *)ac->baryc.c_data(), ac->baryc.capacityByteSize());
        cly->setDrawArrayLength(c.numIndices());
        cly->move(m_rng->randf1() * 30.f + .1f * m_hostData.size(), 
            m_rng->randf1() * 20.f, 
            m_rng->randf1() * 30.f);
        ac->dr = cly;
        m_scene->enqueueCreateDrawable(cly, 99);
    } else {
        DrawableObject *cly = ac->dr;
        cly->move(m_rng->randf1() * 2.f - 1.f, 
            m_rng->randf1() * 2.f - 1.f, 
            -.001f * it);
    }

}
