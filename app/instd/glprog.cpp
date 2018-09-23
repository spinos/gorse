#include "glprog.h"
#include <QOpenGLShaderProgram>
#include <math.h>

GLProg::GLProg()
    : m_program(0)
{
}

GLProg::~GLProg()
{
    cleanup();
}

void GLProg::cleanup()
{
    if (m_program == nullptr)
        return;
    
    m_logoVbo.destroy();
    m_offsetVbo.destroy();
    delete m_program;
    m_program = 0;
}

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "attribute mat4 voffset;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * voffset * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying highp vec3 vert;\n"
    "varying highp vec3 vertNormal;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.0, 0.6, 1.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   gl_FragColor = vec4(col, 1.0);\n"
    "}\n";

void GLProg::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource );
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->bindAttributeLocation("voffset", 2);
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
    m_logoVbo.create();
    m_logoVbo.bind();
    m_logoVbo.allocate((const GLfloat *)m_logo.c_data(), m_logo.count() * sizeof(GLfloat));

    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_logoVbo.release();
    
    QMatrix4x4 aoffset;
    QVector<QMatrix4x4> offsetMats;
    for(int i=0;i<1024;++i) {
        aoffset.setToIdentity();
        aoffset.translate(256 - 0.5 * i, 0,0);
        aoffset.rotate(7.0f * i, 0, 0, 1); /// why rotate after translate?
        offsetMats<<aoffset;
    }
/// vertex buffer object for instance matrices 
    m_offsetVbo.create();
    m_offsetVbo.bind();
    m_offsetVbo.allocate(offsetMats.constData(), 1024 * sizeof(QMatrix4x4));

/// http://www.informit.com/articles/article.aspx?p=2033340&seqNum=5
/// each location can only have 4 floats
/// mat4 needs 4 locations
    for (int i = 0; i < 4; i++) {
        f->glEnableVertexAttribArray(2 + i);
    
        f->glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, /// 4 floats
            sizeof(QMatrix4x4), /// stride
            (void *)(sizeof(QVector4D) * i) ); /// offset
        
/// attribute will advance once every instance 
        f->glVertexAttribDivisor(2 + i, 1);
    }
    m_offsetVbo.release();
}

void GLProg::paintGL(const QMatrix4x4 &projectionMat, const QMatrix4x4 &cameraMat)
{ 
    m_world.setToIdentity();
    m_world.rotate(40.0f, 0, 0, 1);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, projectionMat);
    m_program->setUniformValue(m_mvMatrixLoc, cameraMat * m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    glDrawArraysInstanced(GL_TRIANGLES, 0, m_logo.vertexCount(), 1024);
    
    m_program->release();
}
