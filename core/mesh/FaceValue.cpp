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

void FaceValue::setVertexUV(int vertex0, int uv0,
					int vertex1, int uv1,
					int vertex2, int uv2)
{
	m_vertexId[0] = vertex0;
	m_vertexId[1] = vertex1;
	m_vertexId[2] = vertex2;
	m_uvId[0] = uv0;
	m_uvId[1] = uv1;
	m_uvId[2] = uv2;
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

int FaceValue::vertexUV(int vi) const
{
	if(m_vertexId[0] == vi) return m_uvId[0];
	if(m_vertexId[1] == vi) return m_uvId[1];
	if(m_vertexId[2] == vi) return m_uvId[2];
	return -1;
}

bool FaceValue::replaceVertex(int a, int b)
{
	if(a == m_vertexId[0]) {
		m_vertexId[0] = b; return true;
	}
	if(a == m_vertexId[1]) {
		m_vertexId[1] = b; return true;
	}
	if(a == m_vertexId[2]) {
		m_vertexId[2] = b; return true;
	}
	return false;
}

}
