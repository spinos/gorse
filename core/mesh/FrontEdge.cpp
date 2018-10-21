/*
 * FrontEdge.cpp
 * aloe
 *
 */
 
#include "FrontEdge.h"
#include <math/miscfuncs.h>

namespace alo {
    
FrontEdge::FrontEdge() : 
    m_v0(0), 
    m_v1(0),
    m_e0(0), 
    m_e1(0),
    m_advanceType(GenQuad),
    m_history(EhNone)
{}

FrontEdge::FrontEdge(FrontVertex* v0, FrontVertex* v1) : 
    m_v0(v0), 
    m_v1(v1),
    m_e0(0), 
    m_e1(0),
    m_advanceType(GenQuad),
    m_history(EhNone)
{}

FrontVertex* &FrontEdge::v0()
{ return m_v0; }

FrontVertex* &FrontEdge::v1()
{ return m_v1; }

FrontEdge* &FrontEdge::e0()
{ return m_e0; }

FrontEdge* &FrontEdge::e1()
{ return m_e1; }

void FrontEdge::setAdvanceType(GenType x)
{ 
    if(x == GenTriangle) {
        if(e0()) {
            if(e0()->advanceType() == x) return;
        }
    }
    if(x == GenNone) {
        if(historyType() == EhMerge) return;
    }
    m_advanceType = x; 
}

void FrontEdge::setHistoryType(EdgeHistory x)
{ m_history = x; }

const FrontVertex* FrontEdge::v0() const
{ return m_v0; }

const FrontVertex* FrontEdge::v1() const
{ return m_v1; }

const FrontEdge* FrontEdge::e0() const
{ return m_e0; }

const FrontEdge* FrontEdge::e1() const
{ return m_e1; }

const FrontEdge::GenType& FrontEdge::advanceType() const
{ return m_advanceType; }

const FrontEdge::EdgeHistory& FrontEdge::historyType() const
{ return m_history; }

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

Vector3F FrontEdge::getCenter() const
{ return (*v0()->pos() + *v1()->pos() ) * .5f; }

float FrontEdge::angleToE1(const Vector3F& nml) const
{
    if(!e1()) return 0.f;

    const FrontVertex* vc = e1()->v1();

    const Vector3F ab = *v1()->pos() - *v0()->pos();
    const Vector3F bc = *vc->pos() - *v1()->pos();
    const Vector3F ac = *vc->pos() - *v0()->pos();
    Vector3F abxbc = ab.cross(bc);
    const float labxbc = abxbc.length();
    if(labxbc < 1e-6f) return 0.f;

    float theta = acos(ab.normal().dot(bc.normal()));

    if(nml.dot(abxbc) < 0.f) theta = -theta;
    return theta;
}

float FrontEdge::getLength() const
{ return getDv().length(); }

Vector3F FrontEdge::getDv() const
{ return *v1()->pos() - *v0()->pos(); }

bool FrontEdge::isFlat() const
{ return (  Absolute<float>(v0()->curvature()) < 0.16f && Absolute<float>(v1()->curvature()) < 0.16f) ; }

void FrontEdge::averagePosition(const float& wei)
{
    if(e0())
        v0()->modifyPos((*e0()->v0()->pos() + *v1()->pos() ) * .5f, wei);

    if(e1())
        v1()->modifyPos((*e1()->v1()->pos() + *v0()->pos() ) * .5f, wei);
}

std::ostream& operator<<(std::ostream &output, const FrontEdge & p) 
{
    output << " (" << p.v0()->id() <<"," << p.v1()->id() <<")";
    if(p.advanceType() != FrontEdge::GenQuad) 
        output<<" "<<p.advanceType();
    return output;
}

}