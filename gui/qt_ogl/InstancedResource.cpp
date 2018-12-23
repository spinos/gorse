/*
 *  InstancedResource.cpp
 *  aloe
 * 
 */
 
#include "InstancedResource.h"
#include "DrawableObject.h"

namespace alo {

InstancedResource::InstancedResource() :
m_numInstances(0)
{}

InstancedResource::~InstancedResource()
{}

void InstancedResource::attachToDrawable(DrawableObject *object)
{
    DrawableResource::attachToDrawable(object);
    if(m_numInstances < 2) return;
    object->setTransformMatrix((const float *)m_tms.c_data(), m_tms.capacityByteSize());
    object->setNumInstances(m_numInstances);
}

const SimpleBuffer<Matrix44F> &InstancedResource::transformBuffer() const
{ return m_tms; }

SimpleBuffer<Matrix44F> &InstancedResource::transformBuffer()
{ return m_tms; }

const int &InstancedResource::numInstances() const
{ return m_numInstances; }

void InstancedResource::setNumInstances(int x)
{ m_numInstances = x; }

}
