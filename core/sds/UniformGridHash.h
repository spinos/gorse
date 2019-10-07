/*
 *  UniformGridHash.h
 *  aloe
 *
 *  2019/10/7
 */
 
#ifndef ALO_UNIFORM_GRID_HASH_H
#define ALO_UNIFORM_GRID_HASH_H

#include <boost/shared_ptr.hpp>

namespace alo {
    
class BoundingBox;
class Vector3F;
struct Int3;

namespace sds {
  
class UniformGridHash {
    
public:

    UniformGridHash();
    
    void setCellSize(const float x);
    void setGridBox(const BoundingBox &b);
    void setTableSize(const int n);
    
    void mapPosition(const Vector3F &p, const int v, const int i);
    
    void findCellStarts();
    
    Int3 calculateCellCoord(const Vector3F &p) const;
    Int3 getOffsetCellCoord(const Int3 &c, const int i) const;
    int getCellStart(const Int3 &c) const;

protected:

    int calculatePositionHash(const Vector3F &p) const;
    int calculateCellHash(const Int3 &c) const;
    
    static int TwentySevenOffset[27][3];
    
private:

    struct KeyInd;
    
    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
    
};

}
    
}

#endif
