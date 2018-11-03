#include "VertexValue.h"
#include <math/Vector3F.h>
#include <algorithm>

namespace alo {

VertexValue::VertexValue() : m_cost(1e28f),
m_locked(false),
m_onborder(false)
{}
	
VertexValue::~VertexValue()
{ m_faceInds.clear(); }

void VertexValue::connectToFace(const FaceIndex &x)
{ 
	std::deque<FaceIndex>::iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		if(*it == x)
			return;
	}
	m_faceInds.push_back(x); 
}

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

void VertexValue::higherCost(const Vector3F &Na,
					const Vector3F &Nb)
{
	float c = 1.f - Na.dot(Nb);
	if(m_cost < c) m_cost = c;
}

float &VertexValue::cost()
{ return m_cost; }

const float &VertexValue::cost() const
{ return m_cost; }

void VertexValue::setOnBorder(bool x)
{ m_onborder = x; }

const bool &VertexValue::isOnBorder() const
{ return m_onborder; }

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

void VertexValue::unlock()
{ m_locked = false; }

const bool &VertexValue::isLocked() const
{ return m_locked; }

bool VertexValue::getOneRing(std::vector<int> &vertInds, int vi,
		const Vector3F *pos, const Vector3F &nv) const
{
	const int nf = m_faceInds.size();
	if(nf < 3) return false;
	int va, vb, vc;
	if(!m_faceInds[0].getOppositeEdge(va, vb, vi) )
		return false;

	const Vector3F &pv = pos[vi];
	const Vector3F nab = (pos[va] - pv).cross(pos[vb] - pv);
	if(nab.dot(nv) < 0.f) {
		vc = vb;
		vb = va;
		va = vc;
	}
	vertInds.push_back(va); 
	vertInds.push_back(vb);

	const int endv = va;
	for(;;) {
		if(!findNextVertex(vc, va, vb, vi))
			return false;

		if(vc == endv)
			break;

		vertInds.push_back(vc);

		va = vb;
		vb = vc;
	}

	return vertInds.size() == nf;
}

bool VertexValue::findNextVertex(int &c, const int &a, const int &b, const int &vi) const
{
	for(int i=0;i<m_faceInds.size();++i) {
		const FaceIndex &fi = m_faceInds[i];
		if(!fi.hasV(b)) continue;
		c = fi.getOpposite(b, vi);
		if(c > -1 && c != a)
			return true;
	}
	return false;
}

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
