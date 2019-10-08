/*
 *  SphereCollision.cpp
 *  aloe
 *
 *  2019/10/8
 */
 
#include "SphereCollision.h"
#include <math/Vector3F.h>

namespace alo {
    
struct SphereCollision::Impl {
    
    float _distance;
    float _spring;
    float _damping;
    float _shear;
    
    Impl() : _distance(1.f),
    _spring(.5f),
    _damping(.02f),
    _shear(.1f)
    {}
    
};
    
SphereCollision::SphereCollision() : m_pimpl(new Impl)
{}

Vector3F SphereCollision::calculateForce(const Vector3F &pos1, const Vector3F &pos2, 
                    const Vector3F &vel1, const Vector3F &vel2) const
{
    const Vector3F relPos = pos2 - pos1;
    const float d = relPos.length();
    
    if(d > m_pimpl->_distance) return Vector3F::Zero;
    
    const Vector3F norm = relPos / d;
    const Vector3F relVel = vel2 - vel1;
    const Vector3F tanVel = relVel - norm * relVel.dot(norm);

    Vector3F force = norm * (-m_pimpl->_spring * (m_pimpl->_distance - d));
    force += relVel * m_pimpl->_damping;
    force += tanVel * m_pimpl->_shear;
    return force;
}
    
}
