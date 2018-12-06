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
m_drawArrayLength(0),
m_changedOnFrame(-999999),
m_isDirty(false),
m_toRelocate(false)
{}

void DrawableResource::attachToDrawable(DrawableObject *object)
{
	m_object = object;
	object->setPosnml((const float *)m_posnml.c_data(), m_posnml.capacityByteSize());
    object->setBarycentric((const float *)m_baryc.c_data(), m_baryc.capacityByteSize());
    object->setDrawArrayLength(m_drawArrayLength);
    m_isDirty = false;
    m_toRelocate = false;
}

void DrawableResource::dettachDrawable()
{
	m_object->setDrawArrayLength(0);
	m_object = 0;
	m_isDirty = false;
	m_toRelocate = false;
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

}
