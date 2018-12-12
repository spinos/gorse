/*
 *  BaseProgram.h
 *  aloe
 *
 */

#ifndef OGL_BASE_PROGRAM_H
#define OGL_BASE_PROGRAM_H

#include <QOpenGLExtraFunctions>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QMatrix4x4)

namespace alo {

class BaseProgram
{
    QOpenGLShaderProgram *m_program;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;

public:
    BaseProgram();
    virtual ~BaseProgram();

    void beginProgram(const QMatrix4x4 &projectionMat);
    void setModelView(const QMatrix4x4 &modelMat,
                                const QMatrix4x4 &modelViewMat);
    void endProgram();
    
    virtual void initializeProgram(QOpenGLContext *ctx) = 0;

    virtual void setWireColor(const float *c) = 0;
    virtual void setSurfaceColor(const float *c) = 0;
    
protected:
    void setShaderProgram(QOpenGLShaderProgram *x);
    QOpenGLShaderProgram *shaderProgram();

private:
    void locateMatrices();
    void cleanup();
    
};

}

#endif
