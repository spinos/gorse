#include "glwidget.h"
#include <QMouseEvent>
#include <QCoreApplication>
#include <QMatrix4x4>
#include "ParticleRenderer.h"

GLWidget::GLWidget(QWidget *parent) : alo::View3DWidget(parent),
m_particleRenderer(new ParticleRenderer)
{
}

GLWidget::~GLWidget()
{
    cleanup();
}

void GLWidget::cleanup()
{
    makeCurrent();
    m_particleRenderer->cleanup();
    doneCurrent();
}

void GLWidget::clientInit()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    m_particleRenderer->initializeGL();
}

void GLWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    m_particleRenderer->paintGL(proj, cam);
}
