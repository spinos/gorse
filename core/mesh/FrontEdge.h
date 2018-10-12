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

public:

    enum GenType {
        GenNone = 0,
        GenTriangle,
        GenQuad,
        GenPentagon
    };

    enum EdgeHistory {
        EhNone = 8,
        EhMerge,
        EhSplit
    };

private:

    GenType m_advanceType; 
    EdgeHistory m_history;

/// connected to vertices and faces
    FrontVertex* m_v0;
    FrontVertex* m_v1;
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

    void setAdvanceType(GenType x);
    void setHistoryType(EdgeHistory x);
    
    const FrontVertex* v0() const;
    const FrontVertex* v1() const;
    
    const FrontEdge* e0() const;
    const FrontEdge* e1() const;

    const GenType& advanceType() const;
    const EdgeHistory& historyType() const;
    
    Vector3F getCenter() const;
    Vector3F getDv() const;
    float getLength() const;

    float angleToE1(const Vector3F& nml) const;
    bool isFlat() const;

    void averagePosition(const float& wei);

    friend std::ostream& operator<<(std::ostream &output, const FrontEdge & p);
    
    
private:

};

}

#endif