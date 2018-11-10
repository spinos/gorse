#include "FaceValue.h"
#include "FaceIndex.h"

namespace alo {

FaceValue::FaceValue()
{ 
	m_ind = -1; 
	m_area = 1e28f;
}

FaceValue::FaceValue(int x)
{ 
	m_ind = x; 
	m_area = 1e28f;
}

void FaceValue::setNormal(const Vector3F &nml)
{ m_nml = nml; }

void FaceValue::setArea(const float &x)
{ m_area = x; }
	
int &FaceValue::ind()
{ return m_ind; }

const int &FaceValue::ind() const
{ return m_ind; }

const float &FaceValue::area() const
{ return m_area; }

const Vector3F &FaceValue::normal() const
{ return m_nml; }

}
