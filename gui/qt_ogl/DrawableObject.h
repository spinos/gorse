/*
 *  DrawableObject.h
 *  aloe
 *
 *  draw resources
 *  PN (interleaved position and normal)
 *  B (barycentric coordinate)
 *
 */

#ifndef DRAWABLE_OBJECT_H
#define DRAWABLE_OBJECT_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <math/DarkColors.h>

namespace alo {

class DrawableObject
{

public:
    DrawableObject();
    ~DrawableObject();

    void setDrawId(int x);
    const int &drawId() const;

/// entry to data
    void setPosnml(const float *posnml, int count);
    void setBarycentric(const float *barycoord, int count);
    void setTransformMatrix(const float *tm, int count);
    void move(float x, float y, float z);
    
/// must have context
    void create(QOpenGLContext *ctx);
    void update(QOpenGLContext *ctx);
    void draw(QOpenGLContext *ctx);
    void cleanup();

    void setDrawArrayLength(int x);
    const int &drawArrayLength() const;
    void setEditDrawArrayLength(int x);
    void setNumInstances(int x);

    const QMatrix4x4 &worldMatrix() const;

protected:
    QOpenGLBuffer &posnmlVbo();
    QOpenGLBuffer &barVbo();

private:
    
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_posnmlVbo;
    QOpenGLBuffer m_barVbo;
    QOpenGLBuffer m_offsetVbo;
    QMatrix4x4 m_world;
    const float *m_data[4];
    int m_count[4];
    int m_drawArraySize;
    int m_editArrayLength;
    int m_numInstances;
    int m_drawId;

};

}

#endif