/*
 *  TubularMesher.h
 *  gorse
 *
 *  2019/8/10
 */

#ifndef ALO_TUBULAR_MESHER_H
#define ALO_TUBULAR_MESHER_H

#include <mesh/FrontMesher.h>

namespace alo {

class AdaptableMesh;
class TubularCrossSection;
class TubularProfile;

class TubularMesher : public FrontMesher {

public:

	TubularMesher();
	virtual ~TubularMesher();

	void triangulate(const TubularCrossSection &cs, const TubularProfile &prof);

protected:

private:

};
	
}

#endif
