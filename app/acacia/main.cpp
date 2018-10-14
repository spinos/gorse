
#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(acacia);
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
