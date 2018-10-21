#include "glwidget.h"
#include <QMouseEvent>
#include <QCoreApplication>
#include <qt_ogl/DrawableObject.h>
#include <geom/YCylinder.h>

using namespace alo;

GLWidget::GLWidget(QWidget *parent)
    : alo::View3DWidget(parent)
{
    m_scene = new DrawableScene;
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

void GLWidget::clientInit()
{
    //connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
	//initializeOpenGLFunctions();
    m_scene->setContext(context());
    m_scene->initializeScene();
}

void GLWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    m_scene->draw(proj, cam);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_C) {

        addCylinder();
        update();
    }
}

void GLWidget::addCylinder()
{
    YCylinder c;
    c.createCylinder(16, 8, 7.f, 31.f);
    c.createPositionNormalArray(posnml);
    c.createBarycentricCoordinates(baryc);

    DrawableObject *cly = m_scene->createDrawable();
    cly->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
    cly->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
    cly->setDrawArrayLength(c.numIndices());

}
