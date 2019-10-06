
#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Projective Shape Matching Particle System");
    QCoreApplication::setOrganizationName("QtProject");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);

/// ES 3.0 GLSL 3.00 by ANGLE
    fmt.setVersion(3, 0);
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile);

    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow mainWindow;

    mainWindow.resize(mainWindow.sizeHint());
    int desktopArea = QApplication::desktop()->width() *
                     QApplication::desktop()->height();
    int widgetArea = mainWindow.width() * mainWindow.height();
    if (((float)widgetArea / (float)desktopArea) < 0.75f)
        mainWindow.show();
    else
        mainWindow.showMaximized();
    return app.exec();
}
