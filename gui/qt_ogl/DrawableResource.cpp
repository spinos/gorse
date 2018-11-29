/*
 *  DrawableResource.cpp
 *  aloe
 *
 */

#include "DrawableResource.h"

namespace alo {

DrawableResource::DrawableResource() : m_toRelocate(0)
{}

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
