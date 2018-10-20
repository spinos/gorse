#ifndef DRAWABLE_OBJECT_H
#define DRAWABLE_OBJECT_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

namespace alo {

class DrawableObject
{

public:
    DrawableObject();
    ~DrawableObject();
    
    void create(const float *position,
                const float *normal,
                const float *barycoord,
                int count);
    void draw();
    void cleanup();

    const QMatrix4x4 &worldMatrix() const;

protected:
    void setDrawArraySize(int x);
    QOpenGLBuffer &posVbo();
    QOpenGLBuffer &nmlVbo();
    QOpenGLBuffer &barVbo();

private:
    
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_posVbo;
    QOpenGLBuffer m_nmlVbo;
    QOpenGLBuffer m_barVbo;
    QMatrix4x4 m_world;
    int m_drawArraySize;

};

}

#endif