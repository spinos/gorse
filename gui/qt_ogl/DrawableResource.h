/*
 *  DrawableResource.h
 *  aloe
 * 
 *  data can be modified on cpu
 *  once expanded associated gl resouce must be destroyed and recreated
 *
 */

#ifndef ALO_DRAWABLE_RESOURCE_H
#define ALO_DRAWABLE_RESOURCE_H

#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

namespace alo {

class DrawableObject;

class DrawableResource {

	SimpleBuffer<Vector3F> m_posnml;
    SimpleBuffer<Vector3F> m_baryc;
    DrawableObject *m_object;
    int m_drawArrayLength;
    bool m_isDirty;
    bool m_toRelocate;

public:
	DrawableResource();

	void attachToDrawable(DrawableObject *object);
	void dettachDrawable();
	DrawableObject *drawable();

	const int &size() const;
	const int &drawArrayLength() const;
	bool toRelocate() const;
	void setDrawArrayLength(int x);
	void setToRelocate(bool x);
	void setDirty(bool x);
	bool isDirty() const;

	SimpleBuffer<Vector3F> &posnmlBuffer();
	SimpleBuffer<Vector3F> &barycBuffer();

	const SimpleBuffer<Vector3F> &c_posnmlBuffer() const;
	const SimpleBuffer<Vector3F> &c_barycBuffer() const;

};

}

#endif
