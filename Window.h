#pragma once

#include <QWidget>

class GLWidget;
class Application;

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(Application *app);
private:
    GLWidget *glWidget;
    Application *application;
};
