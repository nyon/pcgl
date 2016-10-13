#include "GLWidget.h"
#include "Window.h"
#include "Application.h"
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include "ValueNoise.h"

Window::Window(Application *app) : application(app)
{
    glWidget = new GLWidget;

    QHBoxLayout *container = new QHBoxLayout;
    container->addWidget(glWidget);

    setLayout(container);

    setWindowTitle(tr("pcgl Demo Application"));
}



