#include "EdgeValue.h"

namespace alo {

EdgeValue::EdgeValue() : m_cost(1e28f)
{}

void EdgeValue::connectToFace(const FaceIndex &x)
{
	if(!m_face0.isValid() ) m_face0 = x;
	else m_face1 = x;
}

bool EdgeValue::isOnBorder() const
{ return !m_face1.isValid(); }

float &EdgeValue::cost()
{ return m_cost; }

const float &EdgeValue::cost() const
{ return m_cost; }

}
