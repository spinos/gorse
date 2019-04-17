/*
 *  SvtTraverseRule.h
 *  aloe
 *
 *  Tc is sfc
 *  N is subdiv order
 *
 *  Created by zhang on 18-2-19.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_SVT_TRAVERSE_RULE_H
#define ALO_SDS_SVT_TRAVERSE_RULE_H

#include <math/rayBox.h>
#include <iostream>

namespace alo {

namespace sds {

template<typename Tc, int N>
class SvtTraverseRule {

/// space filling curve
	Tc m_sfc;
/// threshold to end subdiv
    float m_minSpan;
        
public:
    
    SvtTraverseRule();
    
/// T is tree
    template<typename T>
    void attach(const T& svt);
/// small enough
    bool isLeafCell(const float& halfSpan) const;
    bool isInnerCell(const float& halfSpan) const;
/// sub cell where p is in b
/// update cell coord    
    int keyToCell(float* cellCoord, 
            const float* p, const float* b) const;
    int keyToPoint(const float* p) const;
/// level - N box by p
    void getCoarserCell(float* cellCoord, int& parentLevel,
                    const float* p) const;

    static bool RayBoxIntersect(float* r, const float* b);
/// p is ray at t0 
    static bool IsRayExitingBox(const float* p, 
                                const float* r, const float* b);
    static void PrintRay(const float* r);
    static void PrintRayPnt(const float* r);
    static void PrintRayRange(const float* r);
    static void PrintBox(const float* b);
    static void PrintBoxRange(const float* b);
    static void PrintPnt(const float* p);
    
protected:

private:
    
};

template<typename Tc, int N>
SvtTraverseRule<Tc, N>::SvtTraverseRule()
{}

template<typename Tc, int N>
template<typename T>
void SvtTraverseRule<Tc, N>::attach(const T& svt)
{ 
    m_sfc.setCoord(svt.coord() ); 
    m_minSpan = svt.minSubdivSpan();
    
}

template<typename Tc, int N>
bool SvtTraverseRule<Tc, N>::isLeafCell(const float& halfSpan) const
{ return halfSpan < m_minSpan; }

template<typename Tc, int N>
bool SvtTraverseRule<Tc, N>::isInnerCell(const float& halfSpan) const
{ return halfSpan > m_minSpan; }

template<typename Tc, int N>
int SvtTraverseRule<Tc, N>::keyToCell(float* cellCoord, 
                        const float* p, const float* b) const
{
    const int d = 1<<(N-1);
    const int l = (1<<N) - 1;
    float h = b[3] / d;
    cellCoord[0] = p[0];
    cellCoord[1] = p[1];
    cellCoord[2] = p[2];
    cellCoord[3] = h * .5f;
    
    cellCoord[0] = (cellCoord[0] - b[0] + b[3] ) / h;
    cellCoord[1] = (cellCoord[1] - b[1] + b[3] ) / h;
    cellCoord[2] = (cellCoord[2] - b[2] + b[3] ) / h;
   
    cellCoord[0] = (int)cellCoord[0];
    
    if(cellCoord[0]>l) {
        cellCoord[0] = l;
    }
        
    cellCoord[1] = (int)cellCoord[1];
    
    if(cellCoord[1]>l) {
        cellCoord[1] = l;
    }
    
    cellCoord[2] = (int)cellCoord[2];
    
    if(cellCoord[2]>l) {
        cellCoord[2] = l;
    }
    
    cellCoord[0] = b[0] - b[3] + h * (cellCoord[0]+.5f);
    cellCoord[1] = b[1] - b[3] + h * (cellCoord[1]+.5f);
    cellCoord[2] = b[2] - b[3] + h * (cellCoord[2]+.5f);
    
    return m_sfc.computeKey(cellCoord);
}

template<typename Tc, int N>
int SvtTraverseRule<Tc, N>::keyToPoint(const float* p) const
{ return m_sfc.computeKey(p); }

template<typename Tc, int N>
void SvtTraverseRule<Tc, N>::getCoarserCell(float* cellCoord, int& parentLevel,
                                    const float* p) const
{

    const float s = cellCoord[3] * 2;
    parentLevel = m_sfc.computeLevelBySpan(s) - N;
    m_sfc.getBoxAtLevel(cellCoord, p, parentLevel);
}

template<typename Tc, int N>
bool SvtTraverseRule<Tc, N>::RayBoxIntersect(float* r, const float* b)
{ return rayBoxIntersect(r, b); }

template<typename Tc, int N>
bool SvtTraverseRule<Tc, N>::IsRayExitingBox(const float* p,
                        const float* r, const float* b)
{ return rayExitingBox(p, r, b); }

template<typename Tc, int N>
void SvtTraverseRule<Tc, N>::PrintRay(const float* r)
{
    std::cout<<"\n ray ("<<r[0]
            <<", "<<r[1]
            <<", "<<r[2]
            <<"), ("<<r[3]
            <<", "<<r[4]
            <<", "<<r[5]
            <<"), "<<r[6]
            <<", "<<r[7];
}

template<typename Tc, int N>
void SvtTraverseRule<Tc, N>::PrintRayPnt(const float* r)
{
    std::cout<<"\n ray ("<<(r[0] + r[3] * r[6])
            <<", "<<(r[1] + r[4] * r[6])
            <<", "<<(r[2] + r[5] * r[6])
            <<"), ("<<(r[0] + r[3] * r[7])
            <<", "<<(r[1] + r[4] * r[7])
            <<", "<<(r[2] + r[5] * r[7])
            <<") ";
}

template<typename Tc, int N>
void SvtTraverseRule<Tc, N>::PrintRayRange(const float* r)
{
    std::cout<<"\n t ("<<r[6]
                <<", "<<r[7]<<") ";
}

template<typename Tc, int N>
void SvtTraverseRule<Tc, N>::PrintBox(const float* b)
{
    std::cout<<"\n box ("<<b[0]
            <<", "<<b[1]
            <<", "<<b[2]
            <<"), "<<b[3];
}

template<typename Tc, int N>
void SvtTraverseRule<Tc, N>::PrintBoxRange(const float* b)
{
    std::cout<<"\n box ("<<(b[0] - b[3])
            <<", "<<(b[1] - b[3])
            <<", "<<(b[2] - b[3])
            <<"), ("<<(b[0] + b[3])
            <<", "<<(b[1] + b[3])
            <<", "<<(b[2] + b[3])
            <<") ";
}

template<typename Tc, int N>
void SvtTraverseRule<Tc, N>::PrintPnt(const float* p)
{
    std::cout<<"\n p ("<<p[0]
            <<", "<<p[1]
            <<", "<<p[2]
            <<") ";
}

}

}

#endif
