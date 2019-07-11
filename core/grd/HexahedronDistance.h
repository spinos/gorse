/*
 *  HexahedronDistance.h
 *
 *  sparse grid from samples
 *  distance field from cell corners
 *  find node closest to zero
 *  propagate distance
 *  gradient of distance as sample normals
 *
 *  2019/7/11
 */
 
#ifndef ALO_GRD_HEXAHEDRON_DISTANCE_H
#define ALO_GRD_HEXAHEDRON_DISTANCE_H

#include <sds/SparseCubicField.h>

namespace alo {
    
namespace grd {
    
template<typename T>
class HexahedronDistance : public sds::SparseCubicField<T> {
  
public:

    HexahedronDistance();
    virtual ~HexahedronDistance();

protected:

private:
  
};

template<typename T>
HexahedronDistance<T>::HexahedronDistance()
{}

template<typename T>
HexahedronDistance<T>::~HexahedronDistance()
{}
    
}

}

#endif
