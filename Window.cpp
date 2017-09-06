#include "GLWidget.h"
#include "Window.h"
#include "Application.h"
#include <QHBoxLayout>
#include <QApplication>

Window::Window(Application *app) : application(app)
{
    glWidget = new GLWidget();

    QHBoxLayout *container = new QHBoxLayout();
    container->addWidget(glWidget);

    setLayout(container);

    setWindowTitle(tr("pcgl Demo Application"));
}
