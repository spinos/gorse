#ifndef GLPROG_H
#define GLPROG_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <geom/XArrow.h>

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

    alo::XArrow m_logo;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_logoVbo;
    QOpenGLShaderProgram *m_program;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_world;

};

#endif
