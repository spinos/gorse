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
    Vector3F m_dir;
    float m_shrink;
    Vector3F m_center;
    float m_length;
/// rotate around
    Vector3F m_normal;
    static float MinEdgeLength;

public:
    FrontLine();
    ~FrontLine();
    
    void setDirection(const Vector3F& v);
    void setShrinking(float x);

    void addVertex(Vector3F* pos, int id);
    void closeLine();
    void clearLine();
    
    void preAdvance();

    int numVertices() const;
    int numEdges() const;
    const FrontEdge& getEdge(int i) const;
    const FrontVertex& getVertex(int i) const;
/// v0 of first edge
    const FrontVertex* head() const;
/// v1 of last edge
    const FrontVertex* tail() const;

    FrontVertex* tail();

    Vector3F getAdvanceToPos(const FrontVertex* vert) const;

    void smooth(const float& wei = .07f);

    void setMinEdgeLength(const float& x);
    
    friend std::ostream& operator<<(std::ostream &output, const FrontLine & p);
    
private:
    void addEdge(FrontVertex* v0, FrontVertex* v1);
    void calcCenter();
    void calcNormal();
    void calcVertexDirection();
/// change of tangent direction at each vertex
    void calcVertexCurvature();
    void calcLength();
    void calcEdgeAdvanceType();
};

}

#endif