#include "FaceValue.h"
#include "FaceIndex.h"
#include <math/Vector3F.h>

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

void FaceValue::calculateArea(const Vector3F *pos, const FaceIndex &fi)
{
	Vector3F d[3]; float l[3];

	d[0] = pos[fi.v1()] - pos[fi.v0()];
	l[0] = d[0].length();
	
	d[1] = pos[fi.v2()] - pos[fi.v1()];
	l[1] = d[1].length();
	
	d[2] = pos[fi.v0()] - pos[fi.v2()];
	l[2] = d[2].length();
	
	m_area = (0.5*sqrt(l[0]*l[0]*l[2]*l[2] - (d[0].dot(d[2]))*(d[0].dot(d[2]))));
}
	
int &FaceValue::ind()
{ return m_ind; }

const int &FaceValue::ind() const
{ return m_ind; }

const float &FaceValue::area() const
{ return m_area; }

}
