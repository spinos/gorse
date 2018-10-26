
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
}

class CylinderThread : public QThread
{
    Q_OBJECT

public:
    CylinderThread(alo::DrawableScene *scene, QObject *parent = 0);
    ~CylinderThread();

    void render(double centerX, double centerY, double scaleFactor, QSize resultSize);

signals:
    //void renderedImage(const QImage &image, double scaleFactor);

protected:
    void run() override;

private:
    void addCylinder(int it);

    QMutex mutex;
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
    
};
//! [0]

#endif // CylinderThread_H
