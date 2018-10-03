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
  
/// direction of front
    Vector3F m_dir;
    int m_id;
    Vector3F* m_pos;
    
public:
    FrontVertex();
    FrontVertex(Vector3F* pos, const Vector3F& dir, int id);
    
    Vector3F* &pos();
    int& id();
    
    const Vector3F* pos() const;
    const Vector3F& dir() const;
    const int& id() const;
/// pos + dir
    Vector3F getPos1() const;

/// same id
    bool operator==(const FrontVertex& b) const;
    
};

}

#endif
