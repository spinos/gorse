/*
 *  WireframeProgram.h
 *
 *
 */

#ifndef WIREFRAME_PROGRAM_H
#define WIREFRAME_PROGRAM_H

#include <QOpenGLExtraFunctions>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QMatrix4x4)
QT_FORWARD_DECLARE_CLASS(QOpenGLVertexArrayObject)

namespace alo {

class WireframeProgram
{

public:
    WireframeProgram();
    ~WireframeProgram();
    
    void cleanup();
    void initializeProgram();
    void beginProgram(const QMatrix4x4 &projectionMat, 
                    const QMatrix4x4 &modelMat,
                    const QMatrix4x4 &modelViewMat);
    void endProgram();
    
protected:
    
private:
    QOpenGLShaderProgram *m_program;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPosLoc;

};

}

#endif
