/*
 *  HexahedronDistanceBuilder.h
 *
 *  sparse distance field from samples
 *  find node closest to zero
 *  propagate distance
 *  gradient of distance as sample normals
 *
 *  2019/7/11
 */
 
#ifndef ALO_SGDF_HEXAHEDRON_DISTANCE_BUILDER_H
#define ALO_SGDF_HEXAHEDRON_DISTANCE_BUILDER_H

#include <sds/SparseFieldBuilder.h>
#include "HexahedronDistance.h"

namespace alo {
    
namespace gdf {
    
template<typename T>
class HexahedronDistanceBuilder : public sds::SparseFieldBuilder<T> {
  
public:

    HexahedronDistanceBuilder();
    virtual ~HexahedronDistanceBuilder();

    template<typename Tr>
    void detach(Tr &rule);

protected:

private:
  
};

template<typename T>
HexahedronDistanceBuilder<T>::HexahedronDistanceBuilder()
{}

template<typename T>
HexahedronDistanceBuilder<T>::~HexahedronDistanceBuilder()
{}

template<typename T>
template<typename Tr>
void HexahedronDistanceBuilder<T>::detach(Tr &rule)
{
	sds::SparseFieldBuilder<T>::detach();
	HexahedronDistance<float> hexa;
	hexa.buildGraph<T, Tr>(field(), rule);
	hexa.computeDistance<Tr>(rule);
	const int &nv = hexa.numNodes();
	for(int i=0;i<nv;++i) {
		float d = hexa.nodeDistance(i);
		field()->setNodeValue(i, d);
	}
}

}

}

#endif
