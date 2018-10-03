/*
 * FrontLine.cpp
 * aloe
 *
 */
 
#include "FrontLine.h"

namespace alo {
    
FrontLine::FrontLine()
{}

FrontLine::~FrontLine()
{ 
    clearLine();
}

void FrontLine::clearLine()
{
    m_vertices.clear();
    m_edges.clear();
}

void FrontLine::addVertex(Vector3F* pos, const Vector3F& dir, int id)
{
    FrontVertex* v0 = 0;
    if(m_vertices.size() > 0)
        v0 = &m_vertices.back();
    
    m_vertices.push_back(FrontVertex(pos, dir, id) );
    
    if(v0) 
        addEdge(v0, &m_vertices.back() );
}

void FrontLine::addEdge(FrontVertex* v0, FrontVertex* v1)
{ 
    FrontEdge e1(v0, v1);
    if(m_edges.size() > 0) {
        FrontEdge& e0 = m_edges.back();
        e0.connect(&e1);
    }
    m_edges.push_back(e1); 
}

void FrontLine::closeLine()
{
    FrontVertex* v0 = m_edges.back().v1();
    FrontVertex* v1 = m_edges.front().v0();
    addEdge(v0, v1);
    m_edges.front().connectBack(&m_edges.back());
}

int FrontLine::numVertices() const
{ return m_vertices.size(); }

int FrontLine::numEdges() const
{ return m_edges.size(); }

const FrontEdge& FrontLine::getEdge(int i) const
{ return m_edges[i]; }

const FrontVertex& FrontLine::getVertex(int i) const
{ return m_vertices[i]; }

const FrontVertex* FrontLine::head() const
{ return m_edges.front().v0(); }

const FrontVertex* FrontLine::tail() const
{ return m_edges.back().v1(); }

std::ostream& operator<<(std::ostream &output, const FrontLine & p) 
{
    const int nv = p.numVertices();
    output << " nv: "<<nv<<" [";
    for(int i=0;i<nv;++i) {
        output << " " << p.getVertex(i).id();
    }
    output << " ] ";
    const int ne = p.numEdges();
    output <<" ne: "<<ne<<" [";
    for(int i=0;i<ne;++i) {
        const FrontEdge& ei = p.getEdge(i);
        output << " (" << ei.v0()->id() <<"," << ei.v1()->id() <<")";
    }
    output << " ] ";
    return output;
}

}