/*
 *  RegionEmitter.cpp
 *  pbd
 *
 *  2019/10/6
 */
 
#include "RegionEmitter.h"
#include <math/SimpleBuffer.h>

namespace alo {

struct RegionEmitter::Impl {
    
    int _numRegions;
    SimpleBuffer<int> _offset;
    SimpleBuffer<int> _index;
    
    Impl() : _numRegions(0)
    {}
    
};

RegionEmitter::RegionEmitter() : m_pimpl(new Impl)
{}

RegionEmitter::~RegionEmitter()
{}

void RegionEmitter::clearRegions()
{
    m_pimpl->_numRegions = 0;
    m_pimpl->_offset.resetBuffer(0);
    m_pimpl->_index.resetBuffer(0);
}

void RegionEmitter::regionBegin()
{
    m_pimpl->_numRegions++;
    m_pimpl->_offset << m_pimpl->_index.count();
}

void RegionEmitter::regionEnd()
{   
}

void RegionEmitter::addRegionIndex(const int x)
{ m_pimpl->_index << x; }

const int &RegionEmitter::numRegions() const
{ return m_pimpl->_numRegions; }

int RegionEmitter::regionIndexCount(const int i) const
{
    int a = m_pimpl->_offset[i];
    if(i >= numRegions()-1) return m_pimpl->_index.count() - a;
    return m_pimpl->_offset[i+1] - a;
}

const int *RegionEmitter::regionIndices(const int i) const
{ return &m_pimpl->_index[m_pimpl->_offset[i]]; }

}
