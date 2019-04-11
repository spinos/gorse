
#include <QApplication>
#include "Mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(vachell);
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
