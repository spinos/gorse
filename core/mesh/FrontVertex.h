/*
 * FrontVertex.h
 * aloe
 * 
 * position and id
 *
 */
 
#ifndef ALO_FRONT_VERTEX_H
#define ALO_FRONT_VERTEX_H

#include <math/Vector3F.h>

namespace alo {
     
class FrontVertex {
  
/// move in local space
    Vector3F m_dir;
    int m_id;
    Vector3F m_pos;
    float m_curvature;
    
public:
    FrontVertex();
    FrontVertex(Vector3F* pos, int id);
    
    Vector3F &dir();
    Vector3F &pos();
    int& id();
    float& curvature();
    
    const Vector3F &pos() const;
    const Vector3F &dir() const;
    const int& id() const;
    const float& curvature() const;

    void modifyDir(const Vector3F& v);
    void modifyPos(const Vector3F& v, const float& wei);

/// same id
    bool operator==(const FrontVertex& b) const;
    
};

}

#endif
