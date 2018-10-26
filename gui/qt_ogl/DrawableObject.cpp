#include "DrawableObject.h"
#include <iostream>

namespace alo {

DrawableObject::DrawableObject()
{
	m_drawArraySize = 0;
    m_count[0] = 0;
    m_count[1] = 0;
}

DrawableObject::~DrawableObject()
{
    cleanup();
}

void DrawableObject::cleanup()
{
    m_posnmlVbo.destroy();
    m_barVbo.destroy();
}

void DrawableObject::setPosnml(const float *posnml, int count)
{
    m_data[0] = posnml;
    m_count[0] = count;
}

void DrawableObject::setBarycentric(const float *barycoord, int count)
{
    m_data[1] = barycoord;
    m_count[1] = count;
}

void DrawableObject::create(QOpenGLContext *ctx)
{
	m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_posnmlVbo.create();
    m_posnmlVbo.bind();
    m_posnmlVbo.allocate((const GLfloat *)m_data[0], m_count[0]);

    QOpenGLExtraFunctions *f = ctx->extraFunctions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_posnmlVbo.release();
    
    m_barVbo.create();
    m_barVbo.bind();
    m_barVbo.allocate((const GLfloat *)m_data[1], m_count[1]);

    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    m_barVbo.release();
}

void DrawableObject::update(QOpenGLContext *ctx)
{
    m_posnmlVbo.bind();

    QOpenGLExtraFunctions *f = ctx->extraFunctions();

    const int length = 6 * sizeof(GLfloat) * m_drawArraySize;
    
    void *oldData = f->glMapBufferRange(GL_ARRAY_BUFFER, 0, length,
        GL_MAP_WRITE_BIT );

    memcpy(oldData, m_data[0], length);

    f->glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, length);
    f->glUnmapBuffer(GL_ARRAY_BUFFER);

    m_posnmlVbo.release();
}

QOpenGLBuffer &DrawableObject::posnmlVbo()
{ return m_posnmlVbo; }

QOpenGLBuffer &DrawableObject::barVbo()
{ return m_barVbo; }

void DrawableObject::setDrawArrayLength(int x)
{ m_drawArraySize = x; }

const int &DrawableObject::drawArrayLength() const
{ return m_drawArraySize; }

void DrawableObject::draw(QOpenGLContext *ctx)
{ 
    m_vao.bind();
    QOpenGLExtraFunctions *f = ctx->extraFunctions();
    f->glDrawArrays(GL_TRIANGLES, 0, m_drawArraySize );
}

const QMatrix4x4 &DrawableObject::worldMatrix() const
{ return m_world; }

void DrawableObject::move(float x, float y, float z)
{ m_world.translate(x, y, z); }

}
