/*
 *  DrawableResource.cpp
 *  aloe
 *
 */

#include "DrawableResource.h"
#include "DrawableObject.h"

namespace alo {

const float DrawableResource::FaceBarycentricCoordinate[12] = {1.f, 0.f, 0.f,
0.f, 1.f, 0.f,
0.f, 0.f, 1.f,
1.f, 1.f, 1.f};

DrawableResource::DrawableResource() : 
m_object(0),
m_drawArrayLength(0),
m_changedOnFrame(-999999),
m_isDirty(false),
m_toRelocate(false),
m_deferred(false)
{}

void DrawableResource::attachToDrawable(DrawableObject *object)
{
	m_object = object;
	object->setPosnml((const float *)m_posnml.c_data(), m_posnml.capacityByteSize());
    object->setBarycentric((const float *)m_baryc.c_data(), m_baryc.capacityByteSize());
    object->setDrawArrayLength(m_drawArrayLength);
    m_isDirty = false;
    m_toRelocate = false;
    m_deferred = false;
}

void DrawableResource::dettachDrawable()
{
	//m_object->setDrawArrayLength(0);
	m_object = 0;
	m_isDirty = false;
	m_toRelocate = false;
    m_deferred = false;
}

DrawableObject *DrawableResource::drawable()
{ return m_object; }

const int &DrawableResource::size() const
{ return m_posnml.capacity(); }

const int &DrawableResource::drawArrayLength() const
{ return m_drawArrayLength; }

const int &DrawableResource::changedOnFrame() const
{ return m_changedOnFrame; }

bool DrawableResource::toRelocate() const
{ return m_toRelocate; }

void DrawableResource::setDrawArrayLength(int x)
{ m_drawArrayLength = x; }

void DrawableResource::setToRelocate(bool x)
{ m_toRelocate = x; }

SimpleBuffer<Vector3F> &DrawableResource::posnmlBuffer()
{ return m_posnml; }

SimpleBuffer<Vector3F> &DrawableResource::barycBuffer()
{ return m_baryc; }

const SimpleBuffer<Vector3F> &DrawableResource::c_posnmlBuffer() const
{ return m_posnml; }

const SimpleBuffer<Vector3F> &DrawableResource::c_barycBuffer() const
{ return m_baryc; }

void DrawableResource::setDirty(bool x)
{ m_isDirty = x; }

bool DrawableResource::isDirty() const
{ return m_isDirty; }

void DrawableResource::setChangedOnFrame(int x)
{ m_changedOnFrame = x; }

void DrawableResource::setDeferred(bool x)
{ m_deferred = x; }

bool DrawableResource::isDeferred() const
{ return m_deferred; }

void DrawableResource::createBarycentricCoordinates(int numIndices)
{
	m_baryc.resetBuffer(numIndices);
	Vector3F *d = m_baryc.data();
    const int n = m_baryc.capacity() / 3;
    for(int i=0;i<n;++i) {
    	memcpy(d, FaceBarycentricCoordinate, 36);
    	d += 3;
    }
}

}
