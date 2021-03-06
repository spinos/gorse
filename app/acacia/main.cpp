
#include <QApplication>
#include <QSurfaceFormat>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);
    fmt.setVersion(3, 0);
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile);

    QSurfaceFormat::setDefaultFormat(fmt);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(acacia);
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
