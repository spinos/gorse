/*
 *  SphereCollision.h
 *  aloe
 *
 *  2019/10/8
 */
 
#ifndef ALO_SPHERE_COLLISION_H
#define ALO_SPHERE_COLLISION_H

#include <boost/shared_ptr.hpp>

namespace alo {
    
class Vector3F;
    
class SphereCollision {
  
public:

    SphereCollision();
    
    Vector3F calculateForce(const Vector3F &pos1, const Vector3F &pos2, 
                    const Vector3F &vel1, const Vector3F &vel2) const;
    
private:

    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
    
};
    
}

#endif
