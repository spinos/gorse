/*
 * FrontMesher.cpp
 * aloe
 *
 *
 */
 
#include "FrontMesher.h"
#include <geom/AdaptableMesh.h>

namespace alo {

FrontMesher::FrontMesher() : m_frontId(0)
{}

FrontMesher::~FrontMesher()
{}

void FrontMesher::setFrontId(int x)
{ m_frontId = x; }

void FrontMesher::attachMesh(AdaptableMesh* msh)
{ m_msh = msh; }

void FrontMesher::advanceFront(FrontLine& b, FrontLine& a)
{
    a.preAdvance();
    const int ne = a.numEdges();
    for(int i=0;i<ne;++i) {
        advanceEdge(b, i, a);
    }
    
}

void FrontMesher::advanceEdge(FrontLine& b, int idx, const FrontLine& a)
{
    const FrontEdge& ea = a.getEdge(idx);

    if(ea.advanceType() == FrontEdge::GenTriangle )
        advanceTriangleEdge(b, ea, idx, a);
    else
        advanceQuadEdge(b, ea, idx, a);
}

void FrontMesher::advanceTriangleEdge(FrontLine& b, const FrontEdge& ea, int idx, const FrontLine& a)
{
    const FrontVertex* va0 = ea.v0();
    const FrontVertex* va1 = ea.v1();
    Vector3F pvb0 = (a.getAdvanceToPos(va0)
                    + a.getAdvanceToPos(va1) ) * .5f;
    int ivb0;
    if(idx < 1) {  
/// first one
        ivb0 = addVertex(b, pvb0);
        
    } else if(idx == a.numEdges() - 1) {
/// last edge is closed
        if(ea.e1()) {
            const FrontVertex* bhead = b.head();
            ivb0 = bhead->id();
            b.closeLine();
        } else
            ivb0 = addVertex(b, pvb0);
    } else {
        ivb0 = b.tail()->id();
        Vector3F* p0 = b.tail()->pos();
        *p0 = *p0 * .5f + pvb0 * .5f;
    }

    m_msh->addTriangle(va0->id(), va1->id(), ivb0);
}

void FrontMesher::advanceQuadEdge(FrontLine& b, const FrontEdge& ea, int idx, const FrontLine& a)
{
    const FrontVertex* va0 = ea.v0();
    const FrontVertex* va1 = ea.v1();
    Vector3F pvb0 = a.getAdvanceToPos(va0);
    Vector3F pvb1 = a.getAdvanceToPos(va1);
    int ivb0, ivb1;
    if(idx < 1) {  
/// first two
        ivb0 = addVertex(b, pvb0);
        ivb1 = addVertex(b, pvb1);
        
    } else if(idx == a.numEdges() - 1) {
        ivb0 = b.tail()->id();
/// last edge is closed
        if(ea.e1()) {
            const FrontVertex* bhead = b.head();
            ivb1 = bhead->id();
            b.closeLine();
        } else
            ivb1 = addVertex(b, pvb1);
    } else {
        ivb0 = b.tail()->id();
        ivb1 = addVertex(b, pvb1);
    }
    bool flipped = (idx + m_frontId) & 1;
    m_msh->addQuad(va0->id(), va1->id(), ivb1, ivb0, flipped);
      
}

int FrontMesher::addVertex(FrontLine& b, const Vector3F& pos)
{
    int i = m_msh->addVertex(pos);
    b.addVertex(m_msh->vertexPositionR(i), i);
    return i;
}

}