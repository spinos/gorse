/*
 *  glprog.cpp
 *  gl es wireframe draw
 */

#include "glprog.h"
#include <QDebug>

using namespace alo;

GLProg::GLProg() :
m_program1(nullptr)
{
    m_logo.createCylinder(16, 8, 9.f, 29.f);
    m_logo.createPositionArray(m_posarr);
    m_logo.createNormalArray(m_nmlarr);
    m_logo.createBarycentricCoordinates(m_baryc);
    m_program1 = nullptr;

}

GLProg::~GLProg()
{
    cleanup();
}

void GLProg::cleanup()
{
    m_posVbo.destroy();
    m_nmlVbo.destroy();

    delete m_program1;
    m_program1 = nullptr;
}

void GLProg::initializeGL()
{
    m_program1 = new WireframeProgram();
    m_program1->initializeProgram();
/// a vao can have multiple vertex buffer objects
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    setupVertexAttribs();
}

void GLProg::setupVertexAttribs()
{
/// vertex buffer object for geometry once
    m_posVbo.create();
    m_posVbo.bind();
    m_posVbo.allocate((const GLfloat *)m_posarr.c_data(), m_logo.numIndices() * sizeof(Vector3F));

    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
    m_posVbo.release();
    
    m_nmlVbo.create();
    m_nmlVbo.bind();
    m_nmlVbo.allocate((const GLfloat *)m_nmlarr.c_data(), m_logo.numIndices() * sizeof(Vector3F));

    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12, 0);
    m_nmlVbo.release();
    
    m_barVbo.create();
    m_barVbo.bind();
    m_barVbo.allocate((const GLfloat *)m_baryc.c_data(), m_logo.numIndices() * sizeof(Vector3F));

    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12, 0);
    m_barVbo.release();

}

void GLProg::paintGL(const QMatrix4x4 &projectionMat, const QMatrix4x4 &cameraMat)
{ 

    m_world.setToIdentity();
    m_world.rotate(45.0f, QVector3D(1,0,1));
    //m_world.translate(QVector3D(17,0,0));
    
    QMatrix4x4 modelView = cameraMat * m_world;

    m_vao.bind();
    m_program1->beginProgram(projectionMat, m_world, modelView);

    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    f->glDrawArrays(GL_TRIANGLES, 0, m_logo.numIndices() );

    m_program1->endProgram();
}
