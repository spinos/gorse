#include "VertexValue.h"
#include <math/Vector3F.h>
#include <algorithm>

namespace alo {

VertexValue::VertexValue() : m_cost(1e28f)
{}
	
VertexValue::~VertexValue()
{ m_faceIndices.clear(); }

void VertexValue::connectToFace(int x)
{ m_faceIndices.push_back(x); }

float VertexValue::computeCost(const int &vi,
					const unsigned *indices,
					const Vector3F *normals)
{ 
	m_cost = 0.f;
	const Vector3F &vn = normals[vi];

	std::vector<int>::const_iterator it = m_faceIndices.begin();
	for(;it != m_faceIndices.end(); ++it) {
		const int j = *it;
		int v0 = indices[j * 3];
		if(v0 != vi) 
			higherCost(vn, normals[v0]);

		int v1 = indices[j * 3 + 1];
		if(v1 != vi)
			higherCost(vn, normals[v1]);
		
		int v2 = indices[j * 3 + 2];
		if(v2 != vi)
			higherCost(vn, normals[v2]);
	}
	return m_cost; 
}

void VertexValue::higherCost(const Vector3F &Na,
					const Vector3F &Nb)
{
	float c = 1.f - Na.dot(Nb);
	if(m_cost < c) m_cost = c;
}

const float &VertexValue::cost() const
{ return m_cost; }

const std::vector<int> &VertexValue::faceIndices() const
{ return m_faceIndices; }

int VertexValue::numFaces() const
{ return m_faceIndices.size(); }

void VertexValue::getConnectedVertices(std::vector<int> &vs,
			int vi, const unsigned *indices) const
{
	std::vector<int>::const_iterator it = m_faceIndices.begin();
	for(;it!=m_faceIndices.end();++it) {
		const unsigned *fj = &indices[*it * 3];
		int v0 = fj[0];
		if(v0 != vi) 
			addToVector(vs, v0);

		int v1 = fj[1];
		if(v1 != vi)
			addToVector(vs, v1);
		
		int v2 = fj[2];
		if(v2 != vi)
			addToVector(vs, v2);
	}
}

void VertexValue::addToVector(std::vector<int> &vs, int x) const
{
	if(std::find(vs.begin(), vs.end(), x) == vs.end()) 
		vs.push_back(x);
}

void VertexValue::removeFaceConnectedTo(const int &vi,
					const unsigned *indices)
{
	std::vector<int>::iterator it = m_faceIndices.begin();
	for(;it!=m_faceIndices.end();++it) {
		const unsigned *fj = &indices[*it * 3];
		if(fj[0] == vi ||  fj[1] == vi || fj[2] == vi) {

			m_faceIndices.erase(it);
			std::cout << " remove face " << fj[0] <<","<< fj[1] <<","<< fj[2];
			
			return;
		}
	}
}

void VertexValue::swapFace(int a, int b)
{
	std::vector<int>::iterator it = m_faceIndices.begin();
	//for(;it!=m_faceIndices.end();++it) {
	//	std::cout<<" "<<*it;
	//}

	//it = m_faceIndices.begin();
	for(;it!=m_faceIndices.end();++it) {
		if(*it == a) {
			*it = b;
			std::cout<<" "<<a<<" to "<<b;
		}
	}
}

void VertexValue::operator=(const VertexValue &b)
{
	m_cost = b.m_cost;
	m_faceIndices.clear();
	std::vector<int>::const_iterator it = b.faceIndices().begin();
	for(;it!=b.faceIndices().end();++it) {
		m_faceIndices.push_back(*it);
	}
}

}
