
#include "mainwindow.h"
#include "glwidget.h"
#include "cylinderthread.h"
#include <bvh/BVH.h>
#include <bvh/BVHBuilder.h>
#include <ctime>
#include <rng/Uniform.h>
#include <rng/Lehmer.h>

using namespace alo;

MainWindow::MainWindow()
{
    std::time_t secs = std::time(nullptr);
    Uniform<Lehmer> mrng(secs);
    
    BVH mbvh;
    mbvh.clear();

    BoundingBox box;
    Vector3F pmin;
    for(int i=0;i<50000;++i) {
        BVHPrimitive ap;

        pmin.set(mrng.randf1() * 2000.f - 1000.f,
                mrng.randf1() * 100.f - 10.f,
                mrng.randf1() * 1700.f - 850.f);
        box.setMin(pmin.x, pmin.y, pmin.z);
        box.setMax(pmin.x + 12.f + mrng.randf1() * 12.f, 
                    pmin.y + 15.f + mrng.randf1() * 15.f, 
                    pmin.z + 10.f + mrng.randf1() * 10.f);
        ap.setAABB(box);
        ap.setIndex(i);

        mbvh.addPrimitive(ap);
    }

    mbvh.setRootLeaf();

    BVHBuilder builder;
    builder.build(&mbvh);

    std::cout<<" bvh "<<mbvh;
    
    GLWidget *wig = new GLWidget(this);
    m_thread = new CylinderThread(wig->scene(), this);
    setCentralWidget(wig);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_C) {

        qDebug()<<"MainWindow::keyPressEvent C";
        m_thread->start();
    }
}
