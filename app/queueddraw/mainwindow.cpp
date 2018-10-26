
#include "mainwindow.h"
#include "glwidget.h"
#include "cylinderthread.h"

MainWindow::MainWindow()
{
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
