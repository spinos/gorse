
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

class CylinderThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    virtual void keyPressEvent(QKeyEvent *event);

private:
    CylinderThread *m_thread;
    
};

#endif
