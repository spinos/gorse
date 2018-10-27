#include "EdgeValue.h"

namespace alo {

EdgeValue::EdgeValue() : m_cost(1e28f),
m_face0(-1),
m_face1(-1)
{}

void EdgeValue::connectToFace(int i)
{
	if(m_face0 < 0) m_face0 = i;
	else m_face1 = i;
}

bool EdgeValue::isOnBorder() const
{ return m_face1 < 0; }

float &EdgeValue::cost()
{ return m_cost; }

const float &EdgeValue::cost() const
{ return m_cost; }

}
