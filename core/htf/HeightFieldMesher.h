/*
 * HeightFieldMesher.h
 * aloe
 *
 *
 * 2019/8/8
 */

#ifndef ALO_HTF_HEIGHT_FIELD_MESHER_H
#define ALO_HTF_HEIGHT_FIELD_MESHER_H

#include "HeightField.h"
#include <geom/AdaptableMesh.h>

namespace alo {

class HeightFieldMesher {

	AdaptableMesh *m_mesh;

public:

	HeightFieldMesher();
	virtual ~HeightFieldMesher();

	void attach(AdaptableMesh *msh);
	void triangulate(const HeightField &fld);
	void detach();

protected:

private:

};

}

#endif
