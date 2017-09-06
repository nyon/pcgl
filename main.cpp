#include <QApplication>
#include <QSurfaceFormat>
#include <QDebug>
#include "Application.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setSamples(4);
    surfaceFormat.setVersion(3, 2);
    surfaceFormat.setSwapInterval(1);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);


    Application application;
    application.resize(application.sizeHint());
    application.show();
    return app.exec();
}
