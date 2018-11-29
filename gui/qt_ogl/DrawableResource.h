/*
 *  DrawableResource.h
 *  aloe
 * 
 *  data can be modified on cpu
 *  once expanded gl resouce must be destroyed and recreated
 *
 */

#ifndef ALO_DRAWABLE_RESOURCE_H
#define ALO_DRAWABLE_RESOURCE_H

#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

namespace alo {

class DrawableResource {

	SimpleBuffer<Vector3F> m_posnml;
    SimpleBuffer<Vector3F> m_baryc;
    bool m_toRelocate;

public:
	DrawableResource();

	const int &size() const;
	bool toRelocate() const;
	void setToRelocate(bool x);

	SimpleBuffer<Vector3F> &posnmlBuffer();
	SimpleBuffer<Vector3F> &barycBuffer();

	const SimpleBuffer<Vector3F> &c_posnmlBuffer() const;
	const SimpleBuffer<Vector3F> &c_barycBuffer() const;

};

}

#endif
