#include "DrawableObject.h"

namespace alo {

DrawableObject::DrawableObject()
{
	m_drawArraySize = 0;
}

DrawableObject::~DrawableObject()
{
    cleanup();
}

void DrawableObject::cleanup()
{
    m_posVbo.destroy();
    m_nmlVbo.destroy();
    m_barVbo.destroy();
}

void DrawableObject::setData(const float *position,
                const float *normal,
                const float *barycoord,
                int count)
{
    m_data[0] = position;
    m_data[1] = normal;
    m_data[2] = barycoord;
    m_drawArraySize = count;
}

void DrawableObject::create(QOpenGLContext *ctx)
{
	m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    QOpenGLBuffer &posB = posVbo();
	posB.create();
    posB.bind();
    posB.allocate((const GLfloat *)m_data[0], m_drawArraySize * 12);

    QOpenGLExtraFunctions *f = ctx->extraFunctions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
    posB.release();
    
    QOpenGLBuffer &nmlB = nmlVbo();
    nmlB.create();
    nmlB.bind();
    nmlB.allocate((const GLfloat *)m_data[1], m_drawArraySize * 12);

    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12, 0);
    nmlB.release();
    
    QOpenGLBuffer &barB = barVbo();
    barB.create();
    barB.bind();
    barB.allocate((const GLfloat *)m_data[2], m_drawArraySize * 12);

    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12, 0);
    barB.release();

    qDebug()<<"\n  create drawable "<<m_drawArraySize<<" in ctx"<<ctx;
}

QOpenGLBuffer &DrawableObject::posVbo()
{ return m_posVbo; }

QOpenGLBuffer &DrawableObject::nmlVbo()
{ return m_nmlVbo; }

QOpenGLBuffer &DrawableObject::barVbo()
{ return m_barVbo; }

void DrawableObject::setDrawArraySize(int x)
{ m_drawArraySize = x; }

void DrawableObject::draw(QOpenGLContext *ctx)
{ 
    m_vao.bind();
    QOpenGLExtraFunctions *f = ctx->extraFunctions();
    f->glDrawArrays(GL_TRIANGLES, 0, m_drawArraySize );
}

const QMatrix4x4 &DrawableObject::worldMatrix() const
{ return m_world; }

}
