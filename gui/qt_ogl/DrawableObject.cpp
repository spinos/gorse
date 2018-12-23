#include "DrawableObject.h"
#include <iostream>

namespace alo {

DrawableObject::DrawableObject()
{
	m_drawArraySize = 0;
    m_editArrayLength = 0;
    m_numInstances = 1;
    m_count[0] = 0;
    m_count[1] = 0;
    m_count[2] = 0;
    m_drawId = 0;
}

DrawableObject::~DrawableObject()
{
    cleanup();
}

void DrawableObject::cleanup()
{
    m_posnmlVbo.destroy();
    m_barVbo.destroy();
    m_offsetVbo.destroy();
}

void DrawableObject::setDrawId(int x)
{ m_drawId = x; }
    
const int &DrawableObject::drawId() const
{ return m_drawId; }

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

void DrawableObject::setTransformMatrix(const float *tm, int count)
{
    m_data[2] = tm;
    m_count[2] = count;
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
    
    m_offsetVbo.create();
    m_offsetVbo.bind();
    m_offsetVbo.allocate((const GLfloat *)m_data[2], m_count[2]);

    for(int i=0;i<4;++i) {
        f->glEnableVertexAttribArray(3 + i);
        f->glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 64, (void *)(i<<4));
        f->glVertexAttribDivisor(3 + i, 1);
    }
    m_offsetVbo.release();
}

void DrawableObject::update(QOpenGLContext *ctx)
{
    if(m_drawArraySize < 1) return;

    m_posnmlVbo.bind();

    QOpenGLExtraFunctions *f = ctx->extraFunctions();

    const int length = 24 * m_editArrayLength;
    
    void *oldData = f->glMapBufferRange(GL_ARRAY_BUFFER, 0, length,
        GL_MAP_WRITE_BIT |  QOpenGLBuffer::RangeUnsynchronized );

    memcpy(oldData, m_data[0], length);

    f->glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, length);
    f->glUnmapBuffer(GL_ARRAY_BUFFER);

    m_posnmlVbo.release();
    
    m_drawArraySize = m_editArrayLength;
}

QOpenGLBuffer &DrawableObject::posnmlVbo()
{ return m_posnmlVbo; }

QOpenGLBuffer &DrawableObject::barVbo()
{ return m_barVbo; }

void DrawableObject::setDrawArrayLength(int x)
{ m_drawArraySize = x; }

const int &DrawableObject::drawArrayLength() const
{ return m_drawArraySize; }

void DrawableObject::setEditDrawArrayLength(int x)
{ m_editArrayLength = x; }

void DrawableObject::setNumInstances(int x)
{ m_numInstances = x; }

void DrawableObject::draw(QOpenGLContext *ctx)
{ 
    m_vao.bind();
    QOpenGLExtraFunctions *f = ctx->extraFunctions();
    //f->glDrawArrays(GL_TRIANGLES, 0, m_drawArraySize );
    f->glDrawArraysInstanced(GL_TRIANGLES, 0, m_drawArraySize, m_numInstances);
}

const QMatrix4x4 &DrawableObject::worldMatrix() const
{ return m_world; }

void DrawableObject::move(float x, float y, float z)
{ m_world.translate(x, y, z); }

}
