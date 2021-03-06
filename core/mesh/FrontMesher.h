/*
 * FrontMesher.h
 * aloe
 *
 * create triangle mesh by advancing edge front
 * 2019/8/11
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
    virtual ~FrontMesher();
    
    void attach(AdaptableMesh* msh);
    void detach();
    
    void setFrontId(int x);
/// a --> b
    void advanceFront(FrontLine& b, FrontLine& a);
    
protected:
    AdaptableMesh *mesh();
    
private:

    void advanceEdge(FrontLine& b, int idx, const FrontLine& a );
    void advanceNoneEdge(FrontLine& b, const FrontEdge& ea, int idx, const FrontLine& a);
    void advanceQuadEdge(FrontLine& b, const FrontEdge& ea, int idx, const FrontLine& a);
    void advanceTriangleEdge(FrontLine& b, const FrontEdge& ea, int idx, const FrontLine& a);

    int addVertex(FrontLine& b, const Vector3F& pos);

};

}

#endif