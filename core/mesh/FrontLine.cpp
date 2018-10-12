/*
 * FrontLine.cpp
 * aloe
 *
 */
 
#include "FrontLine.h"

namespace alo {

float FrontLine::MinEdgeLength = 0.f;
    
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

void FrontLine::setWorldSpace(const Matrix33F& mat)
{
    m_invmat = m_mat = mat;
    m_invmat.inverse();
}

void FrontLine::setLocalRotation(const Matrix33F& mat)
{ m_rot = mat; }

void FrontLine::rotateLocalBy(const Quaternion& q)
{ m_rot *= Matrix33F(q); }

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

void FrontLine::closeLine1()
{
    FrontVertex* v0 = m_edges.back().v1();
    FrontVertex* v1 = m_edges.front().v0();
    v0->pos()->mixWith(*v1->pos(), .5f);
    m_edges.back().v1() = m_edges.front().v0();
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
{ 
    if(m_edges.size() < 1)
        return &m_vertices[0];
    return m_edges.front().v0(); 
}

const FrontVertex* FrontLine::tail() const
{ 
    if(m_edges.size() < 1)
        return &m_vertices[0];
    return m_edges.back().v1(); 
}

FrontVertex* FrontLine::tail()
{ 
    if(m_edges.size() < 1)
        return &m_vertices[0];
    return m_edges.back().v1(); 
}

FrontEdge& FrontLine::lastEdge()
{ return m_edges.back(); }

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
        
        Vector3F v2c = toLocal(m_center - *vit->pos()) * m_shrink;
        vit->dir() = v2c;
    }

    std::deque<FrontEdge>::iterator eit = m_edges.begin();
    for(;eit != m_edges.end();++eit) {
        
        Vector3F v2c = toLocal(m_center - eit->getCenter() ) * m_shrink;
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

void FrontLine::calcLength()
{
    m_length = 0.f;
    std::deque<FrontEdge>::const_iterator it = m_edges.begin();
    for(;it != m_edges.end();++it) {

        m_length += it->getDv().length();
    }
}

void FrontLine::calcEdgeAdvanceType()
{
    const float mergeL = length() / (float)numEdges() * 1.4f;
    const bool canConverge = m_edges.size() > 8;
    std::deque<FrontEdge>::iterator it = m_edges.begin();
    for(;it != m_edges.end();++it) {

        const FrontVertex* va0 = it->v0();
        const FrontVertex* va1 = it->v1();
        Vector3F pvb0 = getAdvanceToPos(va0);
        Vector3F pvb1 = getAdvanceToPos(va1);

        const float ml = pvb0.distanceTo(*va0->pos()) * .5f 
                        + pvb1.distanceTo(*va1->pos()) * .5f;

        if(ml < mergeL && it->historyType() != FrontEdge::EhMerge)
            it->setAdvanceType(FrontEdge::GenNone);

/// latitude merge has the priority 
        if(canConverge && it->advanceType() > FrontEdge::GenNone) {
            if(pvb0.distanceTo(pvb1) < MinEdgeLength || it->isFlat() )
                it->setAdvanceType(FrontEdge::GenTriangle);
        }

    }

/// latitude merge line is stradlled by triangles
    it = m_edges.begin();
    for(;it != m_edges.end();++it) {

        bool be0non = false;
        bool be1non = false;

        if(it->e0())
            be0non = it->e0()->advanceType() == FrontEdge::GenNone;

         if(it->e1())
            be1non = it->e1()->advanceType() == FrontEdge::GenNone;

        if(be0non) {
            if(be1non)
                it->setAdvanceType(FrontEdge::GenNone);
            else
                it->setAdvanceType(FrontEdge::GenTriangle);
        }

        if(be1non) {
            if(be0non)
                it->setAdvanceType(FrontEdge::GenNone);
            else
                it->setAdvanceType(FrontEdge::GenTriangle);
        }

    }

/// cannot have continuous triangles
    it = m_edges.begin();
    for(;it != m_edges.end();++it) {
        if(it->advanceType() == FrontEdge::GenTriangle) {
            if(it->e0()) {
                if(it->e0()->advanceType() == FrontEdge::GenTriangle)
                    it->setAdvanceType(FrontEdge::GenQuad);
            }
        }
    }

#if 1
    int nt = 0, nn = 0;
    it = m_edges.begin();
    for(;it != m_edges.end();++it) {
        if(it->advanceType() == FrontEdge::GenTriangle) 
            nt++;
        if(it->advanceType() == FrontEdge::GenNone) 
            nn++;
    }
    std::cout<<"\n triangle "<<nt<<" none "<<nn<<" out of "<<m_edges.size();
#endif
}

Vector3F FrontLine::getAdvanceToPos(const FrontVertex* vert) const
{
    Vector3F vloc = toLocal(*vert->pos() - m_center) + vert->dir() + m_dir;
    return  m_center + toWorld(m_rot.transform(vloc) );
}

void FrontLine::preAdvance()
{
    calcVertexDirection();
    calcVertexCurvature();
    calcLength();
    calcEdgeAdvanceType();
}

void FrontLine::smooth(const float& wei)
{
    std::deque<FrontEdge>::iterator it = m_edges.begin();
    for(;it != m_edges.end();++it) {

        it->averagePosition(wei);
            
    }
}

void FrontLine::setMinEdgeLength(const float& x)
{ MinEdgeLength = x; }

void FrontLine::rotateTo(FrontLine& b) const
{ b.setWorldSpace(m_mat * m_rot); }

Vector3F FrontLine::toWorld(const Vector3F& v) const
{ return m_mat.transform(v); }

Vector3F FrontLine::toLocal(const Vector3F& v) const
{ return m_invmat.transform(v); }

const Matrix33F& FrontLine::worldRotation() const
{ return m_mat; }

const float& FrontLine::length() const
{ return m_length; }

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
        if(ei.advanceType() < FrontEdge::GenQuad) output << " " << ei;
    }
    output << " ] ";
    return output;
}

}