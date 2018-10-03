/*
 * FrontEdge.cpp
 * aloe
 *
 */
 
#include "FrontEdge.h"

namespace alo {
    
FrontEdge::FrontEdge() : 
    m_v0(0), m_v1(0),
    m_f0(-1), m_f1(-1),
    m_e0(0), m_e1(0)
{}

FrontEdge::FrontEdge(FrontVertex* v0, FrontVertex* v1) : 
    m_v0(v0), m_v1(v1),
    m_f0(-1), m_f1(-1),
    m_e0(0), m_e1(0)
{}

FrontVertex* &FrontEdge::v0()
{ return m_v0; }

FrontVertex* &FrontEdge::v1()
{ return m_v1; }

FrontEdge* &FrontEdge::e0()
{ return m_e0; }

FrontEdge* &FrontEdge::e1()
{ return m_e1; }

const FrontVertex* FrontEdge::v0() const
{ return m_v0; }

const FrontVertex* FrontEdge::v1() const
{ return m_v1; }

const FrontEdge* FrontEdge::e0() const
{ return m_e0; }

const FrontEdge* FrontEdge::e1() const
{ return m_e1; }

bool FrontEdge::connect(FrontEdge* another)
{
    if(another->v0() != v1()) 
        return false;
    
    m_e1 = another;
    another->e0() = this;
    return true;
}

bool FrontEdge::connectBack(FrontEdge* another)
{
    if(*another->v1() == *v0()) { 
        m_e0 = another;
        another->e1() = this;
        return true;
    }
    return false;
}

}