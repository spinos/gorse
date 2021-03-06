/*
 *  DrawableResource.h
 *  aloe
 * 
 *  data can be modified on cpu
 *  once relocated, associated gl resouce must be destroyed and recreated
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
    int m_changedOnFrame;
    bool m_isDirty;
    bool m_toRelocate;
    bool m_deferred;
/// only instance
    float m_tm[16];
    float m_surfaceColor[3];
    float m_wireColor[3];
    
public:
	DrawableResource();
    virtual ~DrawableResource();

	virtual void attachToDrawable(DrawableObject *object);
	void dettachDrawable();
	DrawableObject *drawable();

	int size() const;
	const int &drawArrayLength() const;
	const int &changedOnFrame() const;
	bool toRelocate() const;
	void setDrawArrayLength(int x);
	void setChangedOnFrame(int x);
	void setToRelocate(bool x);
	void setDirty(bool x);
	bool isDirty() const;
    void setDeferred(bool x);
    bool isDeferred() const;
    void createBarycentricCoordinates(int numIndices);

	SimpleBuffer<Vector3F> &posnmlBuffer();
	SimpleBuffer<Vector3F> &barycBuffer();

	const SimpleBuffer<Vector3F> &c_posnmlBuffer() const;
	const SimpleBuffer<Vector3F> &c_barycBuffer() const;

	void setSurfaceColor(const float *c);
	void setWireColor(const float *c);
    
protected:

private:
	static const float FaceBarycentricCoordinate[12];

};

}

#endif
