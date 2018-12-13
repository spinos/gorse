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

WireframeProgram::WireframeProgram()
{}

WireframeProgram::~WireframeProgram()
{}

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
    "uniform highp vec3 wireCol;\n"
    "uniform highp vec3 surfaceCol;\n"
    "#extension GL_OES_standard_derivatives  : enable\n"
    "mediump float edgeFactor() { \n"
    "   mediump vec3 d = fwidth(baryc);\n"
    "   mediump vec3 a3 = smoothstep(vec3(0.0), d * 1.25, baryc);\n"
    "   return min(min(a3.x, a3.y), a3.z);\n"
    "}\n"
    "void main() {\n"
    "   gl_FragColor = mix(vec4(wireCol.x, wireCol.y, wireCol.z, 1.0), vec4(surfaceCol, 1.0), edgeFactor());\n"
    "}\n";

void WireframeProgram::initializeProgram(QOpenGLContext *ctx)
{
    QOpenGLShaderProgram *program = new QOpenGLShaderProgram(ctx);
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, wfrm_vertexShaderSource );
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, wfrm_fragmentShaderSource);
    program->bindAttributeLocation("vertex", 0);
    program->bindAttributeLocation("normal", 1);
    program->bindAttributeLocation("bary", 2);
    program->link();

    program->bind();
    
    m_wireColorLoc = program->uniformLocation("wireCol");
    m_surfaceColorLoc = program->uniformLocation("surfaceCol");
    
    program->setUniformValue(m_wireColorLoc, QVector3D(0, 0, 0));
    program->setUniformValue(m_surfaceColorLoc, QVector3D(.8, .8, .8));

    program->release();

    setShaderProgram(program);
}

void WireframeProgram::setWireColor(const float *c)
{ shaderProgram()->setUniformValue(m_wireColorLoc, QVector3D(c[0], c[1], c[2])); }

void WireframeProgram::setSurfaceColor(const float *c)
{ shaderProgram()->setUniformValue(m_surfaceColorLoc, QVector3D(c[0], c[1], c[2])); }

}
