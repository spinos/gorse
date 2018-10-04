/*
 * FrontMesher.h
 * aloe
 *
 * create triangle mesh by advancing edge front
 *
 */
 
#ifndef ALO_FRONT_MESHER_H
#define ALO_FRONT_MESHER_H

#include "FrontLine.h"

namespace alo {
    
class AdaptableMesh;

class FrontMesher {
    
    AdaptableMesh* m_msh;
    int m_frontId;

public:
    FrontMesher();
    ~FrontMesher();
    
    void attachMesh(AdaptableMesh* msh);
    
    void setFrontId(int x);
/// a --> b
    void advanceFront(FrontLine& b, FrontLine& a);
    
private:

    void advanceEdge(FrontLine& b, int idx, const FrontLine& a );
    int addVertex(FrontLine& b, const Vector3F& pos);

};

}

#endif