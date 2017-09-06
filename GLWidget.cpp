#include "GLWidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

#include "Test.h"

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      currentXRotation(0),
      currentYRotation(0),
      currentZRotation(0),
      activeModel(0),
      activeModelBase(0),
      shaderProgram(0),
      shaderProjectionMatrix(),
      shaderModelViewMatrix(),
      shaderNormalMatrix(),
      shaderLightPosition(),
      camera(vec(0,0,40))
{
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    keyPressed[e->key()] = true;

    switch(e->key())
    {
    case Qt::Key_Escape:
        QApplication::closeAllWindows();
        break;
    case Qt::Key_R:
        if(activeModel)
        {
            delete activeModel;
        }
        generateModel();
        break;
    case Qt::Key_E:
        activeModel->data()->exportOBJ("export.obj");
        break;
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    keyPressed[e->key()] = false;
}


void GLWidget::setXRotation(int angle)
{
    if(angle != currentXRotation)
    {
        camera.rotateHeading((currentXRotation-angle) * 0.0174532925f * 0.02f);
        emit xRotationChanged(angle);
        currentXRotation = angle;
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    if(angle != currentYRotation)
    {
        camera.rotatePitch(-(currentYRotation-angle) * 0.0174532925f * 0.02f);
        emit xRotationChanged(angle);
        currentYRotation = angle;
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    if(angle != currentZRotation)
    {
        camera.rotateYaw(-(currentZRotation-angle) * 0.0174532925f * 0.02f);
        currentZRotation = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::cleanup()
{
    makeCurrent();
    delete activeModel;
    delete activeModelBase;
    delete shaderProgram;
    shaderProgram = 0;
    doneCurrent();
}

void GLWidget::move(float amount)
{
    camera.move(amount * 0.25f);
    update();
}

void GLWidget::strafe(float amount)
{
    camera.strafe(amount * 0.25f);
    update();
}

void GLWidget::generateModel()
{
    activeModel = activeModelBase->generate(AABB(vec(-10,-10,-10), vec(10,10,10)));
}

void GLWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

    shaderProgram = new QOpenGLShaderProgram;
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "standard.vs");
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "standard.fs");
    shaderProgram->bindAttributeLocation("vertex", 0);
    shaderProgram->bindAttributeLocation("normal", 1);
    shaderProgram->link();

    shaderProgram->bind();
    shaderProjectionMatrix = shaderProgram->uniformLocation("projMatrix");
    shaderModelViewMatrix = shaderProgram->uniformLocation("mvMatrix");
    shaderNormalMatrix = shaderProgram->uniformLocation("normalMatrix");
    shaderLightPosition = shaderProgram->uniformLocation("lightPos");


    activeModelBase = new Test();

    generateModel();

    shaderProgram->setUniformValue(shaderLightPosition, QVector3D(0, 0, 70));

    shaderProgram->release();
}


void GLWidget::paintGL()
{
    if(keyPressed[Qt::Key_W]) this->move( 1.0f);
    if(keyPressed[Qt::Key_S]) this->move(-1.0f);
    if(keyPressed[Qt::Key_A]) this->strafe( 1.0f);
    if(keyPressed[Qt::Key_D]) this->strafe(-1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_world.setToIdentity();

    shaderProgram->bind();
    shaderProgram->setUniformValue(shaderProjectionMatrix, m_proj);
    shaderProgram->setUniformValue(shaderModelViewMatrix, camera * m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    shaderProgram->setUniformValue(shaderNormalMatrix, normalMatrix);
    activeModel->render();

    shaderProgram->release();
    update();
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if(event->buttons() & Qt::LeftButton)
    {
        setXRotation(currentXRotation + 8 * dy);
        setYRotation(currentYRotation + 8 * dx);
    }
    else if(event->buttons() & Qt::RightButton)
    {
        setXRotation(currentXRotation + 8 * dy);
        setZRotation(currentZRotation + 8 * dx);
    }
    m_lastPos = event->pos();
}
