#include "GLWidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

#include "Test.h"

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      model(0),
      baseModel(0),
      m_program(0),
      m_projMatrixLoc(),
      m_mvMatrixLoc(),
      m_normalMatrixLoc(),
      m_lightPosLoc(),
      camera(vec(0,0,40))
{
    this->setFocusPolicy(Qt::StrongFocus);
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
    case Qt::Key_Left:
    {
        Test *test = ((Test*)baseModel);
        test->big_boulder();

        model->regenerate();
        break;
    }
    case Qt::Key_Right:
        baseModel->select_sphere(model->randomSurfacePoint(),5, 0.2);
        baseModel->rotate_z_selection(0.2);
        model->regenerate();
        break;
    case Qt::Key_E:
        if(model) delete model;
        generateModel();
        break;
    case Qt::Key_F:
        model->data()->exportOBJ("asteroid.obj");
        break;
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    keyPressed[e->key()] = false;
}


void GLWidget::setXRotation(int angle)
{
    if (angle != m_xRot) {
        camera.rotateHeading((m_xRot-angle) * 0.0174532925f * 0.02f);
        emit xRotationChanged(angle);
        m_xRot = angle;
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    if (angle != m_yRot) {
        camera.rotatePitch(-(m_yRot-angle) * 0.0174532925f * 0.02f);
        emit xRotationChanged(angle);
        m_yRot = angle;
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::cleanup()
{
    makeCurrent();
    delete model;
    delete baseModel;
    delete m_program;
    m_program = 0;
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
    QTime myTimer;
    myTimer.start();

    model = baseModel->generate(AABB(vec(-10,-10,-10), vec(10,10,10)));
    int nMilliseconds = myTimer.elapsed();
    qDebug() << nMilliseconds << " ms";
}

void GLWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "standard.vs");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "standard.fs");
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");


    baseModel = new Test();

    generateModel();

    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));

    m_program->release();
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
    //m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    //m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    //m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, camera * m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);
    model->render();

    m_program->release();
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

    if (event->buttons() & Qt::LeftButton)
    {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    }
    else if (event->buttons() & Qt::RightButton)
    {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
}
