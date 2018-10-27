
#ifndef CYLINDER_THREAD_H
#define CYLINDER_THREAD_H

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>
#include <rng/Uniform.h>
#include <rng/Lehmer.h>
#include <vector>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

namespace alo {
class DrawableScene;
class DrawableObject;
class BVH;
}

class CylinderThread : public QThread
{
    Q_OBJECT

public:
    CylinderThread(alo::DrawableScene *scene, alo::BVH *mbvh, QObject *parent = 0);
    ~CylinderThread();

public slots:
    void recvCameraChange();
    
signals:

protected:
    void run() override;

private:
    void addCylinder(int it);

    QWaitCondition condition;
    
    bool restart;
    bool abort;
    
    alo::DrawableScene *m_scene;
    struct CylHostD {
        alo::SimpleBuffer<alo::Vector3F> posnml;
        alo::SimpleBuffer<alo::Vector3F> baryc;
        alo::DrawableObject *dr;
    };
    std::vector<CylHostD *> m_hostData;
    alo::Uniform<alo::Lehmer> *m_rng;
    alo::BVH *m_bvh;
    
};
//! [0]

#endif // CylinderThread_H
