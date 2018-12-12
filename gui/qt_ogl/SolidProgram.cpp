/*
 *  SolidProgram.cpp
 *  aloe
 *
 */

#include "SolidProgram.h"
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

namespace alo {

SolidProgram::SolidProgram()
{}

SolidProgram::~SolidProgram()
{}

static const char *solid_vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "attribute mediump vec3 bary;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *solid_fragmentShaderSource =
    "varying highp vec3 vert;\n"
    "varying highp vec3 vertNormal;\n"
    "uniform highp vec3 lightPos;\n"
    "uniform highp vec3 surfaceCol;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 col = clamp(surfaceCol * 0.2 + surfaceCol * 0.8 * NL, 0.0, 1.0);\n"
    "   gl_FragColor = vec4(col, 1.0);\n"
    "}\n";

void SolidProgram::initializeProgram(QOpenGLContext *ctx)
{
    QOpenGLShaderProgram *program = new QOpenGLShaderProgram(ctx);
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, solid_vertexShaderSource );
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, solid_fragmentShaderSource);
    program->bindAttributeLocation("vertex", 0);
    program->bindAttributeLocation("normal", 1);
    program->bindAttributeLocation("bary", 2);
    program->link();

    program->bind();
    
    m_lightPosLoc = program->uniformLocation("lightPos");
    m_surfaceColorLoc = program->uniformLocation("surfaceCol");
    
    program->setUniformValue(m_lightPosLoc, QVector3D(0, 9000, 9000));
    program->setUniformValue(m_surfaceColorLoc, QVector3D(1, 1, 1));

    program->release();

    setShaderProgram(program);
}

void SolidProgram::setWireColor(const float *c)
{}

void SolidProgram::setSurfaceColor(const float *c)
{ shaderProgram()->setUniformValue(m_surfaceColorLoc, QVector3D(c[0], c[1], c[2])); }

}
