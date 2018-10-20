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

void DrawableObject::create(const float *position,
                const float *normal,
                const float *barycoord,
                int count)
{
	m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    QOpenGLBuffer &posB = posVbo();
	posB.create();
    posB.bind();
    posB.allocate((const GLfloat *)position, count * 12);

    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
    posB.release();
    
    QOpenGLBuffer &nmlB = nmlVbo();
    nmlB.create();
    nmlB.bind();
    nmlB.allocate((const GLfloat *)normal, count * 12);

    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12, 0);
    nmlB.release();
    
    QOpenGLBuffer &barB = barVbo();
    barB.create();
    barB.bind();
    barB.allocate((const GLfloat *)barycoord, count * 12);

    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12, 0);
    barB.release();

    setDrawArraySize(count);
}

QOpenGLBuffer &DrawableObject::posVbo()
{ return m_posVbo; }

QOpenGLBuffer &DrawableObject::nmlVbo()
{ return m_nmlVbo; }

QOpenGLBuffer &DrawableObject::barVbo()
{ return m_barVbo; }

void DrawableObject::setDrawArraySize(int x)
{ m_drawArraySize = x; }

void DrawableObject::draw()
{ 
    m_vao.bind();
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    f->glDrawArrays(GL_TRIANGLES, 0, m_drawArraySize );
}

const QMatrix4x4 &DrawableObject::worldMatrix() const
{ return m_world; }

}
