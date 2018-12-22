/*
 *  VertexValue.cpp
 *  aloe
 *
 */

#include "VertexValue.h"
#include <math/Vector3F.h>
#include <algorithm>

namespace alo {

VertexValue::VertexValue() : m_cost(1e28f),
m_locked(false),
m_onborder(false),
m_isRingConcave(false)
{}
	
VertexValue::~VertexValue()
{}

float &VertexValue::cost()
{ return m_cost; }

const float &VertexValue::cost() const
{ return m_cost; }

void VertexValue::setCost(float x)
{ m_cost = x; }

void VertexValue::setOnBorder(bool x)
{ m_onborder = x; }

void VertexValue::setRingConcave(bool x)
{ m_isRingConcave = x; }

const bool &VertexValue::isOnBorder() const
{ return m_onborder; }

void VertexValue::lock()
{ m_locked = true; }

void VertexValue::unlock()
{ m_locked = false; }

const bool &VertexValue::isLocked() const
{ return m_locked; }

const bool &VertexValue::isRingConcave() const
{ return m_isRingConcave; }

std::ostream& operator<<(std::ostream &output, const VertexValue & p)
{
	output << " vertex cost "<<p.cost() << " on-border " << p.isOnBorder() 
	<< " locked " << p.isLocked();
    return output;
}

}
