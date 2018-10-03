/*
 * FrontEdge.h
 * aloe
 *
 *      f1
 *  v0 ---> v1
 *      f0
 *   
 *  e0 -> e -> e1
 *
 */
 
#ifndef ALO_FRONT_EDGE_H
#define ALO_FRONT_EDGE_H

#include "FrontVertex.h"

namespace alo {

class FrontEdge {

/// connected to vertices and faces
    FrontVertex* m_v0;
    FrontVertex* m_v1;
    int m_f0, m_f1;
/// incoming and outgoing edge
    FrontEdge* m_e0;
    FrontEdge* m_e1;
    
public:
    FrontEdge();
    FrontEdge(FrontVertex* v0, FrontVertex* v1);
    
    bool connect(FrontEdge* another);
    bool connectBack(FrontEdge* another);
    
    FrontVertex* &v0();
    FrontVertex* &v1();
    
    FrontEdge* &e0();
    FrontEdge* &e1();
    
    const FrontVertex* v0() const;
    const FrontVertex* v1() const;
    
    const FrontEdge* e0() const;
    const FrontEdge* e1() const;
    
private:

};

}

#endif