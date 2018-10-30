#include "VertexValue.h"
#include <math/Vector3F.h>
#include <algorithm>

namespace alo {

VertexValue::VertexValue() : m_cost(1e28f),
m_locked(false)
{}
	
VertexValue::~VertexValue()
{ m_faceInds.clear(); }

void VertexValue::connectToFace(const FaceIndex &x)
{ m_faceInds.push_back(x); }

void VertexValue::disconnectFace(const FaceIndex &x)
{
	std::deque<FaceIndex>::iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {

		if(*it == x)  {
			m_faceInds.erase(it);
			return;
		}
	}
}

void VertexValue::clearFaces()
{ m_faceInds.clear(); }

float VertexValue::computeCost(const int &vi,
					const Vector3F *normals)
{ 
	m_cost = 0.f;
	const Vector3F &vn = normals[vi];

	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it != m_faceInds.end(); ++it) {
		const FaceIndex &fi = *it;
		if(fi.v0() != vi) 
			higherCost(vn, normals[fi.v0()]);

		if(fi.v1() != vi)
			higherCost(vn, normals[fi.v1()]);
		
		if(fi.v2() != vi)
			higherCost(vn, normals[fi.v2()]);
	}
	m_cost *= m_faceInds.size();
	return m_cost; 
}

void VertexValue::higherCost(const Vector3F &Na,
					const Vector3F &Nb)
{
	float c = 1.01f - Na.dot(Nb);
	if(m_cost < c) m_cost = c;
}

const float &VertexValue::cost() const
{ return m_cost; }

void VertexValue::getConnectedVertices(std::deque<int> &vs,
			int vi) const
{
	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		const FaceIndex &fi = *it;

		if(fi.v0() != vi) 
			addToVector(vs, fi.v0());

		if(fi.v1() != vi)
			addToVector(vs, fi.v1());
		
		if(fi.v2() != vi)
			addToVector(vs, fi.v2());
	}
}

void VertexValue::getReformedFaces(std::deque<FaceIndex> &faces, int vi, int vb) const
{
	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		const FaceIndex &fi = *it;
		int b[3];
		b[0] = fi.v0() == vi ? vb : fi.v0();
		b[1] = fi.v1() == vi ? vb : fi.v1();
		b[2] = fi.v2() == vi ? vb : fi.v2();

		faces.push_back(FaceIndex(b[0], b[1], b[2]));
	}
}

void VertexValue::addToVector(std::deque<int> &vs, int x) const
{
	if(std::find(vs.begin(), vs.end(), x) == vs.end()) 
		vs.push_back(x);
}

bool VertexValue::check(int vi) const
{
	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		const FaceIndex &fi = *it;
		if(!fi.hasV(vi) ) {
			std::cout<<"\n\n ERROR face"<<fi<< " not connected to vert "<<vi;
			return false;
		}
	}
	return true;
}

void VertexValue::getCollapsedFaces(std::deque<FaceIndex> &faces,
			int va, int vb) const
{
	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		const FaceIndex &fi = *it;

		if(!fi.hasV(va)) 
			continue;

		if(fi.hasV(vb)) 
			continue;

		int fv[3];
		fv[0] = fi.v0();
		fv[1] = fi.v1();
		fv[2] = fi.v2();

		if(fv[0] == va) fv[0] = vb;
		if(fv[1] == va) fv[1] = vb;
		if(fv[2] == va) fv[2] = vb;

		faces.push_back(FaceIndex(fv[0], fv[1], fv[2]) );
	}
}

const std::deque<FaceIndex> &VertexValue::connectedFaces() const
{ return m_faceInds; }

int VertexValue::lastConnectedVertex() const
{
	int r = 0;
	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		const FaceIndex &fi = *it;

		if(r < fi.v2()) r = fi.v2();
	}
	return r;
}

void VertexValue::lock()
{ m_locked = true; }

const bool &VertexValue::isLocked() const
{ return m_locked; }

std::ostream& operator<<(std::ostream &output, const VertexValue & p)
{
	output << " f [";
	std::deque<FaceIndex>::const_iterator it = p.connectedFaces().begin();
	for(;it!=p.connectedFaces().end();++it) {
    	output << " " << *it;
    }
    output << "] ";
    return output;
}

}
