/*
 *  DrawableResource.cpp
 *  aloe
 *
 */

#include "DrawableResource.h"
#include "DrawableObject.h"

namespace alo {

DrawableResource::DrawableResource() : 
m_object(0),
m_toRelocate(0)
{}

void DrawableResource::attachToDrawable(DrawableObject *object, int drawLength)
{
	m_object = object;
	object->setPosnml((const float *)m_posnml.c_data(), m_posnml.capacityByteSize());
    object->setBarycentric((const float *)m_baryc.c_data(), m_baryc.capacityByteSize());
    object->setDrawArrayLength(drawLength);
    m_toRelocate = false;
}

DrawableObject *DrawableResource::drawable()
{ return m_object; }

const int &DrawableResource::size() const
{ return m_posnml.capacity(); }

bool DrawableResource::toRelocate() const
{ return m_toRelocate; }

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

}
