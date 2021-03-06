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
    m_scene->cleanup();
    doneCurrent();
}

void GLWidget::recvAttribChanged()
{ update(); }

void GLWidget::clientInit()
{
    //connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
	m_scene->setContext(QOpenGLContext::globalShareContext());
    m_scene->initializeDrawable();
}

void GLWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    m_scene->draw(proj, cam);
}

void GLWidget::setDisplaySolid()
{
    m_scene->selectProgram(DrawableScene::SolidShaderProgramId);
    update();
}

void GLWidget::setDisplayWire()
{
    m_scene->selectProgram(DrawableScene::WireShaderProgramId);
    update();
}
