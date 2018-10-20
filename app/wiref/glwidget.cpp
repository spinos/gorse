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
    m_scene->initializeScene();

    alo::YCylinder c;
    c.createCylinder(16, 8, 7.f, 31.f);

    SimpleBuffer<Vector3F> posarr;
    SimpleBuffer<Vector3F> nmlarr;
    SimpleBuffer<Vector3F> baryc;
    c.createPositionArray(posarr);
    c.createNormalArray(nmlarr);
    c.createBarycentricCoordinates(baryc);

    DrawableObject *cly = m_scene->createDrawable();
    cly->create((const float *)posarr.c_data(),
                (const float *)nmlarr.c_data(),
                (const float *)baryc.c_data(),
                c.numIndices());
}

void GLWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    m_scene->draw(proj, cam);
}
