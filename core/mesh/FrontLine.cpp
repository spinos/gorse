/*
 * FrontLine.cpp
 * aloe
 *
 */
 
#include "FrontLine.h"

namespace alo {
    
FrontLine::FrontLine() : 
m_dir(0,1,0), m_shrink(.07f)
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

void FrontLine::setDirection(const Vector3F& v)
{ m_dir = v; }

void FrontLine::setShrinking(float x)
{ m_shrink = x; }

void FrontLine::addVertex(Vector3F* pos, int id)
{
    FrontVertex* v0 = 0;
    if(m_vertices.size() > 0)
        v0 = &m_vertices.back();
    
    m_vertices.push_back(FrontVertex(pos, id) );
    
    if(v0) 
        addEdge(v0, &m_vertices.back() );
}

void FrontLine::addEdge(FrontVertex* v0, FrontVertex* v1)
{ 
    FrontEdge e1(v0, v1);
    m_edges.push_back(e1);
    if(m_edges.size() > 1) {
        FrontEdge& e0 = m_edges[m_edges.size() - 2];
        bool connected = e0.connect(&m_edges.back());
        if(!connected) std::cout<<" cannot connect "<<e0<<" and "<<m_edges.back();
    } 
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

void FrontLine::calcCenter()
{
    m_center.set(0.f, 0.f, 0.f);
    float w = 0.f;
    std::deque<FrontEdge>::const_iterator it = m_edges.begin();
    for(;it != m_edges.end();++it) {
        
        float wi = it->getLength();
        w += wi;
        m_center += it->getCenter() * wi;
        
    }
    m_center /= w;
}

void FrontLine::calcNormal()
{
    m_normal.setZero();

    std::deque<FrontEdge>::const_iterator it = m_edges.begin();
    for(;it != m_edges.end();++it) {
        
        m_normal += it->getDv().cross(m_center - it->getCenter());
        
    }

    m_normal.normalize();
}

void FrontLine::calcVertexDirection()
{
    calcCenter();
    std::deque<FrontVertex>::iterator vit = m_vertices.begin();
    for(;vit != m_vertices.end();++vit) {
        
        Vector3F v2c = m_center - *vit->pos();
        vit->dir() = v2c * m_shrink;
    }

    std::deque<FrontEdge>::iterator eit = m_edges.begin();
    for(;eit != m_edges.end();++eit) {
        
        Vector3F v2c = (m_center - eit->getCenter() ) * m_shrink;
        eit->v0()->modifyDir(v2c);
        eit->v1()->modifyDir(v2c);
    }
}

void FrontLine::calcVertexCurvature()
{
    calcNormal();
    std::deque<FrontEdge>::iterator it = m_edges.begin();
    for(;it != m_edges.end();++it) {

        if(!it->e0()) {
            it->v0()->curvature() = 0.f;
        }

        it->v1()->curvature() = it->angleToE1(m_normal);
    }
}

Vector3F FrontLine::getAdvanceToPos(const FrontVertex* vert) const
{
    return *vert->pos() + m_dir + vert->dir();
}

void FrontLine::preAdvance()
{
    calcVertexDirection();
    calcVertexCurvature();
}

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
        output << " " << ei;
    }
    output << " ] ";
    return output;
}

}