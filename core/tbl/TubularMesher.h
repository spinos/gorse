/*
 *  TubularMesher.h
 *  gorse
 *
 *  2019/8/10
 */

#ifndef ALO_TUBULAR_MESHER_H
#define ALO_TUBULAR_MESHER_H

namespace alo {

class AdaptableMesh;
class TubularCrossSection;

class TubularMesher {

	AdaptableMesh *m_mesh;

public:

	TubularMesher();
	virtual ~TubularMesher();

	void attach(AdaptableMesh *mesh);
	void detach();
	void triangulate(const TubularCrossSection &cs);

protected:

private:

};
	
}

#endif
