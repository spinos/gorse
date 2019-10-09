/*
 *  CurveEmitter.h
 *  pbd
 *
 *  2019/10/6
 */
 
#include "CurveEmitter.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>
#include <math/Constant.h>

namespace alo {
    
struct CurveEmitter::Impl {
    
    SimpleBuffer<Vector3F> _pos;
    
};

CurveEmitter::CurveEmitter() : m_pimpl(new Impl)
{}

void CurveEmitter::begin()
{ m_pimpl->_pos.resetBuffer(0); }

void CurveEmitter::end()
{
    clearRegions();
    const int n = m_pimpl->_pos.count();
    for(int i=0;i<n;i+=2) {
        
        const int nleft = n - i;

        regionBegin();
        
        const int nv = nleft < 7 ? nleft : 4;
        
        for(int j=0;j<nv;++j) {
            addRegionIndex(i + j);
        }
        
        regionEnd();
        
        if(nv > 4) break;
    }
}

void CurveEmitter::operator<<(const Vector3F &x)
{ m_pimpl->_pos << x; }

void CurveEmitter::coreSetSeed(int x)
{}

void CurveEmitter::coreUpdate()
{
    const int n = m_pimpl->_pos.count();
            
    createParticles(n);
    
    const float dm = .7f / (float)n;
    for(int i=0;i<n;++i) {
        setPosition(m_pimpl->_pos[i], i);
        setVelocity(Vector3F::Zero, i);
        const float mi = i<1 ? Constant::VeryHigh : 1.f - dm * i ;
        setMass(mi, i);
    }
}
    
}