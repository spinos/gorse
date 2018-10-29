#include "glwidget.h"
#include <QMouseEvent>
#include <QCoreApplication>
#include <qt_ogl/DrawableObject.h>
#include <mesh/FrontMesher.h>
#include <geom/AdaptableMesh.h>
#include <math/miscfuncs.h>
#include <mesh/EdgeCollapse.h>

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
    addPatch();
}

void GLWidget::clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    m_scene->draw(proj, cam);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    //if(event->key() == Qt::Key_C) {
    //}
}

void GLWidget::addPatch()
{
    AdaptableMesh p;
    const int nu = 20;
    for(int i=0;i<nu;++i) {
        float phi = .21f * i;
        p.addVertex(Vector3F(-47.f + 5.3f * i + RandomFn11(), -40.f + 7.f * sin(phi), -30.f -11.f * sin(phi) ));
    }
    
    FrontLine originLine;
    FrontLine::EnableMerging = false;
    
    for(int i=0;i<nu;++i)
        originLine.addVertex(p.vertexPositionR(i), i);
    
    Vector3F up(0.f, 4.9f, 0.1f);
    const Quaternion lq(.029f, Vector3F::ZAxis);
    const Quaternion tq(.011f, Vector3F::YAxis);
    constexpr float cshrinking = .0f;
    originLine.setWorldSpace(Matrix33F::IdentityMatrix);
    originLine.rotateLocalBy(lq);
    originLine.rotateLocalBy(tq);
    originLine.setShrinking(cshrinking);
    originLine.setDirection(up);
    originLine.setMinEdgeLength(.1f);
    
    FrontLine *la = &originLine;
    FrontLine l[25];

    FrontMesher msher;
    msher.attachMesh(&p);

    for(int i=0;i<25;++i) {
        msher.setFrontId(i+1);

        l[i].rotateLocalBy(lq);
        l[i].rotateLocalBy(tq);
        l[i].setShrinking(cshrinking + RandomFn11() * .03f);
        up.y += RandomFn11() * .3f;
        up.z += RandomFn11() * .9f;
        l[i].setDirection(up);
        l[i].setMinEdgeLength(.1f);

        msher.advanceFront(l[i], *la);

        la = &l[i];
    }

    p.calculateVertexNormals();

    EdgeCollapse ech;
    ech.simplify(&p);
    
    p.createPositionNormalArray(posnml);
    p.createBarycentricCoordinates(baryc);

    DrawableObject *cly = m_scene->createDrawable();
    cly->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
    cly->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
    cly->setDrawArrayLength(p.numIndices());

}
