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
    
    Impl() : _distance(2.f),
    _spring(20.f),
    _damping(0.f)
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

    Vector3F force = norm * (-m_pimpl->_spring * (m_pimpl->_distance - d));
    if(m_pimpl->_damping > 0) force += relVel * m_pimpl->_damping;
    return force;
}
    
}
