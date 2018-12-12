/*
 *  BaseProgram.cpp
 *  aloe
 *
 */

#include "BaseProgram.h"
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

namespace alo {

BaseProgram::BaseProgram() :
    m_program(nullptr)
{
}

BaseProgram::~BaseProgram()
{
    cleanup();
}

void BaseProgram::cleanup()
{
    if (m_program == nullptr)
        return;
    
    delete m_program;
    m_program = nullptr;
}

void BaseProgram::beginProgram(const QMatrix4x4 &projectionMat)
{ 
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, projectionMat);
}

void BaseProgram::setModelView(const QMatrix4x4 &modelMat,
                                const QMatrix4x4 &modelViewMat)
{ 
    m_program->setUniformValue(m_mvMatrixLoc, modelViewMat);
    QMatrix3x3 normalMatrix = modelMat.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);
}

void BaseProgram::endProgram()
{ m_program->release(); }

void BaseProgram::setShaderProgram(QOpenGLShaderProgram *x)
{ 
    m_program = x; 
    locateMatrices();
}

void BaseProgram::locateMatrices()
{
    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_program->release();
}

QOpenGLShaderProgram *BaseProgram::shaderProgram()
{ return m_program; }

}
