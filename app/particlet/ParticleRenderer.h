/*
 *  ParticleRenderer.h
 *  particlet
 *
 *  2019/10/5
 */

#ifndef ALO_PARTICLE_RENDERER_H
#define ALO_PARTICLE_RENDERER_H

#include <QOpenGLExtraFunctions>
#include <boost/shared_ptr.hpp>

QT_FORWARD_DECLARE_CLASS(QMatrix4x4)

namespace alo {
class ParticleSystem;
}

class ParticleRenderer : protected QOpenGLExtraFunctions
{

public:
    ParticleRenderer();
    
    void setParticles(const alo::ParticleSystem *x);
    
    void cleanup();
    void initializeGL();
    void paintGL(const QMatrix4x4 &projectionMat, const QMatrix4x4 &cameraMat);
    
protected:
    
private:

    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
    
};

#endif
