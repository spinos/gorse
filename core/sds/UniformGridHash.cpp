/*
 *  UniformGridHash.cpp
 *  aloe
 *
 *  2019/10/7
 */
 
#include "UniformGridHash.h"
#include <math/BoundingBox.h>
#include <math/SimpleBuffer.h>
#include <math/Constant.h>
#include <math/QuickSort.h>
#include <math/Int3.h>
#include <math/Int2.h>

namespace alo {
    
namespace sds {
    
int UniformGridHash::TwentySevenOffset[27][3] = {
{-1,-1,-1},
{ 0,-1,-1},
{ 1,-1,-1},
{-1, 0,-1},
{ 0, 0,-1},
{ 1, 0,-1},
{-1, 1,-1},
{ 0, 1,-1},
{ 1, 1,-1},
{-1,-1, 0},
{ 0,-1, 0},
{ 1,-1, 0},
{-1, 0, 0},
{ 0, 0, 0},
{ 1, 0, 0},
{-1, 1, 0},
{ 0, 1, 0},
{ 1, 1, 0},
{-1,-1, 1},
{ 0,-1, 1},
{ 1,-1, 1},
{-1, 0, 1},
{ 0, 0, 1},
{ 1, 0, 1},
{-1, 1, 1},
{ 0, 1, 1},
{ 1, 1, 1}
};
    
struct UniformGridHash::KeyInd {
    int _key;
    int _ind;
};
  
struct UniformGridHash::Impl {
    
    SimpleBuffer<KeyInd> _table;
    SimpleBuffer<int> _cellStart;
    SimpleBuffer<int> _cellFinish;
    Vector3F _gridOrigin;
    float _cellSize;
    float _invCellSize;
    
    Impl() : _gridOrigin(0.f, 0.f, 0.f),
    _cellSize(1.f),
    _invCellSize(1.f)
    {
        _cellStart.resetBuffer(2097152);
        _cellFinish.resetBuffer(2097152);
    }
    
};

UniformGridHash::UniformGridHash() :
m_pimpl(new Impl)
{}

void UniformGridHash::setCellSize(const float x)
{ 
    m_pimpl->_cellSize = x; 
    m_pimpl->_invCellSize = 1.f / x;
}

void UniformGridHash::setGridBox(const BoundingBox &b)
{
    float &h = m_pimpl->_cellSize;
    m_pimpl->_gridOrigin.set(b.getMin(0) - h, b.getMin(1) - h, b.getMin(2) - h);
    const float span = h * 2.f + b.getLongestDistance();
    if(span > h * 128.f) {
        h = span / 128.f;
        std::cout << "\n WARNING higher cell size " << h;
    }
}

void UniformGridHash::setTableSize(const int n)
{
    if(n != m_pimpl->_table.count())
        m_pimpl->_table.resetBuffer(n);
}

void UniformGridHash::mapPosition(const Vector3F &p, const int v, const int i)
{
    KeyInd &y = m_pimpl->_table[i];
    y._key = calculatePositionHash(p);
    y._ind = v;
}

int UniformGridHash::calculatePositionHash(const Vector3F &p) const
{
    const Vector3F q = p - m_pimpl->_gridOrigin;
    if(q.x < 0.f || q.y < 0.f || q.z < 0.f) 
        return Constant::HighestInt;
    
    int gx = q.x * m_pimpl->_invCellSize;
    int gy = q.y * m_pimpl->_invCellSize;
    int gz = q.z * m_pimpl->_invCellSize;
    if(gx > 127 || gy > 127 || gz > 127) 
        return Constant::HighestInt;
    
    return gz * 16384 + gy * 128 + gx;
}

void UniformGridHash::findCellStarts()
{
    SimpleBuffer<KeyInd> &table = m_pimpl->_table;
    const int &n = table.count();
    QuickSort1::SortByKey<int, KeyInd>(table.data(), 0, n - 1);
    
/// reset to -1
    memset(m_pimpl->_cellStart.data(), 255, 2097152<<2);

    int preK = -1;
    int i=0;
    for(;i<n;++i) {
        const int &curK = table[i]._key;
        if(curK > 2097152) break;
        if(curK != preK) {
            if(preK > -1) m_pimpl->_cellFinish[preK] = i;
            preK = curK;
            m_pimpl->_cellStart[curK] = i;
        }
    }
    if(preK > -1) m_pimpl->_cellFinish[preK] = i;
}

Int3 UniformGridHash::calculateCellCoord(const Vector3F &p) const
{
    const Vector3F q = p - m_pimpl->_gridOrigin;
    return Int3(q.x * m_pimpl->_invCellSize,
                q.y * m_pimpl->_invCellSize,
                q.z * m_pimpl->_invCellSize);
}

Int3 UniformGridHash::getOffsetCellCoord(const Int3 &c, const int i) const
{
    return Int3(c.x + TwentySevenOffset[i][0],
                c.y + TwentySevenOffset[i][1],
                c.z + TwentySevenOffset[i][2]);
}

int UniformGridHash::calculateCellHash(const Int3 &c) const
{
    if(c.x < 0 || c.x > 127) return -1;
    if(c.y < 0 || c.y > 127) return -1;
    if(c.z < 0 || c.z > 127) return -1;
    return c.z * 16384 + c.y * 128 + c.x;
}

Int2 UniformGridHash::getCellRange(const Int3 &c) const
{
    int i = calculateCellHash(c);
    if(i<0) return Int2(-1,-1);
    return Int2(m_pimpl->_cellStart[i], m_pimpl->_cellFinish[i]);
}

const int &UniformGridHash::indirection(const int i) const
{ return m_pimpl->_table[i]._ind; }

}
  
}
