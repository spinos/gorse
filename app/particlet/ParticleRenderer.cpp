/*
 *  ParticleRenderer.cpp
 *  particlet
 *
 *  2019/10/5
 */

#include "ParticleRenderer.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <pbd/SphereEmitter.h>
#include <pbd/ParticleSystem.h>

struct ParticleRenderer::Impl
{
    QOpenGLShaderProgram _program;
    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _spriteVbo;
    QOpenGLBuffer _posVbo;
    int _projMatrixLoc;
    int _mvMatrixLoc;
    int _invCameraMatrixLoc;
};

ParticleRenderer::ParticleRenderer() : m_pimpl(0)
{
}

void ParticleRenderer::cleanup()
{
    m_pimpl.reset();
}

static const char *vertexShaderSource =
    "attribute vec3 vertex;\n"
    "attribute vec3 voffset;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat4 invCameraMatrix;\n"
    "varying vec4 vert;\n"
    "varying vec2 vert2D;\n"
    "void main() {\n"
    "   vert2D = vertex.xy;\n"
    "   vert = invCameraMatrix * vec4(vertex, 1.0);\n"
    "   gl_Position = projMatrix * mvMatrix * (vert + vec4(voffset, 1.0));\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying highp vec4 vert;\n"
    "varying highp vec2 vert2D;\n"
    "void main() {\n"
    "   highp float mag = dot(vert2D, vert2D);\n"
    "   if (mag > 1.0) discard;\n"
    "   highp vec3 nml = vec3(vert2D.xy, 1.0 - mag);\n"
    "   highp vec3 lightDir = vec3(0.577, 0.577, 0.577);\n"
    "   highp float diff = max(0.1, dot(lightDir, nml));"
    "   gl_FragColor = vec4(0.0, 0.8, 1.0, 1.0) * diff;\n"
    "}\n";

void ParticleRenderer::initializeGL()
{
    initializeOpenGLFunctions();
    
    m_pimpl.reset(new Impl);

    QOpenGLShaderProgram *program = &m_pimpl->_program;
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    program->bindAttributeLocation("vertex", 0);
    program->link();

    program->bind();
    m_pimpl->_projMatrixLoc = program->uniformLocation("projMatrix");
    m_pimpl->_mvMatrixLoc = program->uniformLocation("mvMatrix");
    m_pimpl->_invCameraMatrixLoc = program->uniformLocation("invCameraMatrix");
    program->release();
    
    m_pimpl->_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_pimpl->_vao);
    
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    
    QOpenGLBuffer *spriteVbo = &m_pimpl->_spriteVbo;
    
    spriteVbo->create();
    spriteVbo->bind();
    
    QVector<QVector3D> sprite;
    sprite << QVector3D(-1.f, -1.f, 0.f);
    sprite << QVector3D(1.f, -1.f, 0.f);
    sprite << QVector3D(1.f, 1.f, 0.f);
    
    sprite << QVector3D(-1.f, -1.f, 0.f);
    sprite << QVector3D(1.f, 1.f, 0.f);
    sprite << QVector3D(-1.f, 1.f, 0.f);
    
    spriteVbo->allocate((const GLfloat *)sprite.constData(), 6 * 3 * sizeof(GLfloat));
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    spriteVbo->release();
    
    QOpenGLBuffer *posVbo = &m_pimpl->_posVbo;
    
    posVbo->create();
    posVbo->bind();
    
    QVector<float> pos;
    for(int i=0;i<1024;++i) {
        pos << (1.f * i);
        pos << (1.f * i);
        pos << (-1.f * i);
    }

    posVbo->allocate((const GLfloat *)pos.constData(), 1024 * 3 * sizeof(GLfloat));
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
            3 * sizeof(GLfloat), 
            (void *)sizeof(QVector3D));
    f->glVertexAttribDivisor(1, 1);        
            
    posVbo->release();
    
}

void ParticleRenderer::paintGL(const QMatrix4x4 &projectionMat, const QMatrix4x4 &cameraMat)
{ 
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_pimpl->_vao);
    
    QOpenGLShaderProgram *program = &m_pimpl->_program;
    program->bind();
    program->setUniformValue(m_pimpl->_projMatrixLoc, projectionMat);
    program->setUniformValue(m_pimpl->_mvMatrixLoc, cameraMat);
    QMatrix4x4 invCamera = cameraMat.inverted();
    invCamera.setRow(3, QVector4D(0.0, 0.0, 0.0, 1.0));
    program->setUniformValue(m_pimpl->_invCameraMatrixLoc, invCamera);

    f->glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1024);

    program->release();
    
}
