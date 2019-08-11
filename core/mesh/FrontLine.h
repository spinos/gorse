/*
 * FrontLine.h
 * aloe
 *  
 *  e0 , e1 , e2, ... , ei
 * 2019/8/11
 */
 
#ifndef ALO_FRONT_LINE_H
#define ALO_FRONT_LINE_H

#include "FrontEdge.h"
#include <math/Matrix33F.h>
#include <deque>

namespace alo {
    
struct Float2;

class FrontLine {
    
    std::deque<FrontEdge> m_edges;
    std::deque<FrontVertex> m_vertices;
/// world space
    Matrix33F m_mat;
    Matrix33F m_invmat;
/// rotation translation scaling in local space
    Matrix33F m_rot;
    Vector3F m_dir;
    float m_shrink;
    Vector3F m_center;
    float m_length;
/// facing of area surrounded 
    Vector3F m_normal;

    static float MinEdgeLength;

public:
    FrontLine();
    ~FrontLine();

    void setWorldSpace(const Matrix33F& mat);
    void setLocalRotation(const Matrix33F& mat);
/// pitch(y) then yaw(z)
    void setLocalRotation(const Float2& pitchYaw);
    void rotateLocalBy(const Quaternion& q);
    void setDirection(const Vector3F& v);
    void setShrinking(float x);

    void addVertex(Vector3F* pos, int id);
    void closeLine();
/// no edge added
    void closeLine1();
    void clearLine();
    
    void preAdvance();

    int numVertices() const;
    int numEdges() const;
    FrontEdge& lastEdge();
    
    const FrontEdge& getEdge(int i) const;
    const FrontVertex& getVertex(int i) const;
/// v0 of first edge
    const FrontVertex* head() const;
/// v1 of last edge
    const FrontVertex* tail() const;
    FrontVertex* tail();
/// to local, transform, to world
    Vector3F getAdvanceToPos(const FrontVertex* vert) const;
    const float& length() const;
    float getAverageEdgeLength();

    void smooth(const float& wei = .07f);

    void setMinEdgeLength(const float& x);

    void rotateTo(FrontLine& b) const;

    const Matrix33F& worldRotation() const;
    
    static bool EnableMerging;
    
    friend std::ostream& operator<<(std::ostream &output, const FrontLine & p);

protected:
    Vector3F toWorld(const Vector3F& v) const;
    Vector3F toLocal(const Vector3F& v) const;

    
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