/*
 *  EdgeValue.cpp
 *  aloe
 *
 */

#include "EdgeValue.h"
#include "EdgeIndex.h"

namespace alo {

EdgeValue::EdgeValue() : m_ind(0)
{}

bool EdgeValue::connectToFace(const FaceIndex &x)
{
    if(x == m_face0 || x == m_face1) return true;
    
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

void EdgeValue::setInd(int x)
{ m_ind = x; }

const int &EdgeValue::ind() const
{ return m_ind; }

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
