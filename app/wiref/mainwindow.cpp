
#include "mainwindow.h"
#include "glwidget.h"

MainWindow::MainWindow()
{
    setCentralWidget(new GLWidget(this));
}
