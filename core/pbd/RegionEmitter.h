/*
 *  RegionEmitter.h
 *  pbd
 *
 *  2019/10/6
 */
 
#ifndef ALO_REGION_EMITTER_H
#define ALO_REGION_EMITTER_H

#include <boost/shared_ptr.hpp>

namespace alo {

class RegionEmitter {
    
    struct Impl;
    boost::shared_ptr<Impl> m_pimpl;
  
public:
    
    const int &numRegions() const;
    int regionIndexCount(const int i) const;
    const int *regionIndices(const int i) const;
    
protected:

    RegionEmitter();
    virtual ~RegionEmitter();

    void clearRegions();
    void regionBegin();
    void regionEnd();
    void addRegionIndex(const int x);
    
private:
    
};

}

#endif
