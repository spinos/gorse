/*
 * FrontLine.h
 * aloe
 *  
 *  e0 , e1 , e2, ... , ei
 *
 */
 
#ifndef ALO_FRONT_LINE_H
#define ALO_FRONT_LINE_H

#include "FrontEdge.h"
#include <deque>

namespace alo {

class FrontLine {
    
    std::deque<FrontEdge> m_edges;
    std::deque<FrontVertex> m_vertices;
    
public:
    FrontLine();
    ~FrontLine();
    

    void addVertex(Vector3F* pos, const Vector3F& dir, int id);
    void closeLine();
    void clearLine();
    
    int numVertices() const;
    int numEdges() const;
    const FrontEdge& getEdge(int i) const;
    const FrontVertex& getVertex(int i) const;
/// v0 of first edge
    const FrontVertex* head() const;
/// v1 of last edge
    const FrontVertex* tail() const;
    
    friend std::ostream& operator<<(std::ostream &output, const FrontLine & p);
    
private:
    void addEdge(FrontVertex* v0, FrontVertex* v1);
    
};

}

#endif