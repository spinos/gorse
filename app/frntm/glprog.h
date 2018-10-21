#ifndef GLPROG_H
#define GLPROG_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include "tube.h"

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

    Tube m_logo;
    alo::SimpleBuffer<alo::Vector3F> m_posnml;
    alo::SimpleBuffer<alo::Vector3F> m_baryc;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_posnmlVbo;
    QOpenGLBuffer m_barVbo;
    QOpenGLShaderProgram *m_program;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_world;

};

#endif
