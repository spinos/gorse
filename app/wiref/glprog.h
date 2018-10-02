#ifndef GLPROG_H
#define GLPROG_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <geom/YCylinder.h>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLProg : protected QOpenGLExtraFunctions
{

public:
    GLProg();
    ~GLProg();
    
    void initializeGL();
    void paintGL(const QMatrix4x4 &projectionMat, const QMatrix4x4 &cameraMat);
    void cleanup();

protected:
    
private:
    void setupVertexAttribs();

    alo::YCylinder m_logo;
    alo::SimpleBuffer<alo::Vector3F> m_posarr;
    alo::SimpleBuffer<alo::Vector3F> m_nmlarr;
    alo::SimpleBuffer<alo::Vector3F> m_baryc;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_posVbo;
    QOpenGLBuffer m_nmlVbo;
    QOpenGLBuffer m_barVbo;
    QOpenGLShaderProgram *m_program;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_world;

};

#endif
