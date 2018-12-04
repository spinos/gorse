/*
 *  DrawableResource.h
 *  aloe
 * 
 *  data can be modified on cpu
 *  once expanded associated gl resouce must be destroyed and recreated
 *  remove if not visible for a certain number of cycles
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
    short m_visibility;
    bool m_isDirty;
    bool m_isVisibilityChanged;
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
	void setVisible(bool x);
	bool isVisible() const;
	bool isHidden() const;
	bool isVisiblilityChanged() const;
	bool shouldRemove() const;
	void setDirty(bool x);
	bool isDirty() const;

	SimpleBuffer<Vector3F> &posnmlBuffer();
	SimpleBuffer<Vector3F> &barycBuffer();

	const SimpleBuffer<Vector3F> &c_posnmlBuffer() const;
	const SimpleBuffer<Vector3F> &c_barycBuffer() const;

	enum VisibleTag {
		vtUnknown = 0,
		vtDormant = 32757,
		vtHidden = 32766,
		vtNormal = 32767 
	};
};

}

#endif
