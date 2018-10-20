/*
 *  WireframeProgram.cpp
 *
 *  http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates/
 *
 */

#include "WireframeProgram.h"
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

namespace alo {

WireframeProgram::WireframeProgram() :
    m_program(nullptr)
{
}

WireframeProgram::~WireframeProgram()
{
    cleanup();
}

void WireframeProgram::cleanup()
{
    if (m_program == nullptr)
        return;
    
    delete m_program;
    m_program = nullptr;
}

static const char *wfrm_vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "attribute mediump vec3 bary;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "varying vec3 baryc;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   baryc = bary;"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *wfrm_fragmentShaderSource =
    "varying highp vec3 vert;\n"
    "varying highp vec3 vertNormal;\n"
    "varying mediump vec3 baryc;\n"
    "uniform highp vec3 lightPos;\n"
    "#extension GL_OES_standard_derivatives  : enable\n"
    "mediump float edgeFactor() { \n"
    "   mediump vec3 d = fwidth(baryc);\n"
    "   mediump vec3 a3 = smoothstep(vec3(0.0), d * 1.3, baryc);\n"
    "   return min(min(a3.x, a3.y), a3.z);\n"
    "}\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.0, 0.6, 1.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   gl_FragColor = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(col, 1.0), edgeFactor());\n"
    "}\n";

void WireframeProgram::initializeProgram()
{
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, wfrm_vertexShaderSource );
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, wfrm_fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->bindAttributeLocation("bary", 2);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 40, 70));

    m_program->release();
}

void WireframeProgram::beginProgram(const QMatrix4x4 &projectionMat)
{ 
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, projectionMat);
}

void WireframeProgram::beginProgram(const QMatrix4x4 &projectionMat, 
                                const QMatrix4x4 &modelMat,
                                const QMatrix4x4 &modelViewMat)
{ 
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, projectionMat);
    m_program->setUniformValue(m_mvMatrixLoc, modelViewMat);
    QMatrix3x3 normalMatrix = modelMat.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);   
}

void WireframeProgram::setModelView(const QMatrix4x4 &modelMat,
                                const QMatrix4x4 &modelViewMat)
{ 
    m_program->setUniformValue(m_mvMatrixLoc, modelViewMat);
    QMatrix3x3 normalMatrix = modelMat.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);
}

void WireframeProgram::endProgram()
{   
    m_program->release();
}

}
