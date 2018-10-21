#include "GLWidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>

using namespace alo;

GLWidget::GLWidget(DrawableScene *scene, QWidget *parent)
    : View3DWidget(parent)
{
	m_scene = scene;
}

GLWidget::~GLWidget()
{
    cleanup();
}

void GLWidget::cleanup()
{
    makeCurrent();

    doneCurrent();
}

void GLWidget::recvAttribChanged()
{ update(); }

void GLWidget::clientInit()
{
    //connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
	m_scene->setContext(QOpenGLContext::globalShareContext());
    m_scene->initializeScene();
}

void GLWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    m_scene->draw(proj, cam);
}
