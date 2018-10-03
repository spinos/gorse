/*
 *  glprog.cpp
 *
 */

#include "glprog.h"
#include <QOpenGLShaderProgram>

using namespace alo;

GLProg::GLProg() : 
    m_program(0)
{
    m_logo.createTube();
    
    m_logo.createPositionArray(m_posarr);
    m_logo.createNormalArray(m_nmlarr);
    m_logo.createBarycentricCoordinates(m_baryc);
}

GLProg::~GLProg()
{
    cleanup();
}

void GLProg::cleanup()
{
    if (m_program == nullptr)
        return;
    
    m_posVbo.destroy();
    m_nmlVbo.destroy();
    delete m_program;
    m_program = 0;
}

static const char *vertexShaderSource =
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

static const char *fragmentShaderSource =
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

void GLProg::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource );
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->bindAttributeLocation("bary", 2);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

/// a vao can have multiple vertex buffer objects
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    setupVertexAttribs();

    // Light position is fixed.
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 40, 70));

    m_program->release();

}

void GLProg::setupVertexAttribs()
{
/// vertex buffer object for geometry once
    m_posVbo.create();
    m_posVbo.bind();
    m_posVbo.allocate((const GLfloat *)m_posarr.c_data(), m_logo.numIndices() * sizeof(Vector3F));

    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
    m_posVbo.release();
    
    m_nmlVbo.create();
    m_nmlVbo.bind();
    m_nmlVbo.allocate((const GLfloat *)m_nmlarr.c_data(), m_logo.numIndices() * sizeof(Vector3F));

    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12, 0);
    m_nmlVbo.release();
    
    m_barVbo.create();
    m_barVbo.bind();
    m_barVbo.allocate((const GLfloat *)m_baryc.c_data(), m_logo.numIndices() * sizeof(Vector3F));

    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12, 0);
    m_barVbo.release();

}

void GLProg::paintGL(const QMatrix4x4 &projectionMat, const QMatrix4x4 &cameraMat)
{ 
    m_world.setToIdentity();
    //m_world.rotate(90.0f, 1, 0, 0);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, projectionMat);
    m_program->setUniformValue(m_mvMatrixLoc, cameraMat * m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    glDrawArrays(GL_TRIANGLES, 0, m_logo.numIndices() );
    
    m_program->release();
}
