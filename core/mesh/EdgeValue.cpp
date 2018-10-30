#include "EdgeValue.h"

namespace alo {

EdgeValue::EdgeValue() : m_cost(1e28f)
{}

bool EdgeValue::connectToFace(const FaceIndex &x)
{
	if(!m_face0.isValid() ) {
	    m_face0 = x;
	    return true;
	}
	if(m_face1.isValid() )
	    return false;
	
	m_face1 = x;
	return true;
}

bool EdgeValue::disconnectFace(const FaceIndex &x)
{
    if(m_face1 == x) {
        m_face1 = FaceIndex();  
        return true;
    }
    
    if(m_face0 == x) {
        m_face0 = m_face1;
        m_face1 = FaceIndex();  
        return true;
    }
    
    return false;
}

bool EdgeValue::isOnBorder() const
{ return !m_face1.isValid(); }

float &EdgeValue::cost()
{ return m_cost; }

const float &EdgeValue::cost() const
{ return m_cost; }

const FaceIndex &EdgeValue::face0() const
{ return m_face0; }

const FaceIndex &EdgeValue::face1() const
{ return m_face1; }

std::ostream& operator<<(std::ostream &output, const EdgeValue & p)
{
    output << " " << p.face0() << "|" << p.face1();
    return output;
}

}
