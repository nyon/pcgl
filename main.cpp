#include <QApplication>
#include <QSurfaceFormat>
#include <QDebug>
#include "Application.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);
    fmt.setVersion(3, 2);
    fmt.setSwapInterval(1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);


    Application application;
    application.resize(application.sizeHint());
    application.show();
    return app.exec();
}
