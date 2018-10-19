#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>

GLWidget::GLWidget(QWidget *parent)
    : alo::View3DWidget(parent)
{
}

GLWidget::~GLWidget()
{
    cleanup();
}

void GLWidget::cleanup()
{
    makeCurrent();
    m_logo->cleanup();
    doneCurrent();
}

void GLWidget::clientInit()
{
    //connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
	//initializeOpenGLFunctions();

    m_logo = new GLProg;
    m_logo->initializeGL();
    
}

void GLWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    m_logo->paintGL(proj, cam);
}
